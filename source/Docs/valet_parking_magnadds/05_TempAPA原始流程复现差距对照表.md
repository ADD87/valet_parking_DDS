# TempAPA 原始泊车流程复现差距对照表

- 文档编号：DOC-005
- 日期：2026-07-28
- 触发原因：用户询问当前 `ValetParkingStageParkingAdapter::Process()` 能否完美复现原始 `E:\APA\DDS\TempAPA_Code` 的泊车入位流程。
- 当前结论：当前 DDS 适配已复现普通 open space 主链路骨架，但不能声称完美复现原始 Stage/Frame/OpenSpacePathProvider 全量行为。

## 1. 参考源

| 类型 | 路径 |
|---|---|
| 原始流程说明 | `E:\APA\DDS\TempAPA_Code\00_ValetParkingStageParking_超详细流程图总览_重要.md` |
| 原始 Stage 入口 | `E:\APA\DDS\TempAPA_Code\planning\scenarios\valet_parking\stage_valet_parking_parking.cc` |
| 原始 Task 调度 | `E:\APA\DDS\TempAPA_Code\planning\scenarios\stage.cc` |
| 原始 PATH_PROVIDER | `E:\APA\DDS\TempAPA_Code\planning\tasks\optimizers\open_space_path_generation\open_space_path_provider.cc` |
| 原始线程管理 | `E:\APA\DDS\TempAPA_Code\planning\open_space\open_space_thread\open_space_thread_manager.h` |
| 原始平滑器 | `E:\APA\DDS\TempAPA_Code\planning\tasks\optimizers\open_space_path_generation\open_space_path_smoother.cc` |
| 原始直行分支 | `E:\APA\DDS\TempAPA_Code\planning\tasks\optimizers\open_space_straight_path\open_space_straight_path_provider.cc` |
| 当前 DDS Adapter | `applications/source/valet_parking/src/valet_parking_stage_parking_adapter.cpp` |
| 当前 DDS Component | `applications/source/valet_parking/src/valet_parking_component.cpp` |
| 当前本地算法源码 | `applications/source/valet_parking/algorithm/parking_algorithm_standalone` |

## 2. 当前复现程度一句话

当前已经复现的主线：

```text
SelectedSlot
-> ConvertParkingLot
-> BuildVehicleState
-> ROI_DECIDER
-> PATH_PROVIDER_PRECHECK
-> PATH_PROVIDER 核心路径生成
-> PATH_PARTITION
-> SPEED_OPTIMIZER
-> PlanningTrajectory
```

原始普通 open space 主线：

```text
ValetParkingStageParking::Process
-> Stage::ExecuteTaskOnOpenSpace
-> OpenSpaceRoiDecider::Process
-> OpenSpacePathProvider::Process
-> OpenSpacePathPartition::Process
-> OpenSpaceSpeedOptimizer::Process
-> Stage 发布轨迹和状态
```

两者主链路方向一致，但当前 DDS Adapter 省略或轻量替代了原始工程的 Stage 状态机、FunctionManager、Frame/OpenSpaceInfo、完整 OpenSpacePathProvider 线程和平滑器。

## 3. 差距等级定义

| 状态 | 含义 |
|---|---|
| 已接入 | 当前代码已经调用同名或等价算法能力，并有 x86 smoke 或构建验证 |
| 轻量替代 | 当前用 Adapter/RuntimeContext 自己实现了最小可用语义，但不是原始大类完整行为 |
| 缺失 | 当前没有对应能力，不能声称行为等价 |
| 暂不接入 | 当前 MVP 不应该接入，否则会显著扩大范围或引入未验证依赖 |

优先级：

| 优先级 | 含义 |
|---|---|
| P0 | 影响主链路正确性或会造成明显行为偏差，建议近期补 |
| P1 | 影响原始流程还原度，但可以在当前 MVP 稳定后逐步补 |
| P2 | 更接近原车工程完整框架，当前阶段只记录，不急于接 |

## 4. 原始流程 vs 当前 DDS 适配对照

| 原始节点 | 原始行为 | 当前 DDS 适配状态 | 当前实现位置 | 差距与建议 |
|---|---|---|---|---|
| `ValetParkingStageParking::Process` | 设置 `stage_type=PARKING`，标记 open space 轨迹，处理 `next_stage_`、刹停、暂停、任务执行、完成状态 | 轻量替代 | `ValetParkingStageParkingAdapter::Process()` | P0：当前没有完整 StageStatus/next_stage/FinishScenario。建议后续新增轻量 `MissionState`，先覆盖 pause/brake/finish 的可观测行为 |
| `IsParkingBrakeCondition` | `STRAIGHTBRAKE` 或 `PAUSE` 时生成 pause trajectory，并按 warning 区分等待障碍物 | 缺失 | 当前仅输入非法时 estop | P0：需要临时 DDS 控制字段或 C API 输入表达 pause/brake，不能用 `SelectedSlot.is_valid=false` 混淆 |
| `SetParkingType` | 将 `FunctionManagerIn.sys_command` 映射为 `PARKING_IN/PARKING_OUT/DIRECT_FORWARD/DIRECT_BACKWARD/...` | 缺失 | 当前默认按泊车入位处理 | P0：建议引入轻量 `ParkingCommand`/`ParkingMode`，不直接搬 `function_manager.pb.h` |
| `Stage::ExecuteTaskOnOpenSpace` | 遍历 task_list，按 `DIRECT_*` 跳过普通 ROI/PATH/PARTITION，执行 straight path | 轻量替代 | Adapter 手写固定调用顺序 | P0：当前固定执行普通 open space 链路。建议先补 `DIRECT_FORWARD/DIRECT_BACKWARD` 分支，再评估 task list 化 |
| 普通 task 顺序 | `ROI_DECIDER -> PATH_PROVIDER -> PATH_PARTITION -> SPEED_OPTIMIZER` | 已接入 | `Process()` 中 ROI、`RunPathProvider()`、`RunPathPartition()`、`RunSpeedOptimizer()` | 已覆盖主链路骨架，继续保持 x86/m57 回归 |
| `OpenSpaceRoiDecider::Process` | 读取 Frame/OpenSpaceInfo/parking spots，更新 ROI、目标、障碍物、碰撞检查 | 轻量替代 | 本地 `OpenSpaceRoiDecider` + Adapter 输入转换 | P1：算法类已调用，但缺完整 Frame/OpenSpaceInfo 契约和部分状态更新。短期只补必要字段，不搬完整 Frame |
| `Frame::OpenSpaceCollisionCheck` | 在 ROI 阶段执行 open space 碰撞检查 | 轻量替代 | `ValidateVehicleNearParkingLot()`、`RunPathProviderPreCheck()`、障碍物边界 smoke | P1：当前是前置几何保护，不是完整原始碰撞检查 |
| `OpenSpacePathProvider::Process` | 区分 `PARKSTART -> PrePlan` 与非 PARKSTART 的 `PlanningOnPathThread` | 轻量替代 | `RunPathProvider()` + `PathProviderRuntimeState` | P0/P1：当前只有轻量 history/replan/warm_start。若要更接近原车，先补可观察状态和输入契约，再分步迁移大类 |
| `OpenSpacePathProvider::PreCheck` | 基于原车 config、low_fs obstacles、车辆多边形等执行完整前置检查 | 轻量替代 | `RunPathProviderPreCheck()` | P1：当前覆盖 ROI bounds、start/end、障碍物数量和局部边界；仍不是完整 PreCheck |
| `OpenSpaceThreadManager::PrePlan/TargetPlan` | 多 search thread + smooth thread，管理线程状态和耗时 | 缺失 | 当前同步调用 `OpenSpacePathGenerator` | P2：不建议近期直接接。会引入复杂线程状态和调试难度 |
| `OpenSpacePathGenerator::GenerateCoarsePath` | 分发 ILQR/Geometric/GeometryPathGenerator/HybridAStar 等粗路径搜索 | 部分接入 | 本地 `OpenSpacePathGenerator`、`HybridAStar` 等最小闭包 | P1：当前构建闭包以当前 MVP 能跑通为准，未证明全部算法分支都完整可用 |
| `PathGenerator::PathPartition` | 粗路径内部 gear 段切分 | 已接入/间接接入 | 本地 PATH_PROVIDER 输出 path/gear pairs | 继续通过 smoke 验证路径点和 gear |
| `OpenSpacePathSmoother::Smooth` | `XYRoadPreprocessor -> NlpSolver` 对粗路径做 NLP 平滑 | 缺失 | 当前没有 NLP smoother | P2：高风险大块。除非有 IPOPT/NLP 依赖和验证环境，否则只记录不接 |
| `OpenSpacePathPartition::Process` | 任务级路径仲裁、终点判定、HMI 状态收口、`CHOOSE_HISTORY_PATH/CHOOSE_NEW_PATH` 等 | 部分接入 | `RunPathPartition()` 调用本地 `OpenSpacePathPartition::Execute` | P1：当前已接任务级 path partition，但缺原始 Stage/HMI/Frame 状态全量语义 |
| `OpenSpaceSpeedOptimizer::Process` | `PathHandle -> ST 曲线采样 -> StSampleCost -> CombinePathAndSpeed`，并处理 interactive stage | 部分接入 | `RunSpeedOptimizer()` 调用本地 `OpenSpaceSpeedOptimizer::Execute` | P1：核心速度层已接，但缺完整原车 interactive/HMI 状态闭环 |
| `OpenSpaceStraightPathProvider::Process` | `DIRECT_FORWARD/DIRECT_BACKWARD` 跳过普通 ROI/PATH/PARTITION，生成直线路径或 stop path 后进速度层 | 缺失 | 当前无 direct 分支 | P0：这是独立且边界清楚的分支，建议作为近期最高价值功能差距之一 |
| Stage 发布轨迹 | 写 `publishable_trajectory_data`、`target_gear`、trajectory_type、parking_status | 轻量替代 | Adapter 输出 DDS `PlanningTrajectory` | P1：当前能发布轨迹，但没有完整 `parking_status/target_gear/trajectory_type` 语义 |
| `IsReadyToFinishStage` | 根据 `destination_reached`、车辆静止、sys_mode 决定 Stage 完成和切换 | 缺失 | 当前没有 mission finish 状态 | P0/P1：若要车端行为等价，需要补轻量 finish 判定和输出字段 |
| FunctionManager/PlanningContext | 原车跨模块状态来源与输出目的地 | 缺失 | 临时 IDL + Adapter RuntimeContext | P1：不建议直接搬 proto。应先做字段映射表，再扩展临时 IDL |
| 真实车端 Topic 契约 | 使用原车实际消息、单位、坐标系、状态语义 | 缺失 | 临时 simplified IDL | P1：后续真实联调前必须对齐 |
| m57 板端运行 | 原车环境可运行闭环 | 缺失 | 当前仅 m57 交叉编译/ELF/依赖检查 | P0 blocker：无板端时不能标记运行通过 |

## 5. 当前最可信的复现边界

可以说：

```text
当前 DDS MVP 已复现原始普通 open space 的核心算法链路：
ROI_DECIDER -> PATH_PROVIDER 核心粗路径 -> PATH_PARTITION -> SPEED_OPTIMIZER。
```

不能说：

```text
当前已完美复现 TempAPA_Code 的 ValetParkingStageParking 全流程。
```

原因是原始流程还包含：

- FunctionManager 输入到 parking_type 的映射。
- Pause/brake/finish 状态。
- `DIRECT_FORWARD/DIRECT_BACKWARD` 直行分支。
- `Frame/OpenSpaceInfo/PlanningContext` 全量语义。
- 完整 `OpenSpacePathProvider` 的 `PARKSTART/PrePlan/PlanningOnPathThread` 状态机。
- `OpenSpaceThreadManager` 多线程搜索和平滑收口。
- `OpenSpacePathSmoother/NLP`。
- Stage 发布时的 HMI/parking_status/target_gear/trajectory_type 收口。

## 6. 后续接入路线建议

### NEXT-033：保持原计划，补多车位 opt_parking_seq 验证

这个任务仍然值得先做，因为它直接验证当前 `SelectedSlot -> ParkingLot -> ROI/PATH` 的输入选择是否稳定。

目标：

```text
同一条 SelectedSlot 消息内包含多个 ParkingLot；
切换 opt_parking_seq；
确认 Adapter 选择对应 ParkingLot；
确认 PATH_PROVIDER 触发 TARGET_UPDATE；
目标稳定后 history=reused。
```

### NEXT-034：轻量 Stage 控制输入设计

新增文档或 IDL 草案，定义不搬完整 `FunctionManager` 的轻量控制输入：

```text
ParkingCommand:
  PARKING_IN
  PARKING_OUT_LEFT
  PARKING_OUT_RIGHT
  DIRECT_FORWARD
  DIRECT_BACKWARD
  PAUSE
  BRAKE
```

目标是让 Adapter 有能力表达原始 `SetParkingType` 和 `IsParkingBrakeCondition` 的最小语义。

### NEXT-035：接入 DIRECT_FORWARD / DIRECT_BACKWARD 直行分支

这是原始流程里最独立、最适合先补的缺口：

```text
DIRECT_* 不走普通 ROI/PATH_PROVIDER/PATH_PARTITION；
由轻量 straight path provider 生成直线路径或 stop path；
再进入 SPEED_OPTIMIZER 或临时速度输出。
```

注意：是否复制原始 `OpenSpaceStraightPathProvider`，要先看它依赖的 proto/frame/flags 能否轻量剥离。

### NEXT-036：Stage finish/pause/brake 行为等价 smoke

补 smoke 场景：

- pause 输入应输出 pause/stop 轨迹。
- brake 输入应清理或冻结规划状态。
- destination reached + standstill 应输出 mission finish 状态。

### NEXT-037 以后：完整 OpenSpacePathProvider 大类评估

只有在前面轻量状态和 direct 分支稳定后，再评估是否迁移完整大类：

- `OpenSpacePathProvider::PrePlan`
- `PlanningOnPathThread`
- `OpenSpaceThreadManager`
- `OpenSpacePathSmoother`
- NLP solver

这一步需要单独验收第三方依赖和线程行为，不能作为小修小补混入当前 Adapter。

## 7. 执行原则

后续不追求逐行复制原始工程。

采用原则：

```text
行为等价优先；
DDS 适配层结构保持干净；
大框架依赖逐步隔离；
每补一个原始流程节点，都必须有 smoke 或静态验证证据。
```

这样做比“整目录搬 TempAPA_Code”更稳，也更适合 MagnaDDS 中间件集成。
