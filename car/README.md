# Car closed-loop track validation (AT8236 + MPU6050)

## Current motor, encoder, MPU6050, gray sensor and buzzer wiring

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
| PB2 / I2C1_SCL | MPU6050 SCL |
| PB3 / I2C1_SDA | MPU6050 SDA |
| PB1 GPIO interrupt | MPU6050 INT |
| PA28 | NCHD12 SCL (software I2C) |
| PA31 | NCHD12 SDA (software I2C) |
| PB27 GPIO | passive buzzer |

The current project no longer uses H8 for motor PWM. AT8236 logic inputs must be wired directly to the four PA PWM pins above, with common ground. The active attitude source is the raw MPU6050 on I2C1 at address `0x68`; keep the chassis still during startup bias calibration. The external CH340 debug port is UART1: CH340 TXD→PB5/RX and CH340 RXD→PB4/TX. See [MPU6050 notes](docs/analysis/MPU6050_notes.md).

## Demo behavior

The active mission is an encoder-PID plus 12-channel gray-position line-following route. Press B21 or send `RUNPID`/`RUN15` over UART1 to start the first curve immediately, follow the straight, detect the second curve and stop after the second approximately 170° heading change. The MPU6050 must pass startup calibration. Press B21 again or send `STOP` to abort. UART1 is 115200-8-N-1; telemetry is about 10 Hz.

The user reports that the left-wheel forward direction is correct. The BSP inverts Motor B's electrical polarity so positive speed targets mean physical forward for both wheels. The route uses gray position correction in all three track sections and MPU6050 Z-axis angle only for section transitions and stopping.

## Project baseline

- CCS Theia project based on the workspace MSPM0G3507 starter.
- Device MSPM0G3507, TI Arm Clang, MSPM0 SDK 2.11.0.07.
- SysConfig source: `car.syscfg`; entry point: `car.c`.
- Project modules: `app/` (scheduler), `mission/` (track state machine), `protocol/` (UART console), `bsp/` (motor/encoder/gray/MPU6050/buzzer), `control/` (twin wheel incremental PID).
- PID starting values are conservative commissioning placeholders and must be tuned from telemetry; they are not a completed physical calibration.
- User-supplied wheel diameter: 65 mm. The user confirms 1:28 gearing and 12 V motor voltage, and specifies Hall encoder 13 PPR. The active route uses the encoder estimate documented in [MG513X Hall notes](docs/analysis/MG513X_Hall_notes.md).

See [docs/README.md](docs/README.md) for source documents and [docs/analysis/serial_debug.md](docs/analysis/serial_debug.md) for serial operation. The earlier PID trace is retained as historical calibration evidence, not the current firmware behavior.

The old open-loop turn-forward-reverse tutorial is retained as [historical documentation](docs/analysis/write_this_drive_sequence.md); it is not the active route.
