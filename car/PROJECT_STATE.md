# car project continuation state

- Project: `car`
- Status: `experimental-baseline`
- Last handoff: `2026-10-07 15:00 +0800`
- Main baseline source: `ec47eb4` (validated by the user)
- Experiment branch: `codex/pid-tune-8494ded`
- Stable tag: `car-stable-v1.0.0` → `8494ded`
- Stable release: https://github.com/clever-max/nuedc_software/releases/tag/car-stable-v1.0.0
- Validated ec47 release: https://github.com/clever-max/nuedc_software/releases/tag/car-ec47-v1.1.0

## Current objective

保留 ec47 可行版本作为 main 的当前车载基线，同时保留 codex/pid-tune-8494ded 实验分支用于后续采样率、速度规划和控制算法研究。

## Current implementation

main 已同步 ec47 的循迹关键代码和固件：基准速度 300 mm/s、轮速上限 320 mm/s、REFERENCE_TRACK_KP=150、REFERENCE_TRACK_KD=12、REFERENCE_TURN_SCALE=2.0、WHEEL_PID_KP=1.60、LINE_PID_KP=6.5、LINE_PID_KD=0.09。实验分支继续保留后续激进 PID 调参历史，不并入 main。

## Validation

已同步用户确认可行的 ec47 源码和两个 Intel HEX。当前 main 的源码、构建和物理行为应以烧录后串口与实车结果继续确认；尚未在本次同步后重新进行物理验证。

## Known issues

灰度刷新率和弯道减速/直道加速尚未实现。不要把实验分支的高速 PID 版本作为当前 main 的稳定固件。

## Next actions

先使用 main 中的 ec47 HEX 做架空和实车回归；后续在实验分支实现灰度采样与速度规划，再通过独立验证后选择性合并。

## Handoff log

- `2026-10-07 15:00 +0800` — main 同步 ec47 关键源代码、轮速上限、两个 HEX 和状态记录；实验分支保留不变。
