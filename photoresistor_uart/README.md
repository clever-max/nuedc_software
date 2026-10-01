# 光敏电阻 LM393 串口采集工程

目标器件：LCKFB 天猛星 MSPM0G3507（CCS Theia / TI Arm Clang / SDK 2.11.0.07）。

## 接线

| 模块 | 开发板 |
|---|---|
| AO | PA27（ADC0 通道 0） |
| DO | PA26（GPIO 输入，上拉） |
| VCC | 3V3 |
| GND | GND |

板载 CH340E USB-UART 使用 UART0：PA10 为 TX、PA11 为 RX，连接电脑后串口号以设备管理器为准（示例图为 COM8）。固件参数为 **115200 8-N-1**。

## 串口输出

每约 20 ms（约 50 Hz）输出一行，同时更新板载 PB22 LED：

```text
AO=2048  V=1.650 V  DO=1
```

`AO` 是 12 位 ADC 原始值（0–4095），`V` 按 3.3 V VDDA 换算，`DO` 是 LM393 比较器输出电平。电位器可调节 DO 翻转阈值；AO 的电压方向以具体模块分压接法为准。

## Python 双向串口程序

安装依赖后运行：

```powershell
python -m pip install pyserial
python photoresistor_monitor.py --port COM8
```

程序会保持单行显示。直接按键即可操作：`+/-` 每次调整 50，`[`/`]` 每次调整 10，`g` 查询阈值，`q` 退出。软件判定结果 `SW` 会控制板载 PB22 LED；模块上的 LM393 电位器仍只影响硬件 `DO`。

## 验证

1. 在 CCS Theia 导入 `photoresistor_uart` 工程。
2. 生成/构建后下载到 MSPM0G3507。
3. 打开 CH340 串口，选择 115200-8-N-1，即可观察输出。

本工程的 `.syscfg` 是配置源；`ti_msp_dl_config.c/.h` 等文件由 SysConfig 生成，请勿手工编辑。
