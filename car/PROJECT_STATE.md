# car project continuation state

- Project: `car`
- Status: `experimental`
- Last handoff: `2026-10-07 01:21 +0800`
- Stable tag: `car-stable-v1.0.0`
- Stable commit: `9e0046c`
- Release: `https://github.com/clever-max/nuedc_software/releases/tag/car-stable-v1.0.0`
- Stable HEX SHA256: `2385D5A6D3A9B440F299C25CC6266AF414A56DDAA8148D5953E94654970B1526`

## Current objective

在稳定版本基础上复刻 XingShuyu/Car 的灰度特征查表、转弯状态和十字通过逻辑

## Current implementation

LINE30 增加 TRACK/TURN_L/TURN_R/CROSS/LOST；NCHD12 bit0=右、bit11=左；连续3周期确认和重捕获；转弯由编码器速度PID执行；十字默认直行

## Validation

源码静态检查通过；SysConfig未改动且现有配置通过；编译链接通过；car.hex与gray_line_30s_no_gyro.hex已生成并校验；未做实车验证

## Known issues

当前NCHD12仍可能报告gray_bus=NACK1；方向和状态机需在gray_bus=OK后实车验证；固定路线的十字左/右决策表尚未接入

## Next actions

先确认NCHD12通信，再架空采集line_mode，最后低速验证直角和十字

## Handoff log

- `2026-10-07 01:21 +0800` — 完成灰度特征查表、转弯状态机和十字直行通过复刻 (commit `2cb569e`, branch `codex/car-line-tracking-replica`).

- `2026-10-07` — Stable firmware/tag/release recorded; NACK1 remains the next hardware investigation.
