# 工作区文档总览

这是天猛星 MSPM0G3507 CCS Theia 工作区。工程目录彼此独立，项目实际配置以各自的 `.syscfg`、用户源码和目标配置为准。

## 工程目录

| 目录 | 当前用途 |
| --- | --- |
| `empty/` | B21 按键控制板载 PB22 LED 的最小示例 |
| `button_led_test/` | LED 常亮诊断固件，源码与 `empty/` 类似但作为独立测试工程保留 |
| `breathing_led/` | 使用 PB22 和 TIMG8-C1 的呼吸灯示例 |
| `photoresistor_uart/` | PA27 ADC + PA26 LM393 数字输入 + UART0/CH340 + Python 监视器 |
| `car/` | AT8236、MG513X 霍尔编码器和 NCHD12 灰度传感器的 30 秒灰度循迹工程 |
| `open_loop_motor_test/` | 从 `car` 配置分离出的双电机 5 秒固定 PWM 测试 |
| `skills/mspm0-ccs/` | 共享技能、检查脚本和示例 |
| `docs/` | 工作区规划、算法学习资料和板卡/传感器参考 |

## 文档与事实的优先级

1. 当前项目的用户源码和 `.syscfg`。
2. 项目自己的 `README.md`、`AGENTS.md` 和测试记录。
3. 工作区 `docs/` 中的解释性文档。
4. `docs/reference/`、项目 `vendor/` 中的供应商资料和示例；这些是参考快照，不代表当前工程配置。

参考资料中出现的其他板卡、STM32/Arduino 引脚和旧版本参数，不能直接套用到 MSPM0G3507 工程。

## 常用验证

```powershell
python skills/mspm0-ccs/scripts/check_syscfg.py .\car
python skills/mspm0-ccs/scripts/check_syscfg.py .\photoresistor_uart
python skills/mspm0-ccs/scripts/run_sysconfig.py .\photoresistor_uart --compiler ticlang
```

修改 `.syscfg` 后先生成并检查，再进行编译。构建、烧录和实物验证必须分别记录；没有连接并观察板卡时，不把构建成功写成硬件验证成功。

## Codex 对话接续

每个独立工程都维护 `PROJECT_STATE.md`，新对话先运行：

```powershell
python tools/project_context.py show <project>
```

完整流程见 [docs/CONTINUATION_WORKFLOW.md](docs/CONTINUATION_WORKFLOW.md)。
