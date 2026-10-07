# car_stable_8494ded project continuation state

This file is the committed handoff point for a new Codex conversation. Read it
with the project AGENTS.md, README.md, and current Git status before changing files.

- Project: `car_stable_8494ded`
- Status: `experimental`
- Last handoff: `2026-10-07 14:00 +0800`
- Last recorded branch: `codex/pid-tune-8494ded`
- Last recorded commit: pending (source/artifact changes below)

## Current objective`r`n`r`n恢复可运行的 `ec47eb4` 版本，保留其可复查的回滚历史；准备把 `8494ded` 的真实稳定固件作为 `car-stable-v1.0.0` 发布资产。`r`n`r`n## Current implementation

独立分支 `codex/pid-tune-8494ded`；`REFERENCE_BASE_SPEED_MM_S=200`、`REFERENCE_SPEED_RAMP_MM_S2=500`；`WHEEL_PID_KP=1.30`、`LINE_PID_KP=5.5`、`LINE_PID_KD=0.06`；`LINE_CORRECTION_LIMIT_MM_S=70`、`LINE_INTEGRAL_LIMIT=20`；`WHEEL_SPEED_MAX_MM_S=200` 保持安全上限。构建脚本：`car/tools/build_validate_hex.ps1`。

## Validation`r`n`r`n回滚后源码树与 `ec47eb4` 一致；静态检查、SysConfig、TI Arm Clang 编译、链接、Intel HEX 和 alias 校验通过。当前 ec47 固件 SHA256 为 `F91C9A22705D91D92F4375ED1A377EADB5F363EA0EC421A551FFC57524FF5D40`；尚未进行物理验证。`r`n`r`n## Known issues

该副本通过当前机器已有的 Debug 构建目录完成构建，makefile 含原工作区绝对路径；后续应在 CCS 中重新生成/导入项目设置。实车循迹效果未验证。

## Next actions

烧录 `car/firmware/gray_line_30s_no_gyro.hex`，先架空观察速度是否接近 300 mm/s、左右速度差和 `gray_bus`，再在相同赛道上进行遮光/日光对照测试。

## Handoff log`r`n`r`n- `2026-10-07 14:00 +0800` — 用户实测确认 `ec47eb4` 可行；已创建并推送 tag `car-ec47-v1.1.0`，发布独立 GitHub Release 并上传两个 ec47 HEX，SHA256=`F91C9A22705D91D92F4375ED1A377EADB5F363EA0EC421A551FFC57524FF5D40`。`r`n`r`n- `2026-10-07 13:45 +0800` — `car-stable-v1.0.0` 已强制指向 `8494ded`，release 描述和两个 HEX 资产已替换并下载校验通过；SHA256=`2385D5A6D3A9B4402D7FB24FE4D328F7668082EC420FA9BB012B307AF2061EDF`。`r`n`r`n- `2026-10-07 13:30 +0800` — 通过反向提交恢复到 `ec47eb4` 的源码/固件状态；ec47 HEX 已重新编译校验。GitHub 稳定 release 资产已切换到 `8494ded` 的真实固件。`r`n`r`n- `2026-10-07 12:15 +0800` — 基准速度提高到 300 mm/s；提高参考循迹 KP/KD、转向比例、轮速 KP、轮速上限和带陀螺仪循迹参数；HEX 已重新生成并校验，等待架空验证。`r`n`r`n- `2026-10-07 12:00 +0800` — 基准速度提高到 220 mm/s；提高参考循迹 KP/KD、转向比例、轮速 KP 和速度上限，HEX 已重新生成并校验，等待架空和实车验证。

- `2026-10-07 11:45 +0800` — 基准速度提高到 200 mm/s，并适度提高轮速/循迹 PID、启动斜坡和修正上限；HEX 已重新生成并校验，等待架空和实车验证。
- `2026-10-07 11:35 +0800` — 从 `8494ded` 独立分支调高 WHEEL/LINE PID，保留修正限幅和积分限幅；SysConfig、编译、链接和 HEX 校验通过，等待实车验证。 (commit `8b5d3eb`, branch `codex/pid-tune-8494ded`).





