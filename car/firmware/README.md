# 固件测试镜像

这些 HEX 均使用 TI ARM Hex 工具的 8 位字节寻址生成，并检查了 Intel HEX 校验和及 MSPM0 BSL 的 8 字节编程对齐要求。

| 文件 | 功能 |
| --- | --- |
| `button_led_always_on.hex` | PB22 板载 LED 常亮死循环 |
| `motor_open_loop_5s.hex` | 固定 PWM 250/1000，双电机运行 5 秒 |
| `gray_line_30s_no_gyro.hex` | 灰度循迹 30 秒，不初始化 MPU6050 |
| `gyro_curve_route.hex` | MPU6050 初始化、灰度循迹和弯道状态机 |

烧录后先退出 BSL，再启动应用或复位。电机测试应架空车轮，并断开不相关传感器。

