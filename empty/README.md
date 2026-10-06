# `empty` MSPM0G3507 最小工程

## 当前功能

这是一个独立的 CCS Theia 工程，用于验证 MSPM0G3507、SysConfig、板载按键和 LED。上电后程序初始化 SysConfig 外设，并持续读取低电平有效的 B21（PB21）：按下时点亮板载 LED（PB22），松开时熄灭。

当前源码不初始化电机、编码器、串口、ADC 或灰度传感器。

## 配置与入口

- 设备：MSPM0G3507，LQFP-64(PM)
- 入口：`empty.c`
- 配置源：`empty.syscfg`
- 按键：PB21，上拉输入，低电平有效
- LED：PB22，高电平点亮
- 目标配置：`targetConfigs/MSPM0G3507.ccxml`
- 镜像：`empty.hex`

修改 `.syscfg` 后运行共享检查脚本，再构建并重新生成镜像。物理按键和 LED 行为需要连接板卡后确认。
