# car project continuation state

- Project: `car`
- Status: `stable`
- Last handoff: `2026-10-07`
- Stable tag: `car-stable-v1.0.0`
- Stable commit: `9e0046c`
- Release: `https://github.com/clever-max/nuedc_software/releases/tag/car-stable-v1.0.0`
- Stable HEX SHA256: `2385D5A6D3A9B440F299C25CC6266AF414A56DDAA8148D5953E94654970B1526`

## Current objective

Preserve the stable no-gyro 30-second NCHD12 gray-line firmware while keeping a reproducible path for later sensor and cornering work.

## Current implementation

- Entry: `car.c`; configuration source: `car.syscfg`; output: `car.hex`.
- AT8236 PWM: PA0/PA1 for AIN1/AIN2 and PA8/PA9 for BIN1/BIN2.
- Encoders: E1A/E1B/E2A/E2B to PA27/PA25/PB25/PB20. Stable polarity is left `0`, right `1`.
- Motor B uses electrical inversion (`MOTOR_B_FORWARD_BIN1_HIGH=0`) so a positive command makes both wheels move forward.
- NCHD12 software I²C: PA29=SCL, PA30=SDA; VCC=5V, V0=3.3V, common GND.
- Board UART0/CH340E: PA10=TX, PA11=RX, 115200-8-N-1. MPU6050 and JY61S are retained but disabled in this firmware.
- B21, `RUN15`, and `RUNPID` start 30 seconds of gray-line control; `STOP` aborts.

## Validation

- The stable tag was pushed to GitHub and released with `car.hex` and `gray_line_30s_no_gyro.hex`.
- SysConfig, compilation, linking, Intel HEX generation, and HEX validation passed for the stable artifact.
- Physical motor direction and encoder signs were observed in the serial log at approximately `speed=160/160`.
- NCHD12 address probing currently reports `gray_bus=NACK1`; this is a physical bus/address issue still requiring correction.

## Known issues

- Do not claim gray-line tracking is physically validated until the telemetry reports `gray_bus=OK` and nonzero `gray=0x....` values.
- The stable firmware is intentionally the 160 mm/s / ±200 mm/s version; experimental faster versions are not the stable reference.

## Next actions

- Verify PA29/PA30 continuity to the NCHD12 SCL/SDA pins, module power, V0=3.3V selection, pull-ups, and address pads.
- After `gray_bus=OK`, collect a short serial trace over centered, moderate, and sharp line positions before changing PID or cornering behavior.

## Handoff log

- `2026-10-07` — Stable firmware/tag/release recorded; NACK1 remains the next hardware investigation.
