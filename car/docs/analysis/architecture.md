# Current closed-loop route architecture

```text
car.c
  -> app/app.c (timer schedule, button/UART dispatch, sensor update)
       -> mission/demo_mission.c (15 s gray line-following PID state)
       -> control/wheel_speed_controller.c (independent A/B incremental PID)
       -> bsp/encoder.c (signed A-edge counts and mm/s conversion)
       -> bsp/jy61s_uart.c (JY61S UART2 frame parser, retained for future gyro use)
       -> bsp/gray_sensor.c (NCHD12/PCA9555-compatible software I2C on PA28/PA31)
       -> bsp/buzzer.c (PB27 start/stop notification)
       -> bsp/motor_pwm.c (AT8236 signed PWM and Motor-B polarity mapping)
       -> SysConfig generated DriverLib layer
```

## Control cycle

Every 5 ms the timer ISR increments a pending-tick counter. The foreground consumes pending ticks and:

1. Copies encoder counts and converts each wheel delta to mm/s.
2. Reads the 12-channel gray bit map and converts it to a line-position error.
3. Computes a gray P/D correction and updates independent wheel speed PID outputs.
4. Parses any JY61S UART2 frames for telemetry/future use; gyro is not required by this demo.
5. Writes signed PWM to the AT8236 bridge and updates the PB27 buzzer state.

Encoder interrupts do only count A-channel rising edges and sample B for direction. JY61S must be in UART mode; its gyro stream is bias-calibrated while the chassis is stationary. Missing or invalid UART frames mark the gyro unavailable and prevent route start.

## Route states

The active demo is `STRAIGHT_1` for 15 s. It keeps a 200 mm/s base target, applies gray-position correction to the two wheel targets, and stops in `DONE`. A `STOP` command or second B21 press enters `ABORTED` and coasts.

The sign `RIGHT_TURN_YAW_SIGN` is `-1` for the standard MPU6050 Z-axis convention used here. If a physical right turn changes yaw in the opposite sign, change that one constant after checking telemetry.

## Parameters and limits

- Wheel target: 200 mm/s.
- Initial speed PID: `Kp=0.50`, `Ki=0.20`, `Kd=0`, feed-forward `1.0 permille/(mm/s)`, output limit ±1000‰. The derivative term is exposed but starts at zero because the one-edge Hall measurement is quantized at low speed.
- Initial turn angle controller: `Kp=4.0 mm/s/deg`, `Ki=0`, `Kd=0.25 mm/s/(deg/s)`, command limit ±200 mm/s and minimum 45 mm/s outside the ±1.5° settle band.
- Hall encoder scale: 13 PPR × 28 gearbox, one A rising edge per motor revolution cycle, 65 mm wheel; verify with one measured wheel revolution.
- JY61S UART2: PB15=TX, PB16=RX, 115200-8-N-1; its files remain available but are not a start condition for this demo. See [JY61S notes](JY61S_notes.md).
- Gray sensor: PA28=SCL, PA31=SDA, PCA9555-compatible 7-bit address `0x20`.

The PID values and heading gains are starting values, not physical calibration. Use the UART fields `state`, `gyro`, `yaw`, `target_yaw`, `speed` and `pwm_permille` to tune them safely.
