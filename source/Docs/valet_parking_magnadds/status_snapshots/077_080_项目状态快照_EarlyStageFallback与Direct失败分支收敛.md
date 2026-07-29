# 项目状态快照：BATCH-077_080 Early Stage Fallback 与 Direct 失败分支收敛

日期：2026-07-29

## 本批目标

继续对照 `00_ValetParkingStageParking_超详细流程图总览_重要.md` 减少差异，重点补齐两类过去容易散落的路径：

```text
early Stage 输入失败
  -> invalid SelectedSlot
  -> empty/overflow
  -> parking lot convert fail
  -> vehicle-lot precheck fail
  -> ROI_DECIDER fail

direct 分支内部失败
  -> OPEN_SPACE_STRAIGHT_PATH fail
  -> direct SPEED_OPTIMIZER fail
```

本批目标不是接完整原车 `Stage/Frame/DependencyInjector` 框架，而是在当前 MagnaDDS Adapter 结构里，把这些早退/降级路径统一成可观察、可回归的 Stage fallback 契约。

## 代码变更

### Adapter

文件：
```text
applications/source/valet_parking/src/valet_parking_stage_parking_adapter.cpp
```

新增：
- `BuildEarlyEstopFallbackContract()`：early Stage 输入失败统一输出 `STAGE_OUTPUT fallback`。
- `VALET_PARKING_FORCE_ROI_DECIDER_FAIL`：smoke-only ROI 失败入口。
- `VALET_PARKING_FORCE_STRAIGHT_PATH_FAIL`：smoke-only direct straight path 失败入口。

early fallback 覆盖：
```text
selected_slot_invalid
selected_slot_empty
selected_slot_count_overflow
selected_lot_unavailable
parking_lot_convert_failed
vehicle_lot_precheck_failed
roi_decider_failed
```

direct fallback 覆盖：
```text
open_space_straight_path_failed -> publish_stage_control_stop
direct_speed_optimizer_failed   -> publish_open_space_straight_path 或 publish_stage_control_stop
```

### Smoke

文件：
```text
applications/source/valet_parking_tools/smoke_valet_parking_x86.sh
applications/source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh
```

新增 smoke 参数：
```text
--force-roi-decider-fail
--force-straight-path-fail
```

批量矩阵新增：
```text
forced-roi-decider-fallback
empty-selected-slot-fallback
overflow-selected-slot-fallback
nan-parking-lot-fallback
degenerate-parking-lot-fallback
direct-forward-straight-path-fallback
direct-forward-speed-fallback
vehicle-lot-precheck-fallback
```

额外手工补测：
```text
direct-backward + force-straight-path-fail
direct-backward + force-speed-optimizer-fail
```

## 验证结果

通过：
```text
git diff --check
bash -n source/valet_parking_tools/smoke_valet_parking_x86.sh
bash -n source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh
bash source/valet_parking_tools/build_valet_parking.sh --out-dir /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_077_080
```

重点 smoke：
```text
domain_129: forced ROI fail -> roi_decider_failed estop fallback PASS
domain_130: empty SelectedSlot -> selected_slot_invalid fallback PASS
domain_131: overflow SelectedSlot -> selected_slot_count_overflow fallback PASS
domain_132: NaN parking lot -> parking_lot_convert_failed fallback PASS
domain_133: degenerate corners -> parking_lot_convert_failed fallback PASS
domain_134: far localization -> vehicle_lot_precheck_failed fallback PASS
domain_136: direct-forward straight path fail -> stage-control stop fallback PASS
domain_138: direct-forward speed fail -> direct speed fallback PASS
domain_139: direct-backward straight path fail -> stage-control stop fallback PASS
domain_141: direct-backward speed fail -> direct speed fallback PASS
```

批量 smoke：
```text
first-domain-id=170
case_count=32
result=all smoke cases passed
```

说明：
```text
第一次完整 batch 使用 240 秒工具超时被截断，但没有发现残留 valet 进程；
改用 10 分钟工具超时后完整通过。
```

产物：
```text
out/valet_parking_flow_gap_077_080/valet_parking_mvp/x86/lib/libvalet_parking.so
out/valet_parking_flow_gap_077_080/valet_parking_mvp/m57/lib/libvalet_parking.so
```

m57 说明：
```text
m57 已完成交叉编译与 ELF/依赖检查。
这不代表 m57 板端 runtime 已通过，板端仍是 OPEN blocker。
```

## 已减少的原始流程差异

- early Stage 输入失败路径不再只是局部 reason 或裸 estop，已统一为 Stage fallback 输出。
- ROI fail、SelectedSlot 边界、parking lot convert fail、vehicle-lot precheck fail 均有可回归 smoke。
- direct 分支内部路径失败和速度优化失败都有独立 fallback 语义，且不再因为兜底轨迹过短错误进入 estop。
- direct forward/backward 两个方向都补了失败分支手工 smoke。
- 扩展 batch smoke 已纳入 early failure 和 direct failure 回归。

## 仍保留的差异

- 当前 fallback contract 仍是 `replan_reason/estop.reason` 文本诊断，不是正式 DDS typed 字段。
- `selected_lot_unavailable` 代码路径已统一，但缺少稳定自然触发或 smoke-only 入口覆盖。
- direct speed fallback 的 task chain 仍复用通用 `SPEED_OPTIMIZER` 输出诊断，后续可拆为 direct 专用 task contract。
- collision/wheel mask、完整 `Frame/OpenSpaceInfo/PlanningContext`、NLP smoother 仍未完整接入。
- m57 仍未做板端 runtime。

## 下一步

`BATCH-081_084`：继续对照原始流程图做批量差异收敛。

计划：
```text
NEXT-081：批量扫描 remaining high-gap nodes：selected_lot_unavailable 覆盖、direct 专用 task contract、Stage 输出文本键集中化、PlanningContext 写回/清理一致性
NEXT-082：补 selected_lot_unavailable 可控 smoke 或自然输入构造，并把 direct-backward fallback 纳入 batch 矩阵
NEXT-083：收敛 direct 分支 task_contract_chain 与普通链差异，减少通用 SPEED_OPTIMIZER 诊断误导
NEXT-084：统一 x86/m57 build + x86 batch smoke + 文档/Git
```
