# 天猛星本地编译 / 转换 / 烧录 GUI

启动：

```powershell
python tools\tmx_builder_gui.py
```

也可以双击 `tools\run_tmx_builder_gui.bat` 启动。

GUI 是 TI 官方命令行工具的本地封装：

1. **编译**：优先使用 `Debug/makefile` + `gmake`；没有生成 makefile 时调用 CCS 的 `eclipse\ccs-server-cli.bat` 无界面工程构建器（`projectBuild`）。
2. **转换 HEX**：调用 TI Arm Clang 的 `tiarmhex --intel`，把当前项目的 `Debug\<project>.out` 转成 `Debug\<project>.hex`。
3. **烧录并复位**：调用 `DSLite.exe -c targetConfigs\MSPM0G3507.ccxml -e -r 2 -u Debug\empty.out`，使用工程配置的 XDS110。

## 使用边界

- GUI 不改写 `.syscfg`、`ti_msp_dl_config.*`、链接脚本或其他生成文件。
- GUI 不执行 Mass Erase、DSSM 解锁或 Factory Reset。
- 烧录动作需要连接天猛星开发板和匹配的 XDS110；未连接实体板时只能验证 GUI/工具调用，不能声称硬件成功。
- 如果工程使用 CCS 生成的 `Debug/makefile`，GUI 会优先使用它；否则使用 CCS Theia headless build。若本机 CCS 版本不支持该无界面构建参数，请在 GUI 输出中查看命令和错误。

## 工具发现

脚本不会写入某台电脑的绝对安装路径。优先设置环境变量 `CCS_ROOT`（或 `TI_CCS_ROOT`）指向 CCS 安装目录；如果未设置，则尝试从 PATH 查找 `ccs-server-cli`。工程目录始终由脚本位置相对确定。
