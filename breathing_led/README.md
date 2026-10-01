# Tianmengxing MSPM0G3507 breathing LED

独立 CCS Theia 项目，使用板载用户 LED 做周期性呼吸效果。

- 芯片：MSPM0G3507，SDK 2.11.0.07，TI Arm Clang，XDS110。
- 板载 LED：PB22，高电平点亮；PWM 使用 TIMG8-C1。
- PWM 占空比在约 0% 至 100% 间渐变，PWM 周期和频率由 `breathing_led.syscfg` 中的 PWM 配置决定。
- 入口：`breathing_led.c`；配置源：`breathing_led.syscfg`。

在 CCS Theia 导入该目录后构建并下载。硬件表现需在实际天猛星板上确认。

## Flash image

The completed build is provided as reathing_led.hex in TI/Intel HEX format. The matching CCS output is Debug/breathing_led.out. Rebuild and regenerate the HEX after any source or SysConfig changes.
