# 项目状态快照：BATCH-109_112 TaskRuntime 输入构造外移

日期：2026-07-31

状态：`PASS_BATCH_109_112_TASK_RUNTIME_HELPER_EXTERNALIZATION_X86_SMOKE_M57_STATIC_BLOCKED_BOARD`

## 本批目标

继续对照原始 `ValetParkingStageParking` 流程减少 Adapter 的结构差异。本批聚焦“任务输入血管”：车辆状态、障碍物、路径归一化、PathPartition 输入、SpeedOptimizer 输入、OpenSpaceStraightPath 输入和 direct speed profile。

不做：

- 不扩展 formal typed DDS 字段。
- 不接完整 `Frame/DependencyInjector`。
- 不搬动 `compile/` 或 `thirdparty/`。
- 不宣称 m57 板端 runtime 通过。

## 代码变化

新增：

```text
applications/source/valet_parking/src/valet_parking_task_runtime_lite.h
applications/source/valet_parking/src/valet_parking_task_runtime_lite.cpp
```

外移内容：

```text
RuntimeVehicleInput
RuntimeObstacleInput
InputMetadata
DirectSpeedOptimizerProfile
BuildVehicleState()
BuildStartPathPoint()
BuildStaticObstacles()
BuildMovingObstacles()
BuildObstacleSegments()
MapPathProviderPathType()
NormalizeDiscretizedPath()
NormalizePathProviderPathSet()
BuildPathPartitionInput()
BuildSpeedOptimizerInput()
SelectDirectDistance()
SelectDirectSpeed()
BuildDirectSpeedOptimizerProfile()
BuildDirectSpeedOptimizerConfig()
TargetGearForDirectCommand()
BuildOpenSpaceStraightPathInput()
BuildPartitionOutputFromStraightPath()
```

更新：

```text
applications/source/valet_parking/CMakeLists.txt
applications/source/valet_parking/src/valet_parking_stage_parking_adapter.cpp
```

Adapter 行数：

```text
before: ~3527
after : ~3397
```

## 行为边界

本批是结构拆分，不改变当前算法行为。主链仍是：

```text
SelectedSlot -> ROI_DECIDER -> PATH_PROVIDER -> PATH_PARTITION -> SPEED_OPTIMIZER -> PlanningTrajectory
```

direct 分支仍是：

```text
ParkingCommand -> OPEN_SPACE_STRAIGHT_PATH -> SPEED_OPTIMIZER -> PlanningTrajectory
```

## 验证

已通过：

```text
git diff --check
bash -n source/valet_parking_tools/build_valet_parking.sh
bash -n source/valet_parking_tools/smoke_valet_parking_x86.sh
bash -n source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh
bash source/valet_parking_tools/build_valet_parking.sh --out-dir /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_109_112
bash source/valet_parking_tools/build_valet_parking.sh --platform m57 --jobs 8 --out-dir /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_109_112
bash source/valet_parking_tools/smoke_valet_parking_x86.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_109_112/valet_parking_mvp/x86 --domain-id 224 --timeout-ms 25000
bash source/valet_parking_tools/smoke_valet_parking_x86.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_109_112/valet_parking_mvp/x86 --domain-id 225 --command-mode direct-forward --timeout-ms 25000
bash source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_109_112/valet_parking_mvp/x86 --first-domain-id 189 --timeout-ms 25000
```

产物：

```text
out/valet_parking_flow_gap_109_112/valet_parking_mvp/x86/lib/libvalet_parking.so
out/valet_parking_flow_gap_109_112/valet_parking_mvp/m57/lib/libvalet_parking.so
```

Smoke 证据：

```text
out/valet_parking_flow_gap_109_112/valet_parking_mvp/x86/smoke_latest/domain_224_20260731_075530_639
out/valet_parking_flow_gap_109_112/valet_parking_mvp/x86/smoke_latest/domain_225_20260731_075551_896
out/valet_parking_flow_gap_109_112/valet_parking_mvp/x86/smoke_latest/domain_189_20260731_080156_762
out/valet_parking_flow_gap_109_112/valet_parking_mvp/x86/smoke_latest/domain_223_20260731_080707_5164
```

关键结果：

```text
x86 normal open-space smoke PASS
x86 direct-forward smoke PASS
batch_042_046 first-domain-id=189, last-domain=223, all smoke cases passed
x86 libvalet_parking.so PASS
m57 libvalet_parking.so PASS_STATIC_BUILD
m57 board runtime NOT_VERIFIED
```

## 已减少的差异

- PathPartition、SpeedOptimizer、OpenSpaceStraightPath 的输入构造从 Adapter 外移。
- 简化车辆/障碍物输入 DTO 从 Adapter 外移。
- 路径归一化和 direct speed profile 从 Adapter 外移。
- Adapter 更接近 DDS/runtime + task orchestration 壳，而不是所有 task 输入细节的堆叠点。

## 仍保留的差异

- `BuildPathProviderInput`、PathProvider warm_start/trace_adjust 策略切片、`PathProviderRuntimeState` threaded provider 所有权仍在 Adapter。
- `UpdateAfterPartitionFallback()` 和 `UpdateAfterSpeedOptimizer()` 仍是 `RuntimeContext` 私有写回方法。
- `Frame/OpenSpaceInfo/PlanningContext/FunctionManager/HMI/collision/wheel mask/NLP smoother` 仍未完整接入。
- formal typed DDS 字段仍未替代 `replan_reason` / `estop.reason` 文本诊断。
- m57 仍只完成交叉编译和 ELF/依赖检查，未完成板端 runtime 闭环。

## 下一批

`BATCH-113_116`：

- 批量扫描 `BuildPathProviderInput`、warm_start/trace_adjust 策略切片和 `PathProviderRuntimeState` 的可外移边界。
- 评估 `UpdateAfterPartitionFallback()` / `UpdateAfterSpeedOptimizer()` 是否可抽成 runtime writeback helper。
- 只外移稳定 helper；如果 threaded provider 所有权风险较高，先抽 DTO/纯函数，不强搬线程拥有者。
- 继续统一 build/smoke，batch smoke 仍使用 first-domain-id=189，避免 domain-id >= 231。
