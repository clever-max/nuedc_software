# Car project module boundaries

The project follows the repository guide's dependency direction and keeps the current mission as a cautious open-loop demo:

```text
car.c
  -> app/app.c (initialization, scheduling, button and IRQ dispatch)
       -> mission/demo_mission.c (15 s demo state machine)
       -> protocol/serial_console.c (RUN15/STOP parser and telemetry)
       -> control/wheel_speed_controller.c (independent A/B incremental PID; not enabled)
       -> bsp/encoder.c (signed edge counts and speed/distance conversion)
       -> bsp/motor_pwm.c (signed PWM, direction and coast)
       -> SysConfig generated DriverLib layer
```

`car.c` is limited to the application entry point and interrupt vectors. GPIO ISRs dispatch to short BSP handlers. The 5 ms timer ISR only counts elapsed ticks and schedules 10 Hz telemetry. Main-loop code copies the pending tick count, converts encoder deltas using the actual elapsed interval, advances the mission, parses serial commands and formats telemetry.

`control/wheel_speed_controller.c` provides separate incremental PID state for each wheel. It accepts explicit gains and an output limit in signed PWM permille, uses measured `dt`, clamps the accumulated output, and resets a wheel's history at zero target. No default gains are supplied and the 15-second mission does not call this module: no wheel-speed PID is active until encoder polarity/count scale, motor response and safe gains are calibrated.

`bsp/motor_pwm.c` owns AT8236 timer compare indices, forward polarity, reverse polarity, signed command limits and coast output. `bsp/encoder.c` owns the GPIO edge-counter state and count-to-mm/s conversion. `mission/demo_mission.c` owns the timed ramp/run/stop states. `protocol/serial_console.c` only receives newline-terminated RUN15/STOP commands and prints status; it does not control hardware directly.

Encoder conversion uses the user-provided 13 PPR, 1:28 ratio and 65 mm wheel diameter with one A rising edge per encoder cycle. This corresponds to 364 counts per wheel revolution if the PPR denotes channel-A cycles per motor revolution; confirm with a measured revolution before using it for calibrated speed or distance.

The CCS project source entries include `bsp/`, `app/`, `protocol/`, `control/` and `mission/`. SysConfig remains the source of pin and peripheral configuration. Generated files in `Debug/` are tool-owned and must not be edited manually.
