# Phase 0 阶段报告：基线冻结与交接治理

- 阶段：`Phase 0`
- 报告日期：2026-08-01
- 目标分支：`DeepSeeK_V4_pro_20260731`

## 一、阶段目标

冻结当前工程基线，记录关键文件清单与对照工程差异结论，明确禁止动作，建立后续阶段文档编号规则，确保任意阶段可暂停、可交接。

## 二、输入与前置条件

- 输入：当前 workspace HEAD、对照工程差异分析结论、执行计划书 v2.0（`19_执行计划书`）
- 前置条件满足情况：
  - [x] 执行计划书 v2.0 已审批
  - [x] applications git 仓库已初始化并推送到 `DeepSeeK_V4_pro_20260731`
  - [x] 对照工程差异分析已完成（`15_差异对比.md`）
  - [x] 工作树干净，无未提交变更

## 三、实施内容

### 3.1 基线冻结

| 项 | 值 |
|---|---|
| 仓库 | `applications` (git@github.com:ADD87/valet_parking_DDS.git) |
| 分支 | `DeepSeeK_V4_pro_20260731` |
| 基线 commit | `74f2ed1` |
| 工作树状态 | 干净（`git status --short` 无输出） |
| 冻结时间 | 2026-08-01 |

### 3.2 关键文件清单（当前行数 vs 对照工程行数）

| 文件 | 当前行数 | 对照行数 | 差异判定 |
|---|---|---|---|
| `valet_parking_component.cpp` | 1,349 | 1,845 | **差异大，禁止整替** |
| `valet_parking_stage_parking_adapter.cpp` | 3,135 | 4,545 | **差异大，禁止整替** |
| `valet_parking_task_contract_lite.cpp` | 512 | 522 | 差异小（诊断字段） |
| `valet_parking_stage_contract_lite.cpp` | 449 | 449 | 完全一致 |
| `valet_parking_stage_facade_lite.cpp` | 196 | 196 | 完全一致 |
| `valet_parking_stage_process_lite.cpp` | 170 | 170 | 完全一致 |
| `idl/valet_parking_topics.idl` | 246 | — | 单文件 vs 模块化 |
| `CMakeLists.txt` | 116 | — | 依赖差异（eigen vs tempapa+NLP） |
| `algorithm/` | 118 文件 | — | 已本地化 |

### 3.3 对照工程独有文件（仅记录，不自动引入）

- `nlp_path_smoother_lite.cc/.h`
- `scs_shape_path.cc/.h`
- `penalty_function_method.cc/.h`
- `geometry_path_generator.cc/.h`
- `idl/topics/*.idl`（模块化 IDL，9 个子 topic 文件）
- `generated/prk_vin_bus*`
- `cmake/tempapa_thirdparty.cmake`

### 3.4 禁止动作清单（Phase 0~4 全程有效）

| 编号 | 禁止动作 | 原因 |
|---|---|---|
| FORBID-01 | 全量替换 `valet_parking_component.cpp` | 耦合 DDS topic、运行时契约、输出语义 |
| FORBID-02 | 全量替换 `valet_parking_stage_parking_adapter.cpp` | 已与 `*_lite` 运行时体系深度配套 |
| FORBID-03 | 修改 `compile/` 或 `thirdparty/` | 首版不动基础构建和第三方库 |
| FORBID-04 | 引入不可回滚变更 | 所有新增能力必须可开关 |
| FORBID-05 | 删除现有 Topic 字段 | 保留向后兼容 |
| FORBID-06 | 跳过阶段审批闸门 | 未经用户同意不得进入下一阶段 |

### 3.5 文档编号规则

后续所有阶段文档沿用 `NN_描述_phase_Y.md` 格式，从 22 起递增：
- Phase 0 报告：`22_基线冻结阶段报告_phase_0.md`
- Phase 0 快照：`23_基线冻结快照_phase_0.md`
- Phase 1 报告：`24_IDL兼容接入阶段报告_phase_1.md`
- Phase 1 快照：`25_IDL兼容接入快照_phase_1.md`
- …以此类推

## 四、交付物清单

| 类型 | 文件 | 说明 |
|---|---|---|
| 文档 | `22_基线冻结阶段报告_phase_0.md` | 本文件 |
| 快照 | `23_基线冻结快照_phase_0.md` | 当前刻度 checkpoint |
| 代码 | 基线 commit `74f2ed1` | 冻结基线 |

## 五、验收结果

| 验收项 | 标准 | 结果 | 证据 |
|---|---|---|---|
| 基线 commit 可复现 | `git log` 确认 HEAD = `74f2ed1` | **Pass** | `74f2ed1` 已推送 origin |
| 阶段文档齐全 | 22/23 两份文档已生成 | **Pass** | 文件存在于 Docs 目录 |
| 路径固定 | 统一在 `Docs/valet_parking_magnadds/` | **Pass** | 路径一致 |
| L1 编译（x86） | Docker 内 `./build_app.sh --build=x86 --mode=release` | **Skip** | Phase 0 无代码变更，基线编译已在之前验证 |
| L1 编译（m57） | Docker 内 `./build_app.sh --build=m57 --mode=release` | **Skip** | 同上 |
| 用户审批 | 确认进入 Phase 1 | **待审批** | — |

- 阶段结论：**通过（待用户审批）**

## 六、风险复盘

| 风险 | 是否发生 | 影响 | 对策 |
|---|---|---|---|
| 基线未冻结导致后续 diff 污染 | 否 | — | 已冻结 commit `74f2ed1` |
| 文档路径不一致 | 否 | — | 已统一到 `Docs/valet_parking_magnadds/` |
| Git 推送失败 | 是（临时） | HTTPS 403 → 切换 SSH 解决 | 远程 URL 已改为 SSH |

## 七、Git 记录

- 分支：`DeepSeeK_V4_pro_20260731`
- 基线 Commit Hash：`74f2ed1`
- 变更文件（本次 Phase 0 产出）：
  - `source/Docs/valet_parking_magnadds/22_基线冻结阶段报告_phase_0.md`（新增）
  - `source/Docs/valet_parking_magnadds/23_基线冻结快照_phase_0.md`（新增）
- Diff 摘要：
  - 新增 Phase 0 阶段报告与快照两份文档
  - 无业务代码变更

## 八、下一步建议（需审批）

- 建议进入：`Phase 1 — IDL 模块化兼容接入`
- 需用户确认事项：
  - [ ] 确认基线冻结无误
  - [ ] 确认禁止动作清单（FORBID-01 ~ FORBID-06）
  - [ ] 同意进入 Phase 1
- 前置条件：用户明确回复"同意进入 Phase 1"
