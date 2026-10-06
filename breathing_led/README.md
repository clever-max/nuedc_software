# `breathing_led` 呼吸灯工程

独立的 MSPM0G3507 CCS Theia 示例，使用板载 PB22 LED 做周期性渐变。

- 入口：`breathing_led.c`
- 配置源：`breathing_led.syscfg`
- LED：PB22，高电平点亮
- PWM：TIMG8 的 C1 输出到 PB22
- 目标配置：`targetConfigs/MSPM0G3507.ccxml`
- 可烧录镜像：`breathing_led.hex`

占空比由源码中的渐变循环改变，PWM 周期和分频来自 SysConfig。下载后是否达到预期呼吸效果，需要在实际天猛星板上确认。
