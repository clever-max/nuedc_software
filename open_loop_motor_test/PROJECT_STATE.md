# open_loop_motor_test project continuation state

- Project: `open_loop_motor_test`
- Status: `test`
- Last handoff: `2026-10-07`
- Stable artifact: `open_loop_motor_test/car.hex`

## Current objective

Keep an isolated five-second fixed-PWM AT8236 test for motor wiring and direction.

## Current implementation

- Entry: `car.c`; SysConfig source: `car.syscfg`.
- PA0/PA1/PA8/PA9 drive AIN1/AIN2/BIN1/BIN2; UART1 uses PB4/PB5 at 115200-8-N-1.
- B21 or `RUN15`/`RUNPID` starts fixed PWM for five seconds; `STOP` aborts.
- Encoders, gray sensor, MPU6050, JY61S, and speed PID files are not called by this project.

## Validation

- An Intel HEX artifact exists in this project.
- Test only with wheels raised and current limited; physical direction must be recorded separately from the closed-loop car project.

## Known issues

- Do not mix this project's UART1 and five-second behavior with `car`'s board UART0 and gray-line mission.

## Next actions

- Use this project to confirm motor supply, AT8236 input mapping, and physical forward direction before closed-loop changes.

## Handoff log

- `2026-10-07` — Initial committed continuation state created.
