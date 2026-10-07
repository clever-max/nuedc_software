# car_stable_8494ded project continuation state

This file is the committed handoff point for a new Codex conversation. Read it
with the project `AGENTS.md`, the project `README.md`, and the current Git
status before changing files.

- Project: `car_stable_8494ded`
- Status: `experimental`
- Last handoff: `2026-10-07 11:29 +0800`
- Last recorded branch: `main`
- Last recorded commit: `f89dcc3`

## Current objective

在 8494ded 稳定循迹基线上逐步增大轮速与循迹 PID 参数，并通过实车遥测评估稳定性。

## Current implementation

独立分支 codex/pid-tune-8494ded；WHEEL_PID_KP=1.20、LINE_PID_KP=5.0、LINE_PID_KD=0.05；LINE_CORRECTION_LIMIT_MM_S=60、LINE_INTEGRAL_LIMIT=20 保持不变。构建脚本：car/tools/build_validate_hex.ps1。

## Validation

check_syscfg.py 静态检查通过（提示 SysConfig 生成文件缺失，因为该提交的 Debug 未纳入 Git）；SysConfig 生成成功，存在 PWM 寄存器保持性信息提示；TI Arm Clang 编译成功；链接成功；Intel HEX 校验通过，两个固件文件 SHA256=92F42FC1FD7A8283F18F92C16489E85468A0668A347FB1EF5EADBF67A7638160；未烧录，未进行实车与串口验证。

## Known issues

该副本通过复制已生成 Debug 目录进行构建，makefile 含原工作区绝对路径；仅作为当前机器构建手段，后续应在 CCS 中重新生成/导入项目设置。实车循迹效果未验证。

## Next actions

先在架空状态下烧录 car/firmware/gray_line_30s_no_gyro.hex，检查左右速度与 gray_bus；再在同一赛道及遮光条件下实测循迹和遥测。

## Handoff log

- `2026-10-07 11:29 +0800` — 基于 8494ded 建立独立副本并增大 PID 参数；HEX 已生成校验，等待实车验证。 (commit `f89dcc3`, branch `main`).

- `2026-10-07 11:29 +0800` — state file initialized.
