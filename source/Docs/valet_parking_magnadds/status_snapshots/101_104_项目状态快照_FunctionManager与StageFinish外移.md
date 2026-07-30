# 项目状态快照：BATCH-101_104 FunctionManager 与 StageFinish 外移

日期：2026-07-30

## 当前状态

本批已完成 `BATCH-101_104`。

本批目标是继续对照原始 `ValetParkingStageParking` 流程减少架构差异：把 Adapter 中已经稳定的 FunctionManager 投影、direct finish/release 诊断和 StageFinish 连续帧状态机外移到独立 lite helper。当前没有改变 ROI、PATH_PROVIDER、PATH_PARTITION、SPEED_OPTIMIZER、OPEN_SPACE_STRAIGHT_PATH 的算法行为。

## 本批代码变化

新增：

```text
applications/source/valet_parking/src/valet_parking_function_manager_lite.h
applications/source/valet_parking/src/valet_parking_function_manager_lite.cpp
applications/source/valet_parking/src/valet_parking_stage_finish_lite.h
applications/source/valet_parking/src/valet_parking_stage_finish_lite.cpp
```

迁移内容：

```text
DirectFinishEvaluation
FunctionManagerProjection
ParkingCommandModeToString()
IsDirectCommandMode()
DirectReleasedActionName()
BuildDirectFinishEvaluation()
AppendDirectFinishContract()
ProjectSysCommandName()
ProjectSysRunStateName()
ProjectSysWarningInfoName()
ProjectParkingTypeName()
BuildFunctionManagerProjection()
BuildReleasedDirectFunctionProjection()
BuildStageFinishHoldFunctionProjection()
BuildFunctionManagerProjectionContractLite()
BuildStageFunctionManagerLite()
StageFinishRuntimeState
StageFinishEvaluation
UpdateStageFinishEvaluation()
BuildStageFinishEvaluationLite()
BuildStageFinishOutputContractLite()
StageTrajectoryType()
```

Adapter 保留：

```text
DDS 输入读取和 PlanningTrajectory 输出
RuntimeVehicleInput / RuntimeObstacleInput / PathProviderRuntimeState 等私有运行态
ROI / PathProvider / PathPartition / SpeedOptimizer / StraightPath 的算法输入构造
算法调用、fallback 编排和运行态写回
私有状态 -> lite helper 的薄参数桥接
```

Adapter 行数：

```text
before: about 4080
after : 3744
```

## 与原始流程的关系

本批继续收敛的是“Stage 骨架和血管”的代码归属：

```text
ValetParkingStageParking.Process
  -> SetParkingType / FunctionManager projection
  -> ExecuteTaskOnOpenSpace
  -> IsReadyToFinishStage
  -> FinishScenario projection
```

当前 `FunctionManagerProjection` 与 `StageFinishEvaluation` 仍是 MVP 内的 lite/stub 契约。它们让现有 DDS smoke 能观察到原车命名的状态流，但还不是完整原车 `FunctionManagerIn/Out`、`Frame/OpenSpaceInfo`、`PlanningContext` 或 formal typed DDS 字段。

## 验证结果

通过：

```text
git diff --check
bash -n source/valet_parking_tools/build_valet_parking.sh
bash -n source/valet_parking_tools/smoke_valet_parking_x86.sh
bash -n source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh
bash source/valet_parking_tools/build_valet_parking.sh --out-dir /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_101_104
bash source/valet_parking_tools/smoke_valet_parking_x86.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_101_104/valet_parking_mvp/x86 --domain-id 224 --timeout-ms 25000
bash source/valet_parking_tools/smoke_valet_parking_x86.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_101_104/valet_parking_mvp/x86 --domain-id 225 --command-mode direct-forward --timeout-ms 25000
bash source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_101_104/valet_parking_mvp/x86 --first-domain-id 189 --timeout-ms 25000
```

结果：

```text
x86 normal open-space smoke: PASS, domain_224_20260730_234231_639
x86 direct-forward smoke: PASS, domain_225_20260730_234231_647
batch smoke first-domain-id=189, last-domain=223: PASS
x86 libvalet_parking.so: PASS
m57 libvalet_parking.so: PASS_STATIC_BUILD
m57 board runtime: NOT VERIFIED
```

产物：

```text
out/valet_parking_flow_gap_101_104/valet_parking_mvp/x86/lib/libvalet_parking.so
out/valet_parking_flow_gap_101_104/valet_parking_mvp/m57/lib/libvalet_parking.so
```

本环境在 `domain-id >= 231` 时可能出现 `failed to create DomainParticipant`，所以 batch 必须保证最后一个 domain 不超过 230。

## 剩余差异

- Adapter 仍承载算法输入构造、fallback 编排和 runtime 写回，还不是只含 DDS 壳的薄文件。
- `FunctionManagerProjection`、`StageFinishEvaluation`、`OpenSpaceInfo`、`MissionState` 仍通过 `replan_reason` / `estop.reason` 文本诊断承载，不是 formal typed DDS 字段。
- 完整 `Frame/OpenSpaceInfo/PlanningContext/FunctionManager/HMI/collision/wheel mask/NLP smoother` 仍未完整接入。
- m57 只完成交叉编译和 ELF/依赖检查，尚未做板端 runtime。

## 下一步

下一批进入 `BATCH-105_108`：继续批量扫描 Adapter 中仍混合的算法输入构造、fallback 编排和 runtime 写回边界，优先把不依赖 MagnaDDS reader/writer 的稳定流程桥接逻辑外移到 Stage/task helper。节奏保持“先批量检测和批量修订，再统一 build/smoke，再集中排错”。
