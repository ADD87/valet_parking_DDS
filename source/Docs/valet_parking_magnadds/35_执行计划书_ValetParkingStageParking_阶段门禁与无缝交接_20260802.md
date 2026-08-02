# 执行计划书（待你确认后执行）

- 文档编号：DOC-035
- 文档日期：2026-08-02
- 适用分支：`DeepSeeK_V4_pro_20260731`
- 适用工程：`feature_integration_workspace/applications`
- 目标对象：`ValetParkingStageParking` 对照 `TempAPA_Code` 的差异收敛与可交付推进
- 执行原则：**未获你明确同意，不进入下一阶段**

---

## 1. 执行总原则（强约束）

1. **阶段门禁**
	 - 每个阶段完成后，必须先提交阶段报告 + 项目状态快照 + diff 摘要。
	 - 你明确回复“同意进入下一阶段”前，执行停在当前阶段。

2. **状态必须落文档，不留聊天**
	 - 所有阶段状态以本地文档为准：
		 - `STATUS.yaml`
		 - `status_snapshots/*.md`
		 - `decision_records/*.md`

3. **每阶段结束必须提交 Git（applications 仓）**
	 - 分支固定：`DeepSeeK_V4_pro_20260731`
	 - 提交粒度：一阶段一提交（必要时可补 fix commit，但阶段收口必须有标识提交）
	 - 提交后必须输出：commit hash、改动文件清单、diff 摘要、风险与下一步建议。

4. **中断/换机无缝交接优先级高于编码速度**
	 - 每次停工前，必须有“下一步唯一动作（next_action）”。
	 - 接手 AI 或新电脑，只需按文档恢复，不依赖聊天上下文。

---

## 2. 阶段规划总览（本计划）

> 注：阶段编号采用 `Phase-P0~P5`；快照编号采用连续序号（见第 5 节命名规则）。

| 阶段 | 名称 | 目标 | 预计产出 |
|---|---|---|---|
| Phase-P0 | 计划冻结与门禁对齐 | 冻结范围、规则、模板、审批机制 | 计划书确认、快照、决策记录 |
| Phase-P1 | Stage 完成态主链对齐 | 对齐 `IsReadyToFinishStage -> FinishScenario` 语义 | 代码改动、smoke、阶段报告 |
| Phase-P2 | Brake/Pause 与 Stage 控制链对齐 | 对齐 `IsParkingBrakeCondition` 条件链和输出行为 | 代码改动、负向用例、阶段报告 |
| Phase-P3 | PathProvider 状态机深度对齐 | 对齐 `PrePlan/PlanningOnPathThread/UpdatePathStatus` 关键状态 | 代码改动、线程状态 smoke、阶段报告 |
| Phase-P4 | PathPartition + SpeedOptimizer 细节对齐 | 对齐 `PathDecider/IsTaskFinish/UpdateInteractiveStage` | 代码改动、场景矩阵、阶段报告 |
| Phase-P5 | FunctionManager 正式字段化落地方案 | 从“文本投影”走向“正式字段承载” | 设计文档+最小实现+回归报告 |

---

## 3. 各阶段详细执行定义

## 3.1 Phase-P0：计划冻结与门禁对齐（当前阶段）

### 阶段输入
- 现有治理文档：
	- `00_执行计划书_ValetParkingStageParking_MagnaDDS.md`
	- `01_换机中断交接与防偏离方案.md`
	- `05_TempAPA原始流程复现差距对照表.md`
	- `14_原始流程差异收敛执行记录.md`
	- `18_ILQR完整接入执行计划.md`
- 现状状态源：`STATUS.yaml`、`status_snapshots/`、`decision_records/`

### 阶段输出
- 本文档（DOC-035）
- 一份“待确认”项目状态快照
- 若有规则变更，新增对应 `DR` 决策记录

### 验收标准
- [ ] 已明确“未经你同意，不进入下一阶段”门禁
- [ ] 每阶段都有输入/输出/交付物/风险/验收标准
- [ ] 已明确每阶段结束必须提交 `DeepSeeK_V4_pro_20260731`
- [ ] 已明确每阶段结束必须输出 diff 摘要

### 风险点
- 风险：计划不够细导致执行偏离。
	- 缓解：每阶段拆分为“代码任务 + 验证任务 + 文档任务”。
- 风险：执行过程中越过审批直接推进。
	- 缓解：阶段出口强制等待“用户确认”标记。

### 阶段交付物
- `35_执行计划书_ValetParkingStageParking_阶段门禁与无缝交接_20260802.md`
- `status_snapshots/117_项目状态快照_Phase-P0_计划书待确认.md`

---

## 3.2 Phase-P1：Stage 完成态主链对齐

### 阶段输入
- TempAPA 锚点：
	- `stage_valet_parking_parking.cc` 中 `IsReadyToFinishStage`、`FinishScenario`
	- `stage.cc` 中 `ExecuteTaskOnOpenSpace` 输出收口相关路径
- 当前代码：`valet_parking_stage_parking_adapter.cpp` 与 stage/contract/runtime lite 相关文件

### 阶段输出
- 对齐后的完成态代码（不破坏现有通过场景）
- x86 smoke 证据（finish 边界、finish hold、release 恢复）
- 阶段报告 + 快照 + diff 摘要 + Git 提交

### 验收标准
- [ ] `destination_reached + standstill` 的完成判定在多帧连续条件下稳定
- [ ] 完成态后不再错误继续普通 task 链路
- [ ] direct release 与普通链切换不污染 history/runtime
- [ ] 既有 batch smoke 不回归

### 风险点
- 风险：完成态锁存引入“提前退出”或“无法恢复”
	- 缓解：增加 near-destination + target-moves 组合回归
- 风险：仅日志正确，行为不正确
	- 缓解：以轨迹/gear/状态三重断言，不只看 reason 文本

### 阶段交付物
- 代码提交（branch: `DeepSeeK_V4_pro_20260731`）
- `Phase-P1` 阶段报告 md
- `Phase-P1` 项目状态快照 md（带序号）
- diff 摘要（文件数、关键函数、行为变化、回归结果）

---

## 3.3 Phase-P2：Brake/Pause 与 Stage 控制链对齐

### 阶段输入
- TempAPA 锚点：`IsParkingBrakeCondition`、`SetParkingType`
- 当前 command 分支实现与 FunctionManager projection 文档

### 阶段输出
- pause/brake/finish/direct 控制链行为收敛
- 负向样例（invalid clear、conflict speed）稳定通过
- 阶段报告 + 快照 + diff 摘要 + Git 提交

### 验收标准
- [ ] pause 与 brake 行为在轨迹和状态上可区分
- [ ] 命令失活后的 direct release 行为稳定
- [ ] unsupported parking-out 路径保持一致且可观测
- [ ] 不引入普通泊车主链回归

### 风险点
- 风险：控制链改动影响普通链输出。
	- 缓解：normal/direct 两类 smoke 分开执行并交叉验证。

### 阶段交付物
- `Phase-P2` 阶段报告
- `Phase-P2` 快照
- Git 提交 + diff 摘要

---

## 3.4 Phase-P3：PathProvider 状态机深度对齐

### 阶段输入
- TempAPA 锚点：`PrePlan`、`PlanningOnPathThread`、`UpdatePathStatus`、`GetTargetOutput`
- 当前 thread manager + provider runtime 实现

### 阶段输出
- 关键状态机语义对齐（至少：WAIT_RESULT / SEARCH_FAILED / OVER_TIME）
- timeout/cancel/reuse/target_update 证据闭环
- 阶段报告 + 快照 + diff 摘要 + Git 提交

### 验收标准
- [ ] target thread 与 preplan candidate 行为边界清晰
- [ ] timeout/cancel 能稳定复现且输出一致
- [ ] history reuse 与 target update 不互相污染
- [ ] batch smoke 在 domain<=230 下全部通过

### 风险点
- 风险：线程状态改动引入偶发不稳定。
	- 缓解：每次仅小步改动，单独执行 timeout + multi-lot + normal 三组合。

### 阶段交付物
- `Phase-P3` 阶段报告
- `Phase-P3` 快照
- Git 提交 + diff 摘要

---

## 3.5 Phase-P4：PathPartition + SpeedOptimizer 细节对齐

### 阶段输入
- TempAPA 锚点：`PathDecider`、`IsTaskFinish`、`UpdateStatusBasedPartitionResult`、`UpdateInteractiveStage`
- 当前 task runtime/task contract lite 实现

### 阶段输出
- 任务级决策与 finish 边界更贴近原始语义
- 速度层 interactive stage 与 fallback 语义收敛
- 阶段报告 + 快照 + diff 摘要 + Git 提交

### 验收标准
- [ ] task_finish / prepare_finish / track_abnormal 区分清晰
- [ ] speed fallback 与 stop path 输出行为一致
- [ ] chosen path 与 speed trajectory 对齐不漂移
- [ ] 相关 smoke 矩阵通过

### 风险点
- 风险：路径决策与速度层耦合导致回归链较长。
	- 缓解：先固定 PathPartition，再单独推进 SpeedOptimizer。

### 阶段交付物
- `Phase-P4` 阶段报告
- `Phase-P4` 快照
- Git 提交 + diff 摘要

---

## 3.6 Phase-P5：FunctionManager 正式字段化落地方案

### 阶段输入
- 现有 FunctionManagerProjection 文本契约
- 当前 IDL/topic 可扩展空间与风险边界

### 阶段输出
- 字段化迁移方案（先设计后最小落地）
- 不破坏兼容性的增量接入实现
- 阶段报告 + 快照 + diff 摘要 + Git 提交

### 验收标准
- [ ] 明确哪些字段先落地、哪些保持兼容文本
- [ ] 回归不依赖聊天结论，可由文档和日志复核
- [ ] 兼容老脚本和当前 smoke 框架

### 风险点
- 风险：字段化过快导致上下游兼容问题。
	- 缓解：先“双写”阶段（字段 + 文本），再切换消费方。

### 阶段交付物
- `Phase-P5` 阶段报告
- `Phase-P5` 快照
- Git 提交 + diff 摘要

---

## 4. 每阶段固定出口清单（必须全部满足）

每个 Phase 完成后，必须输出以下 6 项：

1. **阶段报告 md**（完成了什么、没完成什么、原因）
2. **项目状态快照 md（带序号）**
3. **Git 提交到分支 `DeepSeeK_V4_pro_20260731`**
4. **diff 摘要**（按模块说明变化，不贴冗长全文）
5. **验证证据**（build/smoke/关键日志路径）
6. **下一步建议**（但默认不执行，等待你批准）

任何一项缺失，阶段状态只能标记为 `INCOMPLETE`。

---

## 5. 状态沉淀规范（无缝交接核心）

## 5.1 快照命名规则（必须带序号）

- 目录：`applications/source/Docs/valet_parking_magnadds/status_snapshots/`
- 命名：`NNN_项目状态快照_Phase-PX_简短主题.md`
- 示例：`118_项目状态快照_Phase-P1_Stage完成态对齐.md`

`NNN` 规则：
- 在现有最大序号基础上 +1。
- 若一次阶段拆多批，可追加区间名，如 `121_124_...`，但必须保持单调递增。

## 5.2 快照最小字段模板（强制）

每份快照必须包含：

- 快照编号 / 日期 / 分支 / commit hash
- 当前阶段 / 阶段状态（PASS、BLOCKED、INCOMPLETE）
- 本阶段完成项（可核验）
- 未完成项与原因
- 风险与阻塞项（含 owner）
- 验证证据（命令、日志路径、产物路径）
- 下一步唯一建议动作（next_action）
- 交接提示（换机后先看哪 3 个文件）

## 5.3 交接三件套（每次中断必须更新）

1. `STATUS.yaml`（当前权威状态）
2. 最新快照（`status_snapshots`）
3. 若有策略变化，对应 `decision_records/DR-xxx_*.md`

---

## 6. 防偏离机制

1. **范围防偏离**
	 - 只允许改：
		 - `applications/source/valet_parking/**`
		 - `applications/source/valet_parking_tools/**`
		 - `applications/source/Docs/valet_parking_magnadds/**`
	 - 默认禁止改：`compile/`、`thirdparty/`（除你明确批准）。

2. **行为防偏离**
	 - 任何“看起来更快”的捷径，只要破坏阶段门禁，一律拒绝。
	 - 没有验证证据的“已完成”视为未完成。

3. **审批防偏离**
	 - 每阶段结束后，先提交“阶段报告 + 快照 + diff 摘要”。
	 - 你回复“同意进入下一阶段”后才继续。

---

## 7. 每阶段 Git 与汇报规范

## 7.1 Git 规范

- 仓库：`applications`
- 分支：`DeepSeeK_V4_pro_20260731`
- 阶段结束动作：
	- `git add -A`
	- `git commit -m "phase PX: <阶段主题>"`
	- `git push origin DeepSeeK_V4_pro_20260731`

## 7.2 向你汇报的最小内容（每阶段）

- 阶段编号与状态
- commit hash
- 变更文件数与关键文件列表
- diff 摘要（功能点级）
- 验收结果（通过/失败项）
- 风险与阻塞
- 下一阶段建议（待你批准）

---

## 8. 本计划当前状态与下一动作

- 当前状态：`Phase-P0 / 待你确认`
- 当前已交付：DOC-035（本计划书）+ 快照（见 status_snapshots）
- 下一动作：
	- 你确认本计划后，我才进入 `Phase-P1` 执行。
	- 未确认前，不改业务代码，不推进下一阶段。

---

## 9. 确认清单（请你勾选/回复）

- [ ] 同意按 `Phase-P0~P5` 执行
- [ ] 同意“每阶段结束必须 Git 提交到 `DeepSeeK_V4_pro_20260731`”
- [ ] 同意“每阶段结束必须输出带序号项目状态快照 md”
- [ ] 同意“未经你同意，不进入下一阶段”
- [ ] 同意“每阶段必须汇报 diff 摘要后再申请进入下一阶段”

> 你确认后，我将只启动 `Phase-P1`，并在阶段完成后先给你报告，不跨阶段推进。
