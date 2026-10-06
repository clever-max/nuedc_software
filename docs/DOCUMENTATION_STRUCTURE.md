# 文档与数据手册分层

本工作区采用 Diátaxis 的四类文档目的：教程、操作指南、参考和解释。Diátaxis 将这四类内容分开，避免把“怎么学习”“怎么完成任务”“精确参数”和“为什么这样设计”混在一个 README 中。

## 工作区文档层

```text
docs/
├─ tutorials/       学习路径和从零开始的教程
├─ how-to/          可执行的排障、烧录、调试和复查步骤
├─ reference/       供应商资料、数据手册、引脚表和示例快照
├─ explanations/    架构、算法和设计取舍
├─ decisions/       ADR：一次决策、证据、取舍和结果
└─ logs/            串口、实车、烧录和实验记录
```

当前已有的 `docs/DEVELOPMENT_PLAN.md`、`docs/ALGORITHM_AND_LEARNING_GUIDE.md` 和各项目 `docs/` 保持不动；新文档按上述用途归类。旧文件移动前必须更新索引和相对链接。

## 参考资料层

```text
docs/reference/
├─ 00_inbox/        新收到但尚未核对、命名或去重的资料
├─ LP-MSPM0G3507/   LaunchPad 原始资料、抽取文本和示例
├─ Tianmengxing/    天猛星原始资料、抽取文本和示例
├─ sensors/         传感器/模块资料
│  ├─ <device>/     原始手册和器件资料
│  ├─ text/         从 PDF/网页提取的可搜索文本
│  └─ examples/     可运行示例快照，保留板卡/工具版本上下文
├─ manifests/       文件 SHA256、来源、版本和核对状态
├─ 99_quarantine/   待删除或待归档的重复文件，不作为当前引用入口
└─ README.md        参考资料总索引和优先级
```

现有 `NCHD1/`、`NCHD1_I2C/` 和 `sensors/examples/nchd12_mspm0g3507/` 是历史资料快照。清理前先看 [DOCUMENT_DUPLICATES.md](reference/DOCUMENT_DUPLICATES.md)，不要直接删除 SDK 示例中的重复启动文件或生产 ODB 文件。

## 数据手册入库规则

1. 新资料先放 `docs/reference/00_inbox/`，保留原始文件名和来源说明。
2. 核对器件、板卡、版本和适用电压后，移动到对应设备目录的 `datasheets/` 或 `manuals/`。
3. PDF/网页抽取文本放 `text/`，文件名与源文件保持可追溯关系。
4. 示例工程放 `examples/`，必须标明目标板、SDK/SysConfig 版本和示例引脚。
5. 每个正式资料加入 manifest：来源、下载日期、SHA256、适用范围、是否已核对。
6. 内容完全相同的文件只保留一个 canonical 文件；旧路径若仍有引用，先保留别名记录，再移动到 `99_quarantine/`。
7. 供应商原始资料只读保存；项目实际接线和实现写在项目目录，不直接改供应商快照。

## 文件命名

- 原始文件：`<vendor>_<device>_<document>_<version>.<ext>`
- 抽取文本：源文件名后追加 `.txt`，不覆盖原始文件。
- 项目说明：使用英文 ASCII 文件名；正文可以中文。
- 不用下载路径、机器绝对路径或临时聊天名称作为正式文件名。

## 去重与删除

运行：

```powershell
python tools/find_duplicate_docs.py
```

工具只生成报告，不删除文件。删除流程是：报告 → 人工确认 canonical → 更新索引/manifest → 移入 `99_quarantine/` → 单独提交 → 再扫描确认。
