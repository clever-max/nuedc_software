# Project instructions

- Shared MSPM0 skill: `../skills/mspm0-ccs/SKILL.md`.
- Treat `breathing_led.syscfg` as the source of truth for pins, clock, PWM, and generated names.
- Active source: `breathing_led.c`; target: `targetConfigs/MSPM0G3507.ccxml`.
- Do not edit SysConfig generated files or build outputs by hand.
- Preserve MSPM0G3507, CCS Theia, TI Arm Clang, XDS110, SDK 2.11.0.07, and declared SysConfig version.
- After `.syscfg` edits, run the shared `run_sysconfig.py` before rebuilding.
- Report source checks, SysConfig generation, compilation, linking, and physical hardware validation separately.
- For Tianmengxing board schematic, pin-map, TI documentation and CCS/Keil examples, use the workspace-wide `../docs/reference/Tianmengxing/INDEX.md`; search its `text/` and `examples/` folders with `rg`.

## 对话接续

新对话先读取 `PROJECT_STATE.md`，再运行 `python ../tools/project_context.py show breathing_led`。完成一个阶段后，用同一工具更新状态并提交状态文件；通用规则见 `../docs/CONTINUATION_WORKFLOW.md`。

## 代码提交硬约束

凡涉及源码、SysConfig、项目元数据、构建脚本、校验脚本或固件镜像的改动，交付前必须创建 Git commit；未提交的代码改动不能报告为完成。相关改动可以合并为一个提交。完成阶段时同步更新 `PROJECT_STATE.md`，并运行 `python ../tools/project_context.py guard <project>`。
