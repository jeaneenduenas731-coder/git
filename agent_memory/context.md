# 项目上下文

## 项目目标

- 当前目录用于初始化为 Git 仓库并推送到 GitHub。
- 目录现有内容仅包含 `agent_memory/` 项目记忆文件。

## 技术与运行方式

- 当前未识别到应用代码、包管理器、构建命令或测试命令。
- Git 可用；GitHub CLI `gh` 当前不可用或不在 PATH。

## 代码结构

- `agent_memory/context.md`：项目级上下文。
- `agent_memory/progress.md`：当前任务进度。
- `agent_memory/bugs.md`：问题与风险记录。
- `agent_memory/archive/`：归档目录。

## 约定与边界

- 默认中文沟通。
- 最小改动；不引入无关重构或无关文件。
- 推送 GitHub 前需要明确远程仓库 URL、认证方式和提交作者信息。

## 当前有效决策

- 本轮先完成本地 Git 初始化和首个提交准备；远程推送等待 GitHub 仓库 URL 或可用认证方式。
