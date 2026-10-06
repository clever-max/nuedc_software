# `empty` 基线工程说明

## 工程角色

`empty/` 是工作区的最小 MSPM0G3507 CCS Theia 工程，用于验证 SysConfig、DriverLib、B21 按键和板载 PB22 LED。它不是当前 `car` 工程，也不包含电机、编码器或闭环控制。

## 工具链

- 器件：MSPM0G3507，LQFP-64(PM)
- 板卡：天猛星 MSPM0G3507
- 编译器：TI Arm Clang
- SDK：MSPM0 SDK 2.11.0.07
- 目标配置：`empty/targetConfigs/MSPM0G3507.ccxml`
- 配置源：`empty/empty.syscfg`

## 当前状态

- CCS 工程和 SysConfig 配置已存在。
- `empty.c` 已实现 B21 低电平有效读取和 PB22 LED 控制。
- `empty.hex` 已提供为 Intel HEX 镜像。
- 电机、编码器、UART 参数协议和闭环运动控制不属于本工程当前功能。

修改配置后先运行共享静态检查和 SysConfig 生成，再报告编译、链接、烧录和实物验证结果。
