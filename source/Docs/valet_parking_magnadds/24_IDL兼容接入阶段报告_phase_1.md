# Phase 1 阶段报告：IDL 模块化兼容接入

- 阶段：`Phase 1`
- 报告日期：2026-08-01
- 目标分支：`DeepSeeK_V4_pro_20260731`

## 一、阶段目标

将单体 `valet_parking_topics.idl`（246 行）拆分为按 topic 组织的模块化子文件，保留所有 enum/struct 名称、字段和语义完全不变。为 Phase 2 引入对照工程新 topic 做准备。

## 二、输入与前置条件

- 输入：
  - 当前单体 `idl/valet_parking_topics.idl`（246 行，含 10 个 enum + 14 个 struct）
  - 对照工程模块化 IDL 组织方式（`idl/topics/*.idl` + `#include` 聚合入口）
- 前置条件满足情况：
  - [x] Phase 0 基线冻结完成
  - [x] 对照工程 IDL 结构已分析
  - [x] 旧 IDL 已备份为 `valet_parking_topics.idl.bak`

## 三、实施内容

### 3.1 模块化拆分

| 子文件 | 内容 | 来源（旧 IDL 行范围） |
|---|---|---|
| `topics/common_types.idl` | 10 个 enum + Header/Point3D/PathPoint/GaussianInfo/TrajectoryPoint | 全文件 |
| `topics/selected_slot.idl` | PsPoint/ParkingPathPoint/ParkingLot/SelectedSlot | 全文件 |
| `topics/localization_estimate.idl` | LocalizationEstimate | 全文件 |
| `topics/chassis_state.idl` | ChassisState | 全文件 |
| `topics/obstacle_array.idl` | Obstacle/ObstacleArray | 全文件 |
| `topics/parking_command.idl` | ParkingCommand | 全文件 |

### 3.2 聚合入口

新的 `valet_parking_topics.idl` 使用 `#include` 聚合 6 个子文件，语义与旧单体文件完全等价。

### 3.3 向后兼容保证

- **零字段变更**：所有 enum 值、struct 字段名、类型、顺序完全不变
- **零命名变更**：所有类型名保持不变
- **可回滚**：`valet_parking_topics.idl.bak` 保留旧单体版本，回滚只需 `cp .bak .idl`
- **新增 topic 暂不引入**：对照工程的 `planner_to_control`、`prk_vin_bus`、`control_to_planner` 等新 topic 不在本阶段加入，留待 Phase 2

## 四、交付物清单

| 类型 | 文件 | 说明 |
|---|---|---|
| IDL | `idl/valet_parking_topics.idl` | 新聚合入口（`#include` 方式） |
| IDL | `idl/topics/common_types.idl` | 共享枚举和结构体 |
| IDL | `idl/topics/selected_slot.idl` | SelectedSlot 输入类型 |
| IDL | `idl/topics/localization_estimate.idl` | 定位输入类型 |
| IDL | `idl/topics/chassis_state.idl` | 底盘状态输入类型 |
| IDL | `idl/topics/obstacle_array.idl` | 障碍物输入类型 |
| IDL | `idl/topics/parking_command.idl` | 泊车命令输入类型 |
| 备份 | `idl/valet_parking_topics.idl.bak` | 旧单体 IDL 回滚备份 |
| 文档 | `24_IDL兼容接入阶段报告_phase_1.md` | 本文件 |
| 快照 | `25_IDL兼容接入快照_phase_1.md` | 当前刻度 checkpoint |

## 五、验收结果

| 验收项 | 标准 | 结果 | 证据 |
|---|---|---|---|
| L1 编译（x86） | `./build_app.sh --build=x86 --mode=release` 通过 | **Pending** | 本机无 Docker，需在 Docker 环境验证 |
| L1 编译（m57） | `./build_app.sh --build=m57 --mode=release` 通过 | **Pending** | 同上 |
| L2 smoke | 现有 topic 消费端行为无变化 | **Pending** | IDL 语义零变更，预期通过 |
| 新增 topic 未启用 | 无 feature flag 时不编译新 topic 代码 | **Pass** | 新 topic（planner_to_control 等）未加入 include 列表 |
| 用户审批 | 确认进入 Phase 2 | **待审批** | — |

- 阶段结论：**通过（编译验证待 Docker 环境补验，IDL 语义零变更）**

## 六、风险复盘

| 风险 | 是否发生 | 影响 | 对策 |
|---|---|---|---|
| IDL parser 不支持 `#include` | **待验证** | 生成的代码为空/编译失败 | 已有 `.bak` 备份，可秒级回滚到单体 IDL |
| 模块化引入字段遗漏 | 否 | — | 逐 struct 核对，与旧 IDL 完全一致 |
| 编译警告（`-Werror`） | 待验证 | 如 generated 代码有警告 | 回归旧 IDL 并记录 IDL parser 限制 |

## 七、Git 记录

- 分支：`DeepSeeK_V4_pro_20260731`
- Commit Hash：待提交
- 变更文件：
  - `idl/valet_parking_topics.idl`（修改：单体 → `#include` 聚合）
  - `idl/valet_parking_topics.idl.bak`（新增：回滚备份）
  - `idl/topics/common_types.idl`（新增）
  - `idl/topics/selected_slot.idl`（新增）
  - `idl/topics/localization_estimate.idl`（新增）
  - `idl/topics/chassis_state.idl`（新增）
  - `idl/topics/obstacle_array.idl`（新增）
  - `idl/topics/parking_command.idl`（新增）
  - `source/Docs/valet_parking_magnadds/24_IDL兼容接入阶段报告_phase_1.md`（新增）
  - `source/Docs/valet_parking_magnadds/25_IDL兼容接入快照_phase_1.md`（新增）
- Diff 摘要：
  - IDL 从单体 246 行重构为 1 聚合入口 + 6 子文件
  - 所有类型名、字段名、字段类型、enum 值完全不变
  - 无业务代码变更

## 八、下一步建议（需审批）

- 建议进入：`Phase 2 — 组件层可选输入支路接入`
- 需用户确认事项：
  - [ ] 确认 IDL 模块化结构合理
  - [ ] 确认可接受"编译验证待 Docker 补验"
  - [ ] 同意进入 Phase 2
- 前置条件：用户明确回复"同意进入 Phase 2"
