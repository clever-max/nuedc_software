# Historical COM8 speed trace — 2026-10-01 (old encoder scale)

## Conditions

- CH340E COM8, 115200 8-N-1.
- User confirmed J5 E1A/E1B/E2A/E2B→PB0/PB1/PB2/PB3, 3.3 V encoder supply and common ground; the drive wheels were raised.
- This trace is from the previously flashed dual-wheel controller, before the new root-guide single-wheel calibration commands were flashed. Gear ratio and distance scaling were still provisional.

## Observation

At a 200 mm/s target, the trace showed Motor A/E1 at about 20 mm/s with PWM at 650/1000, while Motor B/E2 was about 34 mm/s with PWM around 515/1000. By 12.4 s, the reported positions were about 98 mm and 213 mm respectively. E1 also had occasional negative 5 ms speed samples.

The old interpreted speed difference cannot be trusted because its tick-to-distance factor was based on 500 PPR. Do not use this trace to justify the present fixed PWM trims or future PID tuning.

## Root-guide calibration sequence

The workspace root guide's one-wheel-first calibration sequence is a future option, not the current firmware interface. Current commands are documented in [serial_debug.md](serial_debug.md).

Reference: [workspace algorithm and learning guide](../../../docs/ALGORITHM_AND_LEARNING_GUIDE.md), section 4.5.

