# 传感器参考索引

这里保存 NCHD1/NCHD12 灰度传感器、PCA9555 和 MPU6050 的供应商资料与示例快照。参考资料不等于当前工程配置；当前工程必须以各自的 `.syscfg` 和源码为准。

## 资料入口

| 主题 | 位置 |
| --- | --- |
| NCHD1/NCHD12 手册和文本 | `NCHD1/`、`text/` |
| NCHD12 I²C 例程 | `examples/nchd12_mspm0g3507/` |
| PCA9555 数据手册 | `NCHD1/数据手册/` 和对应 `text/` |
| MPU6050 原理图 | `MPU6050/` 和对应 `text/` |

## 已核对的参考事实

- PCA9555 资料常用 8 位写/读地址 `0x40/0x41`；按 7 位地址 API 使用时对应 `0x20`。
- 参考 NCHD12 例程使用 PA0/PA1 软件 I²C；当前 `car` 工程的 PA0/PA1 已分配给电机 PWM，因此 `car` 改用 PA29/PA30 软件 I²C。实际接线和源码优先于旧例程。
- NCHD12 输出电平必须按模块资料设置为适合 MSPM0 的 3.3 V，不能把 5 V 输出直接接入 MCU GPIO。
- 当前 `car` 工程预留 MPU6050 PB2/PB3/PB1，但当前运行版本没有初始化或读取 MPU6050。

## 工程链接

- [car 灰度说明](../../../car/docs/analysis/grayscale_line_following.md)
- [car 接线说明](../../../car/docs/analysis/wiring.md)
- [car MPU6050 说明](../../../car/docs/analysis/MPU6050_notes.md)

`examples/` 和 `text/` 中的内容保留原始资料上下文；其中的板卡、引脚、工具版本和安装路径不能直接当作当前工程事实。
