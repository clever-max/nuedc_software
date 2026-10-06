# Codex 项目接续工作流

工作区内每个独立工程都保留一个可提交的 `PROJECT_STATE.md`。它记录人类决策、当前目标、已验证事实、已知问题和下一步；源码、`.syscfg`、Git 历史和构建产物仍是实际状态的依据。新对话不能只依赖聊天历史。

## 新建同项目 Codex 对话时

在项目目录对应的工作区根目录执行：

```powershell
python tools/project_context.py show car
Get-Content car/AGENTS.md
Get-Content car/PROJECT_STATE.md
git status --short -- car
```

将 `car` 换成 `breathing_led`、`button_led_test`、`empty`、`open_loop_motor_test` 或 `photoresistor_uart`。随后按项目 `AGENTS.md` 的要求读取共享技能、板卡索引和项目 README。`show` 输出的 Git 分支、HEAD 和工作区状态优先于状态文件中的旧记录。

建议新对话第一句直接写：

> 继续 `<project>` 项目。先读取 `AGENTS.md`、`PROJECT_STATE.md` 和当前 Git 状态，保持现有目标和未解决问题，然后再行动。

## 开始工作前

1. 读根目录 `AGENTS.md` 和项目 `AGENTS.md`。
2. 运行 `python tools/project_context.py show <project>`。
3. 区分“已验证事实”“待验证推断”和“下一步计划”。
4. MSPM0 工程先读 `skills/mspm0-ccs/SKILL.md`，涉及天猛星时再读 `docs/tmx-mspm0g3507-wiki-summary.md` 和 `docs/reference/Tianmengxing/INDEX.md`。
5. 不把旧对话中的引脚、烧录结果或串口号直接当成当前事实。

## 完成一个工作阶段时

把可复查的信息写回状态文件：

```powershell
python tools/project_context.py update car `
  --status stable `
  --objective "保持无陀螺仪 30 秒灰度循迹稳定运行" `
  --implementation "PA29=SCL、PA30=SDA；UART0 PA10/PA11；电机与编码器映射见 README" `
  --validation "源码检查通过；SysConfig 通过；编译链接通过；HEX 已校验；实车 NACK1 待排查" `
  --issues "NCHD12 尚未在地址扫描阶段应答" `
  --next "检查 SCL/SDA 排线、上拉和模块地址后重新采集 gray_bus" `
  --note "完成一次串口诊断并生成可烧录镜像"
```

然后检查并提交：

```powershell
python tools/project_context.py show car
git diff --check
git status --short
git add car/PROJECT_STATE.md <本次实际改动>
git commit -m "<清晰描述>"
git push origin <当前分支>
```

`PROJECT_STATE.md` 不应保存密码、令牌、完整串口日志或机器绝对路径；大日志放在项目 `docs/analysis/`，状态文件只保留结论和相对链接。

## 固件和稳定版发布

1. 先完成源码静态检查、SysConfig、编译、链接和 Intel HEX 校验。
2. 把每项结果分别写入 `PROJECT_STATE.md`，不要把构建成功写成实车成功。
3. 提交代码和状态文件，推送当前分支。
4. 稳定版本创建带说明的 Git 标签；标签必须指向已验证提交。
5. 推送标签并创建 GitHub Release，附上该提交生成的 HEX；在状态文件记录标签、Release 链接和 SHA256。
6. 新对话默认从当前分支继续；需要复现稳定版本时明确切换到状态文件记录的标签。

## 状态文件字段约定

- **Current objective**：本阶段用户目标。
- **Current implementation**：当前接线、接口、算法和入口。
- **Validation**：源码、SysConfig、编译、链接、烧录工具、串口/实车分别记录。
- **Known issues**：只写仍未解决的问题。
- **Next actions**：新对话可以直接执行的具体动作。
- **Handoff log**：每次阶段结束追加一行，说明结论、提交和下一步。

状态工具源码：[tools/project_context.py](../tools/project_context.py)。
