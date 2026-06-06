# 项目上下文

## 项目目标

- 待补充：本项目要解决什么问题、主要用户是谁、当前最重要的成功标准是什么。

## 技术与运行方式

- 嵌入式蓝牙音频应用，主要代码为 C，工程包含 `app_src/`、`app_framework/`、`driver/`、`middleware/` 等目录。
- 当前环境未发现可用 `git`，`rg` 执行被系统拒绝，搜索使用 PowerShell 原生命令。

## 代码结构

- `app_src/hmi/key/adc_levels.c`：电位器 ADC 分档扫描，启用宏为 `CFG_ADC_LEVEL_KEY_EN`。
- `app_src/components/audio/audio_vol.c`：音量表、音量设置和 `AdcLevelMsgProcess()`。
- `app_src/system_config/app_config.h`：按键/电位器相关功能宏。

## 约定与边界

- 待补充：架构约束、命名约定、设计系统、接口契约、不要触碰的区域。

## 当前有效决策

- 待补充：仍然有效的产品、技术或流程决策。过期内容应移到 `agent_memory/archive/`。
