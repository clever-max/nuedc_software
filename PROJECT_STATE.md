# car experimental branch continuation state

- Project: `car_stable_8494ded`
- Status: `experiment`
- Last handoff: `2026-10-07 16:10 +0800`
- Baseline: `ec47eb45ff920c17ffb63ff5ab716e1d616d70e8`
- Branch: `codex/pid-tune-8494ded`
- Main branch remains on the validated ec47 baseline with the original 320 mm/s wheel target limit.

## Current objective

Test whether the 320 mm/s wheel target clamp causes turn understeer or line loss, using the user-validated ec47 control baseline and changing only the maximum wheel speed.

## Current implementation

The mission code and all PID, base-speed, ramp, line correction, motor, encoder, and sensor settings match `ec47eb4`. The sole functional source difference is `WHEEL_SPEED_MAX_MM_S=500.0f` instead of `320.0f`. No sampling-rate or speed-planning changes are included.

## Validation

SysConfig static check passed. SysConfig generated successfully with the existing PWM STOP/STANDBY register retention information notice. TI Arm Clang compilation, linking, Intel HEX generation, and HEX validation passed. `car.hex` and `car/firmware/gray_line_30s_no_gyro.hex` SHA256: `7B6EF8EA5076C9F7F0F6F5177CF9D92C5E61588CABC86C6A4B5B502A2EC1EC1B`. No flash or physical-board test has been performed in this handoff.

## Known issues

At a 300 mm/s base speed and 90 mm/s steering correction, a 500 mm/s target ceiling is above the nominal targets; actual wheel speed remains limited by the 1000-permille PWM output and motor capability. This change tests software target clipping, not unlimited physical motor speed.

## Next actions

Flash this experimental HEX and compare against ec47 under identical track, lighting, battery, sensor height, and start conditions. Record wheel speeds, PWM permille, gray bus status, and whether the same bend is tracked.

## Handoff log

- `2026-10-07 16:10 +0800` — Rebuilt the ec47 baseline with only the maximum wheel target raised from 320 to 500 mm/s; HEX checks passed. (commit pending)
