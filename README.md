# MSPM0 CCS Workspace

这是天猛星 MSPM0G3507 的 CCS Theia 工作区。每个工程独立放在根目录下，公共开发规范和脚本集中在 `skills/`，板卡与 Wiki 资料集中在 `docs/`。

## 目录

| 目录 | 用途 |
|---|---|
| `empty/` | MSPM0G3507 GPIO/按键起始工程 |
| `photoresistor_uart/` | LM393 光敏传感器 ADC、UART 和 Python 监视器 |
| `skills/mspm0-ccs/` | 共享 MSPM0 CCS skill、验证脚本、参考和示例 |
| `docs/` | 天猛星 Wiki 摘要、板卡参考和学习资料 |

## 开发入口

从本目录启动 Codex/CCS 相关工作时，先读取根目录的 `AGENTS.md`，再读取目标项目自己的 `AGENTS.md` 和 `skills/mspm0-ccs/SKILL.md`。

项目的 SysConfig 文件是配置源。修改后使用共享脚本检查：

```powershell
python skills/mspm0-ccs/scripts/check_syscfg.py .\photoresistor_uart
python skills/mspm0-ccs/scripts/run_sysconfig.py .\photoresistor_uart --compiler ticlang
```

构建、下载和物理验证必须分别报告；没有连接板卡时不要声称硬件验证成功。
