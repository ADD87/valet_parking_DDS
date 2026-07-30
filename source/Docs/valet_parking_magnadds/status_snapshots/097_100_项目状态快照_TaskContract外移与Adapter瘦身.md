# 项目状态快照：BATCH-097_100 TaskContract 外移与 Adapter 瘦身

日期：2026-07-30

## 当前状态

本批已完成 `BATCH-097_100`。

本批目标是继续对照原始 `ValetParkingStageParking` 流程，减少 Adapter 同时承担 DDS 壳、算法调用和 task-level 文本契约三种职责的差异。没有改变当前 ROI、PATH_PROVIDER、PATH_PARTITION、SPEED_OPTIMIZER、OPEN_SPACE_STRAIGHT_PATH 的算法主链。

## 本批代码变化

新增：

```text
applications/source/valet_parking/src/valet_parking_task_contract_lite.h
applications/source/valet_parking/src/valet_parking_task_contract_lite.cpp
```

迁移内容：

```text
PathProviderPreCheckResult
PathProviderAttemptContractLite
PathProviderHistoryContractLite
PlanningContextPathProjectionLite
AppendDestRegionContract()
AppendRoiOpenSpaceInfoContract()
AppendOpenSpaceTaskContract()
AppendDirectTaskContract()
AppendThreadedProviderDiagnostics()
BuildRoiReason()
BuildPathProviderPreCheckReason()
BuildPathProviderReuseReason()
BuildPathProviderFailureReason()
BuildPathProviderOutputFailureReason()
BuildPathProviderGeneratedReason()
BuildPathPartition*Reason()
BuildSpeedOptimizer*Reason()
BuildOpenSpaceStraightPath*Reason()
```

Adapter 保留：

```text
PathProviderDecision
PathProviderStrategySummary
PathProviderRuntimeState
RuntimeVehicleInput / RuntimeObstacleInput
DDS 输入校验、算法输入构造、算法调用和运行态写回
私有运行态 -> task-lite DTO 的薄映射
```

Adapter 行数：

```text
before: about 4786
after : about 4080
```

## 与原始流程的关系

原始流程中的任务顺序没有改变：

```text
SelectedSlot
  -> ROI_DECIDER
  -> PATH_PROVIDER
  -> PATH_PARTITION
  -> SPEED_OPTIMIZER
  -> PlanningTrajectory
```

direct 分支仍保持：

```text
ParkingCommand DIRECT_FORWARD/DIRECT_BACKWARD
  -> OPEN_SPACE_STRAIGHT_PATH
  -> SPEED_OPTIMIZER
  -> PlanningTrajectory
```

本批主要减少的是“代码归属差异”，不是“完整原车对象差异”。也就是说，task reason 已有独立 helper，但 `Frame/OpenSpaceInfo/PlanningContext/FunctionManager/HMI/collision/wheel mask/NLP smoother` 仍未成为完整 typed DDS/原车对象。

## 验证结果

通过：

```text
git diff --check
bash -n source/valet_parking_tools/build_valet_parking.sh
bash -n source/valet_parking_tools/smoke_valet_parking_x86.sh
bash -n source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh
bash source/valet_parking_tools/build_valet_parking.sh --out-dir /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_097_100
bash source/valet_parking_tools/smoke_valet_parking_x86.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_097_100/valet_parking_mvp/x86 --domain-id 224 --timeout-ms 25000
bash source/valet_parking_tools/smoke_valet_parking_x86.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_097_100/valet_parking_mvp/x86 --domain-id 225 --command-mode direct-forward --timeout-ms 25000
bash source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_097_100/valet_parking_mvp/x86 --first-domain-id 189 --timeout-ms 25000
```

结果：

```text
x86 normal open-space smoke: PASS
x86 direct-forward smoke: PASS
batch smoke first-domain-id=189, last-domain=223: PASS
x86 libvalet_parking.so: PASS
m57 libvalet_parking.so: PASS_STATIC_BUILD
m57 board runtime: NOT VERIFIED
```

本环境在 `domain-id >= 231` 时可能出现 `failed to create DomainParticipant`，所以 batch 必须保证最后一个 domain 不超过 230。

## 剩余差异

- Adapter 仍承载输入转换、算法输入构造、任务调用、运行态写回和部分 fallback 编排，还不是只含 DDS 壳的薄文件。
- task contract 仍通过 `replan_reason` / `estop.reason` 文本承载，不是 formal typed DDS 字段。
- 完整 `Frame/OpenSpaceInfo/PlanningContext/FunctionManager/HMI/collision/wheel mask/NLP smoother` 仍未完整接入。
- m57 只完成交叉编译和 ELF/依赖检查，尚未做板端 runtime。

## 下一步

下一批继续围绕原始流程差异收敛，优先检查 Adapter 中仍混合的输入构造、任务编排和 fallback 边界，先批量扫描和修订，再统一 build/smoke，避免回到逐个小问题调试的节奏。
