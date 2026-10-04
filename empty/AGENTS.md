# Project Instructions

## MSPM0 CCS Development

For all work in this project, read and apply the local MSPM0 skill before editing code or configuration:

`../skills/mspm0-ccs/SKILL.md`

Use the skill's bundled scripts with absolute paths when needed. In particular:

- Validate the project with `scripts/check_syscfg.py`.
- Validate SysConfig output with `scripts/run_sysconfig.py` before rebuilding after `.syscfg` changes.
- If automatic discovery cannot find SysConfig, pass the tool and SDK product through environment variables or local machine configuration; do not write an absolute user-specific path into the project.
- Report the version warning when using SysConfig 1.28.1: `.cproject` declares SysConfig 1.26.2; do not change either version declaration without explicit user approval.
- Treat `empty.syscfg` as the source of truth for pins, clocks, peripherals, DMA, and interrupts.
- Do not hand-edit generated files: `ti_msp_dl_config.c`, `ti_msp_dl_config.h`, `device_linker.cmd`, `device.opt`, `Debug/`, object files, maps, or `.out` files.
- Preserve the existing MSPM0G3507, CCS Theia, TI Arm Clang, XDS110, SDK 2.11.0.07, and SysConfig metadata unless the user explicitly requests a migration.
- The active project entrypoint is `empty.c`; the target configuration is `targetConfigs/MSPM0G3507.ccxml`.
- For LP-MSPM0G3507 board questions, consult the local reference index at `../docs/reference/LP-MSPM0G3507/INDEX.md` and its extracted user-guide text before browsing TI again.

After changes, report separately whether source checks, SysConfig generation, compilation, linking, and physical-board validation succeeded. Do not claim hardware validation without a connected board.

## 天猛星 Wiki 项目知识库

当用户询问本项目、立创·天猛星 MSPM0G3507 开发板、板载资源、引脚、外设、模块移植、CCS/Keil 教程或 PID 训练项目时，先读取本地整理文档：

`../docs/tmx-mspm0g3507-wiki-summary.md`

按以下顺序检索和回答：

1. 先在本地整理文档中按章节、模块名、外设名或关键词定位答案。
2. 使用文档中的原始 Wiki 链接打开对应页面，核对具体参数、接线表、代码 API、配置步骤和页面更新内容。
3. 再检查当前工程的 `empty.c`、`empty.syscfg`、生成头文件和目标配置；工程现状优先于通用教程示例。
4. 回答时区分“本地工程实际配置”“Wiki 教程示例”和“根据资料作出的推断”，不要把不同板卡或不同模块版本混用。
5. 涉及网页资料的回答附上对应原始页面链接；涉及本地工程的回答附上本地文件路径。

如果本地整理文档没有覆盖问题，再沿天猛星 Wiki 根路径检索相关子页面；不要默认引用同一站点的地猛星、地正星或其他 MSPM0 开发板内容。

工作区共享的天猛星板级原理图、引脚图、TI 文档和 CCS/Keil 示例另见 `../docs/reference/Tianmengxing/INDEX.md`，可用 `rg` 检索其 `text/` 与 `examples/`。本工程硬件目标是 LP-MSPM0G3507，不能把天猛星排针接线直接套用到本工程。
