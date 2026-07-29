# 项目状态快照 069_072：Task 契约与 Stage 退出生命周期收敛

- 日期：2026-07-29
- 阶段：Fast-2
- 状态：PASS_TASK_RUNTIME_LIFECYCLE_X86_SMOKE_M57_STATIC_BLOCKED_BOARD
- 范围：继续对照原始 `ValetParkingStageParking` 流程图减少差异，不扩正式 IDL，不修改 `compile/` 或 `thirdparty/`。

## 本批目标

```text
NEXT-069：批量扫描并统一 ROI/PathProvider/PathPartition/SpeedOptimizer task 输出契约
NEXT-070：补 finish_ready 后普通 PARKING task 的保持/退出语义
NEXT-071：补 direct release 对已有普通泊车历史的清理证据
NEXT-072：补 FunctionManagerProjection 与 Stage 输出一致性的组合 smoke
```

## 代码变化

| 文件 | 变化 |
|---|---|
| `applications/source/valet_parking/src/valet_parking_stage_parking_adapter.cpp` | 新增 `AppendOpenSpaceTaskContract`、`AppendRuntimeLifecycleContract`、`stage_exit_requested`、`FINISH_HOLD` 分支和 direct release cleanup 诊断 |
| `applications/source/valet_parking_tools/smoke_valet_parking_x86.sh` | 新增 task/runtime contract 断言，新增 `--pre-command-slot-count`，验证已有历史后的 direct release reset |
| `applications/source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh` | batch matrix 新增 `direct-release-clears-existing-history` |
| `applications/source/Docs/valet_parking_magnadds/05_TempAPA原始流程复现差距对照表.md` | 追加 BATCH-069_072 差异收敛和验证记录 |
| `applications/source/Docs/valet_parking_magnadds/14_原始流程差异收敛执行记录.md` | 追加 BATCH-069_072 执行记录 |
| `applications/source/Docs/valet_parking_magnadds/STATUS.yaml` | 更新当前状态、证据、测试矩阵和 next_action |

## 验证

通过：
```text
git diff --check
bash -n source/valet_parking_tools/smoke_valet_parking_x86.sh
bash -n source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh
bash source/valet_parking_tools/build_valet_parking.sh --out-dir /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_069_072
bash source/valet_parking_tools/smoke_valet_parking_x86.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_069_072/valet_parking_mvp/x86 --domain-id 121 --with-aux-inputs --aux-mode near-destination --aux-chassis-gear drive --timeout-ms 25000 --expect-thread-provider-stop
bash source/valet_parking_tools/smoke_valet_parking_x86.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_069_072/valet_parking_mvp/x86 --domain-id 122 --slot-mode target-moves --command-mode direct-forward-release --pre-command-slot-count 3 --timeout-ms 25000
bash source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_069_072/valet_parking_mvp/x86 --first-domain-id 130 --timeout-ms 25000
```

产物：
```text
out/valet_parking_flow_gap_069_072/valet_parking_mvp/x86/lib/libvalet_parking.so
out/valet_parking_flow_gap_069_072/valet_parking_mvp/m57/lib/libvalet_parking.so
```

smoke 证据：
```text
out/valet_parking_flow_gap_069_072/valet_parking_mvp/x86/smoke_latest/domain_121_20260729_181844_2885
out/valet_parking_flow_gap_069_072/valet_parking_mvp/x86/smoke_latest/domain_122_20260729_182017_646
out/valet_parking_flow_gap_069_072/valet_parking_mvp/x86/smoke_latest/domain_130_20260729_182052_1094
```

## 已减少的差异

- 普通四段 task 输出由各自 reason 进一步收口为统一 `task_contract=lightweight_open_space_task_projection`。
- `finish_ready=true` 后锁存 `stage_exit_requested`，后续 SelectedSlot 帧输出 `STAGE_CONTROL FINISH_HOLD`，不继续跑普通 PARKING task。
- `FINISH_HOLD` 输出 `stage_finish_latched` FunctionManager 投影，表达 `PARKINGFINISH/QUIT/PARKING_IN`。
- direct release 在已有普通路径历史时输出 `path_history_available=true` 和 `path_history_action=reset_after_publish`，组合 smoke 已覆盖。
- `mission_state/parking_status/function_manager/runtime_lifecycle` 的一致性进入 batch smoke。

## 仍保留的边界

- 当前 task/runtime/stage/function manager 字段仍承载在 `replan_reason/estop.reason` 文本里，不是正式 DDS typed 字段。
- `FINISH_HOLD` 是轻量退出锁存，不是完整原车 `FinishScenario()` / `next_stage_` 框架。
- collision/wheel mask、完整 `Frame/OpenSpaceInfo/PlanningContext`、NLP smoother 仍未完整接入。
- m57 只完成交叉编译和 ELF/依赖检查，不能标记为板端 runtime 通过。

## 下一步

进入 `BATCH-073_076`：继续对照原始流程图，优先从失败/降级路径补齐 Stage runtime contract，使 ROI/PreCheck/PathProvider/PathPartition/SpeedOptimizer 失败时的 FunctionManagerProjection、MissionState、fallback 语义与普通成功链保持一致；仍保持批量扫描、批量修订、统一 build/smoke、集中排错的节奏。
