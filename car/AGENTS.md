# Project Instructions

Follow the workspace-level `../AGENTS.md` and shared `../skills/mspm0-ccs/SKILL.md` instructions. This is the `car` MSPM0G3507 CCS project.

- Entrypoint: `car.c`; SysConfig source: `car.syscfg`.
- Target config: `targetConfigs/MSPM0G3507.ccxml`.
- User wiring: PA0/A0→AIN1, PA1/A1→AIN2, PA8/A8→BIN1, PA9/A9→BIN2.
- Keep all four motor inputs low at startup. B21 starts the 15-second demo; a second press during motion aborts and coasts.
- Source layering: `app/` schedules work and dispatches events; `mission/` owns the timed demo state machine; `protocol/` owns UART command parsing/telemetry; `bsp/` owns encoder and PWM hardware; `control/` owns reusable controllers.
- `control/wheel_speed_controller.c` is deliberately not called by the current mission. Do not enable closed-loop motion or copy PID gains from the reference repository without encoder-scale, wheel-up motor response, and safe low-speed calibration.
- The user corrected the installed motors to MG513X Hall encoders (13 PPR); earlier GMR/500 PPR/3.3 V assumptions are obsolete. The motor connector diagram shows E1A/E1B on MOTORC1 pins 3/4 and E2A/E2B on MOTORC2 pins 3/4, with pin 5 labeled 5V. Current MCU mapping is PB0/PB1 and PB2/PB3; verify actual harness and output voltage compatibility before direct connection.
- The current demo is open-loop, with 65 mm wheel diameter (user supplied), user-confirmed 1:28 gearing, 12 V motor supply and fixed 400/190‰ PWM trims. No PID or distance guarantee; trim and trajectory require physical confirmation.
- Hall encoder A/B output voltage and output topology must be verified against the MCU pin tolerance before direct connection; use compatible 3.3 V signaling or level translation if required.
- Do not copy PWM frequency, motor polarity, brake/coast behavior, motor supply, or encoder pin assignments from Arduino/STM32 examples.
- Do not edit generated SysConfig files or build outputs by hand.
- Validate source/static checks and SysConfig generation before building after SysConfig changes. Preserve SDK/compiler/probe metadata.
- Any completed firmware task must include a flashable Intel HEX image in this project directory; distinguish compile/link, flash-tool and physical-board validation.


