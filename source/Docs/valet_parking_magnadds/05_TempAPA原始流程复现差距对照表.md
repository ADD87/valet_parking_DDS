# TempAPA 原始泊车流程复现差距对照表

- 文档编号：DOC-005
- 日期：2026-07-28
- 触发原因：用户询问当前 `ValetParkingStageParkingAdapter::Process()` 能否完美复现原始 `E:\APA\DDS\TempAPA_Code` 的泊车入位流程。
- 当前结论：当前 DDS 适配已复现普通 open space 主链路骨架，但不能声称完美复现原始 Stage/Frame/OpenSpacePathProvider 全量行为。

## 1. 参考源

| 类型 | 路径 |
|---|---|
| 当前主对照流程说明 | `E:\APA\DDS\parking_algorithm_standalone\docs\00_ValetParkingStageParking_超详细流程图总览_重要.md` |
| 原始流程说明备份 | `E:\APA\DDS\TempAPA_Code\00_ValetParkingStageParking_超详细流程图总览_重要.md` |
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

两者主链路方向一致。NEXT-040 后，当前 DDS Adapter 已接入本地化 `OpenSpacePathProvider / OpenSpaceThreadManager` 线程骨架。BATCH-049_052 后，普通主链路已新增轻量 `STAGE_OUTPUT` 收口，把 `PathPartition` 的 `path_decision/finish_status/destination_reached`、`SpeedOptimizer` 的 `interactive_stage` 和最终 `target_gear/trajectory_type/parking_status` 写入可回归诊断契约。BATCH-053_056 后，`IsReadyToFinishStage` 已从单帧诊断升级为连续帧轻量状态机，`FunctionManager` 已从完全缺失升级为可观察投影契约，ROI/PreCheck 也补充了 Frame/OpenSpaceInfo 最小字段和 wheel mask 缺口诊断。BATCH-057_060 后，direct 分支新增命令失活完成条件诊断，ROI/PathProvider/PathPartition/SpeedOptimizer 补齐 `OpenSpaceInfo` 最小读写点契约，collision/wheel mask 的当前边界也进入成功和负向 smoke 断言。当前仍省略或轻量替代原始工程的完整 Stage 状态机、完整 FunctionManager、完整 Frame/OpenSpaceInfo、完整 collision/wheel mask 和 NLP 平滑器。

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
| `IsParkingBrakeCondition` | `STRAIGHTBRAKE` 或 `PAUSE` 时生成 pause trajectory，并按 warning 区分等待障碍物 | 部分接入 | 临时 `ParkingCommand` 的 PAUSE/BRAKE 分支 + `BuildStageControlStopTrajectory()` | P0/P1：NEXT-037 已能用临时 DDS command 表达 pause/brake 并输出非 estop stop trajectory；仍缺原车 warning 区分和完整 `IsParkingBrakeCondition` 判定链 |
| `SetParkingType` | 将 `FunctionManagerIn.sys_command` 映射为 `PARKING_IN/PARKING_OUT/DIRECT_FORWARD/DIRECT_BACKWARD/...` | 部分接入 | 临时 `ParkingCommand` IDL + Adapter command 分支 + `FunctionManagerProjection` | P1：已能表达 PARKING_IN、DIRECT_*、PAUSE/BRAKE/FINISH 和 unsupported parking-out；BATCH-053_056 后会在日志中输出 `function_manager_sys_command/sys_run_state/sys_warning_info/parking_type` 投影，BATCH-057_060 后 smoke 覆盖 unsupported/default/NOSTATE/invalid clear 负向路径；仍不等同原车 FunctionManager 全量字段 |
| `Stage::ExecuteTaskOnOpenSpace` | 遍历 task_list，按 `DIRECT_*` 跳过普通 ROI/PATH/PARTITION，执行 straight path | 部分接入 | Adapter 手写固定调用顺序 | P1：普通链和 direct 分支已分流，但仍没有原车 task_list/StageStatus/Frame 回写 |
| 普通 task 顺序 | `ROI_DECIDER -> PATH_PROVIDER -> PATH_PARTITION -> SPEED_OPTIMIZER` | 已接入 | `Process()` 中 ROI、`RunPathProvider()`、`RunPathPartition()`、`RunSpeedOptimizer()` | 已覆盖主链路骨架，继续保持 x86/m57 回归 |
| `OpenSpaceRoiDecider::Process` | 读取 Frame/OpenSpaceInfo/parking spots，更新 ROI、目标、障碍物、碰撞检查 | 轻量替代 | 本地 `OpenSpaceRoiDecider` + Adapter 输入转换 | P1：算法类已调用，但缺完整 Frame/OpenSpaceInfo 契约和部分状态更新。BATCH-053_056 后 `ROI_DECIDER ok` 会输出 `fine_tuned/slot_inner_fs_valid/scenario_difficulty/roi_wall_segments/virtual_obs_segments` 等最小契约字段；BATCH-057_060 后继续输出 `open_space_path_info_id/xy_bounds_size/dest_region_points/dest_region_area/dest_region_angle`，便于和原始 ROI 建模过程对照；短期仍不搬完整 Frame |
| `Frame::OpenSpaceCollisionCheck` | 在 ROI 阶段执行 open space 碰撞检查 | 轻量替代 | `ValidateVehicleNearParkingLot()`、`RunPathProviderPreCheck()`、障碍物边界 smoke | P1：当前是前置几何保护，不是完整原始碰撞检查。BATCH-057_060 后成功和失败路径都显式输出 `collision_contract=geometry_precheck_only` 与 `wheel_mask_contract=not_exposed_in_current_mvp`，并在 far-obstacles/many-obstacles 负向 smoke 中断言该边界 |
| `OpenSpacePathProvider::Process` | 区分 `PARKSTART -> PrePlan` 与非 PARKSTART 的 `PlanningOnPathThread` | 部分接入 | 本地 `OpenSpacePathProvider` + `OpenSpaceThreadManager` + `RunPathProvider()` | P1：已接入 TargetPlan 线程路径生成、多车位 PrePlan 候选、history/replan/warm_start 诊断；仍不是原车 PathOptimizer/Frame/OpenSpaceInfo 全量状态机 |
| `OpenSpacePathProvider::PreCheck` | 基于原车 config、low_fs obstacles、车辆多边形等执行完整前置检查 | 轻量替代 | `RunPathProviderPreCheck()` | P1：当前覆盖 ROI bounds、start/end、障碍物数量、局部边界、ROI/external segment 计数、near start/end 线段和车辆状态可用性；BATCH-057_060 后成功和失败 reason 都带 collision/wheel-mask 契约；仍不是完整 PreCheck，尤其缺低矮空间、完整车辆多边形和 wheel mask |
| `OpenSpaceThreadManager::PrePlan/TargetPlan` | 多 search thread + smooth thread，管理线程状态和耗时 | 部分接入 | 本地 `planning/open_space/open_space_thread/open_space_thread_manager.*` | P1：已接入 search threads 预热非选中车位、target thread 生成当前车位，并有 timeout/target_source/thread_path_ids 诊断；smooth thread 目前没有接 NLP 平滑 |
| `OpenSpacePathGenerator::GenerateCoarsePath` | 分发 ILQR/Geometric/GeometryPathGenerator/HybridAStar 等粗路径搜索 | 部分接入 | 本地 `OpenSpacePathGenerator`、`HybridAStar` 等最小闭包 | P1：当前构建闭包以当前 MVP 能跑通为准，未证明全部算法分支都完整可用 |
| `PathGenerator::PathPartition` | 粗路径内部 gear 段切分 | 已接入/间接接入 | 本地 PATH_PROVIDER 输出 path/gear pairs | 继续通过 smoke 验证路径点和 gear |
| `OpenSpacePathSmoother::Smooth` | `XYRoadPreprocessor -> NlpSolver` 对粗路径做 NLP 平滑 | 缺失 | 当前没有 NLP smoother | P2：高风险大块。除非有 IPOPT/NLP 依赖和验证环境，否则只记录不接 |
| `OpenSpacePathPartition::Process` | 任务级路径仲裁、终点判定、HMI 状态收口、`CHOOSE_HISTORY_PATH/CHOOSE_NEW_PATH` 等 | 部分接入 | `RunPathPartition()` 调用本地 `OpenSpacePathPartition::Execute` | P1：当前已接任务级 path partition；BATCH-049_052 已把 `decision_name/finish_name/destination_reached` 纳入 smoke 契约；BATCH-057_060 后继续输出 `chosen_path_contract/chosen_path_points/chosen_path_gear/stop_path`，对齐原始 `OpenSpaceInfo` 中 chosen path 的读写点；仍缺原始 Stage/HMI/Frame 状态全量语义 |
| `OpenSpaceSpeedOptimizer::Process` | `PathHandle -> ST 曲线采样 -> StSampleCost -> CombinePathAndSpeed`，并处理 interactive stage | 部分接入 | `RunSpeedOptimizer()` 调用本地 `OpenSpaceSpeedOptimizer::Execute` | P1：核心速度层已接；BATCH-049_052 已把 `stage_name` 与 `STAGE_OUTPUT parking_status` 翻译纳入 smoke 契约；BATCH-057_060 后继续输出 `chosen_path_points/partitioned_paths/stop_path/speed_optimizer_trajectory_points/wheel_mask_considered=false`；仍缺完整原车 HMI 状态闭环 |
| `OpenSpaceStraightPathProvider::Process` | `DIRECT_FORWARD/DIRECT_BACKWARD` 跳过普通 ROI/PATH/PARTITION，生成直线路径或 stop path 后进速度层 | 已接入 | 本地 `OpenSpaceStraightPathProvider` + Adapter direct 分支 | 已覆盖 direct forward/backward、挡位保护、速度方向冲突和 speed bound；BATCH-057_060 后补 direct 命令失活 + standstill 的完成条件诊断；仍用临时 `ParkingCommand` 表达原车命令，且还没有真实“命令失活后 finish_ready=true”输入闭环 |
| Stage 发布轨迹 | 写 `publishable_trajectory_data`、`target_gear`、trajectory_type、parking_status | 部分接入 | Adapter 输出 DDS `PlanningTrajectory` + `STAGE_OUTPUT` 诊断 | P1：BATCH-049_052 已补普通主链的轻量 Stage 输出收口；`parking_status/target_gear/trajectory_type` 仍在 `replan_reason/estop.reason` 文本里，不是正式 DDS 字段 |
| `IsReadyToFinishStage` | 根据 `destination_reached`、车辆静止、sys_mode 决定 Stage 完成和切换 | 轻量替代 | `StageFinishRuntimeState` + `UpdateStageFinishEvaluation()` + `STAGE_OUTPUT` | P0/P1：BATCH-053_056 后已补 `destination_reached && vehicle_standstill` 连续 2 帧状态机，输出 `finish_ready/finish_consecutive_frames/vehicle_standstill/stage_finish_state`；仍未接 sys_mode/next_stage/FinishScenario 的完整切换行为 |
| FunctionManager/PlanningContext | 原车跨模块状态来源与输出目的地 | 轻量替代 | 临时 IDL + Adapter RuntimeContext + `FunctionManagerProjection` | P1：BATCH-053_056 后已建立从当前 `ParkingCommand`/SelectedSlot 到 `sys_mode/sys_command/sys_run_state/sys_warning_info/parking_type` 的投影表，并进入 smoke 断言；仍不建议直接搬 proto，后续若扩展正式 IDL 应以该映射为草案 |
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
- `Frame/OpenSpaceInfo/PlanningContext` 全量语义。
- 完整 `OpenSpacePathProvider` 的 `PARKSTART/PrePlan/PlanningOnPathThread` 原车状态回写。
- `OpenSpaceThreadManager` 的 NLP 平滑收口。
- `OpenSpacePathSmoother/NLP`。
- Stage 发布时的 HMI/parking_status/target_gear/trajectory_type 收口。

BATCH-049_052 后，最后一项已经从“缺少收口”降级为“轻量文本契约收口”：普通主链会输出 `STAGE_OUTPUT open_space`，其中包含 `path_decision`、`finish_status`、`destination_reached`、`target_gear`、`trajectory_type`、`parking_status` 和 `finish_priority=finish_over_interactive`。

BATCH-053_056 后，`IsReadyToFinishStage` 和 `FunctionManager` 也从“缺失/单帧粗略诊断”降级为“轻量文本契约收口”：普通主链会继续输出 `finish_condition=destination_reached_and_standstill`、`finish_ready`、`finish_consecutive_frames`、`stage_finish_state`，并输出 `function_manager_sys_mode/sys_command/sys_run_state/sys_warning_info/parking_type`。这些字段仍在 `replan_reason/estop.reason` 文本里，不是正式 DDS 字段，所以真实车端协议未对齐前不能称为全量等价。

BATCH-057_060 后，`Frame/OpenSpaceInfo` 读写点从“只知道 ROI/PreCheck 的部分字段”继续降级为“ROI、PathProvider、PathPartition、SpeedOptimizer 均有最小文本契约”：可以看到 `open_space_path_info_id/path_info_id`、`dest_region`、`chosen_path`、`stop_path` 和速度轨迹点数。direct 分支也补了 `finish_condition=direct_command_inactive_and_standstill` 诊断。注意：这仍是可观察边界，不是完整 Frame/OpenSpaceInfo 对象迁移。

## 6. BATCH-049_052 差异收敛记录

本批次按原始流程图的“对外状态翻译与覆写链”做了收敛：

```text
Stage::ExecuteTaskOnOpenSpace
-> publishable_trajectory_data
-> frame.target_gear
-> speed_task_interactive_stage -> parking_status
-> IsReadyToFinishStage finish 覆写
```

当前 DDS Adapter 的对应实现：

```text
RunPathPartition()
  -> decision_name / finish_name / destination_reached
RunSpeedOptimizer()
  -> stage_name
BuildOpenSpaceStageOutputContract()
  -> STAGE_OUTPUT open_space
  -> stage_status / target_gear / trajectory_type / parking_status
  -> finish_priority=finish_over_interactive
```

验证证据：

```text
bash applications/source/valet_parking_tools/build_valet_parking.sh --out-dir out/valet_parking_flow_gap_049_052
bash applications/source/valet_parking_tools/build_valet_parking.sh --platform m57 --out-dir out/valet_parking_flow_gap_049_052
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh --run-root out/valet_parking_flow_gap_049_052/valet_parking_mvp/x86 --domain-id 190 --count 3 --interval-ms 500 --expect-thread-provider-stop
bash applications/source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh --run-root out/valet_parking_flow_gap_049_052/valet_parking_mvp/x86 --first-domain-id 191 --timeout-ms 25000
```

本批次减少的差异：

- `Stage 发布轨迹`：从“只有轨迹输出”提升为“轨迹输出 + target_gear/trajectory_type/parking_status 轻量收口”。
- `OpenSpacePathPartition::Process`：从只记录数字状态，提升为同时记录 `decision_name/finish_name/destination_reached`。
- `OpenSpaceSpeedOptimizer::Process`：从只记录 `stage` 数字，提升为记录 `stage_name` 并进入 Stage 状态翻译。
- `IsReadyToFinishStage`：从完全缺失，提升为基于 `destination_reached/finish_status` 的轻量 finish 覆写诊断。

## 7. BATCH-053_056 差异收敛记录

本批次按原始流程图的三段关键差异做收敛：

```text
[I] Stage 收口 finish 判定
[A3] FunctionManager/sys_command -> parking_type 判型
[C/D] ROI -> OpenSpaceInfo -> PathProvider PreCheck 输入契约
```

当前 DDS Adapter 的对应实现：

```text
StageFinishRuntimeState
  -> destination_reached && vehicle_standstill 连续 2 帧
  -> finish_ready / stage_finish_state

FunctionManagerProjection
  -> selected_slot/ParkingCommand
  -> sys_mode / sys_command / sys_run_state / sys_warning_info / parking_type

BuildRoiReason() / BuildPathProviderPreCheckReason()
  -> ROI fine_tuned / slot_inner_fs_valid / scenario_difficulty
  -> ROI/external obstacle segment counts
  -> near_start_segments / near_end_segments
  -> vehicle_has_state
  -> wheel_mask_contract=not_exposed_in_current_mvp
```

验证证据：

```text
bash source/valet_parking_tools/build_valet_parking.sh --out-dir /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_053_056
bash source/valet_parking_tools/smoke_valet_parking_x86.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_053_056/valet_parking_mvp/x86 --domain-id 207 --count 3 --interval-ms 500 --expect-thread-provider-stop
bash source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_053_056/valet_parking_mvp/x86 --first-domain-id 211 --timeout-ms 25000
```

本批次减少的差异：

- `IsReadyToFinishStage`：从“根据结果文本粗略判断”提升为“连续帧 `destination_reached + standstill` 状态机”，能观察 `READY/HOLDING/WAITING`。
- `FunctionManager/SetParkingType`：从“只靠临时 command 分支”提升为“输出原车字段命名的投影契约”，能对照 `sys_command/sys_run_state/sys_warning_info/parking_type`。
- `Frame/OpenSpaceInfo`：从“只知道 ROI/PATH_PROVIDER 成功”提升为“输出 ROI 建模和 PreCheck 最小字段”，方便继续补缺失字段。
- `PreCheck/collision/wheel mask`：从“只做局部几何保护”提升为“同时暴露 ROI/external/near-start/near-end 线段计数和 wheel mask 未暴露事实”，避免误判为完整原车碰撞检查。

仍保留的差异：

- `finish_ready=true` 仍只是当前 Adapter 输出状态，不会真正设置 `next_stage_` 或调用 `FinishScenario()`。
- `FunctionManagerProjection` 是投影，不是真实 `FunctionManagerIn/Out` proto，也不是正式 DDS 字段。
- wheel mask 当前只记录 `not_exposed_in_current_mvp`，没有接入真实轮挡语义。
- 完整 `Frame/OpenSpaceInfo/PlanningContext` 仍未搬入。
- `OpenSpacePathSmoother/NLP` 仍未接入。

## 8. BATCH-057_060 差异收敛记录

本批次继续按原始流程图减少四类差异：

```text
NEXT-057：direct 命令失活 + standstill 的 finish 诊断
NEXT-058：FunctionManager unsupported/default/NOSTATE 负向路径 smoke
NEXT-059：Frame/OpenSpaceInfo 的 path_info_id、dest_region、chosen path、stop path 读写点显式化
NEXT-060：collision/wheel mask 可观测契约和负向样本
```

当前 DDS Adapter 的对应实现：

```text
DirectFinishEvaluation
  -> finish_condition=direct_command_inactive_and_standstill
  -> direct_command_active / direct_command_inactive
  -> direct_vehicle_standstill
  -> direct_stage_finish_state

ROI_DECIDER ok
  -> open_space_info_contract=roi_output
  -> open_space_path_info_id
  -> dest_region_points / dest_region_area / dest_region_angle

PATH_PROVIDER ok
  -> open_space_info_contract=path_provider_output
  -> path_info_id
  -> dest_region_points / dest_region_area / dest_region_angle

PATH_PARTITION ok
  -> open_space_info_contract=path_partition_output
  -> chosen_path_contract=chosen_partitioned_path
  -> chosen_path_points / chosen_path_gear / stop_path

SPEED_OPTIMIZER ok
  -> open_space_info_contract=speed_optimizer_output
  -> chosen_path_points / partitioned_paths / stop_path
  -> speed_optimizer_trajectory_points
  -> wheel_mask_considered=false
```

验证证据：

```text
bash source/valet_parking_tools/build_valet_parking.sh --out-dir /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_057_060
bash source/valet_parking_tools/smoke_valet_parking_x86.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_057_060/valet_parking_mvp/x86 --domain-id 217 --count 3 --interval-ms 500 --expect-thread-provider-stop
bash source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_057_060/valet_parking_mvp/x86 --first-domain-id 180 --timeout-ms 25000
```

本批次减少的差异：

- `DIRECT_FORWARD/DIRECT_BACKWARD`：从只有直接控制输出，提升为同时暴露 direct finish 条件与状态名。
- `Frame/OpenSpaceInfo`：从 ROI/PreCheck 最小字段，提升为 ROI、PathProvider、PathPartition、SpeedOptimizer 四段均可观察关键读写点。
- `chosen path`：从 PathPartition 内部结果，提升为 smoke 可断言的 `chosen_path_points/chosen_path_gear`。
- `collision/wheel mask`：从仅记录缺口，提升为成功和负向 PreCheck 都断言当前边界，避免误报为完整原车碰撞检查。

仍保留的差异：

- direct 分支当前还没有真实“命令失活后 direct_finish_ready=true”的输入闭环，只能先记录命令仍 active 时的 WAITING 边界。
- `OpenSpaceInfo` 仍是文本契约，不是正式 Frame 对象或 DDS 字段。
- collision 仍是几何前置保护，不是完整 `Frame::OpenSpaceCollisionCheck`。
- wheel mask 仍未接入真实轮挡语义。

## 9. 后续接入路线建议

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

### NEXT-057 以后：继续围绕原始流程差异收敛

后续主线应继续围绕 `00_ValetParkingStageParking_超详细流程图总览_重要.md` 减少差异，而不是转去外围问题：

- 继续补 `IsReadyToFinishStage` 的 direct 命令失活 + standstill 细节。
- 扩充 `FunctionManager/sys_mode/sys_command/sys_run_state/sys_warning_info` 投影的 unsupported/default/NOSTATE 负向 smoke。
- 对 `Frame/OpenSpaceInfo` 的 path_info_id、dest_region、chosen path、stop path 读写点做最小字段契约，而不是整套框架一次性搬入。
- 继续收敛 collision/wheel mask：先做可观测契约和负向样本，暂不声称完整原车碰撞检查。

### 历史路线：完整 OpenSpacePathProvider 大类评估

只有在前面轻量状态和 direct 分支稳定后，再评估是否迁移完整大类：

- `OpenSpacePathProvider::PrePlan`
- `PlanningOnPathThread`
- `OpenSpaceThreadManager`
- `OpenSpacePathSmoother`
- NLP solver

这一步需要单独验收第三方依赖和线程行为，不能作为小修小补混入当前 Adapter。

## 10. 执行原则

后续不追求逐行复制原始工程。

采用原则：

```text
行为等价优先；
DDS 适配层结构保持干净；
大框架依赖逐步隔离；
每补一个原始流程节点，都必须有 smoke 或静态验证证据。
```

这样做比“整目录搬 TempAPA_Code”更稳，也更适合 MagnaDDS 中间件集成。
