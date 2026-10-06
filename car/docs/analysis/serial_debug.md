# 串口调试指南

## 连接

使用外置 CH340：CH340 TXD 接 PB5（UART1_RX），CH340 RXD 接 PB4（UART1_TX），两端共地。串口参数为 115200、8 数据位、无校验、1 停止位。命令以换行结束。

## 命令

| 命令 | 作用 |
| --- | --- |
| `RUNPID` | 启动 30 秒灰度循迹（名称保留自早期方案） |
| `RUN15` | 与 `RUNPID` 相同，当前并不代表 15 秒路线 |
| `STOP` | 请求停止，进入 `ABORT` |

B21 与启动命令共用同一状态机。任务运行中再次启动会返回 `ERR busy`。

## 启动和遥测

正常启动横幅应包含：

```text
MPU6050 disabled; encoder speed PID active
```

遥测约每 100 ms 输出一行，例如：

```text
ms=1200 state=LINE30 gyro=OFF backend=MPU6050-ERR line=0.0(OK) gray=0x00FF yaw=0.0 target_yaw=0.0 yaw_rate=0.0 turn_err=0.0 speed=108/112 pwm_permille=220/218
```

字段含义：

- `state`：`IDLE`、`LINE30`、`DONE` 或 `ABORT`。
- `gyro`：当前应为 `OFF`；不是 MPU6050 已通过检测的证明。
- `line`：灰度位置误差及有效标志。
- `gray`：12 路灰度位图。
- `speed`：左右轮估算速度，单位 mm/s。
- `pwm_permille`：左右电机的有符号 PWM 千分比。
- `yaw`、`target_yaw`、`yaw_rate`、`turn_err`：当前版本不使用，通常为 0。

先架空车轮核对正负方向，再进行落地循迹。没有实际串口记录时，不把源码中的参数写成实车性能结论。
