# empty MSPM0G3507 Example Project

## Project role

This directory is the first example project for the `nuedc_software` code-management repository.
It is a CCS Theia project for the MSPM0G3507 LaunchPad and currently provides the smallest verified
MSPM0 baseline: SysConfig initialization and onboard LED1 control.

## Toolchain

- Device: MSPM0G3507
- Board: LP_MSPM0G3507
- IDE: Code Composer Studio Theia
- Compiler: TI Arm Clang
- SDK: MSPM0 SDK 2.11.00.07
- Debug configuration: `targetConfigs/MSPM0G3507.ccxml`

## Repository conventions

- Keep `empty.syscfg` as the source of truth for pins, clocks, peripherals, DMA, and interrupts.
- Do not hand-edit SysConfig generated files.
- Keep application logic in the planned `app/`, `bsp/`, `control/`, and `protocol/` layers.
- Put test notes, hardware mappings, calibration records, and acceptance results under `docs/`.
- Keep build output, generated files, and local IDE state out of Git.
- Use one feature or hardware change per commit.

## Current status

- [x] CCS project imports
- [x] SysConfig generation
- [x] MSPM0G3507 compilation and linking
- [x] Onboard LED1 application code
- [ ] Motor driver and PWM bring-up
- [ ] Encoder sampling
- [ ] UART parameter and telemetry protocol
- [ ] Closed-loop motion control

The macro-level development plan is in `DEVELOPMENT_PLAN.md` in this directory.
