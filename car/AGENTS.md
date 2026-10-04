# Project Instructions

Follow the workspace-level `../AGENTS.md` and shared `../skills/mspm0-ccs/SKILL.md` instructions. This is the `car` MSPM0G3507 CCS project.

- Entrypoint: `car.c`; SysConfig source: `car.syscfg`.
- Target config: `targetConfigs/MSPM0G3507.ccxml`.
- Current PWM wiring: PA0/TIMG8_C1→AIN1, PA1/TIMG8_C0→AIN2, PA8/TIMA0_C0→BIN1, PA9/TIMA0_C1→BIN2.
- Encoder wiring: E1A/E1B/E2A/E2B→PA27/PA25/PB25/PB20. JY61S UART→PB15/PB16. Passive buzzer→PB27.
- Keep all motor inputs low at startup. B21 starts the 15-second grayscale line-following encoder-PID demo; a second press aborts and coasts.
- Source layering: `app/` schedules work and dispatches events; `mission/` owns the PID line-following state machine; `protocol/` owns UART command parsing/telemetry; `bsp/` owns encoder, gray sensor, JY61S UART, buzzer and PWM hardware; `control/` owns the reusable twin-wheel speed controller.
- `control/wheel_speed_controller.c` is called from every active straight/turn control cycle. Its initial gains are commissioning values only; calibrate the encoder count scale and wheel response before relying on the route's distance or turn accuracy.
- The user corrected the installed motors to MG513X Hall encoders (13 PPR); earlier GMR/500 PPR/3.3 V assumptions are obsolete. The current demo counts encoder A rising edges and samples B for signed speed feedback.
- The active demo uses 200 mm/s wheel-speed targets, encoder speed PID and gray-position P/D correction. Gyro data is retained and parsed on UART2 but is not required by this demo.
- Before relying on encoder feedback, verify Hall encoder A/B output voltage and topology against MCU pin tolerance.
- Do not copy PWM frequency, motor polarity, brake/coast behavior, motor supply, or encoder pin assignments from Arduino/STM32 examples.
- Do not edit generated SysConfig files or build outputs by hand.
- Validate source/static checks and SysConfig generation before building after SysConfig changes. Preserve SDK/compiler/probe metadata.
- Any completed firmware task must include a flashable Intel HEX image in this project directory; distinguish compile/link, flash-tool and physical-board validation.
- Workspace-wide Tianmengxing schematic, pin-map, TI-document and CCS/Keil example references are indexed at `../docs/reference/Tianmengxing/INDEX.md` and searchable under `../docs/reference/Tianmengxing/text/` and `../docs/reference/Tianmengxing/examples/`.
