# 12 路灰度循迹 PID Demo

## 当前接线

| NCHD12 | 天猛星 |
| --- | --- |
| SCL | PA28 |
| SDA | PA31 |
| VCC | 3.3V 输出档 |
| GND | GND |

NCHD12 的 PCA9555 兼容接口使用 7 位地址 `0x20`，对应写地址 `0x40`、读地址 `0x41`。输入寄存器从 `0x00` 连续读取两个字节，低 12 位是 12 路灰度状态。样例仓库的 PA0/PA1 软件 I²C 与本项目电机 PWM 冲突，因此本项目在 PA28/PA31 上实现独立软件 I²C。

## 控制链

每个 5 ms 控制周期执行：

1. 读取 12 位灰度位图；
2. 将通道 0…11 映射到位置误差 -11…+11；
3. 计算灰度 P/D 修正量，限幅 ±100 mm/s；
4. 生成左右轮目标速度 `200 + correction`、`200 - correction`；
5. 由左右轮编码器速度 PID 输出 AT8236 PWM。

当前 Demo 必须先通过 MPU6050 启动校准。按键或 `RUNPID`/`RUN15` 后依次执行首弯、直道和次弯，串口显示 `state`、`gyro`、`yaw`、`line=...` 和 `gray=0x...`。无有效灰度输入时修正量回到零，保留编码器定速环。

## 代码入口

- 灰度读取：[bsp/gray_sensor.c](../../bsp/gray_sensor.c)
- 线路误差与速度目标：[mission/demo_mission.c](../../mission/demo_mission.c)
- 调度、蜂鸣器和遥测：[app/app.c](../../app/app.c)
