# car experimental branch continuation state

- Project: `car_stable_8494ded`
- Status: `experiment`
- Last handoff: `2026-10-07 16:20 +0800`
- Baseline: `ec47eb45ff920c17ffb63ff5ab716e1d616d70e8`
- Branch: `codex/pid-tune-8494ded`
- Main branch remains on the validated ec47 baseline with the original 320 mm/s wheel target limit.

## Current objective

Test aggressive proportional-gain scaling alongside the 500 mm/s wheel target ceiling, using ec47's 300 mm/s base-speed setup.

## Current implementation

`REFERENCE_BASE_SPEED_MM_S=300`, `WHEEL_SPEED_MAX_MM_S=500`, `WHEEL_PID_KP=2.50`, `REFERENCE_TRACK_KP=235`, and `LINE_PID_KP=10.2`. The three Kp values were scaled by approximately `500/320=1.5625` from ec47. Other I/D gains, speed ramp, correction limits, sampling, and sensor settings remain at ec47 values.

## Validation

SysConfig static check passed. SysConfig generated successfully with the existing PWM STOP/STANDBY register retention information notice. TI Arm Clang compilation, linking, Intel HEX generation, and HEX validation passed. `car.hex` and `car/firmware/gray_line_30s_no_gyro.hex` SHA256: `2D6CE62D3584BE27F0740BAC4CB94DF108BD3FEE6A86FB09B7F2B6C5FE30CD7D`. No flash or physical-board test has been performed in this handoff.

## Known issues

The Kp scaling is deliberately aggressive and may cause wheel-speed overshoot or line-following oscillation. The 500 mm/s target ceiling is a software target limit, not a promise that the motors can reach that speed; PWM remains limited to 1000 permille.

## Next actions

Flash this experimental HEX with wheels raised first; then compare against ec47 under identical track, lighting, battery, sensor height, and start conditions. Record wheel speeds, PWM permille, gray bus status, and whether the same bend is tracked.

## Handoff log

- `2026-10-07 16:20 +0800` — From ec47's 300 mm/s setup and 500 mm/s target ceiling, scaled wheel, reference-tracking, and line PID Kp values by about 1.56; build and HEX checks passed. Physical test pending.
