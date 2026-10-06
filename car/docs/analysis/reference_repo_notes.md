# 参考仓库分析

参考仓库：[XingShuyu/Car](https://github.com/XingShuyu/Car)。本项目只借鉴了模块分层、编码器计数和速度控制的组织方式，没有复制其 STM32G474 的引脚、HAL API、定时器或电机接线。

当前项目的事实来源仍是 `car.syscfg`、`car.c` 以及 `bsp/`、`mission/`、`control/` 中的源码。任何参考仓库参数都必须经过 MSPM0G3507 引脚和板级接线核对后才能使用。
