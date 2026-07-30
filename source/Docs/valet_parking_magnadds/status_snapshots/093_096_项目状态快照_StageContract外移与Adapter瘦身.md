# 项目状态快照：BATCH-093_096 StageContract 外移与 Adapter 瘦身

日期：2026-07-30

## 当前状态

本批已完成 `BATCH-093_096`。

核心目标是继续按原始 `ValetParkingStageParking` 流程图减少架构差异：Adapter 不再继续承接 Stage 输出契约主体，而是把 `STAGE_OUTPUT`、MissionState、FunctionManagerProjection、RuntimeLifecycle、open-space/fallback/stage-control contract 迁移到独立 helper。

## 本批代码变化

新增：

```text
applications/source/valet_parking/src/valet_parking_stage_contract_lite.h
applications/source/valet_parking/src/valet_parking_stage_contract_lite.cpp
```

迁移内容：

```text
AppendStageProjectionContract()
AppendMissionStateContract()
AppendFunctionManagerProjectionContract()
AppendRuntimeLifecycleContract()
BuildFunctionManagerProjectionReason()
BuildOpenSpaceStageOutputContract()
BuildFallbackStageOutputContract()
BuildEarlyEstopFallbackContract()
AppendStageControlContract()
PathDecisionToString()
FinishStatusToString()
IsFinishSuccessStatus()
TrajectoryTypeName()
StageParkingStatus()
StageStatusFromParkingStatus()
MissionStateFromParkingStatus()
StageTrajectoryType()
```

Adapter 保留：

```text
FunctionManagerProjection -> StageFunctionManagerProjectionContractLite
StageFinishEvaluation -> StageFinishOutputContractLite
PathProviderRuntimeState -> StageRuntimeLifecycleContractLite
DDS/runtime 私有状态、输入校验、算法调用壳
```

Adapter 行数：

```text
before: about 5073
after : 4786
```

## 验证结果

通过：

```text
bash source/valet_parking_tools/build_valet_parking.sh --out-dir /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_093_096
bash source/valet_parking_tools/smoke_valet_parking_x86.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_093_096/valet_parking_mvp/x86 --domain-id 224 --timeout-ms 25000
bash source/valet_parking_tools/smoke_valet_parking_x86.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_093_096/valet_parking_mvp/x86 --domain-id 225 --command-mode direct-forward --timeout-ms 25000
bash source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_093_096/valet_parking_mvp/x86 --first-domain-id 189 --timeout-ms 25000
```

产物：

```text
out/valet_parking_flow_gap_093_096/valet_parking_mvp/x86/lib/libvalet_parking.so
out/valet_parking_flow_gap_093_096/valet_parking_mvp/m57/lib/libvalet_parking.so
```

Smoke 证据：

```text
normal x86 smoke: domain_224_20260730_083455_640
direct x86 smoke: domain_225_20260730_083516_893
batch first domain: domain_189_20260730_084442_727
batch last domain : domain_223_20260730_084954_5126
batch result      : all smoke cases passed
```

## 排错记录

第一次 batch 使用 `first-domain-id=226`，35 个 case 会跑到 domain 260。当前 MagnaDDS 环境在 `domain-id >= 231` 时会出现：

```text
failed to create DomainParticipant
```

改用 `first-domain-id=189` 后，domain 范围为 189..223，batch 全部通过。因此本次失败不是代码回归，而是 smoke domain 范围限制。后续 batch 必须保证最后一个 domain 不超过 230。

## 当前剩余差异

- task 输出契约仍主要留在 Adapter，下一批继续外移 ROI/PATH_PROVIDER/PATH_PARTITION/SPEED_OPTIMIZER/OPEN_SPACE_STRAIGHT_PATH reason builder。
- formal typed DDS 字段仍未替代 `replan_reason` / `estop.reason` 文本诊断。
- 完整 `Frame/OpenSpaceInfo/PlanningContext/FunctionManager/HMI/collision/wheel mask/NLP smoother` 仍未完整接入。
- m57 仍只完成交叉编译和 ELF/依赖检查，未做板端 runtime。

## 下一步

`BATCH-097_100`：继续瘦身 Adapter，批量外移 task-level 输出契约和 diagnostic reason builder。重点对象：

```text
AppendOpenSpaceTaskContract()
AppendDirectTaskContract()
BuildRoiReason()
PathProvider reason / diagnostics
PathPartition reason / diagnostics
SpeedOptimizer reason / diagnostics
OPEN_SPACE_STRAIGHT_PATH direct task reason
```

执行节奏仍保持：先批量检测和批量修订，再统一 build/smoke，再集中排错。
