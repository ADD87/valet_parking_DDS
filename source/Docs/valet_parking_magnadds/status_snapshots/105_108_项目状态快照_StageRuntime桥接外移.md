# 项目状态快照：BATCH-105_108 StageRuntime 桥接外移

日期：2026-07-31

## 当前状态

本批已完成 `BATCH-105_108`。

本批目标是继续对照原始 `ValetParkingStageParking` 流程减少架构差异：把 Adapter 中已经稳定的 Stage runtime 桥接逻辑外移到独立 lite helper。当前没有改变 ROI、PATH_PROVIDER、PATH_PARTITION、SPEED_OPTIMIZER、OPEN_SPACE_STRAIGHT_PATH 的算法行为。

## 本批代码变化

新增：

```text
applications/source/valet_parking/src/valet_parking_stage_runtime_lite.h
applications/source/valet_parking/src/valet_parking_stage_runtime_lite.cpp
```

迁移内容：

```text
BuildStageFacadeInputLite()
command -> StageFacadeBranchLite 判定
BuildStageRuntimeLifecycleContractLite()
AppendRuntimeLifecycleContract() overload
BuildOpenSpaceStageOutputContract() runtime bridge
BuildFallbackStageOutputContract() runtime bridge
BuildEarlyEstopFallbackContract() runtime bridge
```

Adapter 保留：

```text
DDS 输入读取和 PlanningTrajectory 输出
RuntimeContext 私有运行态持有
ROI / PathProvider / PathPartition / SpeedOptimizer / StraightPath 的算法输入构造
算法调用、fallback 分支编排和 PathProvider 私有运行态写回
path_history_available / has_last_speed_frame / direct_command_active 等简单状态桥接
```

Adapter 行数：

```text
before: about 3744
after : 3527
```

## 与原始流程的关系

本批继续收敛的是“Stage 骨架和血管”的代码归属：

```text
ValetParkingStageParking.Process
  -> StageFacadeInput / Frame lite / OpenSpaceInfo lite
  -> FunctionManager lite
  -> RuntimeLifecycle contract
  -> OpenSpace / Fallback / EarlyEstop Stage output
```

当前 `StageRuntime` 仍是 MVP 内的 lite/stub 桥接。它让现有 DDS smoke 能观察到原车命名的状态流，但还不是完整原车 `Frame`、`OpenSpaceInfo`、`PlanningContext`、`FunctionManager` 或 formal typed DDS 字段。

## 验证结果

通过：

```text
git diff --check
bash -n source/valet_parking_tools/build_valet_parking.sh
bash -n source/valet_parking_tools/smoke_valet_parking_x86.sh
bash -n source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh
bash source/valet_parking_tools/build_valet_parking.sh --out-dir /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_105_108
bash source/valet_parking_tools/smoke_valet_parking_x86.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_105_108/valet_parking_mvp/x86 --domain-id 224 --timeout-ms 25000
bash source/valet_parking_tools/smoke_valet_parking_x86.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_105_108/valet_parking_mvp/x86 --domain-id 225 --command-mode direct-forward --timeout-ms 25000
bash source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_105_108/valet_parking_mvp/x86 --first-domain-id 189 --timeout-ms 25000
```

结果：

```text
x86 normal open-space smoke: PASS, domain_224_20260731_043556_673
x86 direct-forward smoke: PASS, domain_225_20260731_043556_671
batch smoke first-domain-id=189, last-domain=223: PASS
x86 libvalet_parking.so: PASS
m57 libvalet_parking.so: PASS_STATIC_BUILD
m57 board runtime: NOT VERIFIED
```

产物：

```text
out/valet_parking_flow_gap_105_108/valet_parking_mvp/x86/lib/libvalet_parking.so
out/valet_parking_flow_gap_105_108/valet_parking_mvp/m57/lib/libvalet_parking.so
```

本环境在 `domain-id >= 231` 时可能出现 `failed to create DomainParticipant`，所以 batch 必须保证最后一个 domain 不超过 230。

## 剩余差异

- Adapter 仍承载算法输入构造、fallback 分支编排和 runtime 写回，还不是只含 DDS 壳的薄文件。
- `StageRuntime`、`FunctionManagerProjection`、`StageFinishEvaluation`、`OpenSpaceInfo`、`MissionState` 仍通过 `replan_reason` / `estop.reason` 文本诊断承载，不是 formal typed DDS 字段。
- 完整 `Frame/OpenSpaceInfo/PlanningContext/FunctionManager/HMI/collision/wheel mask/NLP smoother` 仍未完整接入。
- m57 只完成交叉编译和 ELF/依赖检查，尚未做板端 runtime。

## 下一步

下一批进入 `BATCH-109_112`：批量扫描算法输入构造与运行态写回边界，优先评估 `BuildPathPartitionInput`、`BuildSpeedOptimizerInput`、`BuildOpenSpaceStraightPathInput`、`NormalizePathProviderPathSet`、`UpdateAfterPartitionFallback`、`UpdateAfterSpeedOptimizer` 是否可以拆成独立 task-runtime helper。节奏保持“先批量检测和批量修订，再统一 build/smoke，再集中排错”。
