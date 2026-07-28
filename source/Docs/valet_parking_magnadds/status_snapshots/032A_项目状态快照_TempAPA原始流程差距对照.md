# 项目状态快照 032A：TempAPA 原始流程差距对照

- 快照编号：032A
- 日期：2026-07-28
- 当前阶段：Fast-2 - `.so` + typed DDS + localized valet parking algorithm sources
- 阶段状态：PASS_ORIGINAL_FLOW_GAP_MAP_DOC_ONLY
- 创建原因：用户询问当前 `ValetParkingStageParkingAdapter::Process()` 是否能完美复现原始 `E:\APA\DDS\TempAPA_Code` 的泊车入位流程；本阶段创建原始流程对照表，明确当前 DDS MVP 的已复现范围、轻量替代范围和缺失范围。

---

## 1. 本次完成事项

- [x] 对照原始流程文档：
  - `E:\APA\DDS\TempAPA_Code\00_ValetParkingStageParking_超详细流程图总览_重要.md`
- [x] 对照原始关键源码入口：
  - `planning/scenarios/valet_parking/stage_valet_parking_parking.cc`
  - `planning/scenarios/stage.cc`
  - `planning/tasks/optimizers/open_space_path_generation/open_space_path_provider.cc`
  - `planning/open_space/open_space_thread/open_space_thread_manager.h`
  - `planning/tasks/optimizers/open_space_path_generation/open_space_path_smoother.cc`
  - `planning/tasks/optimizers/open_space_straight_path/open_space_straight_path_provider.cc`
- [x] 新增差距对照文档：
  - `applications/source/Docs/valet_parking_magnadds/05_TempAPA原始流程复现差距对照表.md`
- [x] 新增决策记录：
  - `applications/source/Docs/valet_parking_magnadds/decision_records/DR-005_原始流程复现策略_行为等价优先.md`
- [x] 更新文档索引：
  - `applications/source/Docs/valet_parking_magnadds/README.md`

---

## 2. 结论

当前可以说：

```text
已复现原始普通 open space 主链路的核心算法流程：
ROI_DECIDER -> PATH_PROVIDER -> PATH_PARTITION -> SPEED_OPTIMIZER。
```

当前不能说：

```text
已完美复现 TempAPA_Code 的 ValetParkingStageParking 全流程。
```

原因是当前仍缺失或轻量替代：

- `FunctionManagerIn.sys_command -> parking_type` 映射。
- `IsParkingBrakeCondition` 的 pause/brake 语义。
- Stage `next_stage_`、`FinishScenario`、`IsReadyToFinishStage`。
- `DIRECT_FORWARD/DIRECT_BACKWARD` 直行分支。
- 完整 `Frame/OpenSpaceInfo/PlanningContext`。
- 完整 `OpenSpacePathProvider::PrePlan/PlanningOnPathThread`。
- `OpenSpaceThreadManager` 多 search thread + smooth thread。
- `OpenSpacePathSmoother/NLP`。
- Stage 发布时的 `parking_status/target_gear/trajectory_type` 收口。

---

## 3. 本阶段验证

本阶段为文档和架构决策阶段，没有修改 C++ 源码、IDL 或构建脚本。

执行的检查：

```bash
git diff --check
```

结果：PASS。

不重新编译 `.so`，因为本阶段没有代码变更。

---

## 4. 下一步

保留 NEXT-033：继续沿用简化 DDS Topic，验证 SelectedSlot 多车位列表中 `opt_parking_seq` 选择不同 `ParkingLot` 的行为。

完成 NEXT-033 后，建议进入：

```text
NEXT-034：轻量 Stage 控制输入设计
NEXT-035：DIRECT_FORWARD / DIRECT_BACKWARD 直行分支
NEXT-036：pause/brake/finish 行为等价 smoke
```

完整 `OpenSpacePathProvider` 大类、线程管理和 NLP smoother 继续作为后续单独阶段，不混入小步 smoke。
