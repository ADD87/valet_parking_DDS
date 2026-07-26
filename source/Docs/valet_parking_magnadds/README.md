# ValetParkingStageParking 适配 MagnaDDS 文档目录

本目录用于沉淀泊车入位 `ValetParkingStageParking` 适配当前 `feature_integration_workspace` / MagnaDDS 工程的计划、状态、决策与交接资料。

> 重要原则：**聊天记录不是项目状态来源**。后续换电脑、换 AI、暂停恢复时，以本目录中的文档为准。

## 文档索引

| 文件/目录 | 作用 | 什么时候看 |
|---|---|---|
| `00_执行计划书_ValetParkingStageParking_MagnaDDS.md` | 总体执行计划、阶段划分、输入输出、验收标准、风险点 | 每次开始工作前、阶段验收前 |
| `01_换机中断交接与防偏离方案.md` | AI 无缝交接、防止执行偏离、卡住排查 Runbook | 换电脑/换 AI/卡住/方案变更时 |
| `STATUS.yaml` | 当前唯一状态入口，记录当前阶段、下一步、阻塞项 | 每次接手第一眼先看 |
| `status_snapshots/` | 每个阶段完成后输出一份带序号的项目状态快照 | 阶段完成、暂停、恢复前 |
| `decision_records/` | 所有偏离原计划或关键架构选择的决策留痕 | 方案争议、后续追溯 |
| `templates/` | 状态快照和决策记录模板 | 新增快照/决策时复制 |

## 当前约束

1. 首版只做 MagnaDDS 通信骨架和 m57 可编译 `.so`，不接完整泊车算法。
2. 不修改 `compile/`、`thirdparty/`，不覆盖 `math/`、`sort/`、历史 temp code。
3. 只链接当前 workspace 的 Thirdparty MagnaDDS；官方 SDK 仅用于 IDL 生成和 API 参考。
4. 当前无 m57 板端，板端 DDS 通信验证只能标记为 `BLOCKED_NO_M57_BOARD`，不能冒充已通过。

## 对 AI 的固定开场指令

后续每次让 AI 接手时，建议把下面这句话放在需求最前面：

> 严格按 `Doc/valet_parking_magnadds/00_执行计划书_ValetParkingStageParking_MagnaDDS.md` 和 `STATUS.yaml` 执行，不做计划外功能扩展；遇到方案变更先说明原因并写入 `decision_records/`，每完成一个阶段必须输出带序号的项目状态快照。
