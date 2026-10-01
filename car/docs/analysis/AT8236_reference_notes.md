# AT8236 reference notes

## Sources in this project

- `../vendor/1.用户手册与教程视频/电机驱动模块使用手册—AT8236-2025.08.27.pdf`
- `../vendor/1.用户手册与教程视频/AT8236带稳压模块问题排查和检测方法(2025.07.10) .pdf`
- `../vendor/5.原理图/2.AT8236稳压模块原理图（D157B）.pdf`
- `../vendor/6.芯片手册/AT8236芯片手册.PDF`
- `../vendor/4.例程源码/2.Arduino例程/AT8236MotorControlDemo.ino`
- `../vendor/4.例程源码/1.STM32例程/2.D157B例程/使用说明.txt`

## Facts checked in the supplied manual

- AT8236 is a brushed DC motor H-bridge driver. The manual gives 0–100 kHz PWM and recommends 10 kHz.
- In the input-state table, IN1=0 and IN2=0 means outputs high impedance (coast/sleep); IN1=1 and IN2=1 means both outputs low (brake).
- The PWM table describes two-input direction/speed control, including a PWM signal on one input while the other is held high or low. The exact motor direction depends on wiring and the module/output labels.
- In the D157B module section, the stated VM input is 5.5–17 V and VREF is the logic supply, stated as 0.5–4 V. The manual explicitly warns not to interchange them.
- The D157B schematic and example table show their own pinout. This project's PA0/PA1/PA8/PA9 mapping comes from the user, not that example.

## Encoder wiring and closed-loop notes (manual pp. 8, 14, 18–19)

- Page 8 says the D157B has a standard 6-pin motor connector and routes encoder A/B signals to separate output pins. It does not specify whether those A/B outputs are 3.3 V, 5 V push-pull, or open-drain.
- Page 14's D157B-to-STM32F103C8T6 table maps E2B→B6, E2A→B7, E1B→A1 and E1A→A0. It also lists D157B 5V→STM32 5V and GND→GND. These MCU pin assignments are specific to that example and must not replace the current MSPM0 SysConfig pin map. The 5V supply row does not establish the A/B signal high level.
- Pages 18–19 describe the STM32 encoder mode using timer CH1/CH2 as A/B, then show a PID example. The manual explicitly characterizes that code as feedback from encoder counts and says it does **not** calculate the encoder-to-speed relationship, so it is a closed-loop concept example rather than a calibrated wheel-speed controller.
- Page 19 says a true speed loop needs the count during a known interval, read frequency, counting multiplier, gear ratio, encoder resolution and wheel circumference. In explicit units, use `v_mm_s = delta_count / counts_per_wheel_rev × wheel_circumference_mm / dt_s`. Define `counts_per_wheel_rev` using the actual PPR/CPR meaning and edge-counting mode; verify it against one measured wheel revolution.
- The manual therefore gives a useful wiring route and the steps needed to construct a speed estimate, but it does **not** answer the Hall A/B voltage-compatibility question. Check the motor output level and MSPM0 pin tolerance separately before direct connection.

These notes summarize the supplied AT8236 module manual and D157B schematic. Check the PDF directly before selecting supplies or configuring a motor-control mode.

## Example behavior (not copied as MSPM0 firmware)

The supplied Arduino sketch uses `analogWrite` for AIN1/AIN2/BIN1/BIN2, serial input for a signed PWM value, and periodically reverses both motors. Its `Set_PWMA` and `Set_PWMB` functions hold one input high and PWM the other (a slow-decay style described in the manual). It also includes encoder interrupts, speed calculations, battery ADC reporting, and Arduino-specific pin definitions.

The D157B STM32 instructions say its example can run open-loop or closed-loop and outputs debug data over UART. The accompanying STM32 project and encoder examples are not MSPM0G3507 code. They must not be transplanted without checking timer/PWM mux, interrupt behavior, pinout and board-specific power details.

## Adaptation cautions

- Start with motor outputs physically unloaded/lifted and use a current-limited, correctly rated supply when eventually testing.
- Establish input states and test one bridge at a time before driving both wheels.
- Do not copy the sample's periodic direction reversal into the car project; reversal while spinning can cause abrupt braking/current transients.
- Verify whether low/low coast behavior is acceptable as the project's startup state for the exact module variant.
- UART0 telemetry is configured on PA10/PA11 at 115200 baud through the Tianmengxing board's CH340E.
- Encoder GPIO mapping PB0-PB3 is currently configured as A/B inputs, but the current Hall harness-to-GPIO path must be confirmed. The user-provided motor connectors list E1/E2 signals and a 5V pin; confirm signal voltage and pin tolerance before connection.
- No battery ADC input is assigned. Motor direction polarity remains configurable in `car.c` and must be checked with the wheels off the floor.

## Package version observation

The supplied directory is named V2.8 dated 2026-07-25, while its included update log also lists entries through 2026-08-21 and the D157B archive filenames include HAL examples dated 2026-08-18. Treat the filenames and individual documents as the revision evidence; the folder name alone does not describe the newest item in the folder.

