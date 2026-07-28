# ValetParkingStageParking 适配 MagnaDDS 文档目录

本目录用于沉淀泊车入位 `ValetParkingStageParking` 适配当前 `feature_integration_workspace` / MagnaDDS 工程的计划、状态、决策与交接资料。

> 重要原则：**聊天记录不是项目状态来源**。后续换电脑、换 AI、暂停恢复时，以本目录中的文档为准。

## 文档索引

| 文件/目录 | 作用 | 什么时候看 |
|---|---|---|
| `00_执行计划书_ValetParkingStageParking_MagnaDDS.md` | 总体执行计划、阶段划分、输入输出、验收标准、风险点 | 每次开始工作前、阶段验收前 |
| `01_换机中断交接与防偏离方案.md` | AI 无缝交接、防止执行偏离、卡住排查 Runbook | 换电脑/换 AI/卡住/方案变更时 |
| `02_零基础解释_本阶段代码与DDS术语.md` | 用生活类比解释 DDS 术语、本阶段新增代码文件和数据流 | 对 DDS 或当前代码分层不熟时先看 |
| `03_泊车算法源码本地化执行计划.md` | NEXT-032 源码本地化的范围、路径、步骤和验收标准 | 开始搬迁算法源码前 |
| `04_本地算法接入MagnaDDS中间件说明.md` | 解释本地化 `parking_algorithm_standalone` 如何通过 Component/Adapter 接入 MagnaDDS typed API | 想理解“算法源码”和“DDS 中间件适配层”关系时 |
| `05_TempAPA原始流程复现差距对照表.md` | 对照 `TempAPA_Code` 原始 `ValetParkingStageParking` 流程，标记当前已接入、轻量替代、缺失和后续优先级 | 判断当前 DDS MVP 与原始泊车流程差距时 |
| `06_两版PathProvider复用价值评估与接入切口.md` | 评估 `parking_algorithm_standalone` 与 `Repair_ValetParkingStageParking_260430-main` 两版已修代码节省的工作量，以及为什么仍要做 DDS Adapter 验证 | 判断 PathProvider 后续是否从参考代码复用、复制或继续轻量切片时 |
| `07_轻量Stage控制输入与034到037接入说明.md` | 解释 `ParkingCommand` Topic、direct/pause/brake/finish 分支和新增 mock/smoke 的作用 | 理解 NEXT-034 到 NEXT-037 的 Stage 控制输入接入时 |
| `STATUS.yaml` | 当前唯一状态入口，记录当前阶段、下一步、阻塞项 | 每次接手第一眼先看 |
| `status_snapshots/` | 每个阶段完成后输出一份带序号的项目状态快照 | 阶段完成、暂停、恢复前 |
| `decision_records/` | 所有偏离原计划或关键架构选择的决策留痕 | 方案争议、后续追溯 |
| `templates/` | 状态快照和决策记录模板 | 新增快照/决策时复制 |

## 当前约束

1. 当前 MVP 已接入 `ROI_DECIDER -> PATH_PROVIDER -> PATH_PARTITION -> SPEED_OPTIMIZER` 轻量链路，并新增 `ParkingCommand` 轻量 Stage 控制输入；但仍不接完整 `OpenSpacePathProvider` 大类、完整 `FunctionManager`、完整 `Frame/DependencyInjector`、线程管理或 NLP smoother。
2. 不修改 `compile/`、`thirdparty/`，不覆盖 `math/`、`sort/`、历史 temp code。
3. 只链接当前 workspace 的 Thirdparty MagnaDDS；官方 SDK 仅用于 IDL 生成和 API 参考。
4. 当前无 m57 板端，板端 DDS 通信验证只能标记为 `BLOCKED_NO_M57_BOARD`，不能冒充已通过。

## 对 AI 的固定开场指令

后续每次让 AI 接手时，建议把下面这句话放在需求最前面：

> 严格按 `Doc/valet_parking_magnadds/00_执行计划书_ValetParkingStageParking_MagnaDDS.md` 和 `STATUS.yaml` 执行，不做计划外功能扩展；遇到方案变更先说明原因并写入 `decision_records/`，每完成一个阶段必须输出带序号的项目状态快照。
