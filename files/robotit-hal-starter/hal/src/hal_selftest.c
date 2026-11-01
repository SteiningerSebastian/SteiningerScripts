/*
 * hal_selftest.c - a small self test you can run before you trust the board.
 *
 * Prints one line per part. On the simulated backend this also passes, which is
 * the point: the same test works with and without hardware.
 */

#include <stdio.h>

#include "hal.h"

esp_err_t hal_selftest(void) {
    int failures = 0;

    printf("\n--- HAL self test (backend: %s) ---\n", hal_backend_name());

    /* Boost converter and LED are not readable; we only prove the calls work. */
    if (hal_led_set(true) != ESP_OK) {
        printf("led        FAIL\n"); failures++;
    } else {
        printf("led        ok\n");
    }
    hal_led_set(false);

    /* Motors: command a small duty, advance the model, expect movement. */
    for (int i = 0; i < HAL_NUM_MOTORS; i++) {
        hal_motor_state_t before = {0}, after = {0};
        hal_motor_get(i, &before);
        hal_motor_set_duty(i, 0.4f);
        for (int k = 0; k < 20; k++) {
            hal_motor_update(0.01f);
        }
        hal_motor_get(i, &after);
        if (after.counts == before.counts) {
            printf("motor %d    FAIL (no movement at 0.4 duty)\n", i);
            failures++;
        } else {
            printf("motor %d    ok (counts %ld -> %ld)\n", i,
                   (long)before.counts, (long)after.counts);
        }
        hal_motor_set_duty(i, 0.0f);
    }
    hal_motor_stop_all();

    /* Sensors: read once and report, but do not fail on a missing sensor. */
    hal_sensors_t s;
    if (hal_sensor_read_all(&s) != ESP_OK) {
        printf("sensors    FAIL (read returned an error)\n");
        failures++;
    } else {
        for (int i = 0; i < HAL_NUM_SENSORS; i++) {
            printf("sensor %d   %s (%u mm)\n", i,
                   s.valid[i] ? "ok" : "no reading", (unsigned)s.mm[i]);
        }
    }

    /* Battery. */
    float v = 0.0f;
    if (hal_battery_voltage(&v) != ESP_OK) {
        printf("battery    FAIL (read returned an error)\n");
        failures++;
    } else {
        printf("battery    ok (%.2f V)\n", (double)v);
    }

    printf("--- %d failure(s) ---\n\n", failures);
    return failures == 0 ? ESP_OK : ESP_FAIL;
}
