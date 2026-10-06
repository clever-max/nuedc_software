# `button_led_test` 按键/LED 诊断工程

这是独立于 `empty` 的诊断工程。当前源码 `empty.c` 的实际行为是：初始化 SysConfig 后把 PB22 板载 LED 置为高电平，并保持常亮死循环；它没有读取 PB21，也没有按键消抖或按键控制逻辑。

- LED：PB22，高电平点亮。
- 按键：PB21 已在 SysConfig 中配置，但当前源码未读取。
- 镜像：`empty.hex`。

因此本文档不应描述为“按住 B21 点亮、松开熄灭”。如需按键控制，请以 `empty/empty.c` 的实现为参考，并重新构建验证。
