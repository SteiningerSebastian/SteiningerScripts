# RobotIT HAL Starter Kit

A small, readable **hardware abstraction layer** for the ESP32-S3 robot, plus a
**simulated backend** so the same application code runs with no robot attached.

Everything here is written for a lab about real time operating systems. It is
deliberately simpler than a production driver: fewer features, more comments, and one
clear promise per function.

## The promise of this kit

> Every function returns an `esp_err_t` and **never blocks longer than its documented
> worst case**. Nothing here decides what the robot should do. That is your job.

## Layout

```
CMakeLists.txt             top level project
hal/
  include/hal.h            one header, all parts, all handles
  include/hal_selftest.h
  src/hal_real.c           real backend: pins, PWM, pulse counter, ADC
  src/hal_sim.c            simulated backend: plausible fake behaviour
  src/hal_selftest.c       prints one line per part
  CMakeLists.txt
main/
  main.c                   example application: three tasks, one mutex, one queue
  CMakeLists.txt
README.md
```

Only **one** backend is compiled, chosen by `HAL_BACKEND`:

```cmake
set(HAL_BACKEND "sim")    # or "real"
```

## What is modelled

| Part | Real backend | Simulated backend |
|---|---|---|
| Two DC motors with encoders | PWM on the H-bridge, pulse counter on the encoder pins | First order speed response, counts accumulate |
| Distance sensors | I2C read (see the note below) | Wall at a configurable distance, with noise and dropouts |
| Battery voltage | ADC with a 2 to 1 divider | Slowly draining voltage |
| Status LED | GPIO output | Printed to the console |
| Buzzer | GPIO output | Printed to the console |
| Boost converter | GPIO wake pin | Flag only |

### A note on the distance sensors

`hal_sensor_read_one()` in the **real** backend is a stub that returns
`ESP_ERR_NOT_SUPPORTED`. A VL53L0X needs a real driver: boot, model check, measurement,
wait, then an I2C address change before the next sensor is woken. That driver is a
separate component and out of scope for this kit.

What matters for the lab is preserved: the **timing contract** in `hal.h` says one read
costs at most about 15 ms and returns an error instead of waiting forever. Your task can
budget for that number. The **simulated** backend implements the read fully, which is
where you will develop and test.

## Why the simulated backend matters

When no robot is free, the simulated backend lets you build and test the **real time
part** of your project. Your tasks, your priorities, your mutexes, your queues and your
measurements all run unchanged, because they only ever talk to `hal.h`.

The rule is simple: **if your code compiles against `hal.h` and never touches a pin
directly, you can develop without hardware.**

## Building

```bash
idf.py set-target esp32s3
idf.py build
idf.py -p <YOUR_PORT> flash monitor
```

`<YOUR_PORT>` is whatever port your board shows up as: `COM5`, `COM7`, `COM12`,
`/dev/ttyUSB0`, `/dev/cu.usbserial-0001`. **It is not always COM5.** Check Device
Manager under Ports, or run `idf.py -p detect`.

## Running without a robot

Set `HAL_BACKEND` to `sim` and build. The console shows plausible sensor values, the
encoders turn when you command the motors, and the LED and buzzer calls are printed
instead of driven. There is no hardware to break, so this is also the safest place to
test a control loop for the first time.

On the simulated backend you can also provoke the situations your code must survive:

```c
hal_sim_set_distance(1, 80);         /* a wall 8 cm in front of the front sensor */
hal_sim_set_sensor_missing(2, true); /* the right sensor stops answering */
hal_sim_set_battery_voltage(3.2f);   /* an almost empty cell */
```

These calls return `ESP_ERR_NOT_SUPPORTED` on real hardware, where you cannot set a
distance by software.

## The example application

`main/main.c` is a complete skeleton you can copy:

- **Two tasks on core 1** at priority 10 with a 10 ms period: `sensor` reads the HAL and
  publishes a snapshot, `control` consumes it and commands the motors.
- **One task on core 0** at priority 4 every 250 ms: `monitor` watches heartbeats,
  prints the worst measured period and execution time, and blinks the LED.
- **One mutex** protecting a struct with a **sequence number and timestamp**.
- **One overwrite queue** of length 1 for drive commands.
- A **safe state**: stale data means the motors are stopped, not driven blindly.
- A **period measurement** with `vTaskDelayUntil` plus a recorded worst case.

## What you are expected to change

- Your application lives in `main/`. The `hal/` folder is a library you link against.
- Read `hal_real.c` to understand what a call costs in time. You do not have to modify it.
- If you need a part that is not modelled, add it to `hal.h` **and both** backends. That
  is the whole point of the design.
- Replace the example control law with your own. Keep the task table honest.

## Safety

The real backend clamps the motor duty to 80 percent and rate limits it, so a bug in your
code cannot ask the battery for more current than it can deliver. It does **not** protect
you from a robot that drives off the table. Keep the robot lifted on the first run of new
motor code.

Never probe the battery or charger circuit with a multimeter while it is connected and
running. Measure the power system only in the way the lab instructs, or not at all.
