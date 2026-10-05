# AT8236 Reference Library

This directory keeps the user-provided AT8236 package's useful review materials in `vendor/` and project-specific findings in `analysis/`.

## Start here

1. [Wiring and configuration record](analysis/wiring.md) — user wiring and the matching SysConfig mapping.
2. [AT8236 behavior and source notes](analysis/AT8236_reference_notes.md) — manual facts, example behavior, and adaptation limits.
3. [MG513X Hall notes](analysis/MG513X_Hall_notes.md) — confirmed encoder type, signal precautions, and distance-count formula.
4. [MPU6050 connection notes](analysis/MPU6050_notes.md) — I2C1 PB2/PB3 wiring, startup calibration and yaw route.
5. [12-channel grayscale line following](analysis/grayscale_line_following.md) — PA28/PA31 software I²C, PCA9555 bit map, line error and encoder PID.
5. [Serial debug guide](analysis/serial_debug.md) — onboard CH340E port settings and telemetry fields.
6. [Initial COM8 trace](analysis/serial_debug_20261001.md) — a historical speed comparison captured with the previous encoder scale; its converted speeds are invalid under the corrected Hall specification.
7. [Current module boundaries](analysis/architecture.md) — separation between app scheduling, encoder BSP and motor PWM BSP.
8. [Vendor file inventory](analysis/vendor_inventory.md) — copied reference files and media deliberately not duplicated.
9. [Reference repository analysis](analysis/reference_repo_notes.md) — what was adapted from `XingShuyu/Car` and what remains board-specific.
10. [Historical no-encoder turn-forward-reverse sequence](analysis/write_this_drive_sequence.md) — the earlier open-loop tutorial, retained for review only.

## Vendor material map

- `vendor/1.用户手册与教程视频/`: current AT8236 D157B manual, diagnostics PDF and encoder notes/image.
- `vendor/2.编码器的使用教程与测速原理/`: encoder waveform image and archived STM32/Arduino example packages.
- `vendor/4.例程源码/`: Arduino source plus Arduino and STM32 example archives and wiring image.
- `vendor/5.原理图/`: D107A and D157B schematics.
- `vendor/6.芯片手册/`: AT8236 and companion regulator IC datasheets.

Vendor files are retained as supplied. The manufacturer examples target other MCUs and use different pin assignments; they are reference material, not this project's configuration. This project uses the wiring recorded by the user and `car.syscfg` as the source of truth.





