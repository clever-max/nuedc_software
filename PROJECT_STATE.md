# car experimental branch continuation state

- Project: `car_stable_8494ded`
- Status: `experiment`
- Last handoff: `2026-10-07 16:30 +0800`
- Baseline: `ec47eb45ff920c17ffb63ff5ab716e1d616d70e8`
- Branch: `codex/pid-tune-8494ded`
- Main branch remains on the validated ec47 baseline with the original 320 mm/s wheel target limit.

## Current objective

Test 350 mm/s base speed with P and nonzero D gains scaled in proportion to the speed increase from the 300 mm/s experiment.

## Current implementation

`REFERENCE_BASE_SPEED_MM_S=350`, `WHEEL_SPEED_MAX_MM_S=500`, `WHEEL_PID_KP=2.92`, `REFERENCE_TRACK_KP=274`, `REFERENCE_TRACK_KD=14`, `LINE_PID_KP=11.9`, and `LINE_PID_KD=0.105`. The gains were scaled by `350/300` from the previous experiment. Integral gains, zero wheel D gain, speed ramp, correction limits, sampling, and sensor settings remain unchanged.

## Validation

SysConfig static check passed. SysConfig generated successfully with the existing PWM STOP/STANDBY register retention information notice. TI Arm Clang compilation, linking, Intel HEX generation, and HEX validation passed. `car.hex` and `car/firmware/gray_line_30s_no_gyro.hex` SHA256: `64DE0936E9E5E3D387620E5FFAE2A99698723B7ECC1B1CCEC5903E34AF77B79A`. No flash or physical-board test has been performed in this handoff.

## Known issues

The P/D scaling is aggressive and may cause wheel-speed overshoot or line-following oscillation. The 500 mm/s target ceiling is a software target limit, not a promise that the motors can reach that speed; PWM remains limited to 1000 permille.

## Next actions

Flash this experimental HEX with wheels raised first; then compare against ec47 under identical track, lighting, battery, sensor height, and start conditions. Record wheel speeds, PWM permille, gray bus status, and whether the same bend is tracked.

## Handoff log

- `2026-10-07 16:30 +0800` — Raised base speed from 300 to 350 mm/s and scaled nonzero P/D gains by 350/300; build and HEX checks passed. Physical test pending.
