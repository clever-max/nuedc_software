# Project Instructions

## MSPM0 CCS Development

For all work in this project, read and apply the local MSPM0 skill before editing code or configuration:

`C:\Users\ASUS\workspace_ccstheia\mspm0-ccs\SKILL.md`

Use the skill's bundled scripts with absolute paths when needed. In particular:

- Validate the project with `scripts/check_syscfg.py`.
- Validate SysConfig output with `scripts/run_sysconfig.py` before rebuilding after `.syscfg` changes.
- On this machine, pass `--tool D:\TI\CCS\ccs\utils\sysconfig_1.28.1\sysconfig_cli.bat --product D:\TI\CCS\mspm0_sdk_2_11_00_07\.metadata\product.json --compiler ticlang` because SysConfig is not on PATH.
- Report the version warning when using SysConfig 1.28.1: `.cproject` declares SysConfig 1.26.2; do not change either version declaration without explicit user approval.
- Treat `empty.syscfg` as the source of truth for pins, clocks, peripherals, DMA, and interrupts.
- Do not hand-edit generated files: `ti_msp_dl_config.c`, `ti_msp_dl_config.h`, `device_linker.cmd`, `device.opt`, `Debug/`, object files, maps, or `.out` files.
- Preserve the existing MSPM0G3507, CCS Theia, TI Arm Clang, XDS110, SDK 2.11.0.07, and SysConfig metadata unless the user explicitly requests a migration.
- The active project entrypoint is `empty.c`; the target configuration is `targetConfigs/MSPM0G3507.ccxml`.

After changes, report separately whether source checks, SysConfig generation, compilation, linking, and physical-board validation succeeded. Do not claim hardware validation without a connected board.
