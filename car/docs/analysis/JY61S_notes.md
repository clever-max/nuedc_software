# JY61S 接入说明

当前工程改用 **UART 模式**，JY61S 接 UART2 PB15/PB16；UART0 PA10/PA11 保留给 CH340 调试。工程解析标准 `0x55 0x52` 角速度帧，并在 MSPM0 内积分 yaw。

## 接线

| JY61S | 天猛星 | 说明 |
| --- | --- | --- |
| VCC | 3V3 优先 | 保证 SCL/SDA 上拉不超过 MSPM0 GPIO 电压 |
| GND | GND | 必须共地 |
| TX | PB16 / UART2_RX | 传感器输出接 MCU RX |
| RX | PB15 / UART2_TX | MCU TX，可用于发送配置命令 |
| SCL/SDA/INT | 不接 | 当前 UART 驱动不使用 |

UART 电平应为 3.3V TTL，TX/RX 交叉连接，传感器与天猛星共地。JY61S 通过配置命令 `FF AA 61` 进入 UART 模式；115200-8-N-1 下输出 0x51/0x52/0x53/0x54 帧。

## 启动前检查

1. 用 JY61S 配置工具或 UART 命令 `FF AA 61` 设为 UART 模式，保存并重新上电。
2. 接好 PB15/PB16，保持小车静止，让固件完成 100 个角速度样本的零偏采样。
3. 串口看到 `gyro=OK backend=UART2` 后再按 B21 或发送 `RUN15`。
4. 若仍为 `gyro=ERR`，先检查 UART2 线序、115200 波特率、3.3V TTL 电平和共地。

## 电脑端串口自检

工程内置独立的 JY61S 帧监视器，不依赖小车固件：

```powershell
python car/tools/jy61s_monitor.py --port COM8 --baudrate 115200 --duration 15
```

它检查 `0x55 0x51/0x52/0x53/0x54` 四类 11 字节帧、校验和，并显示加速度、角速度、姿态角和磁场原始值。若 `frames=0`，先检查 JY61S 是否处于串口模式、USB-TTL 的 TX/RX 是否交叉、波特率和 GND；IIC 模式下本工具不会持续收到姿态帧。

电脑端串口监视器仍可用于检查传感器帧；它与小车的 UART0 调试口应分开使用。

参考：

- [JY61 使用说明（IIC 模式章节）](https://m.ruidan.com/infomation/detail/137062)
- [WitMotion JY61 系列手册 PDF](https://images-na.ssl-images-amazon.com/images/I/B1AxgFUp8CS.pdf)
