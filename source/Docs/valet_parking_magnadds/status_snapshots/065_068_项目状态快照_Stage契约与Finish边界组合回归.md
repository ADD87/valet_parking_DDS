# 项目状态快照：BATCH-065_068 Stage 契约与 Finish 边界组合回归

日期：2026-07-29

状态：`PASS_STAGE_CONTRACT_FINISH_BOUNDARY_X86_SMOKE_M57_STATIC_BLOCKED_BOARD`

## 本批目标

继续对照 `00_ValetParkingStageParking_超详细流程图总览_重要.md` 缩小差异：

```text
NEXT-065：集中梳理 Stage 输出文本契约
NEXT-066：补 destination_reached + standstill 的 finish 边界 smoke
NEXT-067：补 reset_history、target update、direct release 后恢复普通泊车组合场景
NEXT-068：记录 collision/wheel mask 正式 IDL 扩展 blocker
```

## 已完成变更

- `ValetParkingStageParkingAdapter` 新增统一 Stage 投影契约输出，普通 open-space 和 stage-control 分支都带 `stage_contract=lightweight_valet_parking_stage_projection`。
- `PartitionInput.is_vehicle_stand_still` 不再固定为 true，改为根据 DDS chassis / vehicle state 速度判断。
- `aux_input_mock_publisher` 新增 `near-destination` 模式：先发原点定位和 0.2 m/s 车速，再切到终点附近定位和 0 m/s 车速。
- near-destination 的远处障碍物 id 固定，避免测试输入误触发 `obstacles_changed`。
- x86 smoke 新增 finish 边界断言：`PATH_PARTITION TASK_FINISH / REACH_TARGET`、`destination_reached=true`、`finish_ready=true`、`MISSION_FINISHED -> FINISH`。
- direct release 后对 `target-moves` 等 SelectedSlot 模式补足 post-clear 帧，验证恢复普通泊车后 target update 与 history reuse。
- 批量矩阵加入 `destination-reached-finish-boundary`、`direct-release-restore-target-update`、`parking-in-reset-target-update`。

## 验证结果

通过：

```text
git diff --check
bash -n source/valet_parking_tools/smoke_valet_parking_x86.sh
bash -n source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh
bash -n source/valet_parking_tools/build_valet_parking.sh
bash source/valet_parking_tools/build_valet_parking.sh --out-dir /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_065_068
bash source/valet_parking_tools/smoke_valet_parking_x86.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_065_068/valet_parking_mvp/x86 --domain-id 210 --with-aux-inputs --aux-mode near-destination --aux-chassis-gear drive --timeout-ms 25000 --expect-thread-provider-stop
bash source/valet_parking_tools/smoke_valet_parking_x86.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_065_068/valet_parking_mvp/x86 --domain-id 213 --slot-mode target-moves --command-mode direct-forward-release --timeout-ms 25000
bash source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_065_068/valet_parking_mvp/x86 --first-domain-id 120 --timeout-ms 25000
```

产物：

```text
out/valet_parking_flow_gap_065_068/valet_parking_mvp/x86/lib/libvalet_parking.so
out/valet_parking_flow_gap_065_068/valet_parking_mvp/m57/lib/libvalet_parking.so
```

关键证据：

```text
out/valet_parking_flow_gap_065_068/valet_parking_mvp/x86/smoke_latest/domain_210_20260729_165326_1071
out/valet_parking_flow_gap_065_068/valet_parking_mvp/x86/smoke_latest/domain_213_20260729_165749_731
/tmp/smoke_batch_065_068_lowdomain.log -> all smoke cases passed
```

## 结论

本批把普通 open-space 输出、stage-control 输出、finish 边界、direct release 后恢复普通泊车、reset_history + target update 组合状态都纳入 smoke。`destination_reached + standstill` 已能通过 DDS 输入自然触发原始 `PathPartition` 的 `REACH_TARGET`，再进入轻量 StageFinish 连续帧 READY。

m57 当前只完成交叉编译和 ELF/依赖检查，尚未进行板端 runtime。collision/wheel mask 仍保持正式 IDL 扩展 blocker，不在未拿到真实车端输入前强行接入。
