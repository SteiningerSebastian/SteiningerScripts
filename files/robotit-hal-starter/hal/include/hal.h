#pragma once

/*
 * hal.h - the one header your application includes.
 *
 * Every function:
 *   - returns esp_err_t (ESP_OK on success),
 *   - never blocks longer than its documented worst case,
 *   - never decides anything (no braking, no logging, no retry loops).
 *
 * Build with HAL_BACKEND = "real" (default) or "sim".
 */

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------- config -- */

#define HAL_NUM_MOTORS   2
#define HAL_NUM_SENSORS  3

/* Motor index. */
#define HAL_MOTOR_LEFT   0
#define HAL_MOTOR_RIGHT  1

/* ---------------------------------------------------------------- types --- */

typedef struct {
    float duty;          /*!< -1.0 .. 1.0, clamped by the backend */
    float rpm;           /*!< measured output-shaft speed */
    int32_t counts;      /*!< accumulated encoder counts */
} hal_motor_state_t;

typedef struct {
    bool     valid[HAL_NUM_SENSORS];      /*!< false when the reading is not trustworthy */
    uint16_t mm[HAL_NUM_SENSORS];         /*!< distance in millimetres, 0 when invalid */
    uint32_t seq;                          /*!< increments on every successful read */
} hal_sensors_t;

/* ---------------------------------------------------------------- system --- */

/**
 * @brief Bring up every part. Call once from app_main.
 *
 * Worst case: about 60 ms (sensor array addressing sequence).
 */
esp_err_t hal_init(void);

/** @brief Human readable name of the backend ("real" or "sim"). */
const char *hal_backend_name(void);

/* ---------------------------------------------------------------- motors --- */

/**
 * @brief Command a motor duty.
 * @param idx  HAL_MOTOR_LEFT or HAL_MOTOR_RIGHT
 * @param duty -1.0 .. 1.0 (sign is direction, magnitude is power)
 *
 * Rate limited internally. Worst case: immediate, no blocking.
 */
esp_err_t hal_motor_set_duty(int idx, float duty);

/** @brief Stop both motors (duty 0, both H-bridge inputs low). Never blocks. */
esp_err_t hal_motor_stop_all(void);

/** @brief Read the current motor state. Worst case: immediate. */
esp_err_t hal_motor_get(int idx, hal_motor_state_t *out);

/**
 * @brief Advance the motor model / refresh the measurement.
 *
 * On real hardware this just reads the hardware counters.
 * On the simulated backend this integrates the model, so it must be called
 * regularly (once per control tick) to produce sensible numbers.
 *
 * Worst case: immediate.
 */
esp_err_t hal_motor_update(float dt_s);

/** @brief Reset an encoder to zero. Worst case: immediate. */
esp_err_t hal_motor_reset_counts(int idx);

/* --------------------------------------------------------------- sensors --- */

/**
 * @brief Read all distance sensors.
 *
 * Worst case on real hardware: 3 x read timeout, about 45 ms with the default
 * 15 ms timeout. Call it from a task that can afford that, or read one sensor
 * per tick with hal_sensor_read_one().
 */
esp_err_t hal_sensor_read_all(hal_sensors_t *out);

/**
 * @brief Read one sensor (round robin helper).
 * @param idx 0 .. HAL_NUM_SENSORS-1
 *
 * Worst case: one read timeout, about 15 ms.
 */
esp_err_t hal_sensor_read_one(int idx, bool *valid, uint16_t *mm);

/* --------------------------------------------------------------- battery --- */

/** @brief Battery voltage in volts. Worst case: about 1 ms. */
esp_err_t hal_battery_voltage(float *volts);

/* ----------------------------------------------------------------- status -- */

/** @brief Set the status LED. Never blocks. */
esp_err_t hal_led_set(bool on);

/**
 * @brief Play a short pattern on the buzzer.
 * @param pattern 0 = short beep, 1 = double beep, 2 = long beep
 *
 * Queues the sound and returns. Worst case: immediate.
 */
esp_err_t hal_buzzer_play(int pattern);

/* ------------------------------------------------------------- simulation -- */

/*
 * The simulated backend exposes a few controls so you can provoke situations
 * your code must survive. These are no-ops on real hardware.
 */

/** @brief Place a wall in front of a sensor (sim only). */
esp_err_t hal_sim_set_distance(int idx, uint16_t mm);

/** @brief Make a sensor report nothing (sim only). */
esp_err_t hal_sim_set_sensor_missing(int idx, bool missing);

/** @brief Make the battery drain faster (sim only). */
esp_err_t hal_sim_set_battery_voltage(float volts);

#ifdef __cplusplus
}
#endif
