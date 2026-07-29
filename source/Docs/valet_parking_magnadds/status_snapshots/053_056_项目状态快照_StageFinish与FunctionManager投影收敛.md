# 项目状态快照 053_056：StageFinish 与 FunctionManager 投影收敛

日期：2026-07-29

## 本批次结论

`BATCH-053_056` 已完成。

本批次继续对照 `E:\APA\DDS\parking_algorithm_standalone\docs\00_ValetParkingStageParking_超详细流程图总览_重要.md`，目标不是修外围小问题，而是减少当前 MagnaDDS Adapter 和原始 `ValetParkingStageParking` 在 Stage 收口、FunctionManager 判型、Frame/OpenSpaceInfo 最小契约、PreCheck/collision/wheel mask 观测边界上的差异。

本批次收敛的核心差异：

```text
IsReadyToFinishStage
FunctionManager/sys_command/sys_run_state/sys_warning_info -> parking_type
ROI/OpenSpaceInfo 最小字段诊断
PathProvider PreCheck/collision/wheel mask 缺口诊断
```

## 代码变更

- `applications/source/valet_parking/src/valet_parking_stage_parking_adapter.cpp`
  - 新增轻量 `StageFinishRuntimeState` 和 `StageFinishEvaluation`。
  - `UpdateStageFinishEvaluation()` 按 `destination_reached && vehicle_standstill` 判断 ready condition，并要求连续 2 帧后才输出 `finish_ready=true`。
  - `STAGE_OUTPUT open_space` 新增 `finish_condition=destination_reached_and_standstill`、`finish_ready`、`finish_ready_condition`、`finish_consecutive_frames`、`finish_required_frames`、`vehicle_standstill`、`stage_finish_state=WAITING/HOLDING/READY`。
  - 新增 `FunctionManagerProjection`，把当前 `SelectedSlot` / `ParkingCommand` 投影为原始流程图里的 `sys_mode/sys_command/sys_run_state/sys_warning_info/parking_type` 命名。
  - 普通 open-space 主链、direct 分支、pause/brake/finish 分支、unsupported parking-out 分支均输出 FunctionManager 投影。
  - `ROI_DECIDER ok` 新增 `fine_tuned`、`slot_inner_fs_valid`、`scenario_difficulty`、`linked_obs_segments`、`high_curb_obs_segments`、`roi_wall_segments`、`virtual_obs_segments`。
  - `PATH_PROVIDER_PRECHECK ok` 新增 `roi_segments`、`external_segments`、`near_start_segments`、`near_end_segments`、`vehicle_has_state`、`wheel_mask_contract=not_exposed_in_current_mvp`。

- `applications/source/valet_parking_tools/smoke_valet_parking_x86.sh`
  - valid 场景新增 `IsReadyToFinishStage` 轻量状态机断言。
  - valid 场景新增普通主链 `FunctionManagerProjection` 断言。

- `applications/source/Docs/valet_parking_magnadds/05_TempAPA原始流程复现差距对照表.md`
  - 更新 `SetParkingType`、ROI、collision、PreCheck、`IsReadyToFinishStage`、FunctionManager/PlanningContext 的差异状态。
  - 新增 BATCH-053_056 差异收敛记录。

- `applications/source/Docs/valet_parking_magnadds/14_原始流程差异收敛执行记录.md`
  - 记录 BATCH-053_056 的收敛目标、已减少差异、验证证据和下一批任务。

## 验证结果

通过：

```text
bash -n source/valet_parking_tools/smoke_valet_parking_x86.sh
bash -n source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh
bash source/valet_parking_tools/build_valet_parking.sh --out-dir /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_053_056
bash source/valet_parking_tools/smoke_valet_parking_x86.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_053_056/valet_parking_mvp/x86 --domain-id 207 --count 3 --interval-ms 500 --expect-thread-provider-stop
bash source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_053_056/valet_parking_mvp/x86 --first-domain-id 211 --timeout-ms 25000
```

产物：

```text
out/valet_parking_flow_gap_053_056/valet_parking_mvp/x86/lib/libvalet_parking.so
out/valet_parking_flow_gap_053_056/valet_parking_mvp/m57/lib/libvalet_parking.so
```

关键日志：

```text
out/valet_parking_flow_gap_053_056/valet_parking_mvp/x86/smoke_latest/domain_207_20260729_092938_738
batch smoke first-domain-id=211: all smoke cases passed
```

## 已减少的差异

| 原始流程节点 | 当前收敛结果 |
|---|---|
| `IsReadyToFinishStage()` | 已有轻量连续帧状态机，覆盖普通泊车 `destination_reached + standstill` 判定 |
| `SetParkingType()` / FunctionManager | 已有 `FunctionManagerProjection` 文本契约，可对照 `sys_command/sys_run_state/sys_warning_info/parking_type` |
| ROI 建模字段 | 已暴露更多 ROI 输出字段，方便继续对照 `OpenSpaceInfo` |
| `PathProvider::PreCheck` | 已暴露 ROI/external/near-start/near-end 线段、车辆状态和 wheel mask 缺口 |

## 仍然不能声称完成的事情

- 不能声称完整复现原始 `ValetParkingStageParking`。
- `FunctionManagerProjection` 只是投影，不是真实原车 FunctionManager proto。
- `finish_ready=true` 只进入诊断文本，不会真正切 `next_stage_` 或调用 `FinishScenario()`。
- `Frame/OpenSpaceInfo/PlanningContext` 仍是轻量替代。
- wheel mask 和完整 open-space collision check 尚未接入。
- `OpenSpacePathSmoother/NLP` 仍未接入。
- m57 仍未板端 runtime 验证。

## 下一批

进入 `BATCH-057_060`：

```text
NEXT-057：direct 命令失活 + standstill 的 finish 诊断
NEXT-058：FunctionManager 投影负向路径 smoke
NEXT-059：Frame/OpenSpaceInfo 读写点差距继续显式化
NEXT-060：collision/wheel mask 可观测契约和负向样本
```
