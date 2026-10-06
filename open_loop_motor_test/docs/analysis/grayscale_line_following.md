# 灰度循迹说明（不适用于当前测试）

当前开环测试不会初始化或读取 NCHD12 灰度传感器。PA28/PA31 仍出现在复制的 SysConfig 和 BSP 文件中，但不是当前测试链路的一部分。

需要验证灰度循迹时，请阅读 `../car/docs/analysis/grayscale_line_following.md`，并以 `car/` 工程源码为准。
