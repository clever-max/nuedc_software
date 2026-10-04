# Car closed-loop right-turn route (AT8236)

## Current motor, encoder, JY61S UART, gray sensor and buzzer wiring

| MCU pin / peripheral | AT8236 / sensor signal |
| --- | --- |
| PA0 / TIMG8_C1 | AIN1 |
| PA1 / TIMG8_C0 | AIN2 |
| PA8 / TIMA0_C0 | BIN1 |
| PA9 / TIMA0_C1 | BIN2 |
| PA27 encoder interrupt | E1A |
| PA25 encoder input | E1B |
| PB25 encoder interrupt | E2A |
| PB20 encoder input | E2B |
| PB15 / UART2_TX | JY61S RX |
| PB16 / UART2_RX | JY61S TX |
| PA28 | NCHD12 SCL (software I2C) |
| PA31 | NCHD12 SDA (software I2C) |
| PB27 GPIO | passive buzzer |

The current project no longer uses H8 for motor PWM. AT8236 logic inputs must be wired directly to the four PA PWM pins above, with common ground. JY61S is now read through UART2; set it to UART mode at 115200 baud. UART0 PA10/PA11 remains the external CH340 debug port. Keep the car still during UART gyro bias calibration. See [JY61S notes](docs/analysis/JY61S_notes.md).

## Demo behavior

The active mission is an encoder-PID plus 12-channel gray-position line-following demo. Press B21 or send `RUNPID`/`RUN15` over UART0 to run for 15 s at a 200 mm/s wheel target. Gray correction changes the two wheel targets; the gyro is retained on UART2 but disabled as a start condition. Press B21 again or send `STOP` to abort. UART0 is 115200-8-N-1; telemetry is about 10 Hz.

The user reports that the left-wheel forward direction is correct. The BSP inverts Motor B's electrical polarity so positive speed targets mean physical forward for both wheels. The active demo does not require JY61S calibration; if UART2 frames are present they are still parsed for future gyro-enabled missions.

## Project baseline

- CCS Theia project based on the workspace MSPM0G3507 starter.
- Device MSPM0G3507, TI Arm Clang, MSPM0 SDK 2.11.0.07.
- SysConfig source: `car.syscfg`; entry point: `car.c`.
- Project modules: `app/` (scheduler), `mission/` (gray line/PID state machine), `protocol/` (UART console), `bsp/` (motor/encoder/gray/JY61S/buzzer), `control/` (twin wheel incremental PID).
- PID starting values are conservative commissioning placeholders and must be tuned from telemetry; they are not a completed physical calibration.
- User-supplied wheel diameter: 65 mm. The user confirms 1:28 gearing and 12 V motor voltage, and specifies Hall encoder 13 PPR. The active route uses the encoder estimate documented in [MG513X Hall notes](docs/analysis/MG513X_Hall_notes.md).

See [docs/README.md](docs/README.md) for source documents and [docs/analysis/serial_debug.md](docs/analysis/serial_debug.md) for serial operation. The earlier PID trace is retained as historical calibration evidence, not the current firmware behavior.

The old open-loop turn-forward-reverse tutorial is retained as [historical documentation](docs/analysis/write_this_drive_sequence.md); it is not the active route.
