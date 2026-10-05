# Current closed-loop route architecture

```text
car.c
  -> app/app.c (timer schedule, button/UART dispatch, sensor update)
       -> mission/demo_mission.c (15 s gray line-following PID state)
       -> control/wheel_speed_controller.c (independent A/B incremental PID)
       -> bsp/encoder.c (signed A-edge counts and mm/s conversion)
       -> bsp/mpu6050.c (raw MPU6050 I2C1 driver, bias calibration and yaw integration)
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
4. Reads the MPU6050 Z-axis rate, removes the startup bias, filters it and integrates yaw.
5. Writes signed PWM to the AT8236 bridge and updates the PB27 buzzer state.

Encoder interrupts do only count A-channel rising edges and sample B for direction. The MPU6050 is initialized at I2C address `0x68`; its gyro stream is bias-calibrated while the chassis is stationary. A failed WHO_AM_I or I2C read marks the gyro unavailable and prevents route start.

## Route states

The active demo starts in `CURVE_1`, follows gray position at a reduced curve speed until the first approximately 170° yaw change, runs `STRAIGHT_TRACK` at 200 mm/s until a sustained yaw rate indicates the next bend, then stops after `CURVE_2` reaches the same angle. A `STOP` command or second B21 press enters `ABORTED` and coasts.

The sign `RIGHT_TURN_YAW_SIGN` is `-1` for the standard MPU6050 Z-axis convention used here. If a physical right turn changes yaw in the opposite sign, change that one constant after checking telemetry.

## Parameters and limits

- Wheel target: 200 mm/s.
- Initial speed PID: `Kp=1.50`, `Ki=0`, `Kd=0.01`, output limit ±1000‰. This follows the mature sample's incremental speed PID structure; tune only after encoder polarity and count scale are verified.
- Safety limit: every wheel target is clamped to ±200 mm/s inside `wheel_speed_controller`, after gray correction and before the speed PID.
- MPU6050: I2C1 PB2=SCL, PB3=SDA, PB1=INT, 7-bit address `0x68`, ±250 dps, 200 Hz sample configuration, 100-sample startup bias and first-order rate filter.
- Track transitions: 170° target per bend, 8°/s entry threshold, 150 ms confirmation and 1 s minimum straight interval.
- Hall encoder scale: 13 PPR × 28 gearbox, one A rising edge per motor revolution cycle, 65 mm wheel; verify with one measured wheel revolution.
- Gray sensor: PA28=SCL, PA31=SDA, PCA9555-compatible 7-bit address `0x20`.

The PID values and heading gains are starting values, not physical calibration. Use the UART fields `state`, `gyro`, `yaw`, `target_yaw`, `speed` and `pwm_permille` to tune them safely.
