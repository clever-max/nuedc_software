# car project continuation state

- Project: `car`
- Status: `rectangle-demo-implemented`
- Last handoff: `2026-10-07 14:43 +0800`
- Main baseline source: `ec47eb4` (validated by the user)
- Experiment branch: `codex/pid-tune-8494ded`
- Stable tag: `car-stable-v1.0.0` → `8494ded`
- Stable release: https://github.com/clever-max/nuedc_software/releases/tag/car-stable-v1.0.0
- Validated ec47 release: https://github.com/clever-max/nuedc_software/releases/tag/car-ec47-v1.1.0

## Current objective

在保留 ec47 普通循迹基线的基础上，提供 B21 两阶段演示：第一次按键运行普通灰度循迹 30 秒并停车，第二次按键运行带自动左右直角识别和差速转弯的矩形跑道循迹 30 秒。

## Current implementation

main 保留 ec47 的普通循迹参数：基准速度 300 mm/s、轮速上限 320 mm/s、REFERENCE_TRACK_KP=150、REFERENCE_TRACK_KD=12、REFERENCE_TURN_SCALE=2.0、WHEEL_PID_KP=1.60、LINE_PID_KP=6.5、LINE_PID_KD=0.09。新增 `RECTANGLE` 任务把 NCHD12 12 路位图分组为 8 个虚拟通道，连续确认左右外侧特征后原地差速转弯，中心线连续恢复后回到循迹；新增 `RUNRECT` 串口命令用于直接启动第二阶段。

## Validation

源码静态检查、SysConfig 1.26.2 生成、TI Arm Clang 编译、链接和 Intel HEX 校验均已完成。HEX 为 `car/car.hex`；尚未烧录，也未在连接的实车上验证直角特征和转向极性。

## Known issues

8 路虚拟通道分组和左右传感器方向仍需通过串口 `gray`/`gray8` 与实车样本确认。直角重捕获超时后进入低速丢线搜索；未完成物理标定前不要提高转弯速度。

## Next actions

先烧录 `car/car.hex` 做架空测试，确认 `LINE30` → 停车 → `RECT30` 按键顺序和 `TURNL/TURNR` 遥测，再在矩形跑道上检查四个直角的重捕获；根据样本调整 12→8 分组和转弯速度。

## Handoff log

- `2026-10-07 15:00 +0800` — main 同步 ec47 关键源代码、轮速上限、两个 HEX 和状态记录；实验分支保留不变。
- `2026-10-07 14:43 +0800` — 增加 B21 两阶段 LINE30/RECT30 演示、12 路到 8 路灰度迁移、左右直角状态机、RUNRECT 命令和可烧录 `car.hex`；源码与构建已验证，等待实车确认。
