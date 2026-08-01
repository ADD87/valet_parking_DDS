# 项目状态快照 — Phase 2 组件层可选输入支路接入

- 快照编号：`27`
- 对应阶段：`Phase 2`
- 日期：2026-08-01
- 分支：`DeepSeeK_V4_pro_20260731`
- 当前状态：`Awaiting Approval`

## 1. 本刻度已完成
1. 新增 `ENABLE_PRKVINBUS` feature flag（`compile/cmake/app_build_feature_flags_config.h.in`）
2. 新增 `kMaxOutputWaypointCount = 100U` 常量 + waypoint 截断保护（始终启用）
3. 新增 4 个 PrkVinBus 坐标转换工具函数（`#if ENABLE_PRKVINBUS` 块内）
4. 引入 3 个参考 IDL 文件（`idl/topics_ref/`，未编译）
5. 产出 Phase 2 阶段报告（`26_组件可选支路阶段报告_phase_2.md`）
6. 产出本快照（`27_组件可选支路快照_phase_2.md`）

## 2. 本刻度未完成
- L1/L2 编译验证（本机无 Docker）
- PrkVinBus DataReader 接入（需 IDL 生成后）
- 等待用户审批

## 3. 风险与阻塞
- 风险：PrkVinBus reader 待 IDL parser 生成代码后接入
- 阻塞：本机无 Docker，无法编译验证
- 缓解：工具函数已就绪，flag 关闭时零行为变化

## 4. 本刻度验收
| 验收项 | 结果 | 证据 |
|---|---|---|
| L1 编译（x86）flag OFF | Pending | 需 Docker 环境 |
| L1 编译（x86）flag ON | Pending | 需 Docker 环境 |
| L1 编译（m57） | Pending | 需 Docker 环境 |
| L2 smoke flag OFF | Pending | waypoint 保护常规场景不触发 |
| feature flag 行为 | Pending | flag ON 仅编译工具函数，无运行行为变更 |

## 5. 变更摘要
- 变更文件：
  - `compile/cmake/app_build_feature_flags_config.h.in`（修改）
  - `src/valet_parking_component.cpp`（修改）
  - `idl/topics_ref/prk_vin_bus.idl`（新增）
  - `idl/topics_ref/planner_to_control.idl`（新增）
  - `idl/topics_ref/control_to_planner.idl`（新增）
  - `source/Docs/.../26_组件可选支路阶段报告_phase_2.md`（新增）
  - `source/Docs/.../27_组件可选支路快照_phase_2.md`（新增）
- Commit Hash：待提交
- Diff 要点：
  - waypoint 保护 + feature flag + 坐标转换工具函数
  - 新增参考 IDL 文件（未编译）

## 6. 唯一下一步动作
- 下一步：等待用户审批。可选择进入 Phase 3（算法灰度）或留在 Phase 2 补完 PrkVinBus reader
- 前置条件：用户明确回复
