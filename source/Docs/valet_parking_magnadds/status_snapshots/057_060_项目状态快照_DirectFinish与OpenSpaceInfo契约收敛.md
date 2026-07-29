# 项目状态快照 057_060：DirectFinish 与 OpenSpaceInfo 契约收敛

日期：2026-07-29

## 本批次结论

`BATCH-057_060` 已完成。

本批次继续对照 `E:\APA\DDS\parking_algorithm_standalone\docs\00_ValetParkingStageParking_超详细流程图总览_重要.md`，目标是减少当前 MagnaDDS Adapter 和原始 `ValetParkingStageParking` 在 direct finish、FunctionManager 负向路径、Frame/OpenSpaceInfo 读写点、collision/wheel mask 边界上的差异。

本批次不是接入完整 `Frame/OpenSpaceInfo/FunctionManager`，而是在当前 DDS MVP 内把关键流程节点转成可观察、可 smoke 回归的文本契约。

## 代码变更

- `applications/source/valet_parking/src/valet_parking_stage_parking_adapter.cpp`
  - 新增 `DirectFinishEvaluation`，用于表达 direct 分支的 `command inactive + standstill` 完成条件。
  - direct 分支输出 `finish_condition=direct_command_inactive_and_standstill`、`direct_command_active`、`direct_command_inactive`、`direct_vehicle_standstill`、`direct_finish_ready`、`direct_stage_finish_state`。
  - ROI 输出新增 `open_space_info_contract=roi_output`、`open_space_path_info_id`、`xy_bounds_size`、`dest_region_points`、`dest_region_area`、`dest_region_angle`。
  - PathProvider 输出新增 `open_space_info_contract=path_provider_output`、`path_info_id` 和 `dest_region_*` 字段。
  - PathPartition 输出新增 `open_space_info_contract=path_partition_output`、`chosen_path_contract=chosen_partitioned_path`、`chosen_path_points`、`chosen_path_gear`、`stop_path`。
  - SpeedOptimizer 输出新增 `open_space_info_contract=speed_optimizer_output`、`chosen_path_points`、`partitioned_paths`、`stop_path`、`speed_optimizer_trajectory_points`、`wheel_mask_considered=false`。
  - PreCheck 成功和失败路径均显式输出 `collision_contract=geometry_precheck_only` 与 `wheel_mask_contract=not_exposed_in_current_mvp`。

- `applications/source/valet_parking_tools/smoke_valet_parking_x86.sh`
  - valid 场景增加 ROI/PathProvider/PathPartition/SpeedOptimizer 的 OpenSpaceInfo 最小契约断言。
  - direct forward/backward 增加 FunctionManager 投影和 direct finish 边界断言。
  - pause/brake/finish/parking-out unsupported/invalid clear 增加 FunctionManager 负向路径断言。
  - far-obstacles 与 many-obstacles 负向样本增加 collision/wheel-mask 当前边界断言。

## 验证结果

通过：

```text
git diff --check
bash -n source/valet_parking_tools/smoke_valet_parking_x86.sh
bash -n source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh
bash -n source/valet_parking_tools/build_valet_parking.sh
bash source/valet_parking_tools/build_valet_parking.sh --out-dir /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_057_060
bash source/valet_parking_tools/smoke_valet_parking_x86.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_057_060/valet_parking_mvp/x86 --domain-id 217 --count 3 --interval-ms 500 --expect-thread-provider-stop
bash source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_057_060/valet_parking_mvp/x86 --first-domain-id 180 --timeout-ms 25000
```

产物：

```text
out/valet_parking_flow_gap_057_060/valet_parking_mvp/x86/lib/libvalet_parking.so
out/valet_parking_flow_gap_057_060/valet_parking_mvp/m57/lib/libvalet_parking.so
```

关键日志：

```text
out/valet_parking_flow_gap_057_060/valet_parking_mvp/x86/smoke_latest/domain_217_20260729_115557_737
batch smoke first-domain-id=180: all smoke cases passed
```

## 已减少的差异

| 原始流程节点 | 当前收敛结果 |
|---|---|
| direct finish | 已有 `direct_command_inactive_and_standstill` 轻量诊断，可观察 active 命令下的 WAITING 边界 |
| FunctionManager 负向路径 | direct、pause、brake、finish、parking-out unsupported、invalid clear 均进入 smoke 断言 |
| ROI/OpenSpaceInfo | 已暴露 path_info_id、xy_bounds、dest_region 最小字段 |
| PathProvider/OpenSpaceInfo | 已暴露 path_info_id 与目标区域读写点 |
| PathPartition/OpenSpaceInfo | 已暴露 chosen path 点数、gear 与 stop_path |
| SpeedOptimizer/OpenSpaceInfo | 已暴露 chosen path 输入、partition 数、stop_path、速度轨迹点数与 wheel mask 当前边界 |
| collision/wheel mask | 已明确当前只做 geometry precheck，wheel mask 尚未暴露，且负向 smoke 覆盖该事实 |

## 仍然不能声称完成的事情

- 不能声称完整复现原始 `ValetParkingStageParking`。
- direct 分支还没有真实命令 active -> inactive 后的 `direct_finish_ready=true` 输入闭环。
- `OpenSpaceInfo` 仍是诊断文本契约，不是完整 `Frame::open_space_info()`。
- `FunctionManagerProjection` 仍是投影，不是真实 FunctionManager proto。
- collision 仍不是完整 `Frame::OpenSpaceCollisionCheck`。
- wheel mask 仍未接入真实轮挡语义。
- `OpenSpacePathSmoother/NLP` 仍未接入。
- m57 仍未板端 runtime 验证。

## 下一批

进入 `BATCH-061_064`：

```text
NEXT-061：轻量 MissionState/next_stage/FinishScenario 诊断契约
NEXT-062：direct 命令生命周期负向/解除路径
NEXT-063：PlanningContext/OpenSpaceInfo 历史路径、replan reason、target update 写回点
NEXT-064：collision/wheel mask 真实输入来源评估与正式 IDL 扩展草案
```
