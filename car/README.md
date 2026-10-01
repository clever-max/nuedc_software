# Car slow straight demo (AT8236)

## Wiring recorded from the user

| MSPM0G3507 pin | AT8236 input |
| --- | --- |
| PA0 (A0) | AIN1 |
| PA1 (A1) | AIN2 |
| PA8 (A8) | BIN1 |
| PA9 (A9) | BIN2 |

The user corrected the motor variant to MG513X Hall encoders. Motor A E1A/E1B and Motor B E2A/E2B are shown on MOTORC1/2 pins 3/4; firmware currently maps them to PB0/PB1 and PB2/PB3. The connector diagram marks pin 5 as 5V; verify signal voltage compatibility before direct MCU connection.

## Demo behavior

This intentionally has **no PID or active speed correction**. Press B21 or send `RUN15` followed by newline over UART0 to start. The motors ramp up over 0.5 s, run at fixed low open-loop PWM for 15 s, then coast. Press B21 again or send `STOP` to abort and coast immediately. UART0 telemetry is 115200-8-N-1 at about 10 Hz; the board's CH340E USB serial port is used.

The current starting trims are A=400‰ and B=190‰, selected as a cautious estimate from the earlier wheel-up speed trace. They are not calibrated and do not guarantee a straight path or a specific speed/distance; surface, battery, load and motor mismatch can make the car veer. First run on a clear, level area with a hand near the power switch, then adjust the fixed trim only after observing behavior. Do not lift the chassis while the motors are powered.

## Project baseline

- CCS Theia project based on the workspace MSPM0G3507 starter.
- Device MSPM0G3507, TI Arm Clang, MSPM0 SDK 2.11.0.07.
- SysConfig source: `car.syscfg`; entry point: `car.c`.
- Project modules: `app/` (scheduler), `mission/` (15 s demo), `protocol/` (UART console), `bsp/` (motor/encoder), `control/` (twin wheel incremental PID interface).
- The PID module is compiled but inactive. The default mission remains open-loop until wheel count scale, logic levels and low-speed motor response have been verified.
- User-supplied wheel diameter: 65 mm. The user now confirms 1:28 gearing and 12 V motor voltage, and specifies Hall encoder 13 PPR. Encoder telemetry remains an estimate until one wheel revolution is measured; see [MG513X Hall notes](docs/analysis/MG513X_Hall_notes.md).

See [docs/README.md](docs/README.md) for source documents and [docs/analysis/serial_debug.md](docs/analysis/serial_debug.md) for serial operation. The earlier PID trace is retained as historical calibration evidence, not the current firmware behavior.


