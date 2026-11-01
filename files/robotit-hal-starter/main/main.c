/*
 * example application - the skeleton your project starts from.
 *
 * Three tasks, three numbers each:
 *   sensor_task    core 1, priority 10, 10 ms   reads the HAL, publishes data
 *   control_task   core 1, priority 10, 10 ms   consumes data, commands motors
 *   monitor_task   core 0, priority  4, 250 ms  watches heartbeats, reports
 *
 * Shared data is one mutex protected struct with a sequence number, and one
 * overwrite queue for commands. Nothing here blocks the control loop.
 *
 * Build with HAL_BACKEND = "sim" to run with no robot attached.
 */

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "esp_timer.h"

#include "hal.h"
#include "hal_selftest.h"

/* ------------------------------------------------------------------ config -- */

#define SENSOR_STACK   4096
#define CONTROL_STACK  4096
#define MONITOR_STACK  3072

#define SENSOR_PRIO    10
#define CONTROL_PRIO   10
#define MONITOR_PRIO   4

#define CONTROL_PERIOD_MS  10
#define SENSOR_PERIOD_MS   10
#define MONITOR_PERIOD_MS  250

#define CORE_REALTIME  1
#define CORE_HOUSEKEEPING 0

/* Data older than this is treated as missing. */
#define FRESH_LIMIT_MS 100

/* Stop when the wall is closer than this. */
#define STOP_DISTANCE_MM 200

/* ------------------------------------------------------------------- data -- */

typedef struct {
    uint32_t seq;
    int64_t  timestamp_ms;
    bool     distance_valid;
    uint16_t distance_mm;
    float    left_rpm;
    float    right_rpm;
} sensor_snapshot_t;

typedef struct {
    float speed;    /* -1 .. 1 */
    float steer;    /* -1 .. 1 */
} drive_cmd_t;

static SemaphoreHandle_t s_sensor_mutex;
static QueueHandle_t     s_cmd_queue;      /* length 1, overwrite */

static sensor_snapshot_t s_snapshot;

/* Heartbeats: incremented by each task, watched by the monitor. */
static volatile uint32_t s_hb_sensor;
static volatile uint32_t s_hb_control;

/* Measured worst case, printed by the monitor. */
static volatile int64_t s_worst_period_us;
static volatile int64_t s_worst_exec_us;

/* --------------------------------------------------------------- helpers ---- */

static int64_t now_ms(void) {
    return esp_timer_get_time() / 1000;
}

/* ----------------------------------------------------------------- sensor --- */

static void sensor_task(void *arg) {
    (void)arg;
    TickType_t last_wake = xTaskGetTickCount();
    int32_t  prev_counts[2] = { 0, 0 };
    uint32_t seq = 0;

    for (;;) {
        int64_t t0 = esp_timer_get_time();

        float dt_s = CONTROL_PERIOD_MS / 1000.0f;
        hal_motor_update(dt_s);      /* refresh the measurement / model */

        hal_motor_state_t l, r;
        hal_motor_get(HAL_MOTOR_LEFT, &l);
        hal_motor_get(HAL_MOTOR_RIGHT, &r);

        /* Counts to RPM, using the real elapsed distance travelled. */
        float l_delta = (float)(l.counts - prev_counts[0]);
        float r_delta = (float)(r.counts - prev_counts[1]);
        prev_counts[0] = l.counts;
        prev_counts[1] = r.counts;

        bool dist_valid = false;
        uint16_t dist_mm = 0;
        hal_sensor_read_one(1, &dist_valid, &dist_mm);   /* front sensor */

        sensor_snapshot_t snap = {
            .seq = ++seq,
            .timestamp_ms = now_ms(),
            .distance_valid = dist_valid,
            .distance_mm = dist_mm,
            .left_rpm = l_delta * 60.0f / (4200.0f * dt_s),
            .right_rpm = r_delta * 60.0f / (4200.0f * dt_s),
        };

        /* One mutex, whole struct, one copy. */
        if (xSemaphoreTake(s_sensor_mutex, pdMS_TO_TICKS(5)) == pdTRUE) {
            s_snapshot = snap;
            xSemaphoreGive(s_sensor_mutex);
        }

        s_hb_sensor++;

        int64_t exec = esp_timer_get_time() - t0;
        if (exec > s_worst_exec_us) {
            s_worst_exec_us = exec;
        }

        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(SENSOR_PERIOD_MS));
    }
}

/* ---------------------------------------------------------------- control --- */

static void control_task(void *arg) {
    (void)arg;
    TickType_t last_wake = xTaskGetTickCount();
    uint32_t last_seq = 0;
    drive_cmd_t cmd = { .speed = 0.35f, .steer = 0.0f };

    for (;;) {
        int64_t t_start = esp_timer_get_time();

        /* Take the newest command, if any. Zero timeout: never block here. */
        drive_cmd_t incoming;
        while (xQueueReceive(s_cmd_queue, &incoming, 0) == pdTRUE) {
            cmd = incoming;
        }

        sensor_snapshot_t snap;
        bool have_fresh = false;
        if (xSemaphoreTake(s_sensor_mutex, pdMS_TO_TICKS(5)) == pdTRUE) {
            snap = s_snapshot;
            xSemaphoreGive(s_sensor_mutex);
            have_fresh = (snap.seq != last_seq) &&
                         (now_ms() - snap.timestamp_ms < FRESH_LIMIT_MS);
            last_seq = snap.seq;
        }

        if (!have_fresh) {
            /* Stale or missing data: safe state, do not guess. */
            hal_motor_stop_all();
        } else {
            float speed = cmd.speed;

            /* Simple obstacle guard: scale forward speed by the distance. */
            if (snap.distance_valid) {
                if (snap.distance_mm < STOP_DISTANCE_MM) {
                    speed = 0.0f;
                } else if (snap.distance_mm < STOP_DISTANCE_MM * 3) {
                    speed *= (float)(snap.distance_mm - STOP_DISTANCE_MM) /
                             (float)(STOP_DISTANCE_MM * 2);
                }
            }

            /* speed + steer -> two wheel duties. */
            float left  = speed + cmd.steer;
            float right = speed - cmd.steer;
            hal_motor_set_duty(HAL_MOTOR_LEFT, left);
            hal_motor_set_duty(HAL_MOTOR_RIGHT, right);
        }

        s_hb_control++;

        int64_t exec = esp_timer_get_time() - t_start;
        if (exec > s_worst_exec_us) {
            s_worst_exec_us = exec;
        }

        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(CONTROL_PERIOD_MS));

        /* Measure the real period. vTaskDelayUntil keeps it stable; this proves it. */
        int64_t period = esp_timer_get_time() - t_start;
        if (period > s_worst_period_us) {
            s_worst_period_us = period;
        }
    }
}

/* ---------------------------------------------------------------- monitor --- */

static void monitor_task(void *arg) {
    (void)arg;
    TickType_t last_wake = xTaskGetTickCount();
    uint32_t prev_sensor = 0, prev_control = 0;
    float volts = 0.0f;

    for (;;) {
        uint32_t hs = s_hb_sensor;
        uint32_t hc = s_hb_control;

        bool sensor_alive  = (hs != prev_sensor);
        bool control_alive = (hc != prev_control);
        prev_sensor = hs;
        prev_control = hc;

        hal_battery_voltage(&volts);

        printf("mon | hb s=%lu c=%lu | %s%s| worst period %lld us | worst exec %lld us | batt %.2f V | stack free %u\n",
               (unsigned long)hs, (unsigned long)hc,
               sensor_alive ? "" : "SENSOR DEAD ",
               control_alive ? "" : "CONTROL DEAD ",
               (long long)s_worst_period_us,
               (long long)s_worst_exec_us,
               (double)volts,
               (unsigned)uxTaskGetStackHighWaterMark(NULL));

        /* Heartbeat LED: fast blink while everything is alive. */
        hal_led_set(true);

        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(MONITOR_PERIOD_MS));
        hal_led_set(false);
    }
}

/* ------------------------------------------------------------------- main --- */

void app_main(void) {
    printf("\nexample: backend = %s\n", hal_backend_name());

    if (hal_init() != ESP_OK) {
        printf("example: hal_init FAILED - nothing will work\n");
        return;
    }

    /* Run the self test once, so a dead part is obvious immediately. */
    hal_selftest();

    s_sensor_mutex = xSemaphoreCreateMutex();
    s_cmd_queue = xQueueCreate(1, sizeof(drive_cmd_t));
    if (s_sensor_mutex == NULL || s_cmd_queue == NULL) {
        printf("example: out of memory creating sync objects\n");
        return;
    }
    memset(&s_snapshot, 0, sizeof(s_snapshot));

    xTaskCreatePinnedToCore(sensor_task, "sensor", SENSOR_STACK, NULL,
                            SENSOR_PRIO, NULL, CORE_REALTIME);
    xTaskCreatePinnedToCore(control_task, "control", CONTROL_STACK, NULL,
                            CONTROL_PRIO, NULL, CORE_REALTIME);
    xTaskCreatePinnedToCore(monitor_task, "monitor", MONITOR_STACK, NULL,
                            MONITOR_PRIO, NULL, CORE_HOUSEKEEPING);

    printf("example: tasks started\n");
}
