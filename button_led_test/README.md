# `button_led_test` 工程说明

这是一个独立的 MSPM0G3507 CCS Theia 诊断工程，入口为 `empty.c`，配置源为 `empty.syscfg`。

当前固件只做一件事：初始化后点亮 PB22 板载 LED 并保持常亮。PB21 虽配置为上拉输入，但当前源码没有读取它。工程不初始化电机、编码器、UART、ADC 或灰度传感器。

目标配置为 `targetConfigs/MSPM0G3507.ccxml`，可烧录镜像为 `empty.hex`。修改配置或源码后，必须重新生成 SysConfig、构建并更新镜像；实物结果需单独记录。
