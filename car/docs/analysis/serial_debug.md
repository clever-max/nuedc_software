# Closed-loop right-turn route UART operation

Open the Tianmengxing CH340E UART0 port at 115200 baud, 8 data bits, no parity, one stop bit. Send commands with a newline.

| Command | Behavior |
| --- | --- |
| `RUN15` | Starts the 4-straight/3-right-turn route if JY61S IIC initialization succeeds. |
| `STOP` | Requests a coast stop and enters `ABORT`. |

B21 has the same start/stop behavior. Keep the chassis stationary after reset for gyro bias calibration. The route is:

```text
STRAIGHT_1 2 s
TURN_1      right 90° target
STRAIGHT_2 2 s
TURN_2      right 90° target
STRAIGHT_3 2 s
TURN_3      right 90° target
STRAIGHT_4 2 s
```

如果 JY61S 未安装、UART2 没有收到有效帧或零偏校准未完成，遥测会显示 `gyro=ERR`，`RUN15` 和 B21 会被拒绝，PWM 保持 `0/0`；这是闭环路线的安全条件，不是电机故障。UART0 PA10/PA11 是调试口，JY61S 使用 UART2 PB15/PB16。

Telemetry is printed at about 10 Hz:

```text
ms=1200 state=FWD gyro=OK backend=WIT50 yaw=-0.4 target_yaw=0.0 yaw_rate=-0.2 turn_err=0.0 speed=198/202 pwm_permille=220/218
```

Fields:

- `state`: `IDLE`, `FWD`, `TURN`, `COAST`, `DONE` or `ABORT`.
- `gyro`: JY61S UART frame reception and zero-bias calibration status.
- `backend`: should be `UART2` when the active sensor path is selected.
- `yaw`: integrated Z-axis yaw in degrees.
- `target_yaw`: heading held during the current straight segment or turn endpoint.
- `yaw_rate`: filtered gyro-Z rate in degrees per second; the sign is useful for checking the right-turn convention.
- `turn_err`: signed remaining right-turn angle in degrees; it is zero outside a turn.
- `speed`: encoder-derived wheel speeds in mm/s (A/B).
- `pwm_permille`: signed PID commands for the two motors.

If `gyro=ERR`, `RUN15` is rejected. If a turn does not settle within ±1.5° and below 12°/s before 3 s, the route aborts. The right-turn sign is controlled by `RIGHT_TURN_YAW_SIGN` in `mission/demo_mission.c`; the default is -1 for the usual JY61S Z-axis convention.

Initial controller values are deliberately marked as tuning values in the source: target wheel speed 200 mm/s, wheel `Kp=0.50`, `Ki=0.20`, `Kd=0`, feed-forward 1.0 permille per mm/s, straight heading `Kp=2.0`, `Kd=0.10`, and turn angle `Kp=4.0`, `Ki=0`, `Kd=0.25`. Tune only after confirming encoder count scale, gyro sign, motor direction and UART telemetry.
