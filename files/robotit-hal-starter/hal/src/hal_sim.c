/*
 * hal_sim.c - simulated backend.
 *
 * Lets your real time application run with no robot attached. The numbers are
 * plausible rather than physically exact: a first order motor response, a wall
 * with noise and dropouts, a battery that drains slowly.
 *
 * Everything here is safe: there is no hardware to damage.
 */

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "esp_timer.h"

#include "hal.h"

/* ---- pin map mirror (kept identical to the real backend for reference) ---- */
#define PIN_NOT_SLEEP 21

/* ---- model tuning -------------------------------------------------------- */
#define MOTOR_TAU_S        0.08f  /* first order time constant, seconds */
#define MOTOR_MAX_RPM      180.0f /* speed at full duty */
#define MOTOR_DEADBAND     0.12f  /* below this duty the motor does not move */
#define COUNTS_PER_REV     4200.0f

#define SENSOR_NOISE_MM    8.0f
#define SENSOR_DROPOUT_PCT 4     /* roughly 4 % of reads are invalid */
#define SENSOR_MIN_MM      20
#define SENSOR_MAX_MM      2000

#define BATTERY_START_V    4.05f
#define BATTERY_EMPTY_V    3.30f

typedef struct {
    float   duty;
    float   rpm;
    float   counts;
    bool    initialised;
} sim_motor_t;

static sim_motor_t   s_motor[HAL_NUM_MOTORS];
static uint16_t      s_wall_mm[HAL_NUM_SENSORS];
static bool          s_sensor_missing[HAL_NUM_SENSORS];
static float         s_battery_v = BATTERY_START_V;
static int64_t       s_last_update_us;
static uint32_t      s_sensor_seq;
static bool          s_led_on;
static uint32_t      s_rng = 0x12345678u;

/* Small deterministic pseudo random generator: reproducible lab results. */
static uint32_t rng_next(void) {
    s_rng = s_rng * 1664525u + 1013904223u;
    return s_rng;
}

esp_err_t hal_init(void) {
    for (int i = 0; i < HAL_NUM_MOTORS; i++) {
        s_motor[i].duty = 0.0f;
        s_motor[i].rpm = 0.0f;
        s_motor[i].counts = 0.0f;
        s_motor[i].initialised = true;
    }
    for (int i = 0; i < HAL_NUM_SENSORS; i++) {
        s_wall_mm[i] = 800;          /* a wall about 80 cm away */
        s_sensor_missing[i] = false;
    }
    s_battery_v = BATTERY_START_V;
    s_last_update_us = esp_timer_get_time();
    s_sensor_seq = 0;
    printf("hal: simulated backend ready (no hardware in use)\n");
    return ESP_OK;
}

const char *hal_backend_name(void) {
    return "sim";
}

/* ----------------------------------------------------------------- motors -- */

esp_err_t hal_motor_set_duty(int idx, float duty) {
    if (idx < 0 || idx >= HAL_NUM_MOTORS) {
        return ESP_ERR_INVALID_ARG;
    }
    if (duty > 1.0f) { duty = 1.0f; }
    if (duty < -1.0f) { duty = -1.0f; }
    s_motor[idx].duty = duty;
    return ESP_OK;
}

esp_err_t hal_motor_stop_all(void) {
    for (int i = 0; i < HAL_NUM_MOTORS; i++) {
        s_motor[i].duty = 0.0f;
    }
    return ESP_OK;
}

esp_err_t hal_motor_update(float dt_s) {
    if (!(dt_s > 0.0f)) {
        return ESP_ERR_INVALID_ARG;
    }
    for (int i = 0; i < HAL_NUM_MOTORS; i++) {
        /* Deadband: a small duty does not overcome static friction. */
        float effective = 0.0f;
        float d = s_motor[i].duty;
        if (fabsf(d) > MOTOR_DEADBAND) {
            effective = d > 0.0f ? (d - MOTOR_DEADBAND) / (1.0f - MOTOR_DEADBAND)
                                 : (d + MOTOR_DEADBAND) / (1.0f - MOTOR_DEADBAND);
        }
        float target = effective * MOTOR_MAX_RPM;

        /* First order approach toward the target speed. */
        float alpha = 1.0f - expf(-dt_s / MOTOR_TAU_S);
        s_motor[i].rpm += (target - s_motor[i].rpm) * alpha;

        /* Integrate the encoder. */
        s_motor[i].counts += (s_motor[i].rpm / 60.0f) * dt_s * COUNTS_PER_REV;
    }
    return ESP_OK;
}

esp_err_t hal_motor_get(int idx, hal_motor_state_t *out) {
    if (idx < 0 || idx >= HAL_NUM_MOTORS || out == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    out->duty = s_motor[idx].duty;
    out->rpm = s_motor[idx].rpm;
    out->counts = (int32_t)s_motor[idx].counts;
    return ESP_OK;
}

esp_err_t hal_motor_reset_counts(int idx) {
    if (idx < 0 || idx >= HAL_NUM_MOTORS) {
        return ESP_ERR_INVALID_ARG;
    }
    s_motor[idx].counts = 0.0f;
    return ESP_OK;
}

/* ---------------------------------------------------------------- sensors -- */

esp_err_t hal_sensor_read_one(int idx, bool *valid, uint16_t *mm) {
    if (idx < 0 || idx >= HAL_NUM_SENSORS || valid == NULL || mm == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (s_sensor_missing[idx]) {
        *valid = false;
        *mm = 0;
        return ESP_OK;   /* a missing sensor is not an error, it is data */
    }

    /* Dropouts and noise, so your validity handling gets exercised. */
    if ((int)(rng_next() % 100) < SENSOR_DROPOUT_PCT) {
        *valid = false;
        *mm = 0;
        return ESP_OK;
    }

    float noise = ((float)(rng_next() % 2001) - 1000.0f) / 1000.0f; /* -1 .. 1 */
    float v = (float)s_wall_mm[idx] + noise * SENSOR_NOISE_MM;

    if (v < SENSOR_MIN_MM) { v = SENSOR_MIN_MM; }
    if (v > SENSOR_MAX_MM) {
        *valid = false;    /* out of range reads as no target */
        *mm = 0;
        return ESP_OK;
    }

    *mm = (uint16_t)v;
    *valid = true;
    return ESP_OK;
}

esp_err_t hal_sensor_read_all(hal_sensors_t *out) {
    if (out == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    bool any_valid = false;
    for (int i = 0; i < HAL_NUM_SENSORS; i++) {
        bool v = false;
        uint16_t d = 0;
        hal_sensor_read_one(i, &v, &d);
        out->valid[i] = v;
        out->mm[i] = d;
        any_valid = any_valid || v;
    }
    if (any_valid) {
        out->seq = ++s_sensor_seq;
    } else {
        out->seq = s_sensor_seq;   /* unchanged: the consumer can detect this */
    }
    return ESP_OK;
}

/* ---------------------------------------------------------------- battery -- */

esp_err_t hal_battery_voltage(float *volts) {
    if (volts == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    /* Slow drain. */
    s_battery_v -= 0.00002f;
    if (s_battery_v < BATTERY_EMPTY_V) {
        s_battery_v = BATTERY_EMPTY_V;
    }
    *volts = s_battery_v;
    return ESP_OK;
}

/* ----------------------------------------------------------------- status -- */

esp_err_t hal_led_set(bool on) {
    if (on != s_led_on) {
        s_led_on = on;
        /* Logged rather than driven, so you can see the pattern in the console. */
        printf("hal: led %s\n", on ? "on" : "off");
    }
    return ESP_OK;
}

esp_err_t hal_buzzer_play(int pattern) {
    printf("hal: buzzer pattern %d\n", pattern);
    return ESP_OK;
}

/* ------------------------------------------------------------- simulation -- */

esp_err_t hal_sim_set_distance(int idx, uint16_t mm) {
    if (idx < 0 || idx >= HAL_NUM_SENSORS) {
        return ESP_ERR_INVALID_ARG;
    }
    s_wall_mm[idx] = mm;
    return ESP_OK;
}

esp_err_t hal_sim_set_sensor_missing(int idx, bool missing) {
    if (idx < 0 || idx >= HAL_NUM_SENSORS) {
        return ESP_ERR_INVALID_ARG;
    }
    s_sensor_missing[idx] = missing;
    return ESP_OK;
}

esp_err_t hal_sim_set_battery_voltage(float volts) {
    s_battery_v = volts;
    return ESP_OK;
}
