# `car` 项目文档索引

本文档只记录当前工程能够由源码、`car.syscfg` 或项目内资料直接核对的事实。相对路径均以本文件所在的 `docs/` 目录为基准。

## 先读这些

1. [接线与配置对应表](analysis/wiring.md)
2. [当前运行架构](analysis/architecture.md)
3. [灰度循迹控制说明](analysis/grayscale_line_following.md)
4. [串口调试指南](analysis/serial_debug.md)
5. [MG513X 霍尔编码器说明](analysis/MG513X_Hall_notes.md)
6. [AT8236 资料核对](analysis/AT8236_reference_notes.md)

## 历史和保留驱动

- [MPU6050 接入说明](analysis/MPU6050_notes.md)：驱动和未来路线的记录；当前 `app/` 未调用。
- [JY61S 接入说明](analysis/JY61S_notes.md)：历史 UART2 方案；当前任务不启用。
- [旧开环动作教程](analysis/write_this_drive_sequence.md)：历史方案，接线和行为不适用于当前固件。
- [旧串口记录](analysis/serial_debug_20261001.md)：历史数据，使用旧编码器比例，不能用于当前标定。
- [参考仓库分析](analysis/reference_repo_notes.md)：说明可借鉴结构与板级差异。
- [供应商资料清单](analysis/vendor_inventory.md)：`vendor/` 中文件的用途和来源。

## 资料边界

`vendor/` 内的 PDF、压缩包和示例是供应商原始资料。它们面向其他 MCU 或其他板卡，不能替代本项目的 `car.syscfg`、源码和实际接线。工程事实优先级为：当前源码和 SysConfig > 本目录分析文档 > 供应商示例。
