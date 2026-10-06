# Changelog

All notable workspace releases are recorded here. Project-specific validation remains in each project's `PROJECT_STATE.md`.

## [Unreleased]

### Changed

- Added a mandatory project continuation, documentation, duplicate-review, and commit-gate workflow.

## [car-stable-v1.0.0] - 2026-10-06

### Added

- Stable 30-second no-gyro car firmware with the validated motor and encoder polarity mapping.
- Flashable `car.hex` and `gray_line_30s_no_gyro.hex` assets in the GitHub Release.

### Known issues

- NCHD12 telemetry still reports `gray_bus=NACK1` until the physical bus/address wiring is verified.
