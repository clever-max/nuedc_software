# Project instructions

- Shared MSPM0 skill: `../skills/mspm0-ccs/SKILL.md`.
- Treat `breathing_led.syscfg` as the source of truth for pins, clock, PWM, and generated names.
- Active source: `breathing_led.c`; target: `targetConfigs/MSPM0G3507.ccxml`.
- Do not edit SysConfig generated files or build outputs by hand.
- Preserve MSPM0G3507, CCS Theia, TI Arm Clang, XDS110, SDK 2.11.0.07, and declared SysConfig version.
- After `.syscfg` edits, run the shared `run_sysconfig.py` before rebuilding.
- Report source checks, SysConfig generation, compilation, linking, and physical hardware validation separately.
- For Tianmengxing board schematic, pin-map, TI documentation and CCS/Keil examples, use the workspace-wide `../docs/reference/Tianmengxing/INDEX.md`; search its `text/` and `examples/` folders with `rg`.
