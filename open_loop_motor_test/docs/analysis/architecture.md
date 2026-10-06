# 架构说明（开环测试）

当前入口为 `car.c`，应用层只负责初始化、按键/UART 命令、5 ms 计时和 5 秒停止。电机输出由 `bsp/motor_pwm.c` 提供，蜂鸣器由 `bsp/buzzer.c` 提供，UART1 由 `protocol/serial_console.c` 提供。

编码器、灰度、MPU6050、JY61S 和速度控制器文件随工程保留，但 `open_loop_motor_test/app/app.c` 不读取这些数据。
