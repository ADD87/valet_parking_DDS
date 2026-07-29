# 项目状态快照 061_064：MissionState 与 Direct 命令生命周期收敛

日期：2026-07-29

## 本批次结论

`BATCH-061_064` 已完成。

本批次继续对照 `E:\APA\DDS\parking_algorithm_standalone\docs\00_ValetParkingStageParking_超详细流程图总览_重要.md`，目标是减少当前 MagnaDDS Adapter 和原始 `ValetParkingStageParking` 在 Stage 状态投影、direct 命令生命周期、PlanningContext 写回点、collision/wheel mask 输入来源上的可观察差异。

本批次仍是当前 DDS MVP 内的行为等价诊断契约，不是完整迁移原始 `MissionState`、`FinishScenario()`、`PlanningContext`、`FunctionManager` 或完整碰撞/轮挡逻辑。

## 代码变更

- `applications/source/valet_parking/src/valet_parking_stage_parking_adapter.cpp`
  - 新增 `DirectCommandRuntimeState`，在 `RuntimeContext` 内记录上一帧 direct command。
  - direct command 从 `DIRECT_FORWARD/DIRECT_BACKWARD` 清成 `NONE` 后，会输出 `DIRECT_FORWARD_RELEASED` 或 `DIRECT_BACKWARD_RELEASED`。
  - 新增轻量 `MissionState` 投影：`mission_state_contract`、`mission_state`、`next_stage`、`finish_scenario_intent`、`finish_scenario_contract=diagnostic_only`。
  - direct active 分支输出 `DIRECT_CONTROL_ACTIVE`，release 分支输出 `DIRECT_FINISH_READY`、`next_stage=FINISH`、`direct_finish_ready=true`。
  - PathProvider 成功日志输出 `planning_context_contract=path_provider_runtime_projection`、`path_history_state`、`planning_context_path_id`、`planning_context_replan_reason`、`target_update_writeback` 和 `planning_context_history_points`。
  - PreCheck 日志明确 `collision_input_source=roi_and_external_segments`、`wheel_mask_input_source=none`、`wheel_mask_idl_extension=required_before_vehicle_integration`。

- `applications/source/valet_parking_tools/smoke_valet_parking_x86.sh`
  - 新增 `direct-forward-release` 与 `direct-backward-release` 命令模式。
  - release smoke 会先发布 direct command，再发布 `NONE` 清命令，再发布 SelectedSlot 触发 release 分支。
  - release 断言验证 `DIRECT_*_RELEASED` 自身带 `skip=ROI_PATH_PROVIDER_PATH_PARTITION`，不误伤 release 后恢复普通泊车链路的 ROI 日志。
  - valid、target-moves、parking-seq-changes、多车位 seq switch 场景增加 `PlanningContext` 投影断言。
  - pause、brake、finish、parking-out unsupported、direct active/release 增加 `MissionState` 投影断言。

- `applications/source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh`
  - 批量 smoke 矩阵加入 direct forward/backward release finish contract 场景。

## 验证结果

通过：

```text
git diff --check
bash -n source/valet_parking_tools/smoke_valet_parking_x86.sh
bash -n source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh
bash source/valet_parking_tools/build_valet_parking.sh --out-dir /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_061_064
bash source/valet_parking_tools/smoke_valet_parking_x86.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_061_064/valet_parking_mvp/x86 --domain-id 198 --count 3 --interval-ms 500 --expect-thread-provider-stop
bash source/valet_parking_tools/smoke_valet_parking_x86.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_061_064/valet_parking_mvp/x86 --domain-id 204 --command-mode direct-forward-release --timeout-ms 25000
bash source/valet_parking_tools/smoke_valet_parking_x86.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_061_064/valet_parking_mvp/x86 --domain-id 205 --command-mode direct-backward-release --timeout-ms 25000
bash source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_061_064/valet_parking_mvp/x86 --first-domain-id 180 --timeout-ms 25000
```

产物：

```text
out/valet_parking_flow_gap_061_064/valet_parking_mvp/x86/lib/libvalet_parking.so
out/valet_parking_flow_gap_061_064/valet_parking_mvp/m57/lib/libvalet_parking.so
```

关键日志：

```text
out/valet_parking_flow_gap_061_064/valet_parking_mvp/x86/smoke_latest/domain_198_20260729_130915_2541
out/valet_parking_flow_gap_061_064/valet_parking_mvp/x86/smoke_latest/domain_204_20260729_132214_792
out/valet_parking_flow_gap_061_064/valet_parking_mvp/x86/smoke_latest/domain_205_20260729_132348_727
batch smoke first-domain-id=180: all smoke cases passed
```

## 已减少的差异

| 原始流程节点 | 当前收敛结果 |
|---|---|
| `MissionState` / `next_stage_` | 已有轻量状态投影，能观察 `PARKING/FINISH` 意图 |
| `FinishScenario()` | 已有 `finish_scenario_intent` 诊断，但明确为 `diagnostic_only` |
| direct command lifecycle | 已验证 `DIRECT_* -> NONE -> DIRECT_*_RELEASED` 轻量闭环 |
| FunctionManager release | release 分支输出 `cleared_direct_command`、`QUIT`、`command=NONE` |
| PlanningContext | PathProvider 输出 history、path id、replan reason、target update writeback |
| collision/wheel mask | 明确当前 collision 输入来自 ROI/external segments，wheel mask 未暴露，正式 IDL 扩展仍是车端集成前置条件 |

## 仍然不能声称完成的事情

- 不能声称完整复现原始 `ValetParkingStageParking`。
- `MissionState`、`next_stage`、`FinishScenario` 仍是诊断文本，不是原车状态机。
- `FunctionManagerProjection` 仍是投影，不是真实 FunctionManager proto。
- `PlanningContextProjection` 仍是当前 Adapter 运行态投影，不是完整 `PlanningContext`。
- collision 仍不是完整 `Frame::OpenSpaceCollisionCheck`。
- wheel mask 仍未接入真实轮挡语义。
- `OpenSpacePathSmoother/NLP` 仍未接入。
- m57 仍未板端 runtime 验证。

## 下一批

进入 `BATCH-065_068`：

```text
NEXT-065：集中梳理 Stage 输出文本契约，减少 mission_state / parking_status / FunctionManagerProjection / OpenSpaceInfo 字段散落风险
NEXT-066：补 destination reached + standstill 的 finish 边界 smoke，继续逼近原始 IsReadyToFinishStage
NEXT-067：补 reset_history、target update、direct release 后恢复普通泊车的组合场景，验证运行态不会互相污染
NEXT-068：形成 collision/wheel mask 正式 IDL 扩展草案，但不在未拿到真实车端输入前强行接入
```
