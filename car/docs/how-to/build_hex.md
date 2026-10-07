# 独立编译和生成 HEX

在工作区根目录运行 PowerShell：

```powershell
$env:CCS_INSTALL_DIR = '<CCS install directory>'
powershell -ExecutionPolicy Bypass `
  -File .\car\tools\build_validate_hex.ps1 `
  -Clean `
  -FirmwareAlias .\car\firmware\gray_line_30s_no_gyro.hex
```

脚本依次执行 SysConfig、CCS 编译/链接、Intel HEX 生成、HEX 校验和 BSL 8 字节地址/长度对齐检查，并输出 SHA256。只有全部通过才使用生成的镜像。

常用参数：

```text
-CcsInstallDir <目录>   覆盖 CCS_INSTALL_DIR
-ProjectDir <目录>      默认是脚本所在 car 目录
-OutputHex <路径>       指定 HEX 输出路径
-FirmwareAlias <路径>   复制并再次校验一个固件副本
-Clean                  执行 clean all
```

完成后必须更新 `PROJECT_STATE.md`、运行 `python tools/project_context.py guard car`，并提交代码和 HEX。
