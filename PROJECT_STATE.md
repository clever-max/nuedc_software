# car experimental branch continuation state

- Project: `car_stable_8494ded`
- Status: `experiment`
- Last handoff: `2026-10-07 16:55 +0800`
- Baseline: `ec47eb45ff920c17ffb63ff5ab716e1d616d70e8`
- Branch: `codex/pid-tune-8494ded`
- Main branch remains on the validated ec47 baseline with the original 320 mm/s wheel target limit.

## Current objective

Preserve the user-tested 420 mm/s firmware as Release `car-ec47-v1.2.0`; retain the experiment branch for further tuning.

## Current implementation

`REFERENCE_BASE_SPEED_MM_S=420`, `REFERENCE_SPEED_RAMP_MM_S2=840`, `WHEEL_SPEED_MAX_MM_S=600`, `WHEEL_PID_KP=3.07`, `REFERENCE_TRACK_KP=288`, `REFERENCE_TRACK_KD=18`, `LINE_PID_KP=12.5`, and `LINE_PID_KD=0.135`. Compared with the 350 mm/s experiment, P gains rose about 5% and nonzero line/track D gains rose about 29%. Wheel PID D remains zero. Integral gains, correction limits, sampling, and sensor settings remain unchanged.

## Validation

SysConfig static check passed. SysConfig generated successfully with the existing PWM STOP/STANDBY register retention information notice. TI Arm Clang compilation, linking, Intel HEX generation, and HEX validation passed. The user reports successful vehicle testing. GitHub Release `Car Stable_SpeedUP v1.2.0` is published from commit `d2094da` with both HEX files; downloaded release assets were SHA256 verified as `D6332CEFECB7B90E7195C03D552DD6CC00AA3B205E37B145E64079CA0F72F6D6`.

## Known issues

The 420 mm/s target, 600 mm/s wheel ceiling, and stronger derivative action are aggressive and may cause oscillation or PWM saturation. Wheel PID D remains zero because encoder-derived speed differentiation is noise-sensitive; steering D is the primary damping increase. PWM remains limited to 1000 permille.

## Next actions

Flash this experimental HEX with wheels raised first; then compare against ec47 under identical track, lighting, battery, sensor height, and start conditions. Record wheel speeds, PWM permille, gray bus status, and whether the same bend is tracked.

## Handoff log

- `2026-10-07 16:55 +0800` — Published `car-ec47-v1.2.0`, matching the previous `Car Stable_SpeedUP` release title convention; uploaded HEX assets and verified downloaded hashes. User confirmed vehicle test success.
- `2026-10-07 16:40 +0800` — Raised base speed to 420 mm/s, ramp to 840 mm/s², wheel target ceiling to 600 mm/s, modestly raised P, and more strongly raised steering D; build and HEX checks passed. Physical test pending.
