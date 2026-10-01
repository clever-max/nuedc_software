# AT8236 Reference Library

This directory keeps the user-provided AT8236 package's useful review materials in `vendor/` and project-specific findings in `analysis/`.

## Start here

1. [Wiring and configuration record](analysis/wiring.md) — user wiring and the matching SysConfig mapping.
2. [AT8236 behavior and source notes](analysis/AT8236_reference_notes.md) — manual facts, example behavior, and adaptation limits.
3. [MG513X Hall notes](analysis/MG513X_Hall_notes.md) — confirmed encoder type, signal precautions, and distance-count formula.
4. [Serial debug guide](analysis/serial_debug.md) — onboard CH340E port settings and telemetry fields.
5. [Initial COM8 trace](analysis/serial_debug_20261001.md) — a historical speed comparison captured with the previous encoder scale; its converted speeds are invalid under the corrected Hall specification.
6. [Current module boundaries](analysis/architecture.md) — separation between app scheduling, encoder BSP and motor PWM BSP.
7. [Vendor file inventory](analysis/vendor_inventory.md) — copied reference files and media deliberately not duplicated.

## Vendor material map

- `vendor/1.用户手册与教程视频/`: current AT8236 D157B manual, diagnostics PDF and encoder notes/image.
- `vendor/2.编码器的使用教程与测速原理/`: encoder waveform image and archived STM32/Arduino example packages.
- `vendor/4.例程源码/`: Arduino source plus Arduino and STM32 example archives and wiring image.
- `vendor/5.原理图/`: D107A and D157B schematics.
- `vendor/6.芯片手册/`: AT8236 and companion regulator IC datasheets.

Vendor files are retained as supplied. The manufacturer examples target other MCUs and use different pin assignments; they are reference material, not this project's configuration. This project uses the wiring recorded by the user and `car.syscfg` as the source of truth.





