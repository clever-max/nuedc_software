# MG513X Hall encoder notes

## Confirmed motor variant and parameters

The user corrected the earlier identification: the installed motors are **MG513X with Hall encoders**, not GMR. The user-provided specifications are:

| Parameter | Value | Evidence/status |
| --- | --- | --- |
| Gear ratio | 1:28 | User supplied [2][3]; not independently checked in the local mechanical drawings |
| Rated voltage | 12 V | User supplied [2][3]; user explicitly says this is 12 V, not 11–16 V |
| Rated current / stall current | 0.36 A / 3.2 A | User supplied [2][3] |
| Rated torque | 1 kgf·cm (some tables 1.02 kgf·cm) | User supplied [2][3] |
| Stall torque | 4.5 kgf·cm | User supplied [2][3] |
| Power / mass | 4 W / 0.11 kg | User supplied [2] |
| Gearbox output no-load / rated speed | 370±12 / 300±12 rpm | User supplied [2][3] |
| Hall encoder | 13 PPR | User supplied [2][3]; source does not give a CPR value |
| Encoder supply range | 3.3–5 V | User supplied [2][3] |
| Encoder cover | Hall version is exposed; GMR version has plastic rear cover | User supplied [2][3] |
| Wheel diameter | 65 mm | User confirmed earlier |

Do not reuse the previous GMR assumptions (500 PPR, 3.3 V supply, GMR power-wire polarity). The local supplied motor drawings are primarily mechanical and do not independently verify these electrical specifications or connector pin order.

## Encoder count model (initial estimate; calibrate before claiming accuracy)

`bsp/encoder.c` counts only **channel A rising edges**, sampling channel B to determine direction. The current estimate therefore uses:

```text
estimated counts per wheel revolution = 13 PPR × 28 = 364 counts
mm per counted edge = π × 65 mm / 364
```

This is a working conversion based on the user's PPR value and current edge mode, not a separately specified CPR. Confirm with a measured wheel revolution before trusting speed or distance telemetry. Do not multiply by quadrature x4 unless the firmware is changed to count all four A/B edges.

## Connector and voltage caution

The user supplied a D157B motor-connector diagram: Motor A/MOTORC1 pins 3 and 4 are E1A/E1B; Motor B/MOTORC2 pins 3 and 4 are E2A/E2B; pin 5 is marked 5V; pins 1 and 6 are the motor outputs. The D157B pinout is also depicted in `../vendor/5.原理图/2.AT8236稳压模块原理图（D157B）.pdf`.

The diagram establishes connector routing and the marked encoder supply, but does not by itself establish whether Hall A/B output highs are 3.3 V, 5 V push-pull, or open-drain. MSPM0G3507 does not make every GPIO 5 V tolerant; only designated pins have 5 V-tolerant open-drain capability. Before connecting the 5 V-powered Hall signals directly to PA27/PA25/PB25/PB20, verify the encoder output circuit/voltage and each MCU pin's tolerance. Use 3.3 V-compatible signaling or suitable level translation where needed. The user's earlier J5→PB mapping and 3.3 V encoder supply were for the previous GMR identification and must not be assumed to describe the current Hall wiring.

## Project implementation status

- The current route uses encoder feedback. E1A/E1B/E2A/E2B are PA27/PA25/PB25/PB20; A-phase rising edges are counted and B phase determines sign.
- Current PWM input wiring is PA0/PA1/PA8/PA9→AIN1/AIN2/BIN1/BIN2, as documented in [wiring.md](wiring.md).
- `bsp/encoder.c` uses the 13 PPR × 28 × 65 mm initial conversion estimate; calibrate one measured wheel revolution before tuning PID.
- `control/wheel_speed_controller.c` now provides the twin-wheel incremental speed PID used by the active route. Its gains are commissioning values and must be tuned from telemetry before claiming calibrated speed.

## References

- User-provided MG513X parameters [2][3]; bibliography URLs/pages were not included in the message, so these are recorded as user-confirmed claims pending independent source verification.
- Supplied D157B schematic: `../vendor/5.原理图/2.AT8236稳压模块原理图（D157B）.pdf`.
- TI MSPM0G3507 datasheet: https://www.ti.com/lit/ds/symlink/mspm0g3507.pdf.
- The Wheeltec motor development manual previously consulted is summarized in the workspace files, but its prior GMR-specific claims are not applicable to this installed Hall variant.
