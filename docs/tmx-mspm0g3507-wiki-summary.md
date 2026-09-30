# 立创·天猛星 MSPM0G3507 Wiki 全量整理

> 阅读日期：2026-09-29  
> 范围：`https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/` 路径下逐页读取的 117 个页面；包含正文、代码片段、接线表、配置说明、下载入口和首页长图中的板卡信息。  
> 说明：网页中的“厂家资料参数”是模块厂家资料的转述；具体采购批次、模块版本和电气限制仍应以对应数据手册和实物为准。

## 1. 一页结论

天猛星是一块围绕 TI **MSPM0G3507** 的入门与竞赛开发板：板上集成 Type-C 供电、CH340E USB-UART、用户 LED、用户/复位/BSL 按键、W25Q128 SPI Flash、3.3 V 参考电压、常用显示屏接口和完整 80 针排针。网页教程同时覆盖 CCS-Theia、Keil、SysConfig、GPIO/中断/UART/定时器/PWM/ADC/DMA/I²C/SPI，以及 68 个显示、传感器、无线和执行器模块的移植案例。根页的主要技术信息以长图形式嵌入，正文文字很少，因此本整理把长图标注与各教程中的接线、参数交叉归纳。

最适合的学习路径是：**板载 LED/按键 → UART 调试 → SysConfig 外设 → ADC/DMA → I²C/SPI → 模块移植 → PID 小车项目**。

## 2. 板卡硬件与资源

### 2.1 主控和尺寸

- 主控：MSPM0G3507，Arm 32 位 Cortex-M0+，最高 80 MHz；LQFP-64；128 KB Flash、32 KB SRAM。
- 板卡外形：约 **69.967 mm × 44.975 mm**；两侧为兼容面包板/洞洞板的 2.54 mm 排针孔，宣传页标注引出 80 pin。
- MCU 端口：GPIOA、GPIOB；外设复用由 MSPM0G3507 数据手册和首页“丰富的引脚接口”图决定，不能把任意 GPIO 当作任意 UART/SPI/PWM 通道。
- 首页长图：[`index_20241209_143545.jpg`](https://wiki.lckfb.com/storage/images/zh-hans/tmx-mspm0g3507/index/index_20241209_143545.jpg)；其中包含资源标注、引脚复用、尺寸和 MCU 说明。

### 2.2 板载器件和保护

| 资源 | 网页整理出的作用/连接 |
|---|---|
| Type-C | 5 V 供电、连接板载 CH340E 串口；旁边有 5 V 自恢复保险丝。 |
| CH340E | USB 转串口；教程明确接到 **PA10/PA11（UART0）**，Type-C 接电脑即可进行串口收发和日志输出。 |
| MSPM0G3507 | 主控芯片；教程中用户 LED 为 PB22，用户按键为 PB21，BSL 为 PA18。 |
| W25Q128 | 板背面默认贴装的 SPI Flash，容量 128 Mbit/16 MB；教程连接为 PB6-CS、PB7-DO/MISO、PB8-DI/MOSI、PB9-CLK，3.3 V/GND。 |
| 电源 | 5 V 输入，板载 5 V→3.3 V 电源；3.3 V 电源指示灯；另有板载 3.3 V 参考电压。 |
| ADC 参考 | MSPM0G3507 可选内部 1.4/2.5 V VREF、MCU VDD、外部 VREF+/VREF− 三类基准；首页同时强调板载三路 ADC 电压基准选择。 |
| 保护 | 首页资源图标注静电/过流/过压保护芯片；显示屏/外设接口旁有电源线序调整和保护设计。 |
| 按键 | B21 用户按键、RST 复位键、BSL 功能键。PA18 为 BootLoader 相关引脚，正常启动时应保持低电平；不要把 BSL 当普通用户 GPIO 使用。 |
| LED | 用户 LED 正极接 PB22，经限流电阻到 GND；教程按高电平点亮。 |
| 显示接口 | 板上提供常用小尺寸显示屏接口；1.9 寸 ST7789 SPI 屏的默认接线为 PB9/PB8/PB10/PB11/PB14/PB26（见后文）。 |
| PA0/PA1 | 首页资源图标注有上拉电阻选择；大量 I²C 模块教程统一使用 PA1=SCL、PA0=SDA。 |

### 2.3 电气使用要点

- 3.3 V 逻辑模块可直接接 3.3 V；5 V 供电模块的信号是否为 3.3 V 兼容，要按模块数据手册确认。
- MQ 气体、继电器、电机、舵机和超声波模块的电源/峰值电流明显高于 MCU GPIO 能力；网页接线表给出了供电端，但没有替代独立电源、限流或电平转换设计。
- ADC 教程的示例把 PA27 接到 3.3 V，并用 `adc_value / 4095.0 * 3.3` 换算；真实项目应按所选 VREF、分压比和输入保护重新计算。

## 3. 常用引脚速查

| 用途 | 引脚/连接 | 备注 |
|---|---|---|
| 板载 LED | PB22 | 高电平点亮；PWM 教程使用 TIMG8-C1。 |
| 用户按键 B21 | PB21→按键→GND | SysConfig 配上拉输入；按下为低电平。 |
| BSL | PA18 | BootLoader 入口；上电/复位时应为低电平。 |
| USB 串口 | PA10/PA11 = UART0 ↔ CH340E | 9600-8-N-1 示例；接收用中断。具体 RX/TX 方向以 SysConfig/芯片手册为准。 |
| 板载 W25Q128 SPI1 | PB6=CS、PB7=MISO/DO、PB8=MOSI/DI、PB9=SCLK | CS 在示例中用 GPIO 控制，SPI 数据线用硬件 SPI。 |
| 1.9 寸 SPI 屏 | PB9=SCL/SCLK、PB8=SDA/MOSI、PB10=RES、PB11=DC、PB14=CS、PB26=BLK，另接 3V3/GND | SPI1_SCK、SPI1_PICO；CS/RES/DC/BLK 为 GPIO。 |
| 常用 I²C | PA1=SCL、PA0=SDA | 0.91/0.96 IIC 屏、AHT10/SHT20/SHT30/BMP180/MS5611/MPU6050/SGP30 等教程均采用。 |
| 常用模拟输入 | PA27 | 光敏、MQ、火焰、土壤、灰度、紫外线等案例常用 ADC。 |
| 常用数字输入 | PA26 或 PA7 | MQ/火焰/雨滴/循迹常用 PA26；人体红外/雷达等常用 PA7。 |
| 常用 UART 模块 | PB16=TXD、PB15=RXD（多数模块页） | HC05、红外编解码、语音模块、JQ8900 等；具体 UART 实例和交叉方向按 SysConfig 配置。 |
| PID 项目电机 PWM | PA26、PA27 | 原理分析页选择 TIMG7-C0/C1；教程有 8/10 kHz 的不同示例，按当前工程配置为准。 |
| PID 项目编码器 | PB00=A、PB01=B、3V3/GND | A/B 外部中断；教程通过相位差判断方向。 |
| PID 项目按键 | PA8、PA9、PA28、PA31 | 扩展板四按键，上拉输入。 |

首页的两张完整复用图同时列出 CAN、DAC、VREF、ADC、I²C、UART、SPI、TIMER 和特殊功能；开发时应按目标封装、引脚冲突和 SysConfig 实际可选项复核，不能只凭图上同名功能猜测。

## 4. CCS-Theia 与 Keil 入门手册

### 4.1 两套手册的共同流程

1. 建立/复制 MSPM0G3507 空白工程。
2. 在 SysConfig 中添加 GPIO、UART、TIMER、PWM、ADC、DMA、I²C 或 SPI 实例。
3. 配置引脚、时钟、触发源和中断，保存 `.syscfg`。
4. 让 SysConfig 生成 `ti_msp_dl_config.c/.h`，应用入口调用 `SYSCFG_DL_init()`。
5. 编译、配置仿真器、下载；串口日志通过 CH340E 送到电脑。

### 4.2 CCS-Theia 章节要点

| 章节 | 关键内容 |
|---|---|
| 环境搭建 | 安装 CCS 和 MSPM0 SDK；在线或手动安装 SDK；配置下载仿真器、SysConfig；示例从 `empty.c` 开始。 |
| 点亮 LED | PB22 配为 GPIOB 上端口输出、默认低电平/下拉；用 `DL_GPIO_setPins`、`DL_GPIO_clearPins` 或 `DL_GPIO_togglePins` 控制。 |
| 系统延时 | 对比空循环、`delay_cycles()` 和滴答定时器；空循环受主频/优化影响，`delay_cycles` 按 CPU 周期延时，精确定时建议用 GPTimer。延时是阻塞式的。 |
| 按键 | PB21 接地触发，内部上拉；RST/BSL/B21 三个按键中只有 B21 是普通用户按键，PA18 BSL 不建议复用。 |
| 外部中断 | PB21 按键触发 PB22 LED 翻转；配置 GPIO 中断、边沿和 NVIC，在固定名称的 ISR 中清标志并翻转 LED。网页文字对“按下/释放对应的上升沿或下降沿”有前后不一致，实际应以 SysConfig 选定边沿和波形验证。 |
| 串口 | PA10/PA11 的 UART0 接 CH340E；示例启用时钟树 SYSOSC_4M→MFCLK=4 MHz，9600、8 数据位、1 停止位、无校验、无硬件流控，接收字节中断，过采样选 16。 |
| 定时器 | MSPM0G 有 7 个定时器（TIMG 通用、TIMA 高级）；示例 BUSCLK=32 MHz，经 8 分频和 100 预分频得到 40 kHz，向下计数、1 s 溢出中断，ISR 翻转 PB22。 |
| PWM | PWM 基于 TIMA/TIMG；PB22 可用 TIMG8-C1；通过固定频率改变占空比做呼吸灯，频率应高于约 80 Hz 以避免可见闪烁。 |
| ADC | 12 位 SAR、17 个外部复用通道、最高 4 Msps；支持单次/重复单次/多通道序列/重复序列。示例 32 MHz ADC 时钟、4 MHz 采样、软件触发、右对齐，PA27 读取 0–3.3 V，并串口输出。 |
| DMA | 7 个独立通道，可配置优先级、触发源、8/16/32/64 位宽、单次/块/重复块/表格等模式；示例把 ADC 结果寄存器搬到 `ADC_VALUE` 内存变量。 |
| I²C | 讲解软件 I²C 与硬件 I²C；软件例程用 PA0/PA1 读取 SHT20，包含起始、地址、ACK、命令（温度 `0xF3`、湿度 `0xF5`）和数据换算。 |
| SPI | W25Q128（128 Mbit/16 MB）实验；PB6–PB9 为 SPI1/CS 组合，讲解 CS、FIFO、忙标志、读写/擦除、4 KB 扇区和 64 KB 块。 |

### 4.3 Keil 手册的环境差异

Keil 手册的外设理论与 CCS 基本相同，重点增加了工程复制和 `syscfg.bat` 配置：从 SDK 的 `empty` 工程复制，补齐 `source`、`.metadata` 和 `tools/keil/syscfg.bat`，重新设置 User 命令、头文件路径和 `driverlib.a` 路径；随后按 **hardware / middle / app** 三层管理工程。PID 训练项目采用这一套 Keil 流程，并用 J-LINK 的 SWD/CLK/5V/GND 接线下载。

Keil 章节还重复给出 LED、延时、按键、外部中断、UART、定时器、PWM、ADC、DMA、I²C、SPI 的完整图形化配置与示例代码；差异主要是下载器、工程属性、SysConfig 生成命令和文件管理，外设引脚/电气事实与 CCS 章节一致。

## 5. 模块移植手册的通用方法

模块手册的固定结构是“模块来源 → 厂家规格 → 移植过程 → 引脚选择 → 代码编写 → 上电验证”。通用动作如下：

1. 下载厂家例程/数据手册，确认供电、协议、时序、地址和线序。
2. 将厂家 `.c/.h` 或字库/图片资源复制进工程；把原工程的 `sys.h`、延时和 GPIO 宏换成 `board.h`、`DL_*` DriverLib API。
3. 在 SysConfig 里先解决引脚复用、方向、上下拉、时钟和中断；保存后再编译生成配置文件。
4. 将底层发送/接收函数替换为 `DL_SPI_*`、`DL_I2C_*`、`DL_UART_*`、`DL_GPIO_*` 等 API；不要重复初始化已经由 SysConfig 生成的引脚。
5. 用 UART/LED/显示屏做最小验证，再接入完整模块。

### 5.1 显示类（11 个页面）

| 模块（原页） | 规格摘要 | 接线表（模块→开发板） |
|---|---|---|
| [0.91寸彩屏](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/screen/0-91-color-screen.html) | 工作电压=3~5V; 工作电流=最大16mA; 模块尺寸=12(H) x 38(V) MM; 像素大小=128(H) x 32(V); 驱动芯片=SSD1306; 通信协议=IIC | GND→GND, VCC→3.3V, SCL→PA1, SDA→PA0 |
| [0.96寸IIC单色屏](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/screen/0-96-iic-single-screen.html) | 工作电压=3.3V; 工作电流=9MA; 模块尺寸=27.3 x 27.8 MM; 像素大小=128(H) x 64(V)RGB; 驱动芯片=SSD1306; 通信协议=IIC; 管脚数量=4 Pin（2.54mm间距排针） | GND→GND, VCC→3.3V, SCL→PA1, SDA→PA0 |
| [0.96寸SPI单色屏](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/screen/0-96-single-spi-screen.html) | 工作电压=3.3V; 工作电流=15MA; 模块尺寸=27.3 x 27.8 MM; 像素大小=128(H) x 64(V)RGB; 驱动芯片=SSD1306; 通信协议=SPI; 管脚数量=7 Pin（2.54mm间距排针） | GND→GND, VCC→3.3V, SCL→PB9, SDA→PB8, RES→PB10, DC→PB11, CS→PB14 |
| [0.96寸彩屏](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/screen/0-96-color-screen.html) | 工作电压=2.8~3.3V; 工作电流=30MA; 模块尺寸=24(H) x 30(V)MM; 像素大小=80(H) x 160(V) RGB; 驱动芯片=ST7735; 通信协议=SPI; 管脚数量=8 Pin（2.54mm间距排针） | GND→GND, VCC→3.3V, SCL→PB9, SDA→PB8, RES→PB10, DC→PB11, CS→PB14, BLK→PB26 |
| [1.28寸圆屏LCD彩色显示屏](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/screen/1-28-round-color-screen.html) | 工作电压=3.3V; 工作电流=20mA; 模块尺寸=44(H) x 36(V) x 2.8(D) MM; 像素间距=0.135(H) x 0.135(V); 驱动芯片=GC9A01; 通信协议=SPI | GND→GND, VCC→3.3V, SCL→PB9, SDA→PB8, RES→PB10, DC→PB11, CS→PB14, BLK→PB26 |
| [1.3寸单色OLED显示屏](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/screen/1-3-single-oled-screen.html) | 工作电压=3.3V ~ 5V; 工作电流=20mA; 模块尺寸=33.5 x 35.4 MM; 像素间距=0.23(H) x 0.23(V); 像素尺寸=0.21(H) x 0.21(V); 驱动芯片=SH1106; 通信协议=SPI（可调IIC） | GND→GND, VCC→3.3V, SCL→PB9, SDA→PB8, RES→PB10, DC→PB11, CS→PB14 |
| [1.3寸彩屏](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/screen/1-3-color-screen.html) | 工作电压=2.4V-3.3V; 工作电流=40MA; 模块尺寸=27.78(H) x 39.22(V) MM; 像素大小=240(H) x 240(V)RGB; 驱动芯片=ST7789V2; 通信协议=SPI | GND→GND, VCC→3.3V, SCL→PA12, SDA→PA14, RES→PA16, DC→PA26, CS1→PA8, BLK→PA27, FSO→PA13, CS2→PA24 |
| [1.47寸彩屏](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/screen/1-47-color-screen.html) | 工作电压=3.3V; 工作电流=90MA; 模块尺寸=30(H) x 37(V) MM; 像素大小=172(H) x 320(V)RGB; 驱动芯片=ST7789V3; 通信协议=SPI; 管脚数量=8 Pin（2.54mm间距排针） | GND→GND, VCC→3.3V, SCL→PB9, SDA→PB8, RES→PB10, DC→PB11, CS→PB14, BLK→PB26 |
| [1.69寸彩屏](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/screen/1-69-color-screen.html) | 工作电压=3.3V; 工作电流=90MA; 模块尺寸=31(H) x 48(V) MM; 像素大小=240(H) x 280(V)RGB; 驱动芯片=ST7789V2; 通信协议=SPI; 管脚数量=8 Pin（2.54mm间距排针） | GND→GND, VCC→3.3V, SCL→PB9, SDA→PB8, RES→PB10, DC→PB11, CS→PB14, BLK→PB26 |
| [8位数码管显示模块](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/screen/8-bit-led-tube.html) | 工作电压=4-5.5V; 工作电流=8-330MA; 扫描速率=500-1300Hz; 通信协议=单总线; 管脚数量=5 Pin（2.54mm间距排针） | VCC→5V, GND→GND, DIN→PA27, CS→PA26, CLK→PA25 |
| [MAX7219四合一点阵模块](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/screen/max7219-matrix-display.html) | 工作电压=4-5.5V; 工作电流=8-330MA; 扫描速率=500-1300Hz; 通信协议=串行通信; 管脚数量=5 Pin（2.54mm间距排针） | VCC→5V, GND→GND, DIN→PA27, CS→PA26, CLK→PA25 |

显示页的共同要点：SPI 屏通常把 SCL→SCK、SDA→MOSI，RES/DC/CS/BLK 用 GPIO；IIC 屏统一落到 PA1/PA0。0.96 寸 ST7735 页面还要求把厂家 `sys.h` 改为 `board.h`、注释掉厂家 `delay.h`、用 `DL_SPI_transmitData8` 等待总线空闲，并调整中文字体索引步长；1.3 寸彩屏使用双片选/FSO 线，不能套用普通单片选屏的宏。显示屏供电多为 3.3 V，1.47/1.69 寸页面标注工作电流约 90 mA，接线和供电应留余量。

### 5.2 传感器类（41 个页面）

| 传感器（原页） | 规格/接口摘要 | 接线表（模块→开发板） |
|---|---|---|
| [ADS1115多路模数转换器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/ads1115-multichannel-a-to-d-sensor.html) | 工作电压=2.0-5.5V; 工作电流=150uA; 采集精度=16位; 采集通道=4通道; 控制方式=IIC; 管脚数量=10 Pin（2.54mm间距排针） | VDD→5V0, GND→GND, SCL→PA1, SDA→PA0, A0→接入要测量的电压 |
| [AGS10有害气体传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/ags10-harmful-gas-sensor.html) | 工作电压=3.0-6.0 V; 典型功率=75mW; 采样周期=≥2s; 接口速率=I2C从机模式（≤15kHz）; 预热时间=≥120s; 工作温度=0～50℃; 工作湿度=0～95%RH; 寿命=＞5年（25℃，清洁空气中） | VCC→3V3, GND→GND, SCL→PA1, SDA→PA0 |
| [AHT10温湿度传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/aht10-temp-humi-sensor.html) | 工作电压=1.8~3.6V; 工作电流=0.25~23uA; 湿度误差=±2%RH; 温度误差=±0.3℃; 输出方式=IIC; 管脚数量=3 Pin | VCC→3V3, GND→GND, SCL→PA1, SDA→PA0 |
| [BH1750光照强度传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/bh1750-light-intensity-sensor.html) | 工作电压=3-5V; 工作电流=200uA; 探测范围=1~65536 lx; 模块尺寸=32.6mm×15.2mm×11.6mm; 输出方式=IIC; 管脚数量=5 Pin | VCC→3V3, GND→GND, SCL→PA1, SDA→PA0 |
| [BMP180气压传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/bmp180-pressure-sensor.html) | 工作电压=1.8~3.6V; 工作电流=0.1~1000uA; 温度精度=±1℃; 温度范围=0~65℃; 气压范围=300~1100 hPa; 气压精度=1 hPa; 输出方式=IIC; 管脚数量=3 Pin | VCC→3V3, GND→GND, SCL→PA1, SDA→PA0 |
| [DHT11温湿度传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/dht11-temp-humi-sensor.html) | 工作电压=3-5.5V; 工作电流=1MA; 测量分辨率=8 bit; 湿度量程=20 - 90 %RH; 湿度精度=±5 %RH; 温度量程=0 - 50 ℃; 温度精度=±2 ℃; 通信协议=单总线 | VCC→5V, DAT→PA9, GND→GND |
| [DS18B20温度传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/ds18b20-temp-sensor.html) | 工作电压=3-5.5V; 工作电流=750nA~1.5mA; 测量分辨率=9位到12位可编程分辨率; 温度量程=-55 ~ +125 ℃; 测量精度=±0.5 ℃; 通信协议=单总线; 管脚数量=3 Pin（2.54mm间距排针） | VCC→5V0, GND→GND, DQ→PA7 |
| [GP2Y1014AU粉尘传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/gp2y1014au-dust-sensor.html) | 工作电压=5-7V; 消耗电流=最大20mA; 最小粒子检出值=0.8微米; 灵敏度=0.5V(0.1mg/m3); 清洁空气中电压=0.9V （典型）; 重量=15g; 尺寸大小=46x30x17.6mm | 蓝线→3V3, 绿线→GND, LED-白线→PA26, 黄线→GND, VO-黑线→PA27, 红线→5V0 |
| [HX711称重传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/hx711-weighing-sensor.html) | 工作电压=2.6V-5.5V; 工作电流=100~1500uA; ADC精度=24位; 输出方式=串行输出; 管脚数量=4 Pin | VCC→3V3, GND→GND, SCK→PA1, DT→PA0 |
| [JY61P三维姿态测量传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/jy61p-measurement-sensor.html) | 工作电压=5V; 供电电流【工作5V】=8343mA; 供电电流【休眠5V】=9.91uA; 回传速率=200HZ（最高）; 带宽=256HZ（最高）; 控制方式1=串口【默认9600】; 控制方式2=IIC; 管脚数量=12 Pin（2.54mm间距排针） | VCC→5V0, GND→GND, SCL→PA1, SDA→PA0 |
| [MLX90614无接触测温传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/mlx90614-non-contact-temp-sensor.html) | 工作电压=4.5~5.5V; 工作电流=1.3~2.5mA | VCC→5V, GND→GND, SCL→PA1, SDA→PA0 |
| [MPU6050六轴传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/mpu6050-six-axis-sensor.html) | 工作电压=3-5V(模块带有LDO); 工作电流=5MA; 通信接口=IIC | VCC→3V3, GND→GND, SCL→PA1, SDA→PA0 |
| [MQ-135空气质量传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/mq-135-sensor.html) | 工作电压=3.3V ~ 5V; 工作电流=150MA; 输出方式=DO接口为数字量输出 AO接口为模拟量输出; 读取方式=ADC; 管脚数量=4 Pin（2.54mm间距排针） | VCC→5V0, GND→GND, DO→PA26, AO→PA27 |
| [MQ-2烟雾检测传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/mq-2-sensor.html) | 工作电压=5V; 工作电流=150MA; 输出方式=DO接口为数字量输出 AO接口为模拟量输出; 读取方式=ADC; 管脚数量=4 Pin（2.54mm间距排针） | VCC→5V0, GND→GND, DO→PA26, AO→PA27 |
| [MQ-3酒精检测传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/mq-3-sensor.html) | 工作电压=3.3V ~ 5V; 工作电流=150MA; 输出方式=DO接口为数字量输出 AO接口为模拟量输出; 读取方式=ADC; 管脚数量=4 Pin（2.54mm间距排针） | VCC→5V0, GND→GND, DO→PA26, AO→PA27 |
| [MQ-4甲烷检测传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/mq-4-sensor.html) | 工作电压=3.3V ~ 5V; 工作电流=150MA; 输出方式=DO接口为数字量输出 AO接口为模拟量输出; 读取方式=ADC; 管脚数量=4 Pin（2.54mm间距排针） | VCC→5V0, GND→GND, DO→PA26, AO→PA27 |
| [MQ-5液化气检测传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/mq-5-sensor.html) | 工作电压=3.3V ~ 5V; 工作电流=150MA; 输出方式=DO接口为数字量输出 AO接口为模拟量输出; 读取方式=ADC; 管脚数量=4 Pin（2.54mm间距排针） | VCC→5V0, GND→GND, DO→PA26, AO→PA27 |
| [MQ-6丙烷检测传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/mq-6-sensor.html) | 工作电压=3.3V ~ 5V; 工作电流=150MA; 输出方式=DO接口为数字量输出 AO接口为模拟量输出; 读取方式=ADC; 管脚数量=4 Pin（2.54mm间距排针） | VCC→5V0, GND→GND, DO→PA26, AO→PA27 |
| [MQ-7一氧化碳检测传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/mq-7-sensor.html) | 工作电压=3.3V ~ 5V; 工作电流=150MA; 输出方式=DO接口为数字量输出 AO接口为模拟量输出; 读取方式=ADC; 管脚数量=4 Pin（2.54mm间距排针） | VCC→5V0, GND→GND, DO→PA26, AO→PA27 |
| [MQ-8氢气检测传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/mq-8-sensor.html) | 工作电压=3.3V ~ 5V; 工作电流=150MA; 输出方式=DO接口为数字量输出 AO接口为模拟量输出; 读取方式=ADC; 管脚数量=4 Pin（2.54mm间距排针） | VCC→5V0, GND→GND, DO→PA26, AO→PA27 |
| [MQ-9可燃气体检测传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/mq-9-sensor.html) | 工作电压=3.3V ~ 5V; 工作电流=150MA; 输出方式=DO接口为数字量输出 AO接口为模拟量输出; 读取方式=ADC; 管脚数量=4 Pin（2.54mm间距排针） | VCC→5V0, GND→GND, DO→PA26, AO→PA27 |
| [MS1100气体传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/ms1100-gas-sensor.html) | 工作电压=5V; 工作电流=< 50uA; 输出方式=ADC+GPIO; 管脚数量=4 Pin | VCC→5V0, GND→GND, DO→PA26, AO→PA27 |
| [MS5611气压传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/ms5611-pressure-sensor.html) | 工作电压=1.8~3.6V; 工作电流=0.25~23uA; 温度精度=0.8℃; 温度范围=-40~85℃; 气压范围=10~1200 mbar; 气压精度=1.5 mbar; 输出方式=IIC; 管脚数量=3 Pin | VCC→3V3, GND→GND, SCL→PA1, SDA→PA0 |
| [S12SD紫外线传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/s12sd-uv-sensor.html) | 工作电压=2.7-5V; 工作电流=1mA; 测量角度=130度; 温飘=0.08%/℃; 检测波长范围=240nm~370nm; 输出方式=ADC; 管脚数量=3 Pin | VCC→3V3, GND→GND, SIG→PA27 |
| [SGP30气体传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/sgp30-gas-sensor.html) | 工作电压=3.3V; 工作电流=40mA; 输出方式=IIC; 管脚数量=4 Pin | VCC→3V3, GND→GND, SCL→PA1, SDA→PA0 |
| [SHT20温湿度传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/sht20-temp-humi-sensor.html) | 工作电压=2.1~3.6V; 工作电流=0.1~1000uA; 温度精度=±0.3℃; 温度范围=-40~125℃; 湿度范围=0~100 %RH; 湿度精度=±3%RH; 输出方式=IIC; 管脚数量=4 Pin | VCC→3V3, GND→GND, SCL→PA1, SDA→PA0 |
| [SHT30温湿度传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/sht30-temp-humi-sensor.html) | 工作电压=2.4-5.5V; 工作电流=0.2~1500uA; 温度测量范围=-40~125℃; 温度测量精度=±0.3℃; 湿度测量范围=0~100%RH; 湿度测量精度=±2%RH; 输出方式=IIC; 管脚数量=4 Pin | VCC→3V3, GND→GND, SCL→PA1, SDA→PA0 |
| [SR04超声波测距传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/sr04-ultrasonic-ranging-sensor.html) | 工作电压=3-5.5V; 工作电流=5.3MA; 感应角度=小于15度; 探测距离=2CM-600CM; 探测精度=0.1CM+1%; 输出方式=GPIO; 管脚数量=4 Pin | VCC→3V3, Trig→PA9, Echo→PA8, GND→GND |
| [TCS34725颜色识别传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/tcs34725-color-recognition-sensor.html) | 工作电压=3.3-5V; 工作电流=2.5~330uA; 输出方式=IIC; 管脚数量=7 Pin | VIN→5V0, GND→GND, 3V3→3V3, SCL→PA1, SDA→PA0 |
| [TTP224触摸传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/ttp224-touch-sensor.html) | 工作电压=2.4-5.5V; 工作电流=2.5uA~9uA; 模块尺寸=35x29 mm; 最快响应时间=100Ms; 控制方式=GOIO; 管脚数量=6 Pin（2.54mm间距排针） | OUT1→PA14, OUT2→PA15, OUT3→PA16, OUT4→PA17, GND→GND, VCC→3V3 |
| [US-016超声波测距传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/us-016-ultrasonic-ranging-sensor.html) | 工作电压=3.3V-5V; 工作电流=3.8MA; 感应角度=小于15度; 探测距离=2CM-300CM; 探测精度=0.3CM+1%; 输出方式=模拟电压; 管脚数量=4 Pin | VCC→3V3, GND→GND, Range→NC（不接）, Out→PA27 |
| [人体红外传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/human-body-infrared-sensor.html) | 工作电压=4.5~20V; 工作电流=< 50uA; 电平输出=高3.3V/低0V; 感应角度=< 100度锥角; 输出方式=GPIO; 管脚数量=3 Pin | VCC→5V0, GND→GND, OUT→PA7 |
| [光敏电阻光照传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/photoresistance-sensor.html) | 工作电压=3.3-5V; 工作电流=1MA; 模块尺寸=31.1475 x 14.097mm; 输出方式=DO接口为数字量输出 AO接口为模拟量输出; 读取方式=ADC; 管脚数量=4 Pin（2.54mm间距排针） | VCC→3V3, GND→GND, DO→PA26, AO→PA27 |
| [土壤湿度传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/soil-moisture-sensor.html) | 工作电压=3.3V-5V; 工作电流=150MA; 输出方式=DO接口为数字量输出 AO接口为模拟量输出; 读取方式=ADC; 管脚数量=4 Pin（2.54mm间距排针） | VCC→5V0, GND→GND, DO→PA26, AO→PA27 |
| [微波多普勒无线雷达传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/microwave-doppler-radar-sensor.html) | 工作电压=5V±0.25V; 工作电流=30~50mA; 探测距离=2-16m 连续可调; 尺寸=R=30.6mm; 输出方式=GPIO; 管脚数量=3 Pin | VCC→5V0, GND→GND, OUT→PA7 |
| [指纹识别传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/fingerprint-recognition-sensor.html) | 工作电压=3.0-3.6V; 工作电流=30~60mA; 指纹存容量=300 枚(ID:0~299); 认假率=<0.001%; 搜索时间=<0.3(S); 控制方式=串口或USB; 管脚数量=8 Pin（2.54mm间距排针） | 红线（3V3）→3V3, 黄线（TXD）→PA9, 白线（RXD）→PA8, 黑线（GND）→GND, 蓝线（WAK）→PA12, 绿线（VTI）→3V3 |
| [火焰传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/flame-sensor.html) | 工作电压=3.3V-5V; 探测距离=1米; 输出方式=DO接口为数字量输出 AO接口为模拟量输出; 读取方式=ADC与数字量（0和1）; 管脚数量=4 Pin（2.54mm间距排针） | VCC→5V0, GND→GND, DO→PA26, AO→PA27 |
| [灰度传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/grayscale-sensor.html) | 工作电压=3.3V-5V; 工作电流=< 20mA; 输出格式=模拟信号输出; 控制接口=ADC; 管脚数量=3 Pin（2.54mm间距排针） | VCC→5V0, GND→GND, OUT→PA27 |
| [红外循迹传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/Infrared-tracking-sensor.html) | 工作电压=3.3V-5V; 检测反射距离=1mm~25mm适用; 输出方式=DO接口为数字量输出；AO接口为模拟量输出; 读取方式=ADC; 管脚数量=4 Pin（2.54mm间距排针） | VCC→5V, GND→GND, DO→PA26, AO→PA27 |
| [红外测距传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/Infrared-distance-sensor.html) | 工作电压=3.3-5V; 工作电流=33MA; 模块尺寸=37 x 21.6mm; 输出方式=模拟量输出; 读取方式=ADC; 管脚数量=3 Pin | VCC→5V, GND→GND, DATA→PA27 |
| [雨滴传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/rain-sensor.html) | 工作电压=3.3V-5V; 探测距离=1米; 输出方式=DO接口为数字量输出 AO接口为模拟量输出; 读取方式=ADC与数字量（0和1）; 管脚数量=4 Pin（2.54mm间距排针） | VCC→5V0, GND→GND, DO→PA26, AO→PA27 |

传感器页可按接口分组：

- **I²C**：AHT10、SHT20、SHT30、BMP180、MS5611、SGP30、BH1750、MPU6050、TCS34725、ADS1115、AGS10、MLX90614 等，绝大多数接 PA1/PA0；注意地址、测量命令、等待时间和供电电平。
- **ADC/数字量**：光敏、红外测距、红外循迹、MQ-2/3/4/5/6/7/8/9/135、火焰、雨滴、灰度、土壤、MS1100、S12SD、US-016 等；AO/SIG 接 PA27 的示例最多，DO 接 PA26/PA7。5 V 模块的 AO/DO 不能默认认为可直接进入 3.3 V MCU，须核对输出范围。
- **单总线/时序**：DHT11 使用 40 bit、高位先出、起始低电平至少 18 ms，并用校验和验证；DS18B20 使用 9–12 bit 可编程分辨率和单总线温度转换。
- **脉冲/串行**：SR04 用 Trig/ Echo 的 GPIO 时序测距；HX711 用 SCK/DT 串行读取 24 位 ADC；指纹模块用 UART；JY61P 支持默认 9600 UART 或 I²C，页面还说明姿态数据的回传速率/带宽。
- **特殊模块**：GP2Y1014AU 需要 LED 脉冲和模拟采样配合；微波雷达/人体红外输出 GPIO；TTP224 输出四路触摸状态。

### 5.3 无线/视觉类（5 个页面）

| 模块（原页） | 规格/接口摘要 | 接线表（模块→开发板） |
|---|---|---|
| [HC05蓝牙模块](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/rf/hc05-bluetooth-module.html) | 工作电压=3.6-6V; 供电电流=40mA; 发射功率=4dBm(最大); 参考距离=10米; 控制方式=串口; 管脚数量=6 Pin（2.54mm间距排针） | VCC→5V0, GND→GND, TXD→PB16, RXD→PB15, STATE→PA07 |
| [OpenMV4摄像头](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/rf/open-mv4-camera.html) |  | VCC→5V0, GND→GND, TXD(P4)→PA09, RXD(P5)→PA08 |
| [RC522射频IC卡识别模块](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/rf/rc522-rf-ic-card-identification-module.html) | 工作电压=3.3V; 工作电流=10-26mA; 模块尺寸=40mm×60mm; 支持的卡类型=mifare1 S50、mifare1 S70、mifare UltraLight、mifare Pro、mifare Desfire; 控制方式=SPI | SDA(CS)→PB25, SCK→PB18, MOSI→PB17, MISO→PB19, IRQ→NC不接, GND→GND, RST→PB0, 3.3V→3V3 |
| [红外接收模块](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/rf/infrared-receiving-module.html) |  | +→3V3, -→GND, DAT→PA07 |
| [红外解码编码模块](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/rf/Infrared-decoding-coding-module.html) | 工作电压=5V; 供电电流=> 100mA; 发射距离=6-10米（根据环境光线不同和收发情况有偏差）; 接收距离=6-10米（和发射设备的功率有关）; 控制方式=串口; 管脚数量=4 Pin（2.54mm间距排针） | VCC→5V0, GND→GND, TXD→PB16, RXD→PB15 |

- HC05、红外编解码和 OpenMV4 走串口；常用接线为 PB16/PB15（OpenMV4 页面使用 PA09/PA08）。
- 红外接收页讲 NEC 协议、载波/时序和 GPIO 解码；红外编解码模块本身通过 UART 发送/接收。
- RC522 用 SPI，IRQ 可不接，页面给出 RST、CS、SCK、MOSI、MISO 的完整映射。
- OpenMV4 页包含颜色阈值设置、OpenMV IDE 操作、串口案例和任意颜色循迹案例，需同时烧录摄像头脚本和 MSPM0G3507 端代码。

### 5.4 控制/执行类（11 个页面）

| 模块（原页） | 规格/接口摘要 | 接线表（模块→开发板） |
|---|---|---|
| [16路舵机驱动模块](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/control/16-ch-servo-drive-module.html) | 输入电压=3.3V~5V; 额定电流=15mA; 控制方式=IIC; 尺寸=21(长)*21(宽)[单位:mm] | VCC→3V3, V+→5V0, GND→GND, SCL→PA1, SDA→PA0 |
| [AT24C02-EEPROM存储器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/control/at24c02-eeprom-memory.html) | 工作电压=1.8V-5.5V; 工作电流=最大3mA; 通信接口=IIC; 内存=2048位; 时钟速度=5V时最大1000Khz，其余为400Khz | VCC→3V3, GND→GND, SCL→PA1, SDA→PA0 |
| [JQ8900语音播报模块](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/control/jq8900-voice-broadcast-module.html) | 输入电压=2.8V-5.5V; IO电压=3.3V（模块引脚输出电压）; 额定电流=500uA~10mA; 控制方式=串口 | DC-5V→5V0, GND→GND, TXD→PB16, RXD→PB15, SPK+(BP)→喇叭正极, SPK-(BN)→喇叭负极, VPP→PA07 |
| [L298N电机驱动模块](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/control/l298n-motor-drive-module.html) | 驱动电压=5V~24V; 驱动电流=2A; 逻辑电压=5V; 逻辑电流=36mA; 控制方式=PWM | GND→GND, +12V→接电源(6V ~ 24V), IN1→PA17(TIMA1-C0), IN2→PA16(TIMA1-C1), IN3→PA12(TIMG0-C0), IN4→PA13(TIMG0-C1), OUT1→电机A, OUT2→电机A, OUT3→电机B, OUT4→电机B |
| [N20直流减速电机-带霍尔编码器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/control/n20-hall-encoder-motor.html) | 驱动电压=根据采购的电机确定，比如我买的是6V300转; 驱动电流=根据电机输入电压决定; 控制方式=PWM | VCC→3V3, GND→GND, LENCODE1→PA27, LENCODE2→PA26, RENCODE1→PA31, RENCODE2→PA28, PWML1→PA25, PWML2→PA24, PWMR1→PA08, PWMR2→PB04 |
| [SG90舵机](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/control/sg90-steering-engine.html) | 驱动电压=3V~7.2V; 工作扭矩=1.6KG/CM; 控制方式=PWM; 转动角度=180度 | 棕色线→GND, 黄色线→PA12(TIMG0-C0), 红色线→5V0 |
| [TB6612电机驱动模块](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/control/tb6612-motor-drive-module.html) | VM电机电压=< 12V; VCC芯片电压=2.7~5.5V; 输出电流=1A; 控制方式=PWM | GND→GND, VCC→5V0, VM→接入电源（3.7V~12V）, STBY→3.3V, PWMA→PA16, AIN1→PA14, AIN2→PA15, PWMB→PA17, BIN1→PA12, BIN2→PA13 |
| [WS2812彩灯](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/control/ws2812-color-rgb-led.html) | 工作电压=3.7-5.3V; 工作电流=16MA; 控制方式=单总线; 管脚数量=4 Pin（2.54mm间距排针） | GND→GND, DIN→PB8, VCC→5V0, GND→GND |
| [双轴按键摇杆模块](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/control/two-axis-keystroke-rocker-module.html) | 驱动电压=3.3V~5V; 控制方式=ADC+GPIO | GND→GND, +5V→5V0, VRX→PA27, VRY→PA26, SW→PA22 |
| [继电器模块](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/control/relay-module.html) | 工作电压=5V; 可控制的交流电压范围=可达250V，10A; 可控制的交流电压范围=可达30V，10A; 控制方式=GPIO; 管脚数量=4 Pin（2.54mm间距排针）; 说明=采用光耦隔离保护MCU引脚；采用三极管驱动； | VCC→5V0, GND→GND, IN1→PA07 |
| [语音合成播报模块](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/control/syn6288-speech-synthesis-broadcast-module.html) | 输入电压=2.4V~5.1V; 额定电流=2.0uA~280mA; 控制方式=串口 | VCC→5V0, GND→GND, TXD→PB16, RXD→PB15, BP→喇叭正极, BN→喇叭负极 |

控制类的接口模式：

- **PWM**：TB6612、L298N、SG90、N20 电机；速度/角度由占空比或脉宽控制。TB6612 还要处理 STBY、AIN/BIN 方向脚和独立电机电源 VM。
- **GPIO/单总线**：继电器为 GPIO（光耦隔离、三极管驱动），WS2812 用严格时序的单总线；板上 5 V 供电和地线必须可靠。
- **I²C**：16 路舵机驱动器、AT24C02，均使用 PA1/PA0 示例。
- **UART/语音**：语音合成模块和 JQ8900 使用 PB16/PB15，JQ8900 还要接扬声器、VPP 等控制脚。
- **编码器电机**：N20 页面给出双电机 PWM 与四路霍尔编码器输入的接线，适合配合定时器/外部中断测速度与方向。

## 6. TI 简易 PID 入门项目

项目总页说明：以天猛星为主控、BDR6126D 为电机驱动、1.9 寸 ST7789 SPI 屏和带霍尔编码器电机为对象，实现 **PID 定速**、**PID 定距**、按键调参和屏幕曲线显示；软件采用裸机的“轮询 + 中断 + 状态机”组合。

### 6.1 硬件

- 开发板：LCKFB-TMX-MSPM0G3507。
- 电机驱动：BDR6126D；原理图选 PA26/PA27 为 TIMG7-C0/C1 PWM，PWM 示例在不同章节出现 8 kHz 或 10 kHz，需以当前工程值为准。
- 编码器：A/B 接 PB00/PB01、3.3 V/GND；A/B 相位差用于判断方向，20 ms 定时器中断用于测速。
- 屏幕：1.9 寸 ST7789、170×320、SPI，接线为 PB9/PB8/PB10/PB11/PB14/PB26。
- 按键：扩展板四按键接 PA8、PA9、PA28、PA31，上拉输入；开源 FlexibleButton 库提供去抖、短按、长按、双击等事件。
- 项目还讲解 Type-C、电源、插件器件选型、嘉立创 EDA 原理图/PCB、Gerber 导出、免费打样和焊接顺序。

### 6.2 软件结构和章节成果

| 章节 | 整理后的核心产出 |
|---|---|
| 电路原理 | 电源、BDR6126D、电机/编码器、按键和开发板接口的作用。 |
| 原理图与 PCB | 嘉立创 EDA 工程、器件搜索、网络标签、模块框选、布局、走线和 DRC。 |
| 免费 PCB 打样 | 领券、导出 Gerber/BOM/坐标、2 层 FR-4、1.6 mm 等下单设置。具体工艺按平台当前页面为准。 |
| 购买器件 | 立创商城基础器件 + 淘宝屏幕/电机/结构件；页面列出器件型号和商城编号。 |
| 焊接练习 | 电阻/二极管/Type-C/DIP 座/驱动芯片/按键/电容/XH2.54/排针排母/屏幕/电机的焊接次序与极性注意。 |
| 开发环境 | Keil、MSPM0G3507 芯片包、SysConfig、J-LINK SWD；补齐 SDK `source/.metadata` 与 `syscfg.bat`。 |
| 工程框架 | hardware（硬件驱动）/middle（按键、PID 等协议和库）/app（界面与业务）；配置 80 MHz、PB22 LED、PA10/PA11 串口。 |
| 彩屏驱动 | SysConfig 配 SPI/GPIO，底层 `spi_write_bus`，移植厂家 ST7789 驱动并验证点屏。 |
| 按键驱动 | 读取四个上拉输入，移植 FlexibleButton，按 5–20 ms 周期扫描并分发按键事件。 |
| 电机驱动 | BDR6126D 两个方向脚 + PWM；长按按键启动/停止，限制 PWM 最大值。 |
| 编码器 | 外部中断获取脉冲；固定时间窗计算脉冲速率；A/B 相位判断方向。 |
| UI | 手工绘制圆角矩形、标题、参数选择框、目标/当前值和波形；只有数据变化时刷新屏幕，减少闪烁。 |
| 事件/状态机 | 页面：首页、定速、定距、设置、调参；前台主循环 + 后台 20 ms 定时器中断 + 事件管理器。 |
| PID 定速 | 误差 `e=target-current`，积分累加、微分 `e-last_e`，积分限幅和输出限幅；输出映射到 0–9999 PWM。 |
| PID 定距 | 将电机目标角度/距离换算为目标脉冲数；暂停“每 20 ms 清零测速”的逻辑，持续累计编码器脉冲，用 PID 逼近目标。页面以 13 线编码器、1:48 减速比、AB 双边沿计数为示例。 |
| 完整案例 | 集成硬件、屏幕、按键、编码器、状态机、PID 和波形；完整工程通过下载中心获取。 |

PID 页面强调经验调参：先把 Kp/Ki/Kd 设为 0，再逐步加入 P、I、D，观察响应、超调、稳态误差和噪声；实际工程还要考虑 PWM 饱和、积分风up、采样周期和编码器量化误差。

## 7. 下载、视频和常见问题

- [下载中心](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/download-center.html) 提供两类百度网盘：完整例程/软件/数据手册，以及模块移植代码；页面给出提取码 `lckf`。
- [视频教学链接](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/video/) 指向 TI 开发板入门视频（Bilibili）。
- [常见问题](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/question/) 主要覆盖：例程头文件路径、J-LINK 显示未知芯片、CCS `.syscfg` 打不开、VS Code/CCS CIO、复位后不运行、Keil 识别 J-LINK 但不识别芯片、CCS 堆栈/堆大小。

### 7.1 Q&A 里的可执行结论

1. 工程报错先检查头文件搜索路径是否指向当前工程。
2. J-LINK 显示未知芯片时可先跳过提示；Keil 若仍不能识别，删除工程目录下旧 J-LINK 配置，重新选择 Cortex-M0+。
3. CCS SysConfig 打不开通常与 SysConfig/SDK 版本不匹配有关；新建工程并更新 SDK。
4. CIO/`printf` 需要 `<stdio.h>`、足够的 Stack/Heap 和 Debug 属性中的 CIO 选项；页面建议 Stack 至少 400 B、Heap 至少 `0x400`。
5. 80 MHz 配置后“烧录能跑、复位不跑”，页面建议增加起振等待时钟周期；这属于时钟配置问题，应结合当前 SysConfig 版本核对。
6. CCS 工程属性中的 Stack Size 可能不直接作用于最终链接脚本；页面给出复制并编辑 `device_linker.cmd`、关闭自动链接器生成、检查 `.map` 的方法。此方法会改生成文件，实际项目应先确认自己的构建流程和版本。

## 8. 阅读时发现的文档边界与风险

- 根页的关键板卡介绍是图片长图；下载中心和部分项目页包含外链/图片，网页文本抽取会看不到图片中的细线标注，因此本文件把可读的标注转成文字，并保留原图链接。
- 手册存在复制粘贴痕迹：部分 Keil 页面标题仍出现“地猛星”，外部导航还混入其他 MSPM0 开发板；本整理只纳入天猛星 URL 下的内容。
- 外部中断章节对 PB21 按键的上升沿/下降沿描述前后不一致；应按当前原理图、SysConfig 配置和实际波形确认。
- 不同模块页面的参数来自不同厂家资料，电流、供电和引脚线序可能随采购版本变化；接线前应优先核对实物丝印和数据手册。
- 网页给出的示例代码面向教学，未统一处理超时、并发、输入过压、总线冲突、PID 积分饱和和电源完整性；移植到正式项目需要补充这些工程化保护。

## 9. 全量页面索引

本次阅读覆盖以下页面；点击标题可回到原文。

### 根页、下载、视频、问答（4 页）

- [【立创·天猛星MSPM0G3507开发板】介绍](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/)
- [天猛星下载中心](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/download-center.html)
- [立创开发板技术文档中心](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/question/)
- [【立创·天猛星 MSPM0G3507 开发板】视频教学链接](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/video/)

### CCS-Theia 入门（13 页）

- [【立创·天猛星MSPM0G3507开发板】CCS-Theia入门手册](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/ccs-beginner/)
- [ADC采集](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/ccs-beginner/adc.html)
- [天猛星入门手册](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/ccs-beginner/delay.html)
- [DMA传输](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/ccs-beginner/dma.html)
- [天猛星入门手册](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/ccs-beginner/exti.html)
- [1. I2C协议](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/ccs-beginner/i2c.html)
- [1. 环境搭建](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/ccs-beginner/install.html)
- [天猛星入门手册](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/ccs-beginner/key.html)
- [1. 点亮第一个灯](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/ccs-beginner/led.html)
- [PWM输出](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/ccs-beginner/pwm.html)
- [1. SPI协议](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/ccs-beginner/spi.html)
- [1. 定时器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/ccs-beginner/timer.html)
- [天猛星入门手册](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/ccs-beginner/uart.html)

### Keil 入门（13 页）

- [天猛星入门手册](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/keil-beginner/)
- [ADC采集](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/keil-beginner/adc.html)
- [地猛星入门手册](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/keil-beginner/delay.html)
- [DMA传输](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/keil-beginner/dma.html)
- [地猛星入门手册](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/keil-beginner/exti.html)
- [1. I2C协议](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/keil-beginner/i2c.html)
- [天猛星入门手册](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/keil-beginner/install.html)
- [地猛星入门手册](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/keil-beginner/key.html)
- [3. 点亮第一个灯](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/keil-beginner/led.html)
- [PWM输出](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/keil-beginner/pwm.html)
- [1. SPI协议](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/keil-beginner/spi.html)
- [1. 定时器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/keil-beginner/timer.html)
- [地猛星入门手册](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/keil-beginner/uart.html)

### 模块移植（68 页）

- [16路舵机驱动模块](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/control/16-ch-servo-drive-module.html)
- [AT24C02-EEPROM存储器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/control/at24c02-eeprom-memory.html)
- [JQ8900语音播报模块](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/control/jq8900-voice-broadcast-module.html)
- [L298N电机驱动模块](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/control/l298n-motor-drive-module.html)
- [N20直流减速电机-带霍尔编码器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/control/n20-hall-encoder-motor.html)
- [继电器模块](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/control/relay-module.html)
- [SG90舵机](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/control/sg90-steering-engine.html)
- [语音合成播报模块](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/control/syn6288-speech-synthesis-broadcast-module.html)
- [TB6612电机驱动模块](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/control/tb6612-motor-drive-module.html)
- [双轴按键摇杆模块](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/control/two-axis-keystroke-rocker-module.html)
- [WS2812彩灯](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/control/ws2812-color-rgb-led.html)
- [红外解码编码模块](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/rf/Infrared-decoding-coding-module.html)
- [HC05蓝牙模块](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/rf/hc05-bluetooth-module.html)
- [红外接收模块](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/rf/infrared-receiving-module.html)
- [OpenMV4摄像头](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/rf/open-mv4-camera.html)
- [RC522射频IC卡识别模块](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/rf/rc522-rf-ic-card-identification-module.html)
- [0.91寸彩屏](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/screen/0-91-color-screen.html)
- [0.96寸彩屏](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/screen/0-96-color-screen.html)
- [0.96寸IIC单色屏](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/screen/0-96-iic-single-screen.html)
- [0.96寸SPI单色屏](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/screen/0-96-single-spi-screen.html)
- [1.28寸圆屏LCD彩色显示屏](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/screen/1-28-round-color-screen.html)
- [1.3寸彩屏](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/screen/1-3-color-screen.html)
- [1.3寸单色OLED显示屏](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/screen/1-3-single-oled-screen.html)
- [1.47寸彩屏](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/screen/1-47-color-screen.html)
- [1.69寸彩屏](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/screen/1-69-color-screen.html)
- [8位数码管显示模块](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/screen/8-bit-led-tube.html)
- [MAX7219四合一点阵模块](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/screen/max7219-matrix-display.html)
- [红外测距传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/Infrared-distance-sensor.html)
- [红外循迹传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/Infrared-tracking-sensor.html)
- [ADS1115多路模数转换器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/ads1115-multichannel-a-to-d-sensor.html)
- [AGS10有害气体传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/ags10-harmful-gas-sensor.html)
- [AHT10温湿度传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/aht10-temp-humi-sensor.html)
- [BH1750光照强度传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/bh1750-light-intensity-sensor.html)
- [BMP180气压传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/bmp180-pressure-sensor.html)
- [DHT11温湿度传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/dht11-temp-humi-sensor.html)
- [DS18B20温度传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/ds18b20-temp-sensor.html)
- [指纹识别传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/fingerprint-recognition-sensor.html)
- [火焰传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/flame-sensor.html)
- [GP2Y1014AU粉尘传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/gp2y1014au-dust-sensor.html)
- [灰度传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/grayscale-sensor.html)
- [人体红外传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/human-body-infrared-sensor.html)
- [HX711称重传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/hx711-weighing-sensor.html)
- [JY61P三维姿态测量传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/jy61p-measurement-sensor.html)
- [微波多普勒无线雷达传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/microwave-doppler-radar-sensor.html)
- [MLX90614无接触测温传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/mlx90614-non-contact-temp-sensor.html)
- [MPU6050六轴传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/mpu6050-six-axis-sensor.html)
- [MQ-135空气质量传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/mq-135-sensor.html)
- [MQ-2烟雾检测传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/mq-2-sensor.html)
- [MQ-3酒精检测传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/mq-3-sensor.html)
- [MQ-4甲烷检测传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/mq-4-sensor.html)
- [MQ-5液化气检测传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/mq-5-sensor.html)
- [MQ-6丙烷检测传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/mq-6-sensor.html)
- [MQ-7一氧化碳检测传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/mq-7-sensor.html)
- [MQ-8氢气检测传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/mq-8-sensor.html)
- [MQ-9可燃气体检测传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/mq-9-sensor.html)
- [MS1100气体传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/ms1100-gas-sensor.html)
- [MS5611气压传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/ms5611-pressure-sensor.html)
- [光敏电阻光照传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/photoresistance-sensor.html)
- [雨滴传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/rain-sensor.html)
- [S12SD紫外线传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/s12sd-uv-sensor.html)
- [SGP30气体传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/sgp30-gas-sensor.html)
- [SHT20温湿度传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/sht20-temp-humi-sensor.html)
- [SHT30温湿度传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/sht30-temp-humi-sensor.html)
- [土壤湿度传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/soil-moisture-sensor.html)
- [SR04超声波测距传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/sr04-ultrasonic-ranging-sensor.html)
- [TCS34725颜色识别传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/tcs34725-color-recognition-sensor.html)
- [TTP224触摸传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/ttp224-touch-sensor.html)
- [US-016超声波测距传感器](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/sensor/us-016-ultrasonic-ranging-sensor.html)

### PID 训练营（18 页）

- [项目列表](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/training/)
- [电赛TI:简易PID入门项目](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/training/easy-pid-beginner-kit/)
- [购买器件](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/training/easy-pid-beginner-kit/buy-materials.html)
- [编码器驱动](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/training/easy-pid-beginner-kit/encoder-drives.html)
- [事件与状态机](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/training/easy-pid-beginner-kit/event-state.html)
- [免费PCB打样](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/training/easy-pid-beginner-kit/free-pcb.html)
- [完整功能案例](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/training/easy-pid-beginner-kit/full-functionality.html)
- [电路原理分析](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/training/easy-pid-beginner-kit/hardware.html)
- [开发环境搭建](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/training/easy-pid-beginner-kit/install.html)
- [按键驱动](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/training/easy-pid-beginner-kit/key-press-drives.html)
- [电机驱动](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/training/easy-pid-beginner-kit/motor-drives.html)
- [PCB焊接练习](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/training/easy-pid-beginner-kit/pcb-solder.html)
- [PID 定距功能](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/training/easy-pid-beginner-kit/pid-distance.html)
- [PID 定速功能](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/training/easy-pid-beginner-kit/pid-speed.html)
- [原理图与PCB设计](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/training/easy-pid-beginner-kit/sch-pcb-design.html)
- [彩屏驱动](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/training/easy-pid-beginner-kit/screen-drives.html)
- [建立工程框架](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/training/easy-pid-beginner-kit/template-project.html)
- [UI 与界面管理](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/training/easy-pid-beginner-kit/ui-display.html)

## 10. 参考入口

- [天猛星 MSPM0G3507 开发板介绍](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/)
- [CCS-Theia 入门手册](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/ccs-beginner/)
- [Keil 入门手册](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/keil-beginner/)
- [模块移植手册](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/module/)
- [TI 简易 PID 入门项目](https://wiki.lckfb.com/zh-hans/tmx-mspm0g3507/training/easy-pid-beginner-kit/)
