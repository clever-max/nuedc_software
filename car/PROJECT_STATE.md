# car project continuation state

- Project: `car`
- Status: `experimental`
- Last handoff: `2026-10-07 10:21 +0800`
- Stable tag: `car-stable-v1.0.0`
- Stable commit: `9e0046c`
- Release: `https://github.com/clever-max/nuedc_software/releases/tag/car-stable-v1.0.0`
- Stable HEX SHA256: `2385D5A6D3A9B440F299C25CC6266AF414A56DDAA8148D5953E94654970B1526`

## Current objective

Preserve the stable no-gyro 30-second NCHD12 gray-line firmware while keeping a reproducible path for later sensor and cornering work.

## Current implementation

当前 demo_mission.c 使用用户调参 WHEEL_PID_KP=1.20、LINE_PID_KP=5.0、LINE_PID_KD=0.05；使用 car/tools/build_validate_hex.ps1 独立生成 HEX

## Validation

脚本实际成功：SysConfig、编译、链接、Intel HEX 和 alias 校验通过；car.hex SHA256=92F42FC1FD7A8283F18F92C16489E85468A0668A347FB1EF5EADBF67A7638160；未做实车验证

## Known issues

- Do not claim gray-line tracking is physically validated until the telemetry reports `gray_bus=OK` and nonzero `gray=0x....` values.
- The stable firmware is intentionally the 160 mm/s / ±200 mm/s version; experimental faster versions are not the stable reference.

## Next actions

烧录 car/firmware/gray_line_30s_no_gyro.hex 后低速架空观察速度和 gray_bus，再进行实车调参

## Handoff log

- `2026-10-07 10:21 +0800` — 用独立编译脚本验证用户 PID 修改并生成 HEX (commit `84fab1f`, branch `main`).

- `2026-10-07` — Stable firmware/tag/release recorded; NACK1 remains the next hardware investigation.
