# Project Documentation

Use this directory for information that must remain understandable outside the source code.

## Planned documents

- `hardware_interface.md`: frozen pin map, voltage levels, connectors, and peripheral ownership
- `power_and_safety.md`: battery, regulators, motor protection, emergency stop, and laser safety
- `mechanical_constraints.md`: chassis dimensions, wheel geometry, sensor mounting, and parking clearances
- `calibration.md`: sensor and motor calibration procedures with recorded parameters
- `test_plan.md`: repeatable test cases, acceptance criteria, and test results
- `protocol.md`: UART command, telemetry, parameter storage, and versioning rules

Do not store temporary screenshots, raw build logs, or unreviewed data here. Put large test data in a
separate artifact location and record its identifier and purpose in the relevant test document.

