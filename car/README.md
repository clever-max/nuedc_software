# car 小车闭环验证项目

## 当前结论（以源码和 SysConfig 为准）

当前固件入口是 `car.c`，配置源是 `car.syscfg`。已实现并由启动信息确认的运行模式是：

- 上电后所有电机输入保持低电平，电机处于滑行停止状态。
- 按 B21，或在板载 Type-C/UART0 串口发送 `RUNPID`/`RUN15`，启动 30 秒灰度循迹。
- 每 5 ms 读取编码器和 12 路灰度传感器，灰度位置环修正左右轮目标速度，编码器速度环输出 AT8236 PWM。
- 30 秒到时进入 `DONE` 并滑行停止；运行中再次按 B21 或发送 `STOP` 进入 `ABORT`。
- 当前版本不初始化、不读取 MPU6050；遥测中的 `gyro=OFF` 是预期状态。`MPU6050` 驱动文件保留作后续实验，不能据此宣称已完成陀螺仪路线。

## 实际接线

| MSPM0G3507 引脚/外设 | 信号 |
| --- | --- |
| PA0 / TIMG8_C1 | AT8236 AIN1 |
| PA1 / TIMG8_C0 | AT8236 AIN2 |
| PA8 / TIMA0_C0 | AT8236 BIN1 |
| PA9 / TIMA0_C1 | AT8236 BIN2 |
| PA27（上升沿中断） | 左编码器 E1A |
| PA25 | 左编码器 E1B |
| PB25（上升沿中断） | 右编码器 E2A |
| PB20 | 右编码器 E2B |
| PA29 | NCHD12 SCL（软件 I²C） |
| PA30 | NCHD12 SDA（软件 I²C） |
| PB21 | B21 启动/停止按键，低电平有效 |
| PB27 | 无源蜂鸣器 |

左轮是 Motor A：AIN1/AIN2=PA0/PA1，编码器 E1A/E1B=PA27/PA25；右轮是 Motor B：BIN1/BIN2=PA8/PA9，编码器 E2A/E2B=PB25/PB20。NCHD12 使用 5V 供电，并将 V0 与 3V3 短接选择 3.3V 输出。先用开环固件确认两轮正方向。
| PA10 / UART0_TX | 板载 CH340E RXD |
| PA11 / UART0_RX | 板载 CH340E TXD |
| PB2 / I2C1_SCL | MPU6050 SCL（当前未使用） |
| PB3 / I2C1_SDA | MPU6050 SDA（当前未使用） |
| PB1 | MPU6050 INT（当前未使用） |

电机逻辑输入必须直接接上表四个 PWM 引脚并共地。不要把旧教程中的 H8 引脚或 Arduino/STM32 引脚表套用到本项目。

## 串口操作

板载 UART0 参数为 115200-8-N-1，命令以换行结束：

| 命令 | 当前行为 |
| --- | --- |
| `RUNPID`、`RUN15` | 启动 30 秒灰度循迹；任务运行中返回 `ERR busy` |
| `STOP` | 请求停止并进入 `ABORT` |
| 其他 | 返回命令提示 |

遥测约每 100 ms 一行，字段包括 `state`、`gyro`、`line`、`gray`、`speed` 和 `pwm_permille`。当前正常启动横幅包含 `MPU6050 disabled` 与 `gray line 30s`。

## 工程结构

- `app/`：初始化、5 ms 调度、按键和串口命令分发。
- `mission/`：30 秒灰度循迹状态机及目标速度生成。
- `control/`：左右轮独立增量式速度控制器。
- `bsp/`：电机 PWM、编码器、灰度传感器、蜂鸣器以及保留的 MPU6050/JY61S 驱动。
- `protocol/`：UART0 命令解析和遥测输出。
- `targetConfigs/`：MSPM0G3507 的 CCS 目标配置。
- `docs/`：项目事实、接线、调试和供应商资料索引。

## 当前参数和边界

- 轮径 65 mm，电机为 MG513X 霍尔编码器，用户提供 13 PPR、1:28 减速比。
- 固件按 A 相上升沿计数、B 相判向，初始按 364 count/轮估算；必须通过实测一圈校准后再把速度当作标定值。
- 灰度位置环初值为 `Kp=4.0`、`Kd=0.02`，修正限幅 ±60 mm/s；基础目标速度为 200 mm/s。
- 实车架空验证表明 Motor B 需要电气反相，才能使正命令代表两轮物理前进；左编码器 B 相判向取反，右编码器保持原判向。
- PID 参数、编码器电平兼容性和电机方向尚未完成实车标定。

## 构建与验证记录

本目录中的 `car.hex` 是可直接烧录的 Intel HEX 固件镜像。源码检查、SysConfig 生成、编译、链接、烧录工具结果和实车串口观察必须分开记录；没有连接并观察小车时，不把构建成功写成硬件验证成功。

详细说明见 [文档索引](docs/README.md)。

设置 `CCS_INSTALL_DIR` 后，使用 `powershell -ExecutionPolicy Bypass -File car/tools/build_validate_hex.ps1 -Clean` 重新生成 `car.hex`。脚本生成后强制执行 Intel HEX 校验和与 MSPM0 BSL 8 字节地址/长度对齐检查，校验失败时不报告构建完成。
