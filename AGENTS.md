<!-- DO NOT EDIT - This part is automatically generated. -->

# Agent Guidelines

## CCStudio IDE Installation Directory

CCStudio IDE is installed at `D:/TI/CCS`. Save it as `{ccs-install-dir}` for the session — scripts and tools will need it.

## MANDATORY Pre-Task Requirement (DO NOT SKIP)

**CRITICAL - NO EXCEPTIONS**: Before ANY CCS/Texas Instruments-related task (even simple ones), you MUST read `D:/TI/CCS/ccs/theia/resources/ai/CCS.md`. This file includes information on how to interact with CCS as well as device-specific information (UART backchannel pins, LED setup, transmit best practices, etc.). 

Do NOT call any ccs-project, ccs-debug, ccs-sysconfig, or ccs-serial MCP tools until CCS.md has been read.


<!-- DO NOT EDIT - This part is automatically generated. -->

<!-- User instructions should be added below this line -->

# Workspace Instructions

## Workspace layout

This directory is the repository root. Keep each CCS project in its own immediate child directory:

- `empty/`: the original MSPM0G3507 starter project.
- `photoresistor_uart/`: LM393 photoresistor, ADC, UART and Python monitor project.
- `skills/mspm0-ccs/`: the shared MSPM0 CCS skill, scripts, references and examples.
- `docs/`: workspace-wide Tianmengxing and LP-MSPM0G3507 reference material.

Do not put project source files directly in the workspace root. A new project must have its own `AGENTS.md`, `.project`, `.cproject`, `.ccsproject`, `targetConfigs/`, SysConfig file and user source files. Start from the nearest existing project structure and update the project-specific entrypoint and configuration names.

## Required skill lookup

Before editing any MSPM0 project or configuration, read the shared skill at the relative path:

`skills/mspm0-ccs/SKILL.md`

Use the skill's scripts through paths relative to this repository root, for example:

```powershell
python skills/mspm0-ccs/scripts/check_syscfg.py <project-dir>
python skills/mspm0-ccs/scripts/run_sysconfig.py <project-dir> --compiler ticlang
```

When `.syscfg` changes, run SysConfig validation before rebuilding. Preserve the project-declared SysConfig version and report any installed-tool version warning. Do not hand-edit generated SysConfig or build outputs.

## Local references

For any workspace project using Tianmengxing MSPM0G3507, first read `docs/tmx-mspm0g3507-wiki-summary.md` and `docs/reference/Tianmengxing/INDEX.md`. Search its `text/` and `examples/` with `rg`; consult the original Wiki shortcuts when online updates are needed. Then inspect the current project's `.syscfg`, generated header and target configuration. For LP-MSPM0G3507 LaunchPad questions, use `docs/reference/LP-MSPM0G3507/INDEX.md` and its local extracted text. Keep the two board references distinct.

Distinguish the current project's actual configuration, a Wiki tutorial example and an inference. Do not mix pin assignments from different boards or modules.

## Paths and portability

Workspace documentation and project-to-workspace references must use relative paths. Do not add `C:\Users\...`, `D:\...` or another machine-specific workspace path to source, documentation, scripts or `AGENTS.md`.

External TI installations are machine dependencies. Resolve them through CCS/SysConfig discovery, environment variables or the existing CCS project variables. If an external tool cannot be found, report the missing dependency and its expected role instead of embedding a new absolute path.

CCS-generated metadata may contain an SDK origin field. Keep the SDK install variable form (`${COM_TI_MSPM0_SDK_INSTALL_DIR}`) and do not replace it with a user-specific absolute path.

## Validation reporting

Report these separately after implementation:

1. source/static checks;
2. SysConfig generation;
3. compilation;
4. linking;
5. flash-tool result;
6. physical-board and serial behavior.

Never claim physical validation without a connected board and an observed result.

## Mandatory flashable firmware artifact

This rule applies to every existing and future project and its documentation in this repository: whenever a firmware task is reported complete, provide a directly flashable firmware image in that project's directory. Produce an Intel HEX `.hex` file by default; a `.txt` file is acceptable only when it contains a documented flash-tool-compatible image format. A source file, build log, or plain-text description is not a firmware image. If the image cannot be generated, do not report the firmware task as complete; state which build or tool dependency prevents producing it.
