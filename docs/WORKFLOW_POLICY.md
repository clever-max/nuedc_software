# 强约束工程工作流

本仓库采用一个适合嵌入式项目的组合方案：GitHub Flow 的短分支和 Pull Request、GitHub Ruleset 的主分支门禁、Conventional Commits、pre-commit、GitHub Actions，以及 Diátaxis 文档分层。

## 为什么采用这套组合

- **Diátaxis** 把文档分成教程、操作指南、参考和解释，减少把实验记录、数据手册和设计结论混在一起。
- **Conventional Commits** 用 `feat:`、`fix:`、`docs:`、`build:`、`ci:` 等前缀表达提交意图，方便审阅和生成发布说明。
- **pre-commit** 在提交前运行格式、冲突、工作流和提交信息检查。
- **GitHub Actions** 在 Pull Request 和 `main` 推送上重复执行仓库检查。
- **GitHub Ruleset** 应将 `main` 设为受保护分支，要求 Pull Request 和 `repository-checks` 成功后才能合并。
- **Keep a Changelog** 用人工筛选的 `Unreleased` 和版本段落记录用户可见变化，不把完整 Git 日志直接当成变更日志。

## 本地已落地

- 根目录 `.pre-commit-config.yaml`。
- `tools/check_workflow.py`：检查六个项目的状态文件、AGENTS 接续规则、参考资料层和绝对路径。
- `tools/check_commit_message.py`：检查 Conventional Commit 格式。
- `tools/project_context.py guard <project>`：代码、SysConfig、工程元数据、脚本或固件改动未提交时失败。
- `tools/find_duplicate_docs.py`：按 SHA256 生成重复资料报告，不自动删除。
- `.github/workflows/repository-checks.yml`：在 Pull Request 和 `main` 推送上执行仓库级门禁。

## MSPM0 项目的额外门禁

仓库级 CI 只检查通用文档和工具。涉及 MSPM0 源码或 `.syscfg` 时，开发者仍必须本地运行：

```powershell
python skills/mspm0-ccs/scripts/check_syscfg.py <project>
python skills/mspm0-ccs/scripts/run_sysconfig.py <project> --compiler ticlang
```

随后编译、链接、生成 Intel HEX、运行项目自己的 HEX 校验脚本，并把各结果写入 `PROJECT_STATE.md`。物理板卡和串口结果单独记录。

## 推荐的 GitHub Ruleset 设置

在仓库 Settings → Rules → Rulesets 为 `main` 建立规则：

1. Require a pull request before merging。
2. Require at least one approval（单人开发可先设为 0，但保留 Pull Request 门槛）。
3. Require status check `repository` / `repository-checks`。
4. Require branches to be up to date before merging。
5. Block force pushes and tag deletion。
6. 保留管理员绕过权限，仅用于恢复操作，并在 `PROJECT_STATE.md` 记录原因。

当前本地流程已落地；GitHub Ruleset 是仓库管理员设置，需在 GitHub 网页中启用后才会真正阻止直接推送。

## 参考方案

- [Diátaxis](https://www.diataxis.fr/)
- [Conventional Commits 1.0.0](https://www.conventionalcommits.org/en/v1.0.0/)
- [pre-commit](https://pre-commit.com/)
- [GitHub Rulesets](https://docs.github.com/en/repositories/configuring-branches-and-merges-in-your-repository/managing-rulesets/about-rulesets)
- [GitHub Actions workflows](https://docs.github.com/en/actions/concepts/workflows-and-actions/workflows)
- [Keep a Changelog](https://keepachangelog.com/en/1.1.0/)
