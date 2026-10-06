# `open_loop_motor_test` 项目协作说明

本工程是独立的 MSPM0G3507 CCS 电机开环测试，入口为 `car.c`，SysConfig 源文件为 `car.syscfg`，目标配置为 `targetConfigs/MSPM0G3507.ccxml`。

当前 `app/app.c` 只初始化电机、蜂鸣器和 UART1；B21、`RUN15`、`RUNPID` 启动 250/1000 固定 PWM，运行 5 秒后停止。编码器、灰度、MPU6050、JY61S 和速度 PID 文件虽然随工程保留，但当前任务不调用。

PWM 接线为 PA0/PA1/PA8/PA9→AT8236 AIN1/AIN2/BIN1/BIN2；UART1 为 PB4/PB5，115200-8-N-1。不要将 `car/` 的 30 秒灰度循迹文档或陀螺仪路线描述套用到本工程。

修改 `.syscfg` 后先运行共享 SysConfig 检查，再构建并更新本目录 `car.hex`。编译、链接、烧录和实车结果分别记录。
