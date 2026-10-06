# `car` 项目协作说明

本目录是 MSPM0G3507 CCS 工程。请同时遵守工作区根目录的 `../AGENTS.md` 和共享技能说明 `../skills/mspm0-ccs/SKILL.md`。

## 工程入口

- 源码入口：`car.c`
- SysConfig 源文件：`car.syscfg`
- 目标配置：`targetConfigs/MSPM0G3507.ccxml`
- 可烧录镜像：`car.hex`

## 当前硬件映射

- 电机 PWM：PA0/TIMG8_C1→AIN1、PA1/TIMG8_C0→AIN2、PA8/TIMA0_C0→BIN1、PA9/TIMA0_C1→BIN2。
- 编码器：E1A/E1B/E2A/E2B→PA27/PA25/PB25/PB20。
- 灰度总线：PA29=SCL、PA30=SDA（软件 I²C）。
- MPU6050 预留：PB2/PB3 I²C，PB1 INT；当前固件未调用。
- 蜂鸣器：PB27；B21：PB21。
- 板载 CH340E：PA10/UART0_TX、PA11/UART0_RX，115200-8-N-1。

## 当前软件行为

- 所有电机输入在启动时保持低电平。
- B21、`RUNPID`、`RUN15` 启动 30 秒灰度循迹；`STOP` 或再次按 B21 中止并滑行。
- `app/` 负责调度，`mission/` 负责状态机，`protocol/` 负责 UART，`bsp/` 负责硬件，`control/` 负责双轮速度控制。
- 当前任务不依赖 MPU6050 或 JY61S；遥测 `gyro=OFF` 是正常状态。

## 编码器和标定边界

安装电机为 MG513X 霍尔编码器，用户提供 13 PPR、1:28 减速比。当前代码按 A 相上升沿计数、B 相判向，初始使用 364 count/轮。使用前必须实测一圈计数并确认 A/B 输出电平满足 MSPM0 GPIO 容限。PID 初值只是调试起点，不能视为实车标定结果。

不要从 Arduino/STM32 示例复制 PWM 频率、方向、电机供电、刹车/滑行方式或引脚。不要手工修改 SysConfig 生成文件和构建输出。修改 `.syscfg` 后先做静态检查和 SysConfig 生成，再构建。

完成固件任务时，必须在本目录提供 Intel HEX 镜像，并分别报告源码检查、SysConfig、编译、链接、烧录工具和实车串口结果。

## 对话接续

新对话先读取 `PROJECT_STATE.md`，再运行 `python ../tools/project_context.py show car`。完成一个阶段后，用同一工具更新状态并提交状态文件；通用规则见 `../docs/CONTINUATION_WORKFLOW.md`。

## 代码提交硬约束

凡涉及源码、SysConfig、项目元数据、构建脚本、校验脚本或固件镜像的改动，交付前必须创建 Git commit；未提交的代码改动不能报告为完成。相关改动可以合并为一个提交。完成阶段时同步更新 `PROJECT_STATE.md`，并运行 `python ../tools/project_context.py guard car`。
