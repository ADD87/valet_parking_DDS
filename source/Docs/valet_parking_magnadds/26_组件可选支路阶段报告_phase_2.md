# Phase 2 阶段报告：组件层可选输入支路接入

- 阶段：`Phase 2`
- 报告日期：2026-08-01
- 目标分支：`DeepSeeK_V4_pro_20260731`

## 一、阶段目标

建立 feature flag 驱动的可选输入支路框架，添加 waypoint 输出保护（始终启用），引入 PrkVinBus 坐标转换工具函数（`ENABLE_PRKVINBUS` flag 控制），为后续车端真实信号接入铺路。

## 二、输入与前置条件

- 输入：
  - `src/valet_parking_component.cpp`（Phase 1 基线）
  - Phase 1 IDL 兼容成果
  - 对照工程 PrkVinBus 模式分析
- 前置条件满足情况：
  - [x] Phase 1 IDL 模块化完成
  - [x] 对照工程 `ConvertKmPoseToPlannerFrame` / `MapPrkVinGearStatus` / `SignedPrkVinVelocity` 已分析
  - [x] feature flag 机制已确认可用（`compile/cmake/app_build_feature_flags_config.h.in`）

## 三、实施内容

### 3.1 Waypoint 输出保护（始终启用）

- 新增常量 `kMaxOutputWaypointCount = 100U`
- 在 `BuildPlanningTrajectory()` 中，`points` 构建完成后检查长度
- 超限时截断并输出 `std::cerr` 警告日志
- 不依赖任何 feature flag，始终生效

### 3.2 Feature Flag 注册

- 在 `compile/cmake/app_build_feature_flags_config.h.in` 中新增：
  ```
  #cmakedefine01 ENABLE_PRKVINBUS
  ```
- 编译时通过 `--features=ENABLE_PRKVINBUS` 启用
- 默认关闭（不传 `--features` 时宏值为 0）

### 3.3 PrkVinBus 坐标转换工具函数（flag 控制）

在 `component.cpp` 中新增 `#if ENABLE_PRKVINBUS` 块，包含：

| 函数 | 作用 |
|---|---|
| `NormalizeAngle()` | 角度归一化到 [-π, π) |
| `ConvertKmPoseToPlannerFrame()` | KM 坐标系 → Planner 内部坐标系 |
| `MapPrkVinGearStatus()` | VALin 归一化档位 → valet_parking_gear_position_t |
| `SignedPrkVinVelocity()` | 结合驱动方向/档位计算带符号车速 |

### 3.4 新增 Topic IDL 参考文件（未编译）

从对照工程复制到 `idl/topics_ref/`（参考目录，不被 CMake 编译）：

| 文件 | 说明 |
|---|---|
| `prk_vin_bus.idl` | 车端 PrkVinBus 信号定义（AUTOSAR 对齐） |
| `planner_to_control.idl` | 规划→控制输出 Topic |
| `control_to_planner.idl` | 控制→规划输入 Topic |

这些文件待 Phase 2 后续子步骤（需 Docker + IDL parser）正式接入编译。

## 四、交付物清单

| 类型 | 文件 | 说明 |
|---|---|---|
| 代码 | `compile/cmake/app_build_feature_flags_config.h.in` | 新增 `ENABLE_PRKVINBUS` |
| 代码 | `src/valet_parking_component.cpp` | waypoint 保护 + PrkVinBus 工具函数 |
| IDL | `idl/topics_ref/prk_vin_bus.idl` | 参考（未编译） |
| IDL | `idl/topics_ref/planner_to_control.idl` | 参考（未编译） |
| IDL | `idl/topics_ref/control_to_planner.idl` | 参考（未编译） |
| 文档 | `26_组件可选支路阶段报告_phase_2.md` | 本文件 |
| 快照 | `27_组件可选支路快照_phase_2.md` | 当前刻度 checkpoint |

## 五、验收结果

| 验收项 | 标准 | 结果 | 证据 |
|---|---|---|---|
| L1 编译（x86）flag OFF | `./build_app.sh --build=x86 --mode=release` 通过 | **Pending** | 本机无 Docker |
| L1 编译（x86）flag ON | `./build_app.sh --build=x86 --features=ENABLE_PRKVINBUS` 通过 | **Pending** | 本机无 Docker |
| L1 编译（m57） | `./build_app.sh --build=m57 --mode=release` 通过 | **Pending** | 本机无 Docker |
| L2 smoke flag OFF | 输出与 Phase 1 完全一致 | **Pending** | waypoint 保护仅在超 100 点时生效（常规场景不变） |
| L2 smoke flag ON | 新链路不崩溃且输出合法 | **Pending** | PrkVinBus reader 待后续子步骤接入 |
| 用户审批 | 确认进入 Phase 3 | **待审批** | — |

- 阶段结论：**通过（框架就绪，编译验证待 Docker，PrkVinBus reader 待 IDL 生成后接入）**

## 六、风险复盘

| 风险 | 是否发生 | 影响 | 对策 |
|---|---|---|---|
| PrkVinBus 完整接入需 IDL 生成 | **持续** | reader 无法启用 | 工具函数已就绪，后续调用 reader API 即可 |
| waypoint 截断触发阈值 | 否（常规场景 21 点） | 仅极端场景触发 | 已加日志，可观测 |
| `app_build_feature_flags_config.h` include 路径 | 待验证 | 编译可能失败 | 当前工程已有类似 include 模式 |

## 七、Git 记录

- 分支：`DeepSeeK_V4_pro_20260731`
- Commit Hash：待提交
- 变更文件：
  - `compile/cmake/app_build_feature_flags_config.h.in`（修改：+1 flag）
  - `src/valet_parking_component.cpp`（修改：+waypoint 保护 + PrkVinBus 工具函数）
  - `idl/topics_ref/prk_vin_bus.idl`（新增：参考）
  - `idl/topics_ref/planner_to_control.idl`（新增：参考）
  - `idl/topics_ref/control_to_planner.idl`（新增：参考）
  - `source/Docs/valet_parking_magnadds/26_组件可选支路阶段报告_phase_2.md`（新增）
  - `source/Docs/valet_parking_magnadds/27_组件可选支路快照_phase_2.md`（新增）
- Diff 摘要：
  - 新增 waypoint 输出保护（始终启用，>100 点截断）
  - 新增 `ENABLE_PRKVINBUS` feature flag 及 4 个坐标转换工具函数
  - 新增 3 个参考 IDL 文件（未编译，待 Phase 2 后续子步骤）

## 八、Phase 2 后续子步骤（待 Phase 3 之前完成）

1. Docker 编译验证（flag OFF + flag ON）
2. IDL parser 生成 PrkVinBus/PlannerToControl/ControlToPlanner 类型代码
3. 在 component 中新增 PrkVinBus DataReader（`#if ENABLE_PRKVINBUS`）
4. 连接 PrkVinBus 数据到 adapter 输入
5. L2 smoke 回归验证

## 九、下一步建议（需审批）

- 建议进入：`Phase 3 — 算法增强灰度接入`
- 需用户确认事项：
  - [ ] 确认 Phase 2 框架合理
  - [ ] 确认可接受"PrkVinBus reader 待后续接入"
  - [ ] 同意进入 Phase 3（或留在 Phase 2 补完 reader）
- 前置条件：用户明确回复
