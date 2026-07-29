# 项目状态快照：BATCH-073_076 Fallback 路径 Stage 生命周期收敛

日期：2026-07-29

## 本批目标

继续对照 `00_ValetParkingStageParking_超详细流程图总览_重要.md` 减少差异，重点不是补外围小问题，而是把普通泊车主链后半段失败/降级路径统一成可观察的 Stage 语义：

```text
SelectedSlot
  -> ROI_DECIDER
  -> PATH_PROVIDER_PRECHECK
  -> PATH_PROVIDER
  -> PATH_PARTITION
  -> SPEED_OPTIMIZER
  -> PlanningTrajectory
```

本批覆盖的 fallback：

```text
PathProvider timeout/fail -> publish_roi_seed
PreCheck fail             -> publish_estop
PathPartition fail        -> publish_path_provider_path
SpeedOptimizer fail       -> publish_path_partition_path
```

## 代码变更

### Adapter

文件：
```text
applications/source/valet_parking/src/valet_parking_stage_parking_adapter.cpp
```

新增：
- `BuildFallbackStageOutputContract()`：统一输出 `STAGE_OUTPUT fallback`。
- `VALET_PARKING_FORCE_PATH_PARTITION_FAIL`：smoke-only PathPartition 强制失败开关。
- `VALET_PARKING_FORCE_SPEED_OPTIMIZER_FAIL`：smoke-only SpeedOptimizer 强制失败开关。

fallback 输出包含：
```text
stage_contract=lightweight_valet_parking_stage_projection
stage_contract_record=fallback_output
fallback_event=...
fallback_action=...
mission_state_contract=lightweight_stage_projection
function_manager_*
runtime_lifecycle_contract=lightweight_stage_runtime_projection
```

### Smoke

文件：
```text
applications/source/valet_parking_tools/smoke_valet_parking_x86.sh
applications/source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh
```

新增 smoke 参数：
```text
--force-path-partition-fail
--force-speed-optimizer-fail
```

批量矩阵新增：
```text
forced-path-partition-fallback
forced-speed-optimizer-fallback
```

## 验证结果

通过：
```text
git diff --check
bash -n source/valet_parking_tools/smoke_valet_parking_x86.sh
bash -n source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh
bash source/valet_parking_tools/build_valet_parking.sh --out-dir /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_073_076
```

重点 smoke：
```text
domain_123: PathProvider timeout -> ROI seed fallback PASS
domain_124: forced PathPartition fail -> PathProvider fallback PASS
domain_125: forced SpeedOptimizer fail -> PathPartition fallback PASS
domain_126: far-obstacles PreCheck fail -> estop fallback PASS
domain_127: many-obstacles PreCheck fail -> estop fallback PASS
```

批量 smoke：
```text
first-domain-id=160
result=all smoke cases passed
```

产物：
```text
out/valet_parking_flow_gap_073_076/valet_parking_mvp/x86/lib/libvalet_parking.so
out/valet_parking_flow_gap_073_076/valet_parking_mvp/m57/lib/libvalet_parking.so
```

m57 说明：
```text
m57 已完成交叉编译与 ELF/依赖检查。
这不代表 m57 板端 runtime 已通过，板端仍是 OPEN blocker。
```

## 已减少的原始流程差异

- 后段 task 失败不再只靠局部 reason，已统一补 Stage fallback 语义。
- PathProvider timeout、PreCheck fail、PathPartition fail、SpeedOptimizer fail 都能看见 `fallback_event`、`fallback_action`、MissionState、FunctionManagerProjection 和 runtime lifecycle。
- 难自然触发的 PathPartition/SpeedOptimizer 失败分支已经有 smoke-only 可控入口，后续不会靠偶发错误验证。

## 仍保留的差异

- 当前 fallback contract 仍是 `replan_reason/estop.reason` 文本诊断，不是正式 DDS typed 字段。
- early Stage 输入失败分支尚未全部统一：invalid SelectedSlot、empty/overflow、selected lot missing、parking lot convert fail、vehicle-lot precheck fail、ROI fail。
- direct 分支内部 `OPEN_SPACE_STRAIGHT_PATH` 失败和 direct speed fallback 尚未独立补 Stage fallback smoke。
- collision/wheel mask、完整 Frame/OpenSpaceInfo/PlanningContext、NLP smoother 仍未完整接入。
- m57 仍未做板端 runtime。

## 下一步

`BATCH-077_080`：继续减少 early Stage 退出/失败分支差异。

计划：
```text
NEXT-077：扫描 invalid SelectedSlot、empty/overflow、parking lot convert、vehicle-lot precheck、ROI fail、direct straight path fail 的 Stage 输出差异
NEXT-078：补 early Stage fallback / stage-control-fallback 统一契约
NEXT-079：扩展 smoke 覆盖 invalid/empty/overflow/far-localization/degenerate/direct-straight-fail
NEXT-080：统一 x86/m57 build + x86 batch smoke + 文档/Git
```
