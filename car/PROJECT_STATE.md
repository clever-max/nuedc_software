# car project continuation state

- Project: `car`
- Status: `experimental`
- Last handoff: `2026-10-07 10:13 +0800`
- Stable tag: `car-stable-v1.0.0`
- Stable commit: `9e0046c`
- Release: `https://github.com/clever-max/nuedc_software/releases/tag/car-stable-v1.0.0`
- Stable HEX SHA256: `2385D5A6D3A9B440F299C25CC6266AF414A56DDAA8148D5953E94654970B1526`

## Current objective

在稳定版本基础上复刻 XingShuyu/Car 的灰度特征查表、转弯状态和十字通过逻辑

## Current implementation

demo_mission.c 参数含轮速闭环、灰度位置环和特征循迹状态机；car/tools/build_validate_hex.ps1 独立完成 CCS 构建、Intel HEX 生成、校验和 BSL 对齐检查

## Validation

独立脚本实际构建成功；SysConfig、编译、链接、HEX 校验通过；当前 car.hex SHA256=90003E4CFF595DB5380C053DC7130EA3DA590ABD42C0945A2BC44263A4924FE6；本轮未做实车验证

## Known issues

NCHD12 通信与实车循迹仍需硬件验证

## Next actions

用 build_validate_hex.ps1 自行生成；gray_bus=OK 后再实车验证 line_mode 和 PID

## Handoff log

- `2026-10-07 10:13 +0800` — 提交 standalone HEX workflow，并提交 WHEEL_PID_KP=1.0、LINE_PID_KP=5.0、LINE_PID_KD=0.05 的调参版本 (commit `504f293`, branch `codex/car-line-tracking-replica`).

- `2026-10-07 10:12 +0800` — 新增独立 HEX 构建与校验脚本，已用其完成一次干净构建 (commit `7b8503b`, branch `codex/car-line-tracking-replica`).

- `2026-10-07 01:23 +0800` — 补充特征防抖：普通线形会清零未完成的转弯候选计数 (commit `08997e3`, branch `codex/car-line-tracking-replica`).

- `2026-10-07 01:21 +0800` — 完成灰度特征查表、转弯状态机和十字直行通过复刻 (commit `2cb569e`, branch `codex/car-line-tracking-replica`).

- `2026-10-07` — Stable firmware/tag/release recorded; NACK1 remains the next hardware investigation.
