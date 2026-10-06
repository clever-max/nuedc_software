# 当前接线与配置对应表

本文档以 `../../car.syscfg` 和当前 BSP 为准。

## 电机与编码器

| MCU | 外设/模式 | 外部信号 |
| --- | --- | --- |
| PA0 | TIMG8_C1 | AIN1 |
| PA1 | TIMG8_C0 | AIN2 |
| PA8 | TIMA0_C0 | BIN1 |
| PA9 | TIMA0_C1 | BIN2 |
| PA27 | GPIO 上升沿中断 | 左 E1A |
| PA25 | GPIO 输入 | 左 E1B |
| PB25 | GPIO 上升沿中断 | 右 E2A |
| PB20 | GPIO 输入 | 右 E2B |

编码器代码统计 A 相上升沿并读取 B 相判向。按 13 PPR、1:28 先估算 364 count/轮；实际使用前要让车轮转一整圈核对计数。

## 灰度传感器

NCHD12 的 SCL 接 PA28，SDA 接 PA31，GND 共地。按资料，模块 VCC 使用 5V，输出选择区必须将 `V0` 与 `3V3` 短接，使逻辑输出为 3.3V；默认 `V0` 与 `VCC` 短接时输出约 5V，不应直接接入 MSPM0。驱动按 PCA9555 兼容器件读取 7 位地址 `0x20`（资料使用写 `0x40`、读 `0x41`），从寄存器 `0x00` 读取两个字节，低 12 位为通道状态。PA0/PA1 已分配给电机 PWM，不能照搬旧示例作为软件 I²C。

## 串口和按键

- 板载 UART0：PA10 为 MCU TX、PA11 为 MCU RX，直接使用 Type-C 上的 CH340E，115200-8-N-1。
- B21：PB21，上拉输入，低电平有效。
- 蜂鸣器：PB27，GPIO 输出，启动为低电平。

## 预留传感器

PB2/PB3/PB1 分别为 MPU6050 的 I2C1 SCL/SDA/INT；`car.syscfg` 保留配置，但当前 `app.c` 不初始化或读取 MPU6050。UART2 PB15/PB16 只服务于保留的 JY61S 驱动，同样未接入当前任务。

供应商示例中的 STM32/Arduino 引脚、PWM 频率和电平假设不属于本项目配置。
