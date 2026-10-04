# XingShuyu/Car 参考仓库分析

参考仓库：[XingShuyu/Car](https://github.com/XingShuyu/Car)。本文件记录本工程借鉴的控制结构；仓库中的 STM32G474 引脚、HAL API、定时器编号和电机接线没有直接复制到 MSPM0G3507。

## 借鉴点

- `Motor/` 把电机底层驱动、编码器测量和速度控制分开；本工程对应 `bsp/encoder.c`、`bsp/motor_pwm.c` 和 `control/wheel_speed_controller.c`。
- 参考仓库把左右轮速度作为两个独立目标，用编码器计数在固定采样窗口换算速度，再分别做增量 PID。当前工程保留这一控制思想，采样周期改为 `CONTROL_TICK=5 ms`，单位改为 mm/s。
- 参考仓库的应用层以状态/任务方式组合直行、转向和传感器更新；当前工程由 `mission/demo_mission.c` 管理四段直行和三次右转，`app/app.c` 负责调度和事件输入。
- 当前工程新增 MPU6050 Z 轴角速度积分。直行阶段以进入该段时的 yaw 做航向保持，转弯阶段用目标角度误差和角速度做有界角度 PD/PID，再交给左右轮速度 PID 执行。

## 没有直接复制的内容

- 参考仓库的 MCU、定时器实例、GPIO、SysTick/HAL 和工程文件与本项目不同。
- 参考仓库的编码器线数、减速比和脉冲边沿定义不能替代 MG513X Hall 13 PPR、1:28、当前“一路 A 上升沿 + B 判向”的实物核对。
- 参考仓库的 PWM 极性、左右电机安装方向和制动/滑行策略不能替代 AT8236 手册与当前接线。当前 SysConfig 的真实映射仍是 PA0/PA1/PA8/PA9→AIN1/AIN2/BIN1/BIN2。

## 当前实现的限制

编码器速度换算使用 `13 × 28 = 364` 个 A 上升沿/轮，先作为待校准估计。MPU6050 在复位时静止校准 Z 轴偏置；转弯精度还受安装水平、陀螺零偏、轮胎打滑、PWM/速度 PID 参数和电机方向极性的影响。必须先在架空状态确认计数方向和右转 yaw 符号，再在低速空旷地面测试。

## 对应源码

- 电机 PWM：[`bsp/motor_pwm.c`](../../bsp/motor_pwm.c)
- 编码器：[`bsp/encoder.c`](../../bsp/encoder.c)
- MPU6050：[`bsp/mpu6050.c`](../../bsp/mpu6050.c)
- 双轮速度控制：[`control/wheel_speed_controller.c`](../../control/wheel_speed_controller.c)
- 路线状态机：[`mission/demo_mission.c`](../../mission/demo_mission.c)
