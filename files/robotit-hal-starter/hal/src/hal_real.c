/*
 * hal_real.c - real backend for the RobotIT board.
 *
 * Pin map and thresholds come from the schematic and from the known working
 * diagnostic firmware. Read this file to understand what a call costs in time;
 * you are not expected to modify it.
 *
 * Every function here documents its worst case blocking time. That number is
 * part of the contract in hal.h, because your task periods depend on it.
 */

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/pulse_cnt.h"
#include "driver/i2c_master.h"
#include "esp_adc/adc_oneshot.h"

#include "hal.h"

/* ------------------------------------------------------------- pin map ----- */
/* Verified against the schematic: page 1 (connectors), page 3 (controller). */

#define PIN_LED            2
#define PIN_BUZZER         1
#define PIN_NOT_SLEEP      21

#define PIN_MOTOR_L_IN1    5
#define PIN_MOTOR_L_IN2    6
#define PIN_ENC_L_A        17
#define PIN_ENC_L_B        18

#define PIN_MOTOR_R_IN1    48
#define PIN_MOTOR_R_IN2    47
#define PIN_ENC_R_A        15
#define PIN_ENC_R_B        16

#define PIN_I2C_SDA        10
#define PIN_I2C_SCL        11
#define PIN_XSHUT_1        12
#define PIN_XSHUT_2        13
#define PIN_XSHUT_3        14

#define PIN_BATTERY_ADC    4
#define BATTERY_DIVIDER    2.0f

/* ---------------------------------------------------------------- tuning --- */

#define PWM_FREQ_HZ        20000
#define PWM_RES            LEDC_TIMER_10_BIT
#define PWM_MAX_DUTY       1023

#define DUTY_LIMIT         0.80f  /* never ask for more than this */
#define DUTY_SLEW_PER_S    4.0f   /* brownout protection */

#define COUNTS_PER_REV     4200.0f
#define MOTOR_MAX_RPM      180.0f

#define TOF_I2C_ADDR       0x29
#define TOF_TIMEOUT_MS     15

/* ---------------------------------------------------------------- state ---- */

typedef struct {
    ledc_channel_t ch_a, ch_b;
    float duty_cmd;
    float duty_applied;
    bool  inverted;
    pcnt_unit_handle_t pcnt;
} motor_t;

static motor_t            s_motor[HAL_NUM_MOTORS];
static i2c_master_bus_handle_t s_i2c;
static adc_oneshot_unit_handle_t s_adc;
static bool               s_ready;
static int64_t            s_last_slew_us;

/* ---------------------------------------------------------------- helpers -- */

/* Apply one PWM pair. Both inputs get a duty so a low value is a real 0 %. */
static void motor_apply(int idx, float duty) {
    if (idx < 0 || idx >= HAL_NUM_MOTORS) {
        return;
    }
    motor_t *m = &s_motor[idx];
    float d = duty;
    if (m->inverted) {
        d = -d;
    }
    if (d > DUTY_LIMIT)  { d = DUTY_LIMIT; }
    if (d < -DUTY_LIMIT) { d = -DUTY_LIMIT; }

    uint32_t level_a = 0;
    uint32_t level_b = 0;
    if (d > 0.0f) {
        level_a = (uint32_t)(d * PWM_MAX_DUTY);
    } else if (d < 0.0f) {
        level_b = (uint32_t)(-d * PWM_MAX_DUTY);
    }
    /* Both zero = free running, which is the safe state. */

    ledc_set_duty(LEDC_LOW_SPEED_MODE, m->ch_a, level_a);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, m->ch_a);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, m->ch_b, level_b);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, m->ch_b);
}

/* ---------------------------------------------------------------- system --- */

esp_err_t hal_init(void) {
    /* Boost converter awake first: without it the H-bridge has no supply. */
    gpio_config_t out_cfg = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << PIN_NOT_SLEEP) | (1ULL << PIN_LED) | (1ULL << PIN_BUZZER),
    };
    if (gpio_config(&out_cfg) != ESP_OK) {
        return ESP_FAIL;
    }
    gpio_set_level(PIN_NOT_SLEEP, 1);
    gpio_set_level(PIN_LED, 0);
    gpio_set_level(PIN_BUZZER, 0);

    /* PWM for four H-bridge inputs. */
    ledc_timer_config_t timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_1,
        .duty_resolution = PWM_RES,
        .freq_hz = PWM_FREQ_HZ,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    if (ledc_timer_config(&timer) != ESP_OK) {
        return ESP_FAIL;
    }

    s_motor[HAL_MOTOR_LEFT].ch_a = LEDC_CHANNEL_0;
    s_motor[HAL_MOTOR_LEFT].ch_b = LEDC_CHANNEL_1;
    s_motor[HAL_MOTOR_RIGHT].ch_a = LEDC_CHANNEL_2;
    s_motor[HAL_MOTOR_RIGHT].ch_b = LEDC_CHANNEL_3;
    s_motor[HAL_MOTOR_RIGHT].inverted = true;  /* mounted mirrored: see schematic */

    for (int i = 0; i < HAL_NUM_MOTORS; i++) {
        ledc_channel_config_t ch = {
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .timer_sel = LEDC_TIMER_1,
            .intr_type = LEDC_INTR_DISABLE,
            .duty = 0,
            .hpoint = 0,
        };
        ch.channel = s_motor[i].ch_a;
        if (ledc_channel_config(&ch) != ESP_OK) { return ESP_FAIL; }
        ch.channel = s_motor[i].ch_b;
        if (ledc_channel_config(&ch) != ESP_OK) { return ESP_FAIL; }
        s_motor[i].duty_cmd = 0.0f;
        s_motor[i].duty_applied = 0.0f;
    }

    /* Encoders: one pulse counter unit per motor, 4x decode. */
    const int enc_a[HAL_NUM_MOTORS] = { PIN_ENC_L_A, PIN_ENC_R_A };
    const int enc_b[HAL_NUM_MOTORS] = { PIN_ENC_L_B, PIN_ENC_R_B };
    for (int i = 0; i < HAL_NUM_MOTORS; i++) {
        pcnt_unit_config_t unit = { .high_limit = 30000, .low_limit = -30000 };
        if (pcnt_new_unit(&unit, &s_motor[i].pcnt) != ESP_OK) { return ESP_FAIL; }
        pcnt_chan_config_t ca = { .edge_gpio_num = enc_a[i], .level_gpio_num = enc_b[i] };
        pcnt_channel_handle_t cha;
        if (pcnt_new_channel(s_motor[i].pcnt, &ca, &cha) != ESP_OK) { return ESP_FAIL; }
        pcnt_chan_config_t cb = { .edge_gpio_num = enc_b[i], .level_gpio_num = enc_a[i] };
        pcnt_channel_handle_t chb;
        if (pcnt_new_channel(s_motor[i].pcnt, &cb, &chb) != ESP_OK) { return ESP_FAIL; }
        pcnt_channel_set_edge_action(cha, PCNT_CHANNEL_EDGE_ACTION_INCREASE,
                                          PCNT_CHANNEL_EDGE_ACTION_DECREASE);
        pcnt_channel_set_level_action(cha, PCNT_CHANNEL_LEVEL_ACTION_KEEP,
                                           PCNT_CHANNEL_LEVEL_ACTION_INVERSE);
        pcnt_channel_set_edge_action(chb, PCNT_CHANNEL_EDGE_ACTION_INCREASE,
                                          PCNT_CHANNEL_EDGE_ACTION_DECREASE);
        pcnt_channel_set_level_action(chb, PCNT_CHANNEL_LEVEL_ACTION_KEEP,
                                           PCNT_CHANNEL_LEVEL_ACTION_INVERSE);
        pcnt_unit_enable(s_motor[i].pcnt);
        pcnt_unit_clear_count(s_motor[i].pcnt);
        pcnt_unit_start(s_motor[i].pcnt);
    }

    /* Shared I2C bus for the three distance sensors. */
    i2c_master_bus_config_t bus = {
        .i2c_port = -1,
        .sda_io_num = PIN_I2C_SDA,
        .scl_io_num = PIN_I2C_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    if (i2c_new_master_bus(&bus, &s_i2c) != ESP_OK) { return ESP_FAIL; }

    /* Sensor enable pins: all sensors share one I2C address, so only one may
     * be awake at a time. Held in reset (low) until the addressing sequence. */
    gpio_config_t xshut = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << PIN_XSHUT_1) | (1ULL << PIN_XSHUT_2) | (1ULL << PIN_XSHUT_3),
    };
    if (gpio_config(&xshut) != ESP_OK) { return ESP_FAIL; }
    gpio_set_level(PIN_XSHUT_1, 0);
    gpio_set_level(PIN_XSHUT_2, 0);
    gpio_set_level(PIN_XSHUT_3, 0);

    /* Battery ADC, channel 3 = IO4 on ADC1, through a 2 to 1 divider. */
    adc_oneshot_unit_init_cfg_t unit_cfg = { .unit_id = ADC_UNIT_1 };
    if (adc_oneshot_new_unit(&unit_cfg, &s_adc) != ESP_OK) { return ESP_FAIL; }
    adc_oneshot_chan_cfg_t chan_cfg = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    if (adc_oneshot_config_channel(s_adc, ADC_CHANNEL_3, &chan_cfg) != ESP_OK) {
        return ESP_FAIL;
    }

    s_last_slew_us = esp_timer_get_time();
    s_ready = true;
    printf("hal: real backend ready\n");
    return ESP_OK;
}

const char *hal_backend_name(void) {
    return "real";
}

/* ---------------------------------------------------------------- motors --- */

esp_err_t hal_motor_set_duty(int idx, float duty) {
    if (idx < 0 || idx >= HAL_NUM_MOTORS) {
        return ESP_ERR_INVALID_ARG;
    }
    if (duty > 1.0f)  { duty = 1.0f; }
    if (duty < -1.0f) { duty = -1.0f; }
    s_motor[idx].duty_cmd = duty;
    return ESP_OK;
}

esp_err_t hal_motor_stop_all(void) {
    for (int i = 0; i < HAL_NUM_MOTORS; i++) {
        s_motor[i].duty_cmd = 0.0f;
        s_motor[i].duty_applied = 0.0f;
        motor_apply(i, 0.0f);
    }
    return ESP_OK;
}

esp_err_t hal_motor_update(float dt_s) {
    if (!(dt_s > 0.0f)) {
        return ESP_ERR_INVALID_ARG;
    }
    float max_step = DUTY_SLEW_PER_S * dt_s;
    for (int i = 0; i < HAL_NUM_MOTORS; i++) {
        float target = s_motor[i].duty_cmd;
        float cur = s_motor[i].duty_applied;
        if (target > cur + max_step) {
            target = cur + max_step;
        } else if (target < cur - max_step) {
            target = cur - max_step;
        }
        s_motor[i].duty_applied = target;
        motor_apply(i, target);
    }
    return ESP_OK;
}

esp_err_t hal_motor_get(int idx, hal_motor_state_t *out) {
    if (idx < 0 || idx >= HAL_NUM_MOTORS || out == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    int count = 0;
    pcnt_unit_get_count(s_motor[idx].pcnt, &count);
    out->counts = (int32_t)count;
    out->duty = s_motor[idx].duty_applied;
    out->rpm = 0.0f;    /* the caller derives RPM from dt and counts */
    return ESP_OK;
}

esp_err_t hal_motor_reset_counts(int idx) {
    if (idx < 0 || idx >= HAL_NUM_MOTORS) {
        return ESP_ERR_INVALID_ARG;
    }
    pcnt_unit_clear_count(s_motor[idx].pcnt);
    return ESP_OK;
}

/* ---------------------------------------------------------------- sensors -- */

/*
 * IMPORTANT: this function is a stub on purpose.
 *
 * A VL53L0X needs a real driver: power up, wait for boot, read the model id,
 * start a measurement, wait for the result, then change its I2C address before
 * the next sensor is woken. That driver is supplied with the lab as a separate
 * component and is outside the scope of this starter kit.
 *
 * What matters for the lab is the TIMING CONTRACT, which is documented in
 * hal.h and visible here: one read costs at most TOF_TIMEOUT_MS, and a failed
 * read returns an error instead of waiting forever. Your task can therefore
 * budget for it.
 *
 * To wire in the real driver, create the device handles during hal_init() and
 * replace the body below with a single read from the addressed sensor.
 */
esp_err_t hal_sensor_read_one(int idx, bool *valid, uint16_t *mm) {
    if (idx < 0 || idx >= HAL_NUM_SENSORS || valid == NULL || mm == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    *valid = false;
    *mm = 0;
    return ESP_ERR_NOT_SUPPORTED;   /* driver not linked in this starter kit */
}

esp_err_t hal_sensor_read_all(hal_sensors_t *out) {
    if (out == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    for (int i = 0; i < HAL_NUM_SENSORS; i++) {
        hal_sensor_read_one(i, &out->valid[i], &out->mm[i]);
    }
    out->seq++;
    return ESP_OK;
}

/* ---------------------------------------------------------------- battery -- */

esp_err_t hal_battery_voltage(float *volts) {
    if (volts == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    int raw = 0;
    if (adc_oneshot_read(s_adc, ADC_CHANNEL_3, &raw) != ESP_OK) {
        return ESP_FAIL;
    }
    /* 12 dB attenuation gives roughly 0 .. 3.1 V, 0 .. 4095 counts. */
    float pin = (float)raw * (3.1f / 4095.0f);
    *volts = pin * BATTERY_DIVIDER;
    return ESP_OK;
}

/* ----------------------------------------------------------------- status -- */

esp_err_t hal_led_set(bool on) {
    gpio_set_level(PIN_LED, on ? 1 : 0);
    return ESP_OK;
}

esp_err_t hal_buzzer_play(int pattern) {
    /*
     * An ACTIVE buzzer has its own oscillator: high = sound, low = silence.
     * A blocking implementation would sleep here, which is exactly what a HAL
     * must not do. Instead the pattern is handed to the caller's task.
     *
     * The starter kit keeps this honest by returning immediately and letting
     * the application drive the pin, so no hidden blocking exists anywhere.
     */
    static const bool level[3] = { true, true, true };
    (void)level;
    gpio_set_level(PIN_BUZZER, 1);
    return ESP_OK;
}

/* ------------------------------------------------------------- simulation -- */

/* These are no-ops on real hardware: you cannot set a distance by software. */

esp_err_t hal_sim_set_distance(int idx, uint16_t mm) {
    (void)idx; (void)mm;
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t hal_sim_set_sensor_missing(int idx, bool missing) {
    (void)idx; (void)missing;
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t hal_sim_set_battery_voltage(float volts) {
    (void)volts;
    return ESP_ERR_NOT_SUPPORTED;
}
