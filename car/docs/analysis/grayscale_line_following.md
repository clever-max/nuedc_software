# 12 路灰度循迹控制说明

## 接线和读取

NCHD12：SCL→PA28，SDA→PA31，VCC 使用 3.3 V 档，GND 共地。驱动按 PCA9555 兼容协议读取 7 位地址 `0x20`，低 12 位作为灰度通道状态。

## 控制链

每 5 ms：读取灰度位图，映射通道 0…11 为位置误差，使用 `Kp=4.0`、`Kd=0.02` 计算修正（限幅 ±60 mm/s），生成左右轮目标 `200 + correction` 与 `200 - correction`，再由编码器速度 PID 输出 PWM。无有效灰度输入时暂时取消位置修正，保留定速环。

当前任务持续 30 秒，由 `mission/demo_mission.c` 的 `LINE_ONLY_DURATION_MS` 控制。

## 代码入口

- 读取传感器：`../../bsp/gray_sensor.c`
- 线路误差和目标速度：`../../mission/demo_mission.c`
- 调度和遥测：`../../app/app.c`
