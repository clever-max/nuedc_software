# 开环测试串口

UART1 参数为 115200-8-N-1，PB4 为 MCU TX、PB5 为 MCU RX。发送 `RUN15` 或 `RUNPID` 启动固定 250/1000 PWM，发送 `STOP` 停止。启动、停止和自动超时信息由 `protocol/serial_console.c` 输出；不会输出编码器速度或灰度遥测。
