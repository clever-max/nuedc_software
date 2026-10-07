# 独立编译和生成 HEX

这个流程不需要 Codex/Agent。它直接调用 CCS 的 `gmake`、TI Arm Clang 的 `tiarmhex`，最后运行仓库自带的 Intel HEX 校验器。

## 前置条件

- 已安装 CCS、MSPM0 SDK、SysConfig 和 TI Arm Clang。
- Python 可执行 `skills/mspm0-ccs/scripts/validate_hex.py`。
- 当前工程目录是 `car/`，不要在 `Debug/` 目录中运行脚本。

## 首次设置 CCS 路径

在当前 PowerShell 会话中设置 CCS 安装目录。路径由你的机器决定，不要把它写进项目文件：

```powershell
$env:CCS_INSTALL_DIR = '<CCS install directory>'
```

## 编译、生成和校验

推荐使用干净构建：

```powershell
powershell -ExecutionPolicy Bypass `
  -File .\car\tools\build_validate_hex.ps1 `
  -Clean
```

脚本会依次执行：

1. `gmake clean all`：重新运行 SysConfig 并编译、链接 `car.out`。
2. `tiarmhex --intel --memwidth=8 --romwidth=8`：生成 `car/car.hex`。
3. `validate_hex.py`：检查 Intel HEX 字符、记录长度、校验和、EOF、数据记录以及 MSPM0 BSL 的 8 字节地址/长度对齐。
4. `Get-FileHash`：输出 SHA256，便于记录和发布核对。

只有四步都成功，才把 HEX 交给烧录工具。

## 同步固件副本

```powershell
powershell -ExecutionPolicy Bypass `
  -File .\car\tools\build_validate_hex.ps1 `
  -Clean `
  -FirmwareAlias .\car\firmware\gray_line_30s_no_gyro.hex
```

脚本会在复制后再次校验副本。

## 可选参数

```text
-CcsInstallDir <目录>  覆盖 CCS_INSTALL_DIR
-ProjectDir <目录>     默认是脚本上一级 car 目录
-OutputHex <路径>      改变输出 HEX 路径
-FirmwareAlias <路径>  复制并再次校验一个固件副本
-Clean                 执行 clean all；不加时只执行 all
```

## 失败处理

- 找不到 CCS：设置 `CCS_INSTALL_DIR`。
- SysConfig 失败：检查 `car/car.syscfg` 和工具版本，不要手工编辑 `Debug/` 生成文件。
- 编译/链接失败：修复源码后重新运行 `-Clean`。
- HEX 校验失败：不要烧录，检查地址/长度对齐和构建输出。

## 完成后的 Git 交接

```powershell
python .\tools\project_context.py update car `
  --validation "本地编译、链接、HEX 校验通过；SHA256=<记录脚本输出>" `
  --note "使用 car/tools/build_validate_hex.ps1 生成并校验 HEX"
python .\tools\project_context.py guard car
git add car
git commit -m "build(car): refresh validated firmware"
git push origin main
```

没有 commit 的代码或固件改动不能报告完成。
