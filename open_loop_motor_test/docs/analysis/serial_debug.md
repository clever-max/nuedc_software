# Closed-loop right-turn route UART operation

Open the external CH340 UART1 connection at 115200 baud, 8 data bits, no parity, one stop bit. Wire CH340 TXD to PB5 (UART1_RX), CH340 RXD to PB4 (UART1_TX), and share GND. Send commands with a newline.

| Command | Behavior |
| --- | --- |
| `RUN15` | Starts the MPU6050 curve-straight-curve route after I²C initialization succeeds. |
| `STOP` | Requests a coast stop and enters `ABORT`. |

B21 has the same start/stop behavior. Keep the chassis stationary after reset for MPU6050 bias calibration. The route is:

```text
CURVE_1    approximately 170° yaw change
STRAIGHT_TRACK  until the next sustained bend
CURVE_2    approximately 170° yaw change, then stop
```

如果 MPU6050 未安装、I²C 地址 `0x68` 无应答或零偏校准未完成，遥测会显示 `gyro=ERR`，`RUN15` 和 B21 会被拒绝，PWM 保持 `0/0`；这是闭环路线的安全条件，不是电机故障。外置 CH340 使用 UART1 PB4/PB5。

Telemetry is printed at about 10 Hz:

```text
ms=1200 state=CURVE gyro=OK backend=MPU6050 yaw=12.4 target_yaw=170.0 yaw_rate=34.2 turn_err=157.6 speed=108/112 pwm_permille=220/218
```

Fields:

- `state`: `IDLE`, `CURVE`, `STRAIGHT`, `DONE` or `ABORT`.
- `gyro`: MPU6050 I²C initialization and zero-bias calibration status.
- `backend`: `MPU6050` when the active sensor path is selected.
- `yaw`: integrated Z-axis yaw in degrees.
- `target_yaw`: heading held during the current straight segment or turn endpoint.
- `yaw_rate`: filtered gyro-Z rate in degrees per second; the sign is useful for checking the right-turn convention.
- `turn_err`: signed remaining right-turn angle in degrees; it is zero outside a turn.
- `speed`: encoder-derived wheel speeds in mm/s (A/B).
- `pwm_permille`: signed PID commands for the two motors.

If `gyro=ERR`, `RUN15` is rejected. The first bend determines the signed yaw direction automatically. A second bend is accepted after the straight interval when the same signed yaw rate remains above 8°/s for 150 ms.

Initial controller values are deliberately marked as tuning values in the source: curve target 110 mm/s, straight target 200 mm/s, wheel `Kp=0.50`, `Ki=0.20`, `Kd=0`, feed-forward 1.0 permille per mm/s, and line `Kp=8.0`, `Kd=0.05`. Tune only after confirming encoder count scale, gyro sign, motor direction and UART telemetry.
