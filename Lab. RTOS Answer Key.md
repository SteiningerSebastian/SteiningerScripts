Instructor copy. This key exists so that every group gets the same explanation, not so that answers can be handed out before a meeting.

# How to Use This Key

The lab script is written as a sequence of **prompts**, not as a list of questions. Five kinds appear, and each needs a different treatment in the room.

- **Check your understanding:** an oral question, answered in the moment. Expect thirty seconds of thinking and one sentence of answer. Wrong answers here are the most useful teaching material in the lab.
- **Try it yourself:** a hands on experiment. It needs a working toolchain or the simulator, takes between five and twenty five minutes, and must actually be run. A group that only reads it learns nothing.
- **Predict before you run:** the answer is written down **first**, then the experiment runs. The gap between prediction and result is the lesson.
- **Quiz round:** opens a meeting, covers the previous chapter, and is scored by speed.
- **Checkpoint:** a gate. A group does not move to the next chapter until it is shown. Nothing is graded, everything is verified.

Everything in this document is organised the same way: the prompt is quoted in a plain heading, then the expected answer, then the teacher information for that item.

- **Answers are model answers, not the only answers:** a student answer that is different but defensible is a pass. Ask them to justify it, then move on.
- **Teacher info is written for a three hour meeting:** the time estimate assumes one instructor circulating between six to eight groups.
- **The numbers in this key come from the reference board and the reference HAL:** if a group measures something different, the measurement wins over the key.

## Assessment Quick Reference

| Checkpoint | Chapter | Pass condition | Evidence |
|---|---|---|---|
| Toolchain | Toolchain, Build and First Flash | A program runs and prints its backend name and at least one task line | Serial log or screenshot |
| Timing | Tasks, Scheduling and Priorities | Four measured numbers: worst period and worst execution time, both delay functions | Printed log lines |
| Data | Sharing Data Between Tasks | Torn read counter at zero with the mutex and climbing without it | Two runs, same test |
| Health | Diagnostics and Fault Handling | The system reports its own health without a debugger | LED codes on hardware or printed heartbeats in the simulator |
| Project simulator | Your Project, Design in the Simulator | Three tasks, one mutex struct with a freshness check, one queue, live jitter numbers | Running session |
| Project hardware and review | Your Project, Hardware, Demo and Review | Three minute demo, one provoked failure, the numbers defended | Demo and short report |

## Lab Level Teaching Notes

- **First meeting, first five minutes:** start the ESP-IDF download before anything else is said. It is several hundred megabytes and it dominates the meeting. Talk about the three worlds while it installs.
- **Assume the first meeting is lost to installation:** plan it that way and nothing goes wrong. Having three prepared USB cables and one known good board saves the evening.
- **Do not hand out the whole answer key:** hand out one chapter after that chapter has been discussed, or keep it instructor only and use the model answers orally.
- **Assessment is formative:** the point of a checkpoint is to catch a wrong mental model in week two, not to produce a mark. Say that out loud, or groups will hide mistakes.
- **Keep the failure case first:** every experiment in this lab is more valuable when something breaks. If a group reports "everything worked", ask them to break it on purpose.
- **Safety supervision:** nobody runs new motor code on the table with the wheels down. Check this yourself, do not rely on the written rule. The rule about lifting the robot before the first run of new motor code is the one that prevents injury and destroyed boards.
- **Robot scarcity:** decide at the start of each meeting who has hardware and write it on the board. Simulator groups work in parallel, so nobody waits.

# Introduction

## Teaching Notes for the Introduction

- **Time:** twenty minutes of the first meeting, mostly while the installer runs.
- **Goal:** the sentence "a real time system is not a fast system, it is a system that can promise something about time" must survive the meeting. Everything later is a consequence of it.
- **Watch for:** students who equate real time with fast. Ask them for the deadline of a mouse and then for the deadline of a dashboard. The contrast does the work.
- **The superloop counter example is the hook:** one blocking I2C read and the motors keep their last command. Groups that have already built a line follower will recognise this immediately.

## Try it yourself - deadlines around you

- **Mouse, about one millisecond:** a gaming mouse reports every 1 ms, an office mouse every 8 ms. A late report is a stuttering cursor. The consequence is annoyance, not damage.
- **Drone, about two and a half milliseconds:** an attitude loop at 400 Hz. A missed deadline means the controller is late to a disturbance, which produces oscillation, and oscillation in the air is a crash.
- **ABS, about five to ten milliseconds:** wheel speed and slip are sampled fast enough to release the brake before the wheel locks. A missed deadline means the wheel locks, braking distance grows and steering is lost.
- **Teacher info, three minutes per group:** the exact numbers do not matter, the **ratio between deadline and consequence** does. Award the point when the student states a deadline and then names what a miss physically does. Students often give the ABS loop as "as fast as possible", which is the wrong mental model, so ask what happens at 100 ms.
- **Extension if a group is fast:** ask for the deadline of the **brake light** in a car and for the deadline of the **infotainment screen**. Same vehicle, two orders of magnitude apart, which shows that real time is a per function property, not a system property.

## Teaching Notes for the Lab Structure and Assessment

- **The chapter based structure is deliberate:** a group that needs a second meeting for priorities loses nothing. Say this in the first meeting or the fast groups will rush the slow ones.
- **Quizzes:** open with the quiz before any new material. Three points for first correct, two for second, one for third. Keep it under ten minutes.
- **Final report:** the four required elements are the personal contribution, the built task table, the measured numbers and one failure found and fixed. Reject reports that only contain averages.
- **Project review question you will always ask:** why does this task have this priority. A wrong answer is acceptable if the student says it is a wrong answer and how they would test it. Bluffing is not.
- **Safety briefing must be repeated every meeting:** lift the robot, keep the power switch reachable, look at the robot and not the screen, never probe the power circuitry, and know how your program reaches a safe state.

# The Three Worlds - Bare Metal, RTOS and Linux

## Teaching Notes for the Three Worlds

- **Time:** one full meeting, this chapter plus the layering chapter. The three worlds part alone is about seventy minutes.
- **Goal:** students answer the question "can a late answer still be a correct answer?" automatically by the end.
- **The comparison table is the exam:** if a student can fill it from memory, the chapter landed.
- **Watch for:** the belief that an RTOS is a small Linux. Correct it early: no memory protection, no fairness, no files, no processes.
- **The Digi-Key video is fifteen minutes of the meeting:** play it, do not assign it as homework that nobody does.

## Check your understanding - which call to move out of the loop

- **send_status leaves the loop first:** it is the only call in the bare metal loop with an external dependency, so it is the one that can block for an unbounded time. A network send can take tens or hundreds of milliseconds and occasionally seconds.
- **Keeping the motor updated while it runs:** two bare metal answers are acceptable. Either make the send **non blocking** (state machine plus interrupt or DMA, so the call only starts a transfer and returns) or drive the motor from a **timer interrupt or PWM update triggered by hardware**, so the loop is not on the critical path at all.
- **The RTOS answer, accepted with a note:** in the next section the same thing is achieved by putting the send in a low priority task. Say that this is the real fix and that it is what the RTOS buys you.
- **Wrong but common:** "call delay less" or "make the loop faster". Both make the problem less frequent, not bounded. Ask what the worst case becomes, and the student usually finds the answer themselves.
- **Teacher info, two minutes:** insist on the word "bounded". A student who says "it is faster now" has not understood the chapter.
- **Watch for:** the answer "read the button first". The order of the loop is not the problem; the blocking call is.

## Check your understanding - two tasks with a ten millisecond delay

- **No, both do not run every ten milliseconds:** `vTaskDelay` waits **from the moment the task resumes**, so the true period is work time plus ten milliseconds. Twenty milliseconds of work gives a thirty millisecond period.
- **A second reason they can miss the rhythm:** both tasks are at the same priority, so on one core they take turns at the tick and each one's effective rate is halved if either of them has real work to do.
- **Conditions where one misses its rhythm:** work time grows close to or past the period, a higher priority task becomes ready often enough to consume the slack, a critical section or a blocking call inside the task takes longer than documented, or the same priority round robin favours the other task.
- **Teacher info, three minutes:** the exact answer you want is "a delay is not a period". Write the formula on the board: real period equals work plus delay. Then write the `vTaskDelayUntil` version next to it, because the fix is taught later and previewing it costs nothing.
- **Watch for:** "they run in parallel because there are two cores". Only true if they are pinned to different cores, and never true with only one task per core doing nothing else.

## Try it yourself - the HAL example as one bare metal loop

- **Acceptable loop order:** read sensors, compute control, set the motor duty, update odometry, then everything slow (status, printing, network) behind a counter so it runs rarely.
- **The blocking points:** the sensor read is the first, about fifteen milliseconds per sensor and up to forty five milliseconds for all three. The network or print is the second and it is unbounded.
- **What stops working while it blocks:** the motor keeps its last duty, so obstacle reactions are late by the length of the block. The controller also sees a stale `dt` and therefore a wrong speed.
- **A blocking call in position three stalls every step after it:** the student should discover that loop order alone does not save the design. The lesson is that a single sequence has no isolation.
- **Teacher info, ten minutes, paper only:** collect one drawing per group and ask one question: "show me where the motors are updated while the sensor times out". Groups that cannot answer have not got the superloop problem yet.

## Check your understanding - camera stream and a five millisecond stop

- **Two worlds, both used:** the camera stream belongs in the Linux world, a small computer or the ESP32 with buffering, because a late frame is still a correct frame. The motor stop belongs in bare metal or RTOS, because five milliseconds is a hard deadline.
- **The hard part is the boundary:** the stop decision must never depend on the stream. Whatever detects the obstacle must be wired to the actuator side of the system, not filtered through the network or the image pipeline.
- **A second hard part:** both sides share power and often a bus. A current spike from motors can brown out the computer, and a full disk or a page cache flush on the computer can stall the link.
- **The clean architecture:** a hard real time MCU owns the actuator and its own obstacle sensing, and the Linux side only sends high level intent. Losing the link must default to a safe state, not to "keep the last command".
- **Teacher info, four minutes:** this is the best question in the chapter because it forces the phrase "hard part". Reward the student who names the boundary rather than the board.
- **Watch for:** "use the Pi because it is faster". Ask what the jitter of a Python loop on a Pi is, and whether they would bet a five millisecond stop on it.

## Check your understanding - a 400 Hz drone control loop

- **The loop lives in bare metal or an RTOS, never in Linux:** at 400 Hz the period is two and a half milliseconds, so a jitter of tens of milliseconds is a fifty times overrun. It runs on a microcontroller with a hardware timer, and preferably with the loop driven by that timer rather than by a scheduler tick.
- **A 100 Hz tick cannot schedule a 400 Hz loop:** the loop needs a hardware timer or a faster tick rate. This is the practical detail that catches groups: a delay of two and a half milliseconds rounds to zero or one tick at the default rate.
- **The camera can live there, but not the loop:** the video stream belongs on a separate processor or on the same chip only if it is buffered and its timing is not on the control path. A dropped frame is a dropped frame, a late attitude correction is a crash.
- **The usual real design:** a flight controller on a microcontroller and a companion computer for video, navigation and telemetry, linked by a protocol where losing the link makes the flight controller enter its own safe state.
- **Teacher info, three minutes:** the arithmetic is the answer, and the second point about the tick rate is the one worth writing on the board. It previews the tick section of the task chapter.
- **Watch for:** "use a faster chip". Speed does not fix determinism, and a faster chip with a general purpose operating system still has an unbounded tail.

# How an RTOS Application Is Organized

## Teaching Notes for the Layering Chapter

- **Time:** forty five minutes, usually in the same meeting as the three worlds.
- **Goal:** the dependency direction, downward only. If a student can place a given file into one of the four layers, the chapter landed.
- **The whiteboard exercise:** draw the four layer stack and have groups shout out where printing belongs, where a PID belongs, where a pin belongs. Printing is genuinely ambiguous and worth two minutes.
- **Watch for:** the student who wants the HAL to brake when an obstacle is near. That is the design bug the chapter exists to prevent, so spend time on it.

## Try it yourself - build and run the starter kit

- **Expected output:** one line per part from the self test, then a monitor line per task containing at minimum the task name, the counters, the sensor values and the measured period.
- **Every column of the monitor line should have an owner:** task name, run counter, distance per sensor, battery voltage, measured period, worst period. If a student cannot say what a column means, they have not read the example.
- **Teacher info, fifteen minutes per group:** do this in the first or second meeting and do not let it slide. A group that has not run the example will struggle with every later chapter.
- **Common failure:** the backend was left at `real` while running in the simulator, so the self test reports missing hardware instead of simulated values. Check `HAL_BACKEND` in the top level `CMakeLists.txt` first, always.

## Check your understanding - one thing in the HAL, one thing not

- **Must be in the HAL:** the knowledge of which pin, which bus, which register and which timing a part needs, exposed as one function with an error return and a documented worst case blocking time. Any hardware specific number belongs here and nowhere else.
- **Must not be in the HAL:** any decision. Braking on an obstacle, retrying a failed read in a loop, logging, or deciding that a stale value is unacceptable is policy, and policy changes per project and must be testable without hardware.
- **Second acceptable "must not":** task creation or long delays. The HAL reports and returns, it does not take ownership of time.
- **Teacher info, two minutes:** grade the sentence, not the example. "The HAL does not decide" and "the HAL is bounded" are the two sentences that matter.
- **Watch for:** "the obstacle check belongs in the HAL because it needs the sensor". That is the exact confusion the next item resolves.

## Try it yourself - a wall at eight centimetres

- **Expected:** after `hal_sim_set_distance(1, 80)` the control task applies its obstacle limit and the motor duty goes to zero, so the simulated speed drops to zero and the monitor prints the stop.
- **Second step:** remove the distance check and the robot keeps driving with the commanded speed. The monitor shows a nonzero duty and a nonzero wheel speed right up to the wall.
- **Teacher info, five minutes:** the value of the exercise is the **before and after**, so make the group run it both ways and look at the duty column, not only at the log text. A group that only reads the source and predicts the result has not done the experiment.
- **Watch for:** sensor index. Index one is the front sensor on the reference board. If the group changes index two nothing happens, which is a fine teaching moment about the sensor array.

## Check your understanding - which layer the obstacle check belongs to

- **Components or tasks, never the HAL:** "wall closer than twelve centimetres means stop" is a policy with a tunable threshold. It changes with the project, the robot and the speed, so it cannot live in a thin driver.
- **Why not the HAL, in one sentence:** the HAL must be reusable by any project and must never decide, otherwise the same driver would brake in a logger and not brake in a follow robot.
- **Where to draw the line:** the HAL answers "the front sensor reports eighty millimetres". The component answers "eighty is closer than my stop distance, so the limit is active". The task answers "the limit is active, so command zero duty".
- **Teacher info, two minutes:** accept "components" or "tasks" but ask which one and why. The component is the better answer because it can be unit tested on a laptop with no scheduler.

# The Hardware and the Schematic

## Teaching Notes for the Schematic Chapter

- **Time:** sixty to ninety minutes across two meetings. Students read a schematic slowly, and that is fine.
- **Goal:** every pin number in the code traces to page three. The student should know that the schematic is the authority and the code is a claim.
- **Print the four pages:** one paper copy per group beats a PDF on a shared laptop. Groups annotate the paper and keep it.
- **Nobody probes the power circuitry with a multimeter:** say it twice, at the start and before the charger page.
- **Watch for:** groups that jump to page three and skip pages one and two. Page two is what explains a board that charges slowly and survives reversed polarity, and page four explains why the motor call takes a duty and not a boolean.

## Check your understanding - why the two motors are treated differently

- **The right motor is mounted mirrored:** the gearbox and wheel are on the opposite side of the chassis, so a positive input that turns the left wheel forward turns the right wheel backward. The HAL inverts the sign for one motor so that positive means forward on both.
- **The encoders are mirrored as well:** counting up on the left and up on the right represent opposite directions, so the encoder sign is also inverted. If it is not, odometry reports a spin instead of a straight line.
- **Two separate handles:** each motor keeps its own state, its own duty limit and its own rate limiter, which is why one call cannot write to the other.
- **Teacher info, three minutes:** push the student to say **what breaks** if it is not inverted. The answer is "the robot spins when you ask for straight", not "the code is wrong".
- **Watch for:** "the pins are different". True and irrelevant, since the electrical interface is identical. The difference is mechanical.

## Check your understanding - which current limit wins

- **The input limit wins:** the charger cannot draw more than about half an ampere from the port, so the charge current is limited by what is available at the input, not by the resistor that sets eight hundred milliamperes. Charging from a weak port is simply slower and the extra headroom is never used.
- **What this tells you about a weak port:** a bigger or faster charger does not charge faster, because the port is the bottleneck. It also means the board must not assume a high current source, which is why the duty limits and the rate limits exist for the motors.
- **Bonus answer, award it:** some ports enumerate as a low current device and the board will still work, just slowly. This is also why the board must not brown out when the motors spike.
- **Teacher info, three minutes:** the arithmetic is less important than the principle, which is "the weakest link sets the limit". Ask which other limits in the design are set the same way, and expect "battery internal resistance" and "boost converter".
- **Watch for:** students who think the charge current setting is a target. It is a ceiling.

## Check your understanding - why the boost wake pin is on the controller

- **Software control of a power rail:** tying the enable pin to the battery would power the motor rail from the moment the cell is inserted. Driving it from the ESP32 lets the firmware sequence power up after the drivers and the encoders are ready.
- **No current spike at start-up:** the motor rail is about ten volts and the converter draws a surge when it starts. Turning it on after boot keeps that surge away from the moment the chip is enumerating USB or reading flash.
- **A real off switch:** the firmware can disable the motor rail for a safe state, which is stronger than setting the duty to zero, because it removes the energy source from the H-bridge.
- **Standby current:** an unpowered boost converter draws less, which matters on a battery powered robot that sits between meetings.
- **Teacher info, four minutes:** this is also the answer to "everything builds but the robot does nothing" in the troubleshooting table, so connect the two in the room.
- **Watch for:** the answer "so the ESP32 can turn the motors on and off", which is circular. Ask why that is useful, and the sequencing argument appears.

## Check your understanding - why a duty between minus one and plus one

- **One continuous value encodes direction and magnitude:** the sign is direction and the magnitude is power, so the interface has one degree of freedom and no invalid combinations. A separate direction plus power would allow "forward at minus forty percent".
- **It matches the controller:** a PID output is a signed number, so it maps to duty without a conversion, a clamp and a special case at zero.
- **It makes rate limiting and deadband trivial:** you interpolate toward a signed target, you clamp to plus or minus the duty limit, and the deadband is a band around zero. With a direction field you would have to special case every crossing of zero.
- **It keeps the brake state out of the API:** both inputs high is a brake, and a signed duty never produces that, so the caller cannot request a brake by accident.
- **Teacher info, three minutes:** the strongest answer mentions the zero crossing. Robots reverse, and a signed duty makes reversal a smooth ramp instead of a state change.
- **Watch for:** "it is shorter". Concision is not the reason. Ask what the caller can express wrongly with the other API.

## Try it yourself - one line per pin table row

- **Expected:** for each row of the pin table, one line in the HAL that uses that pin, for example the LED pin in `hal_led_set`, the two PWM pins in the motor init, the pulse counter channels for the encoders, the ADC channel for the battery, the I2C pins in the bus init and the three enable pins in the sensor addressing sequence.
- **What to look for in the student's answer:** they can point at a line and say "this number is that number on page three". They are not expected to understand a register write, and they should be told so.
- **The interesting finding:** the battery divider ratio appears as a multiplication by two in the code, and the encoder constant appears as four thousand two hundred counts per revolution. Both trace to the schematic, not to a datasheet.
- **Teacher info, fifteen minutes:** let them search, do not hand them a line number. The search is the lesson.
- **Watch for:** groups who only look at `hal.h`. The pin numbers are in the implementation, so point them at the real and simulated source files.

## Check your understanding - one schematic detail with no effect on the code

- **Decoupling capacitors:** the hundred nanofarad parts next to each connector keep the supply clean when a motor switches. The firmware neither knows nor cares, because the effect is hidden inside the hardware.
- **Reverse polarity protection:** the two transistors and the second chip stop a cell inserted backwards from destroying the board. Software never sees it unless the board fails to power up at all.
- **Other acceptable answers:** the output capacitor voltage rating, the LED series resistors, the charger temperature sense input that is not used, the switching frequency resistor of the boost converter, the expansion header that is not fitted.
- **The important half of the answer:** why it has no effect. Say it as "the hardware handles it locally and no software choice can change it".
- **Teacher info, two minutes:** a wrong answer that is worth correcting is "the H-bridge". That one has a large effect, because it dictates the PWM and the duty interface.
- **Watch for:** answers that are really "I do not know what it does". Push for the one sentence that explains the part.

# Toolchain, Build and First Flash

## Teaching Notes for the Toolchain Chapter

- **Time:** one full meeting, and expect it to overrun. This is the slowest chapter in the lab.
- **Goal:** every student has built the starter kit and seen it run, in the simulator at minimum.
- **Start the download in the first five minutes:** this is the single most important scheduling decision in the whole lab.
- **Have spares ready:** three known good USB cables, one known good board, one prepared ESP-IDF export environment, and one screen with the install page open.
- **Watch for:** the installer not run as administrator. The USB driver step then fails silently and the port never appears.
- **Do not debug students' installations in the room one by one:** keep a working reference machine on the side so the group can continue while the broken one is fixed.

## Predict before you run - COM5 and COM7

- **Neither port number is wrong:** the port belongs to the machine and to the board, not to the project. Your colleague's board may enumerate as five and yours as seven, on the same day, on the same desk. What is actually wrong is copying a port number from any other source, including this script, without checking.
- **How to find yours:** Device Manager under Ports on Windows, `idf.py -p detect`, or `/dev/ttyUSB*` and `/dev/cu.*` on the other systems.
- **Teacher info, three minutes:** ask the prediction **before** anyone plugs a board in. The value is in the group that predicts "mine will be COM5 because I have no other device", then discovers otherwise.
- **Watch for:** boards that appear twice. One entry is the serial console and one is the debug interface. The console one prints the boot log, so let them try both and see which one talks.

## Try it yourself - idf.py size

- **The largest part is usually the application:** code plus read only data, which includes the WiFi stack, the FreeRTOS kernel and any printing machinery. The bootloader and the partition table are small by comparison.
- **Why it matters:** flash is finite and the application partition is smaller than the chip. When the image grows past the partition, the build fails at the link stage or, worse, the image no longer fits alongside the storage partition.
- **Printf with floating point is the hidden cost:** the formatting library is large, so removing it or printing integers is a real size reduction and a real time saving.
- **Teacher info, ten minutes:** have each group report the number in kilobytes and one thing they could remove. The number is the evidence, the removal is the understanding.
- **Watch for:** groups who report percentages without a size. Ask for the absolute numbers first.

## Checkpoint - the toolchain gate

- **Pass condition:** a program runs and prints at least one line from a task, and the line names the backend.
- **What to look at, not just for:** the student can say which backend they ran and why, and can name the task that printed. A group that ran `real` without a robot and got an empty log has not passed.
- **Evidence to keep:** a screenshot of the monitor with the boot log and the backend line. Ask for it, because it is the first item of the final report.
- **Teacher info, five minutes per group:** this checkpoint exists to stop a group from drifting three chapters behind with a broken toolchain. Do not wave it through, and do not let a group continue without it.
- **If a group fails:** do not debug in the group. Send them to the troubleshooting table, give them the spare board and cable, and check again in twenty minutes.

# Tasks, Scheduling and Priorities

## Teaching Notes for the Task Chapter

- **Time:** one full meeting for tasks and priorities, a second one if the group needs it. This and the shared data chapter are the heart of the lab.
- **Goal:** the student can predict which task runs next, and can defend four numbers per task.
- **The priority inversion video:** play it at the start of the meeting, and again after the shared data chapter. It lands twice.
- **A well written task is blocked almost all of the time:** that single sentence is what everything else in the chapter builds on.
- **Watch for:** students who measure nothing and claim "it works". The checkpoint is the four numbers, and averages do not count.

## Check your understanding - why a task needs an explicit stack size

- **There is no memory manager:** the RTOS has no MMU and no virtual memory, so every stack must be carved out of a fixed heap at creation time. Nothing can grow on demand.
- **Overflow is corruption, not an exception:** on a PC the operating system can catch a bad access, here a task that overruns its stack writes into the neighbour's memory. With `configCHECK_FOR_STACK_OVERFLOW` you get a hook, without it you get a mysterious panic or a wrong value.
- **The unit is words:** four bytes on this chip, so four thousand ninety six means about sixteen kilobytes. This is why the numbers look large and why students who assume bytes over-allocate by four times.
- **Teacher info, three minutes:** the answer you want is "the stack is statically assigned because there is no protected memory". Mention the word unit and move on.
- **Watch for:** "the default is fine". Ask what the default is and how they would know. Then ask for the watermark, which is the next section.

## Try it yourself - a blocking task and a spinning task

- **Build two tasks, one blocking and one spinning:** the blocking task increments a counter and delays one millisecond, the spinner increments its own counter and never blocks. Print both counters from a third task once per second.
- **On one core at equal priority the ratio is about ten to one:** the delayed task gets roughly a tenth of the increments of the fast one, because each activation costs one tick of delay. The exact ratio depends on the work per iteration.
- **A sharper experiment starves the delayed task completely:** raise the spinner to a higher priority and the delayed task stops incrementing entirely, because a task that never blocks and outranks it is never preempted.
- **A low priority spinner still steals everything spare:** run the spinner at priority one and a good task at priority ten. The good task keeps its period, but the idle task never runs, so free heap drops and, with idle starvation detection on, a watchdog complaint appears.
- **Reversing the two proves the rule the other way:** spinner above the good task and the good task stops counting entirely, for the same reason as the sharper experiment.
- **Teacher info, fifteen minutes:** this is the most valuable experiment in the chapter. Make every group do all three steps and write the numbers on the board.
- **Watch for:** measuring only the counters. Ask which task states were occupied. The correct description is "the spinner is running or ready at every instant".

## Check your understanding - ready versus running at the same priority

- **The tick decides:** with time slicing enabled, tasks of equal priority that are both ready take turns, and the rotation happens at the scheduler tick. So at the next tick the other one runs.
- **Without time slicing:** the running task keeps the core until it blocks or is preempted by a higher priority task. Then the ready task that has waited longest runs.
- **A third factor on this chip:** core affinity. If a task is pinned, only that core matters. If it is not pinned, a free core may pick it up immediately, so "the next tick" is not the whole story.
- **Teacher info, three minutes:** this question separates the students who read the scheduler rules from those who memorised them. Ask the follow up: what if the running task never blocks? Then the ready one waits forever unless time slicing saves it.
- **Watch for:** "priority decides", which is true but not the answer here, because the priorities are equal.

## Try it yourself - breaking the scheduler on purpose

- **Step one, two good tasks:** roughly a ten to one ratio between the one millisecond task and the ten millisecond task, because activations are one order of magnitude apart. Report the actual counters, not the theory.
- **Step two, spinner instead of the fast task:** the slow counter stops or crawls. The spinner is always ready or running, so the slow task is ready but never scheduled on that core.
- **Step three, slow task above the fast one:** predict first. The slow task keeps its ten millisecond rhythm because it preempts the fast one, and the fast task gets whatever is left and therefore runs **less** often than before, not more.
- **Teacher info, twenty minutes:** collect the predictions on paper before any run. The group whose prediction fails learns more than the group that guessed right.
- **Watch for:** students who think preemption happens only at the tick. Rule three is the surprise: a higher priority task interrupts immediately, not at the next tick.

## Check your understanding - same priority, ten millisecond and one millisecond blocks

- **Both get scheduled, and the one millisecond task gets more activations:** ten times as many runs per second. CPU **share** is activations times execution time, not priority, so it depends on how long each run takes.
- **On one core:** they take turns at the tick, so each sees its effective rate reduced by whatever the other consumes. Ten activations of one millisecond of work plus one activation of ten milliseconds is the same load, and neither dominates.
- **On two cores:** if they are not pinned they can run at the same instant, so both get their nominal rate. That is the advantage, and the cost is that the timing now depends on what the other core does.
- **A caution worth saying:** if the one millisecond task's work grows to four milliseconds, it can no longer meet its own period, and it will drag the ten millisecond task down with it on a shared core.
- **Teacher info, four minutes:** the phrase to extract is "share is work times frequency". A student who says "higher priority gets more CPU" has confused priority with fairness.

## Try it yourself - pin the spinner to one core

- **Spinner on core 0 and the good task on core 1:** the good task keeps its period, because the spinner cannot preempt a task on the other core. This is the demonstration that pinning protects the real time path from unrelated load.
- **Both on the same core:** the good task suffers again, and the amount of suffering is the load of the spinner. That number is the cost of pinning wrong.
- **What to write down:** the measured worst period of the good task in each of the three configurations, not just a verdict. Three numbers make the argument, one impression does not.
- **The second lesson, which students miss:** the spinner is still burning a whole core. Pinning moved the damage, it did not remove it, and the runtime statistics will show the idle task starving on that core.
- **Teacher info, twenty minutes:** run all three configurations and tabulate the worst period. Then ask which configuration they would ship, and why not the middle one.
- **Watch for:** the group that pins the spinner and then declares the problem solved. Ask what else is on core 0 and what it now shares with the spinner. The answer is usually the monitoring and the network, which is exactly the wrong core to poison.

## Check your understanding - one thing pinning improves and one it worsens

- **Pinning buys determinism:** a pinned task's worst case no longer depends on what the other core is doing, so the network stack can block for tens of milliseconds and the motor loop never notices.
- **Pinning costs the spare core:** the other core is now off limits, so idle capacity is wasted and a burst of work on the pinned core cannot borrow from the other. Load balancing disappears.
- **A second drawback worth mentioning:** you must then maintain the second core yourself, and both cores still share memory, buses, cache and power, so pinning removes interference from the scheduler and not from the hardware.
- **Teacher info, three minutes:** the honest answer "I did not pin it and here is how I measured that it does not matter" is a pass in the review. Say so, or students will pin everything to look competent.

## Try it yourself - four numbers for one task

- **Expected shape of the four numbers:** vTaskDelay without extra work, about ten milliseconds. vTaskDelay with three milliseconds of busy work, about thirteen. vTaskDelayUntil without extra work, about ten. vTaskDelayUntil with three milliseconds of busy work, about ten.
- **What the numbers prove:** the delay function waits from now, the delay until function waits until the next absolute deadline. Only the second holds the period.
- **The fifth number, which separates good groups:** the jitter, meaning the worst period minus the target. With delay from now it equals the work, with delay until it is a small tick granularity artefact.
- **Teacher info, twenty minutes:** require all four numbers written on paper, with the units. Then require the group to say which one they would use in a control loop and why.
- **Watch for:** students who measure the period with a print inside the loop and do not notice that the print is part of the work. Ask them what the print costs, and let them measure it.

## Check your understanding - worst period of eleven milliseconds

- **It is not finished:** the target is ten and the worst case misses it, so there is a deadline miss to explain. The project is finished when the miss is either fixed or proven irrelevant.
- **What must be explained:** which run was late, and what the task was doing at that moment. Common honest answers are the first period after start-up, a sensor read that hit its timeout, a print that filled a buffer, or a flash write.
- **How to present it:** state the target, the worst measured, the number of occurrences in the run and the cause. A single outlier at start-up with the rest at ten is a different engineering statement from a periodic eleven.
- **The acceptable finish:** if the outlier recurs, reduce the work or move it out of the loop. If it occurs once at start-up, exclude the warm up explicitly and measure again.
- **Teacher info, four minutes:** this question appears twice in the script, so make sure the group gives the same disciplined answer both times. "It was only one time" is not an explanation until the cause is named.
- **Watch for:** the student who answers "eleven is close enough". Ask what the deadline is for, then ask what happens at forty.

## Checkpoint - the timing gate

- **Pass condition:** four measured numbers, worst period and worst execution time, for both delay functions, with units and a stated setup.
- **What to ask:** which delay function belongs in a control loop, and what happens if the work exceeds the period. Both answers must come without notes.
- **Evidence to keep:** a copied log excerpt with the measured lines, plus one sentence on the setup: core, priority, duration.
- **Teacher info, five minutes per group:** an average is not a pass. A group that shows only an average gets sent back to print the worst case.
- **If a group fails:** they usually measured the loop without the real work. Ask them to add the work and re-measure rather than to argue.

# Sharing Data Between Tasks

## Teaching Notes for the Shared Data Chapter

- **Time:** one and a half meetings. This chapter is the densest in the lab and it deserves the time.
- **Goal:** the student chooses the mechanism from the problem: mutex for latest value, queue for events and for state, sequence number for freshness.
- **The three mechanisms are not interchangeable:** say that in every meeting, and refuse answers that say "a mutex and a queue are both for sharing data".
- **The stress test is the evidence:** torn reads at zero with the mutex and climbing without it. Do not accept a verbal claim of correctness.
- **Watch for:** the group whose consumer is fast and whose producer is slow, so the race never appears. They must run the asymmetric load or they will learn nothing.

## Try it yourself - the unprotected version and torn reads

- **Build an asymmetric pair, writer ten milliseconds and reader slower:** increment a counter inside the writer, and let the reader check that the two halves of the data belong to the same cycle.
- **Expect a nonzero torn read count within seconds:** it appears fastest when the reader is on the other core. On a single core it may take longer to appear, which is itself a lesson.
- **The three forms to look for:** a torn read, where one field is new and another is old. A half finished update, where the reader runs between two assignments. A split value, where a wider value is read while it is being written.
- **Teacher info, twenty minutes:** require the counter, not just the demonstration. Then require the same test with the mutex, and compare.
- **Watch for:** the reader copying field by field outside a lock and calling that "protected". That is the exact failure the next section fixes.

## Check your understanding - why a race is more likely on two cores

- **Two tasks truly execute at the same instant:** on one core the only interleaving points are preemptions, which are coarse and comparatively rare. On two cores the writer can be halfway through a multi instruction update while the reader reads, so the interleaving is a real physical event rather than a scheduler artefact.
- **A second reason:** the two cores have no shared sequence of instructions, so a reader can observe a value that no single core would ever have produced.
- **A third, subtle reason:** the compiler and the hardware can reorder accesses, so even a "single" assignment may be visible in pieces. That is why a `volatile` flag is not the same as a lock.
- **Teacher info, three minutes:** the phrase to insist on is "true concurrency". A student who says "it is faster on two cores, so it happens more" has missed the point.

## Check your understanding - how long a task can be delayed without inheritance

- **Unbounded:** the low priority holder is pushed aside by the medium priority task, so the high priority waiter waits for as long as the medium task runs. If the medium task is a network task with a long loop, that can be hundreds of milliseconds or forever.
- **The chain can be worse than one medium task:** every task above the low one and below or equal to the medium one adds to the delay. This is the general priority inversion bound, and it grows with the number of intermediate tasks.
- **Priority inheritance is the fix:** FreeRTOS mutexes boost the holder to the priority of the highest waiter for the length of the critical section. The delay is then bounded by the critical section, and that is why critical sections must be short.
- **Teacher info, five minutes:** draw the three task timeline on the board. Ask what the delay is when the medium task loops forever. The answer is "no bound", which is the sentence you want in the report.
- **Watch for:** "a few milliseconds". That is the answer with inheritance, not without it, and confusing the two is the single most common error in this chapter.

## Check your understanding - why an overwrite queue is wrong for buttons

- **A button press is an event:** each press is unique and must be counted. An overwrite queue keeps only the newest item, so two presses that arrive before the consumer reads result in one press being lost, silently.
- **A joystick position is state:** only the latest position matters. Older positions are already stale, and applying them in order makes the robot follow the operator through history.
- **The test to apply:** ask what is lost when the consumer is slow. For a button the answer is "an input the user performed", which is unacceptable. For a joystick the answer is "an outdated position", which is exactly what should be dropped.
- **Teacher info, three minutes:** make the student say the word **event** for the button and **state** for the joystick. Then ask which one needs a length of four and which needs a length of one.
- **Watch for:** "use a queue for both and make it big enough". Big enough is not a bound, and it still loses data when it overflows.

## Try it yourself - suspending the producer

- **Expected behavior:** after `vTaskSuspend` on the producer, the sequence number stops changing and the timestamp stops advancing, so the consumer's freshness check fails after one hundred milliseconds and the consumer switches to its defined safe behaviour.
- **The proof:** two log lines tell the story, one while the data is fresh and one after the timeout, with the age in milliseconds printed.
- **The follow up, which is the real lesson:** the consumer must **act** on the stale data, not just print. Zero duty, or a held value, depending on the design, and the state must be visible in the log and on the LED.
- **Teacher info, fifteen minutes:** require the safe state to be observable. A group that only prints "stale" has an instrument, not a design.
- **Watch for:** the consumer blocking on the mutex with `portMAX_DELAY` while the producer is suspended. That is a different bug than the one being taught, and it is worth ten seconds: the mutex is free, the data is old, so the take succeeds and the freshness check does the work.

## Check your understanding - a producer that publishes an error every time

- **A validity flag is missing:** freshness says "the producer is alive and publishing", validity says "the payload means something". A sequence number that increments over garbage data is technically fresh and completely useless.
- **What to add:** a `valid` flag per reading, or an error code per field, plus a counter of consecutive failures. The consumer then treats a run of invalid publications as a soft failure and degrades.
- **A second missing piece:** an aggregate health state. One sensor valid and two invalid is different from all three invalid, and the safe state should reflect that difference.
- **Teacher info, four minutes:** this is the best question in the chapter because it breaks the "fresh means good" assumption. Award the answer that says "liveness is not correctness".
- **Watch for:** students who propose a longer freshness timeout. That makes old data accepted longer, which is the opposite of a fix.

## The Data Handover Exercise

- **Expected structure:** one struct with at least four fields plus a sequence number and a millisecond timestamp, one mutex, a setter and a getter that both copy the whole struct under the lock, one length one overwrite queue for the command, and a consumer that rejects anything older than one hundred milliseconds.
- **Expected stress test configuration:** producer at ten milliseconds, a second producer at one millisecond, consumer at five milliseconds, two minutes of runtime, counters for accepted reads, rejected stale reads and torn reads.
- **Expected numbers:** torn reads stable at zero with the mutex. Rejected reads nonzero but small, which proves the freshness check is reachable and not dead code. Accepted reads in the thousands.
- **Expected damaged run:** with the mutex removed, torn reads climb within seconds. That run is the evidence that the test is real; a test that never fails a broken design proves nothing.
- **Teacher info, thirty five minutes, no robot needed:** this is the chapter's main deliverable and it feeds the project directly. Circulate and check three things: the struct copy is inside the lock, the queue is a length one overwrite, and the torn read counter exists before the mutex is added.
- **The getter returning a pointer is the most common error:** it defeats the mutex completely, because the reader dereferences it after the lock is released. Fix it in the group, gently, and make them run the damage test again.
- **A delay or a printf inside the critical section is the second:** ask how long the section takes, and what every other waiter is doing during it.

## Check your understanding - safe default or last known good

- **Safe default is better when the risk is asymmetric:** for a distance reading, assuming an obstacle means stopping, and stopping is cheap, so the default should be "something is in front". The alternative risks a collision on a guess.
- **Last known good is better when a default would be disruptive and the value changes slowly:** a commanded heading, or a temperature setpoint from a remote UI. Jumping to zero on a stale link would stop a process that was running perfectly, so holding the last value for a bounded time is the lesser harm.
- **Decide per variable and bound the hold time:** write the decision down, and bound how long the last known good may be held. Unbounded holding is how a robot drives on a command from ten minutes ago.
- **Teacher info, four minutes:** accept any pair of cases with a reason, and reject "safe default always". The question exists to break that habit before the project starts.

## Quiz round - mutex versus queue

- **A mutex protects a value:** several tasks may read the latest state, and the mechanism provides exclusion plus priority inheritance. It carries no data and it does not queue anything.
- **A queue transfers data:** each item is handled once, in order, and a receiver blocks instead of polling. It does not protect an existing variable and it does not inherit priority.
- **A mutex asks permission, a queue reports events:** a mutex answers "may I touch this now", a queue answers "what happened and in what order".
- **Teacher info, five minutes for the quiz:** three points for the first correct, two for the second, one for the third. Ask for the one sentence version, not the long one.

## Checkpoint - the data gate

- **Pass condition:** torn read counter at zero with the mutex and clearly climbing without it, on the same test, shown twice.
- **What to ask:** which mechanism protects which variable, and why not the other one. Also: is the critical section short, and where is the copy.
- **Evidence to keep:** both runs, with the counters visible, and one paragraph for the report.
- **Teacher info, five minutes per group:** the before and after is the whole check. A group that only shows the good run gets sent back.
- **If a group fails:** they usually have no torn read counter, which means no test. Have them add the counter first and re-run, then add the mutex.

# The Control Path and the Cost of Delay

## Teaching Notes for the Control Path Chapter

- **Time:** one meeting, including the network milestone if the hardware is available.
- **Goal:** the student sees the control path as a pipeline, and treats a millisecond as an engineering quantity rather than a style preference.
- **The four odd lines in the reference control task are the chapter:** rate limit in both directions, raw value for the hard stop, held value through flicker, latched stop. Each one is a bug that was fixed.
- **The PID video:** play it, but tell the group they are not expected to derive a controller. They only need to recognise the shape.
- **Watch for:** the group that reports a clean simulator measurement and an ugly hardware measurement and calls the hardware broken. The honest numbers are the point, so make them explain the difference.

## Check your understanding - a seven millisecond control task in a ten millisecond period

- **The effective period with vTaskDelay:** seventeen milliseconds, because the delay waits from the end of the work. The controller then runs at about fifty nine hertz instead of a hundred, and every gain tuned for ten milliseconds is now wrong.
- **The effective period with vTaskDelayUntil:** ten milliseconds, because the work still fits, but the margin is three milliseconds and any extra call pushes it over. The CPU load of that task is seventy percent, which leaves very little for anything at equal or higher priority.
- **What to fix first:** the work, not the delay function. Typical wins are removing the print, splitting the sensor read into a different task, caching a conversion, and removing an allocation.
- **Teacher info, four minutes:** insist on the word **margin**. A task that meets its period by three milliseconds is not finished if it has no measured headroom against its own worst case.
- **Watch for:** the answer "raise the priority". Priority does not make work smaller and does not solve an overrun.

## Try it yourself - a seven millisecond delay versus a busy wait

- **With a delay:** the period becomes about seventeen milliseconds, because the delay is relative and the work adds to it. The CPU load is low but the timing is wrong.
- **With a busy wait:** the wall clock period is about ten milliseconds, because the wait absorbs the difference, but the task never blocks, so it holds the core and starves everything at or below its priority on that core. The CPU load is a hundred percent.
- **The conclusion the group should reach:** neither is acceptable. The correct answer is to make the work fit and use the absolute deadline, so the task blocks for the remainder and gives the core back.
- **Teacher info, fifteen minutes:** run both and compare not only the period but the runtime statistics. The busy wait version should show an obvious CPU share, and that number is the argument.
- **Watch for:** groups who measure the delay with `esp_timer_get_time` inside the same task and forget that their own measurement code is work. Ask them to subtract it or to measure outside.

## Try it yourself - the HAL worst case list

- **Expected list:** initialization about sixty milliseconds, motors immediate and never blocking, sensor read one at most about fifteen milliseconds, sensor read all up to about forty five milliseconds, battery about one millisecond, LED and buzzer immediate.
- **Avoid hal_sensor_read_all inside a ten millisecond task:** forty five milliseconds is four and a half periods. One sensor per tick is the fix, or read in the sensor task and publish.
- **Avoid initialization after start-up too:** it belongs in the start-up or in an init task before the control loop exists.
- **The general rule to state:** a call whose worst case is larger than your period is not a blocking call you tolerate, it is a design error. The worst case is part of the interface, which is why the HAL documents it.
- **Teacher info, twelve minutes:** the list must include the **worst case number**, not just the function name. A list of names is not engineering.
- **Watch for:** students who read only `hal.h` and not the documentation in the header. The numbers are in the comments and in the appendix of the script.

## Try it yourself - duty and RPM for a step from zero to half speed

- **Expected curve:** a delay before anything moves, because of the deadband. Then a ramp limited by the rate limiter and the motor inertia. Then an overshoot or a slow approach, depending on the gains and the load.
- **What the two curves show:** the commanded duty is a step and the measured RPM is a rounded shape. That gap is the reason a controller exists and the reason the controller needs feedback.
- **What students should notice:** the lag grows with the step size and with the load, and a low command may not move the wheel at all until the deadband boost is applied.
- **Teacher info, fifteen minutes:** require the two plots or the two log columns, side by side. Then ask which part of the shape the rate limiter caused and which part the motor caused.
- **Watch for:** measuring with the robot on the table, wheels down. Lift it or put it on blocks, always, and say why.

## Check your understanding - why the controller needs measured wheel speed

- **The command is not the outcome:** battery voltage, friction, wheel slip, load and the deadband all change the relationship between duty and speed. The same duty gives a different speed on carpet, on a full battery and on a nearly empty one.
- **A closed loop corrects for what you cannot model:** the controller compares the measured speed with the setpoint and adjusts, which is what makes straight line driving possible on two slightly different motors.
- **The measurement also gives you the diagnostics:** a commanded duty with zero measured RPM is a stalled or disconnected motor, and a sudden speed drop at constant duty is a battery problem or a slip.
- **Teacher info, three minutes:** the answer to reject is "because the controller is a PID and PIDs need feedback". That is circular. Push for a physical reason, and expect "the motor does not do what you tell it".
- **Watch for:** "the encoder is only for odometry". That is a real second use, but it does not answer the control question.

## Try it yourself - distance as JSON, then kill the connection

- **Expected:** the robot keeps driving when the link dies, and the status LED reports the link loss with its own code. The control path never notices the network at all, because it only reads the shared snapshot and the command queue.
- **The last command keeps driving until freshness expires:** the robot runs on the last command for as long as the command freshness limit allows, and then must degrade. Without a freshness limit it drives forever on a command from before the outage.
- **What to check in the code:** the network task is on the other core, at low priority, with blocking calls and finite timeouts, and it writes to an overwrite queue of length one. Nothing in the control task references the network.
- **Teacher info, twenty minutes if hardware is available:** this is the network milestone, so it must actually be demonstrated once per group. If the school network blocks the port, use a small server on a laptop and say so.
- **Watch for:** the group that calls the WiFi connect from the control task "just for the handshake". That is the next question, and it is the most common real design error in student projects.

## Check your understanding - why the WiFi connect never belongs in the control task

- **Three hundred milliseconds is thirty missed periods:** the deadline is ten milliseconds, so a call that takes three hundred destroys the promise for the whole duration, including the obstacle reaction inside it.
- **And it is not bounded at three hundred:** association, DHCP and retries can take seconds, and the documented time is a typical case, not a worst case. A real time task cannot call something with an unknown bound.
- **Put the connect in its own low priority network task:** it runs on the network core, publishes its state through a shared flag, and reports failure through the diagnostics path. The control task never learns that WiFi exists.
- **Teacher info, four minutes:** the strongest answer names the obstacle reaction, because that is what a missed period physically costs. Award the student who connects the delay to braking distance.
- **Watch for:** "it only happens once at start-up". Then it belongs in an init task before the control loop, not in the control task. Same conclusion, and it shows the student understood the reason.

# Diagnostics and Fault Handling

## Teaching Notes for the Diagnostics Chapter

- **Time:** one meeting. It is the last theory chapter and it should feel like relief, not like more theory.
- **Goal:** students add observability **while** they build, not after the demo fails. Say that as a design rule, not as advice.
- **The LED code table is the deliverable:** by the end of the meeting every group has its own table, written down, with the codes they actually use.
- **Watch for:** groups whose only diagnostic is `printf` in the fast loop. Measure the cost with them and let the number make the argument.
- **The monitor task discipline:** report, never repair. A monitor that takes action becomes part of the failure.

## Try it yourself - heartbeat counter and worst execution time

- **Expected:** a counter that increments every run, a `esp_timer_get_time` pair around the body, a running maximum, and one print per second with the maximum and the current value.
- **What the long run shows:** outliers that a short run hides, usually a sensor read that hit its timeout, a print that filled a buffer, or the first run after a flash write. Those outliers are the reason a five minute run is required in the report.
- **A useful addition:** print the stack watermark in the same line. It costs almost nothing and the two numbers belong together.
- **Teacher info, fifteen minutes, then run all meeting:** tell the group to start it early and read it at the end. The surprise is the lesson; do not spoil it by saying what they will find.
- **Watch for:** a maximum computed in a wrong scope, so it resets every second. Ask whether the number they see is the maximum since boot, and how they know.

## Check your understanding - why a persisted boot error is more useful

- **A printed message is gone:** it only exists in the console of whoever was attached at that second. A reboot in a loop erases it from the screen, and an unattended robot has nobody attached at all.
- **A persisted code survives:** writing one byte to non volatile storage means the next boot can show the code on the LED, or report it later, or refuse to start in an unsafe configuration. The failure becomes evidence instead of a rumour.
- **The cost is small and the discipline is large:** one write per failure, not per loop, and a documented way to clear it. Otherwise the stored error becomes permanent noise.
- **Teacher info, three minutes:** ask the follow up: what clears it. A persisted error with no clear path is the next bug, and students rarely think of it.
- **Watch for:** "the log is enough". Ask how they read the log on a robot that is on the floor and rebooting.

## Check your understanding - the network task dies holding the drive command queue

- **What actually happens:** the queue object belongs to the kernel, so it is not corrupted, but the producer is gone. The consumer blocks until its timeout, then repeats. If the consumer treats a timeout as "keep the last command", the robot drives on the last command indefinitely.
- **What it should do:** the consumer's command freshness expires and the robot enters its defined safe state, which for a drive command is a stop. The monitor detects the missing heartbeat and reports it, so the cause is visible and not just the symptom.
- **The ownership lesson:** the control side must not depend on the producer being alive for its safety. Safety is decided by the side that owns the actuator.
- **Teacher info, four minutes:** the distinction to draw is between holding a **handle** and holding a **mutex**. A dead task holding a mutex is a deadlock; a dead task holding a queue handle is just a missing producer. Two different failures, two different fixes.
- **Watch for:** "the monitor restarts the network task". Restarting is sometimes right, but a monitor that repairs its own inputs can hide a permanent fault. Report first; repair only with a bounded policy.

## Try it yourself - a deliberate stack overflow

- **Expected:** a panic with a message naming the task, in the shape "a stack overflow in task X has been detected", plus a backtrace. The task name is the point, which is why tasks must be named.
- **What to look at:** the task name first, then the backtrace from the top down to the first recognisable function, then the size requested at creation.
- **Measure, then set the number:** read the watermark of the same task with a generous stack, then set the real size with a margin. Guessing is what caused the panic.
- **Teacher info, ten minutes:** do this on purpose, early, so the panic message is familiar before it appears by accident. A student who has seen it once debugs in a minute instead of an hour.
- **Watch for:** panic output that scrolls past. Say the monitor paging command or scroll up in the terminal, because the top of the message is what names the cause.
- **Caution:** do this in the simulator or on a bare board, not on the shared robot during a demo slot.

## Check your understanding - reboots at full duty on both motors

- **Brownout is the fit:** a current spike from both motors at full duty pulls the supply down past the brownout threshold, and the chip resets. The reset timing, right at the command, is the signature.
- **How to confirm:** read the panic or reset reason in the boot log, which names the brownout detector. Then log the battery voltage around the event and watch it sag, and check whether lowering the duty limit removes the reboot.
- **Fix it in software, not with parts:** a duty limit of about eighty percent with a rate limit, plus a battery voltage check that reduces the limit before the sag becomes a reset. Not a bigger capacitor and not a retry.
- **Teacher info, three minutes:** the other three causes are stack overflow, watchdog and assert, so ask the student to rule them out explicitly. A student who says "brownout because motors" without ruling anything out gets half the point.
- **Watch for:** "the board is broken". The board survived; the design exceeded the supply, and that is a fixable software problem.

## Quiz round and checkpoint - the health gate

- **Quiz content:** the LED code meanings, why the lowest code wins, why a persisted error beats a print, and what graceful degradation means for a lost network versus a lost sensor.
- **Pass condition:** a running system that reports its own health without a debugger, either LED codes on hardware or printed heartbeats and stack numbers in the simulator.
- **What to ask:** which code appears when the sensor is unplugged, and which appears when the network dies. Then: what does the robot do in each case, and why is that ranking correct.
- **Teacher info, five minutes per group:** provoke one fault yourself and watch the reaction. A demo of the healthy state proves nothing.
- **If a group fails:** they usually have prints but no codes and no persisted error. One LED and one stored byte is enough to pass, so send them back for ten minutes.

# Your Project - Design in the Simulator

## Teaching Notes for the Project Design Chapter

- **Time:** half a meeting for the design review, then two to three meetings of build time.
- **Goal:** a one page design that can be reviewed in five minutes, then a simulator project that can be demoed in three.
- **Scope control is your main job:** students propose projects that are twice too large. Cut features, keep the failure case.
- **The best projects have an obvious way to break:** ask every group what they will break on purpose in the demo. If there is no answer, the design is not finished.
- **Do not review code before you review the page:** the one page forces the decisions to be explicit, and it takes five minutes instead of twenty.

## The one page design review

- **The idea, two or three sentences:** if it takes more, the student has not decided what the system does. Ask which sentence is the core and which is a feature.
- **The success criteria must be measurable:** "the control task keeps its ten millisecond period with jitter under two milliseconds for five minutes" is a criterion. "Runs smoothly" is not, and should be rewritten in the room.
- **The task table must be complete:** core, priority, period or trigger, stack and one line of job for every task, including the ones whose numbers are guesses. Guesses are marked as guesses.
- **The data map must be one mechanism per variable:** every shared item appears exactly once, with a writer, a reader, a mechanism and a freshness limit. Two mechanisms for one variable is a bug in the design, visible before any code exists.
- **The hardware budget includes the fallback:** what the project does if a sensor or the network is unavailable. A project with no fallback will be the one that cannot be demoed.
- **Teacher info, five minutes per group, ten per group in practice:** do not rewrite the design. Ask the three questions below and let the student fix it.
- **The three questions you always ask:** which task has the shortest deadline and is it also the highest priority. What happens when each task dies. Which line of the data map protects the thing the robot does most often.

## The design checks you run before the group writes code

- **Deadline first:** the task with the shortest deadline must be the highest priority, or there must be a stated reason why not. Reasons like "it does not matter if the log is late" are fine, and "it is the same as the example" is not.
- **Every task blocks:** no spinning task anywhere. A periodic task must be able to meet its period with measured margin, not with hope.
- **One mechanism per variable:** cross out every variable that appears twice in the data map, and let the student choose one. This check catches more real bugs than any other.
- **Readable start-up:** the start-up must fit on one screen. Everything that does not fit belongs in a component or in an init function with a name.
- **Known failure path:** for each task, write what happens when it dies, even if the answer is "nothing, and I accept that for now". The acceptance must be explicit.
- **Layering:** every file must be assignable to one of the four layers. A file that is hard to place mixes two concerns, and that mix is where the timing bug will live.
- **Teacher info, ten minutes per design:** mark the page up in red rather than discussing it in prose. A marked page is actionable, a conversation is forgotten.

## The task table and data map - what a good answer looks like

- **A good task table has numbers that relate to each other:** the two real time tasks at priority ten on the control core, the monitor below them, the command path below that, every stack justified by a watermark or marked as a guess.
- **A good data map has no gaps:** the snapshot with a mutex, sequence number and timestamp, the command with an overwrite queue of length one, the heartbeats as volatile counters, the health state as an atomic flag. Every row has a freshness limit or an explicit "not critical".
- **The honest differences:** the review will show changes between design and build. Every difference needs one line of explanation in the report, and that line is worth more than a perfect original design.
- **Teacher info, four minutes:** do not ask for a perfect table. Ask for a table the student can defend, including the numbers they guessed and how they will check the guess.

## Implementation order - the sequence that keeps you runnable

- **Skeleton first:** tasks with the right core, priority and stack, each printing its name and a counter. Nothing else works and that is the point, because you can run it.
- **Heartbeats second:** a counter in each task and a monitor that prints them. Now the whole system is visible at a glance, which is what the next five steps depend on.
- **Shared data third:** one producer, one consumer, real data, mutex, sequence number, and the consumer acting only on new data.
- **Queue fourth:** the overwrite queue plus a fake command source, drained with a zero timeout in the consumer. A fake source is legitimate and is removed at the port.
- **Freshness fifth:** stop the producer on purpose and prove the consumer degrades. This is the step students skip and then fail to demo.
- **Measure sixth:** period, jitter and execution time in the real time task, left running. Measurement before behaviour, always.
- **Behaviour last:** the actual idea of the project. If it does not fit in the remaining time, the project is still complete, because steps one to six are the lab.
- **Teacher info, this is the schedule:** check the group at step two and step five. A group that went straight to behaviour and has no heartbeats will spend the rest of the project debugging blind.
- **Watch for:** the group that prints in the ten millisecond loop and reports a period of twelve. Ask them to move the print to the monitor and re-measure, and let the improvement speak.

## Provoking faults in the simulator

- **The three provocations:** a wall at six centimetres with the distance setter, a silent sensor with the missing setter, an almost empty cell with the battery setter. Each one must produce a visible, defined reaction.
- **What to record:** the code or the log line the fault produced, and the state the robot ended in. A fault with no visible reaction is an untested design.
- **The simulator limits:** no motor physics, no real I2C timing, no reliable WiFi. A design that depends on the network needs a substitute producer, and the design page must say so.
- **Teacher info, ten minutes per group, and repeat it:** faults are provokable only in the simulator, so this is the cheapest evidence in the whole lab. Do not let a group leave the design phase without at least two provoked faults.
- **Watch for:** `hal_sim_*` calls left in code that is later flashed to hardware. They return not supported on real hardware, which is a clean failure, but the fallback path should be tested too.

## Evidence collection - the minimum set

- **The task table as built, compared with the design:** every difference with one line of explanation.
- **Period and jitter:** the worst case for the real time task over at least five minutes of the real workload.
- **Execution time:** the worst case per run, so the period can be justified against the work.
- **Stack headroom:** the watermark for every task, with the margin added and why.
- **The stress test:** accepted, rejected and torn read counts, with the mutex in place.
- **One failure found and fixed:** what broke, how it was noticed, what changed, how the fix was proven. This single item is worth more than the rest combined, and it should be said out loud in the room.
- **Teacher info, five minutes:** insist on the setup line for every number. A number without a setup is not evidence, and the report should be marked as incomplete rather than wrong.
- **Watch for:** tables typed from memory. Ask for the log excerpt or the screenshot behind at least one row, and the honesty of the rest improves immediately.

## Check your understanding - worst period of eleven milliseconds, again

- **The same disciplined answer as before:** not finished, and the miss must be explained by cause and frequency, not excused as close. This repetition in the script is deliberate.
- **The project specific addition:** name what the task was doing in that particular run, since in a project the workload is variable and the honest answer is often "the network publish coincided with it".
- **The pass condition in the review:** the student can say whether the outlier is at start-up, periodic, or load dependent, and what they would change if it were periodic.
- **Teacher info, three minutes:** connect this to the earlier answer. A group that gives a different quality of answer here has regressed, and that is worth a minute of correction.

## Checkpoint - the simulator gate

- **Pass condition:** three tasks running, one mutex protected struct with a freshness check, one queue, and live period and jitter numbers on screen.
- **What to ask:** which of your tasks blocks and for how long, and what your safe state is. Then provoke one fault and watch the reaction.
- **Evidence to keep:** a screenshot of the running monitor with the measurements, and the task table as built.
- **Teacher info, ten minutes per group:** this is the largest gate in the lab. Do not pass a group that has tasks but no freshness check, because the port to hardware will then fail in the demo.
- **If a group fails:** the usual cause is step five and six skipped. Send them back to stop the producer on purpose and to add the measurement, and hold the port to hardware until both are shown.

# Your Project - Hardware, Demo and Review

## Teaching Notes for the Porting Chapter

- **Time:** two to three meetings, and the bottleneck is robot availability, not understanding.
- **Goal:** a small port, honest numbers, a three minute demo with a deliberate failure.
- **The port should be small:** if it is large, that is the lesson and it belongs in the report. Ask what had to change and why, and treat the answer as evidence of the design quality.
- **Lift the robot before the first run of new motor code, every time, in every group:** this is the one rule you enforce personally.
- **Watch for:** the group that changes three things at once and cannot say which one broke it. Enforce one step at a time, with a check after each.

## The port order and what each step proves

- **Flash unchanged first:** if the simulator project runs on hardware untouched, the design and the HAL usage are correct. This is the single most informative step in the lab, because it isolates design from detail.
- **Swap fake inputs for real ones second:** replace the substitute producer with the real sensor task. The struct, sequence number and mutex stay, so nothing downstream changes. If downstream code must change, the boundary was in the wrong place.
- **Bring motors up safely third:** lifted or on blocks, small forward speed, and check the sign convention before anything else. One wheel backwards is a sign bug, not a motor bug.
- **Check the boost converter fourth:** no wake pin, no motor power. It is the first suspect for "nothing moves", not the code.
- **Tune slowly fifth:** watch the measured RPM, not the wheels. The numbers tell you what the eye cannot, and the eye cannot see a deadband or a slow rate limit.
- **Run the safety case sixth:** stop the producer, cut the network, put a hand in front of a sensor. Confirm the designed safe state and the diagnostic code.
- **Teacher info, one slot per group on the shared robot:** enforce the order. A group that starts by tuning control gains will spend the slot debugging a sleeping boost converter.
- **Watch for:** the robot moving unexpectedly while someone reads the screen. Stop the run, say the rule again, and continue. Nobody is exempt.

## Check your understanding - which files change when the command source moves to the network

- **Only the producer changes:** that is the task generating commands from a timer, plus one line in the start-up that creates the network task instead of the fake one.
- **Nothing below the queue may change:** the control task, the components, the HAL, the drive mapping and the queue type or length all stay. The queue is the seam, and the seam is exactly what the design promised.
- **The signal that something is wrong:** if the control task needs to know where commands come from, the layering has leaked and the design should be corrected before the demo, not after.
- **A legitimate exception worth naming:** the freshness limit may change, because a network source is slower and more variable than a timer. That is a parameter of the design, not a structural change, and it should be stated.
- **Teacher info, four minutes:** this is the cleanest question in the chapter for testing whether the group understands their own architecture. Expect "only the producer" and probe for confidence.

## Try it yourself - five minutes driving and the worst period

- **What to record:** the single worst period during real driving, with the time stamp, and then what the task was doing at that instant.
- **How to investigate:** line up the measurement with the log lines around it, then eliminate candidates one at a time. The usual answers are a sensor read that timed out, a network publish, a flash access, or a print that filled a buffer.
- **What to conclude:** either the cause is removed, or it is bounded and the margin is documented. "It was only once" is not a conclusion.
- **Teacher info, fifteen minutes, done by the group:** the investigation is the exercise, so do not hand over the answer. If they are stuck, ask which calls have a worst case larger than one millisecond.
- **Watch for:** measuring while the robot is idle. An idle measurement proves nothing, and the report requirements say so explicitly.

## Check your understanding - nine point six milliseconds of execution in ten milliseconds

- **Not acceptable, even though it technically meets the deadline:** ninety six percent load leaves no room for jitter, for a higher priority preemption, or for one more line of code. The next change breaks it.
- **What to change first:** the largest single cost in the task, found by bisecting with the timer rather than by guessing. In practice it is a print, an allocation, or a sensor read that belongs in another task.
- **The target to state:** enough headroom that the worst case plus jitter is comfortably inside the period, which in practice means half the period or better for a control loop.
- **A second reason to fix it:** the measured worst case is a sample, not a bound. A workload at ninety six percent is one scheduling hiccup away from a visible failure, and the hiccup will happen in the demo.
- **Teacher info, four minutes:** reject "it fits, so it is fine". Ask what happens when the sensor times out in the same run, and the arithmetic does the rest.

## Demo, questions and answers

- **The priority follows the deadline:** shorter deadline means higher priority, and the log task sits below the control task because a late log line costs nothing.
- **The core follows the interference:** either a real time argument, because the task must not be disturbed by the network stack, or an honest "I did not need to pin it, and here is how I checked". Both are passes; pretending is not.
- **The mechanism follows the data shape:** a mutex because several tasks read the latest snapshot and it provides priority inheritance. A queue because each item must be handled once, or an overwrite queue because only the newest command matters. Then say what would break with the other choice.
- **A dead task needs a consequence, a detection and a reaction:** for each task, in that order. Groups that have not thought about one of their tasks should say so and say what they would do.
- **The honest gap beats an invented certainty:** ask specifically for the thing they did not measure, and accept "stack headroom on the network task" as an answer.
- **One more meeting should name the weakest part:** a student who says "nothing, it is finished" has not looked at their own numbers.
- **Teacher info, six minutes per group, hard stop:** three minutes of demo, three of questions. Hold the time, or the last group gets nothing.
- **Watch for:** a demo that only shows the healthy path. Ask for the failure, since the script requires it, and a project that cannot fail has not been tested.

## The report - what a good one contains

- **Personal contribution stated plainly:** which tasks, which mechanisms and which measurements were the student's own. Group work is expected, ownership is what is assessed.
- **The built design, not the designed design:** the task table and data map as they exist, with every difference from the original and one sentence of reason.
- **Numbers with their setup:** worst case, not average, each with the setup line and one log excerpt or screenshot.
- **One failure found and fixed, told as a story:** what broke, how it was noticed, what changed, how the fix was proven. This is the item that separates a report from a protocol.
- **One paragraph of reflection that names a gap:** what would be done differently, and what is still not understood. A report without an admitted gap reads as a report without thought.
- **Teacher info, ten minutes to mark per report:** check for the four required items and for the word "worst". A report containing only averages goes back once, and after that the standard holds.

## Final quiz round - model answers

- **vTaskDelayUntil keeps a stable period:** it waits to an absolute deadline. Neither function helps when the work itself exceeds the period, because the deadline has already been missed before the delay is reached.
- **A mutex blocks, a critical section disables interrupts:** the mutex gives exclusion between tasks with ownership and priority inheritance, and it may block. A critical section freezes the whole chip including the network stack, has no ownership and no inheritance, and is the wrong tool against another task. What a mutex does not give you is protection from code that never takes it.
- **An overwrite queue is right when only the newest value matters:** older values are stale by definition. It silently loses data whenever each item matters, because a producer faster than the consumer drops items without a trace.
- **A changed sequence number proves a new publication happened:** the reader is not acting twice on the same sample. It does not prove the values are valid, it does not prove they are recent in time, and on its own it does not prove the fields are consistent, which is the mutex's job.
- **Priority inversion lets a medium task delay a high one:** a low priority task holds a lock a high priority task needs, and a medium priority task preempts the holder, so the high priority task waits for the medium one. Priority inheritance stops it by temporarily raising the holder to the priority of the highest waiter, which bounds the wait by the critical section.
- **The network task belongs on the other core because it blocks:** the stack blocks for tens of milliseconds and is not deterministic. On the other core its blocking cannot delay the control loop, so the control worst case no longer depends on the network.
- **The safe state is zero duty with both H-bridge inputs low:** or the motor rail disabled. It must be reachable by code that is still alive, meaning the control task or the monitor, without the broken task, and it must survive a watchdog reset by being the state the hardware comes up in.
- **Teacher info, ten minutes for the quiz:** these seven questions are the exam in disguise. A group that answers all seven in one sentence each has understood the lab, and you can say so.

# Cross Chapter Notes for Instructors

## The three sentences students should leave with

- **A real time system promises correct values at the correct time, and the worst case is the number that matters.**
- **Every task blocks, every shared variable has exactly one mechanism, and every actuator has a safe state.**
- **Never claim timing you have not measured, and always report the worst case against a target.**

## The five mistakes that recur every year

- **Averages instead of worst cases:** catches almost every group. Fixed by asking for the maximum in every measurement from the first meeting.
- **A spinning task:** usually introduced to "watch" something. Fixed by pointing at the runtime statistics and the idle task starvation.
- **A pointer returned from a getter:** looks protected and is not, because the reader dereferences after the lock is released. Fixed by requiring the damage test to fail first.
- **A freshness check that is dead code:** the producer never stops, so the rejection path is never executed. Fixed by suspending the producer on purpose.
- **A large port to hardware:** the sign of a HAL boundary that leaked. Fixed by asking which files changed, and treating the list as a design review.

## Time budget per meeting

| Meeting | Content | Notes |
|---|---|---|
| 1 | Installation, three worlds, layering | Installation runs in parallel, expect overrun |
| 2 | Schematic pages one to four | Print the pages, one copy per group |
| 3 | Toolchain checkpoint, tasks and priorities | The four numbers checkpoint lands here or in meeting 4 |
| 4 | Shared data, mutexes, queues, stress test | The data checkpoint lands here |
| 5 | Control path, diagnostics, health checkpoint | Network milestone if hardware is free |
| 6 | Project design review, simulator build | Design page reviewed before code |
| 7 | Simulator checkpoint, port to hardware | Robot slots assigned at the start |
| 8 | Demos, review, final quiz | Three minutes demo, three minutes questions |

- **Adjust freely:** the table is a plan, not a contract, and the script says so. Groups move at different speeds and nothing later breaks.
- **The two gates that may not move:** the toolchain checkpoint before the task chapter, and the simulator checkpoint before the port. Everything else can slide.

## What to do when a group is stuck

- **Ask what they measured:** a stuck group usually has an opinion and no number. One measurement usually ends the discussion.
- **Ask which task state the system is in:** running, ready, blocked or suspended. The answer narrows the search to one mechanism.
- **Ask what the safe state is:** if they cannot name it, that is the real problem, not the symptom they are describing.
- **Send them to the troubleshooting tables:** the toolchain table and the port failure table exist for exactly these moments, and reading them is faster than guessing.
- **Never debug for them for more than five minutes:** point at the measurement, then leave. The group that learns to measure is the group that finishes.
