# 灰度传感器与 MPU6050 共享参考

这里保存用户提供的 NCHD1 多路灰度传感器资料和 `MPU6050-V1-SCH.pdf`，供工作区所有项目检索。原始资料保留在 `NCHD1/` 和 `MPU6050/`；PDF 文本提取在 `text/`；从 NCHD12 I²C 资料包中筛出的 MSPM0G3507 例程在 `examples/nchd12_mspm0g3507/`。

## 资料入口

| 主题 | 原始资料 | 可检索内容 |
| --- | --- | --- |
| NCHD1 用户手册 | [灰度传感器用户使用手册](NCHD1/NCHD1_user_manual_20230119.pdf) | [提取文本](text/NCHD1_user_manual_20230119.txt) |
| 12 路输出电平设置 | [输出电压说明](NCHD1/12路灰度传感器设置输出电压说明.pdf) | [提取文本](text/NCHD1__12路灰度传感器设置输出电压说明.pdf.txt) |
| NCHD12 I²C 资料与例程 | [原始 ZIP](NCHD1/NCHD12通过I2C方式读取数据资料与例程（20241205）.zip) | [ZIP 文件清单](text/NCHD1__NCHD12通过I2C方式读取数据资料与例程（20241205）.zip.listing.txt) |
| MSPM0G3507 NCHD12 例程 | [筛选后的源码](examples/nchd12_mspm0g3507/) | `ncontroller.syscfg`、`main.c`、`drivers/nchd12.*`、`drivers/soft_i2c.*` |
| PCA9555 扩展器 | [PCA9555 数据手册](NCHD1/数据手册/PCA9555-IO扩展器数据手册.pdf) | [提取文本](text/NCHD1__数据手册__PCA9555-IO扩展器数据手册.pdf.txt) |
| MPU6050 原理图 | [MPU6050-V1-SCH](MPU6050/MPU6050-V1-SCH.pdf) | [提取文本/图像层说明](text/MPU6050__MPU6050-V1-SCH.pdf.txt) |

## 已核对的关键点

- JY61/JY61S 类六轴模块的 I²C 模式资料说明：模块释放内部 MPU6050 总线，主控读取的是原始加速度和角速度，不直接得到姿态角；使用前需用上位机切换到 I²C 模式并重新上电。小车工程的接入记录见 [`car/docs/analysis/JY61S_notes.md`](../../car/docs/analysis/JY61S_notes.md)。

- NCHD12 I²C 资料中的 PCA9555 默认地址说明为：地址焊盘悬空时写地址 `0x40`、读地址 `0x41`（资料使用 8 位读写地址表示法）；实际驱动应按所用 I²C API 的 7 位地址约定转换为 `0x20`。
- NCHD12 输出电平说明写明：传感器供电为 5V，输出电平可通过 V0 与 `3V3`/`VCC` 焊盘选择；默认配置是 5V 输出。接入 MSPM0 前必须把输出电平设置为 3.3V，或增加电平转换。
- 例程资料的 `ncontroller.syscfg` 使用软件 I²C：SCL=PA0、SDA=PA1；这只是资料例程配置，和当前小车 PA0/PA1 电机 PWM 分配冲突，不能直接复制。
- 当前小车已经把 MPU6050 接到 PB2/PB3（I2C1）和 PB1 INT。灰度传感器推荐使用此前安排的 PA28/PA31（I2C0）独立总线；不要把 NCHD12 例程的 PA0/PA1 直接套到当前项目。
- MPU6050 原理图 PDF 主要是图形资料，文本提取可能为空；需要核对器件供电、SCL/SDA 上拉电压和 INT 连接时，以原始 PDF 图纸为准。

## 检索命令

```powershell
rg -n "0x40|0x41|PCA9555|SCL|SDA|V0|3V3|5V|PA0|PA1|PA28|PA31" docs/reference/sensors
```

这些资料是硬件和示例参考；实际工程仍以项目自己的 `.syscfg`、器件数据手册和实测电平为准。
