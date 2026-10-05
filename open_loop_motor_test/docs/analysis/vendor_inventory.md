# Vendor package inventory

Source supplied by user: the WHEELTEC AT8236 driver-module package labeled V2.8, dated 2026-07-25. The material below was copied into this project for offline review. Relative paths are from this file.

## Copied reference assets

- `../vendor/版权声明.pdf`
- `../vendor/资料更新记录.txt`
- User guide/tutorial folder: AT8236 D157B manual dated 2025-08-27; D157B diagnostics PDF dated 2025-07-10; encoder troubleshooting text and image.
- Encoder folder: waveform/orientation image and STM32F1 and Arduino UNO example ZIPs.
- Source examples: D107A STM32 archive, D157B STM32 standard-library and HAL archives (Hall and GMR encoder variants), STM32 wiring image, Arduino source and Arduino archive.
- Schematics: D107A schematic dated 2024-12-06 and D157B regulated-module schematic.
- Chip datasheets: AT8236, RT8279 and RT9013-33GB.

## Not duplicated

The original package also contains large tutorial videos (including a roughly 1.3 GB D157B tutorial video and a DC motor tutorial video) and a STEP 3D model. These are not copied into the project. They remain available in the user's supplied source package; see the package update log and folder map there. The contacts PDF, encapsulation usage text and D157B JSON footprint are likewise not duplicated because they are not needed to review the current pin mapping or driver behavior.

## Integrity and portability

The archived ZIPs are retained intact. Example contents target Arduino or STM32. This index intentionally avoids embedding the machine-specific source drive path so the project can move with the workspace. If the source package is moved later, the copied review assets in this folder remain available.
