# 项目状态快照 019：PATH_PROVIDER轻量策略与warm_start切片

- 快照编号：019
- 日期：2026-07-27
- 当前阶段：Fast-2 - `.so` + typed DDS + ROI_DECIDER/PATH_PROVIDER/PATH_PARTITION/SPEED_OPTIMIZER PATH_PROVIDER轻量策略切片接入
- 阶段状态：PASS_PATH_PROVIDER_STRATEGY_SLICE_X86_M57_STATIC_BLOCKED_BOARD
- 创建原因：在 NEXT-018 已完成 PATH_PROVIDER 轻量运行态复用后，继续对照剥离版 `OpenSpacePathProvider` 的 `SetWarmStartPath`、`SetPathStrategy` 和 splice path 思路，接入不依赖完整 `Frame/DependencyInjector` 的可验证代码切片。

---

## 1. 本次完成事项

- [x] 对照 `E:\APA\DDS\Repair_ValetParkingStageParking_260430-main\planning\tasks\optimizers\open_space_path_generation\open_space_path_provider.cc` 中的 `UpdateReplanInfo`、`LoadOptimizerData`、`SetWarmStartPath`、`SetPathStrategy`。
- [x] 在 `ValetParkingStageParkingAdapter` 中新增 `BuildWarmStartPathFromHistory()`，能从上一帧 `OpenSpacePathOutput` 中投影当前起点并截取历史尾段作为 `OpenSpacePathInput::warm_start_path`。
- [x] 新增保守启用条件：只有同目标、同障碍物、历史路径存在、且 replan 原因仅为 `TRACE_REPLAN` 时才允许使用历史 warm start。
- [x] 明确禁止在以下情况把旧路径作为 warm start fallback：
  - `TARGET_UPDATE`
  - `BLOCK_BY_STATIC_OBSTACLE`
  - `DYNAMIC_REPLAN`
  - `REPLAN_FOR_SPEED_WARN`
- [x] 新增轻量 `ApplyPathProviderStrategy()`，对照 `SetPathStrategy` 写入当前可安全表达的字段：
  - `init_moving_direction`
  - `init_path_direction`
  - `enable_init_kappa_cost`
  - `limit_init_steer_margin`
  - `is_plan_from_start`
  - `space_structure`
  - `park_direction`
- [x] runner 状态日志新增：
  - `warm_start=none|history_splice|not_applied`
  - `warm_start_points=...`
  - `splice_points=...`
  - `strategy_init_move=...`
  - `strategy_init_path=...`
  - `strategy_kappa_cost=...`
  - `strategy_limit_steer=...`
  - `strategy_disable_search=...`

当前主链路保持不变：

```text
SelectedSlot
  -> ROI_DECIDER
  -> PATH_PROVIDER
  -> PATH_PARTITION
  -> SPEED_OPTIMIZER
  -> PlanningTrajectory
```

---

## 2. 重要边界

本阶段仍不是完整 `OpenSpacePathProvider` 大类接入完成。

已接入的是轻量策略切片：

- 历史路径 warm start 构造。
- 历史路径 warm start 的保守启用条件。
- 一组不依赖完整 `Frame/DependencyInjector` 的 path strategy 字段。
- 可观测状态日志。

仍未接入：

- 完整 `OpenSpacePathProvider` 线程管理。
- 完整 `PreCheck`。
- 完整 trace adjust 策略。
- 完整 splice trajectory 与上一帧发布轨迹的时间拼接。
- NLP smoother。
- 完整 `Frame/DependencyInjector`。
- 真实车端 Topic 协议。

当前 mock 工具没有移动定位模式，因此默认 smoke 不会直接触发 `warm_start=history_splice` 分支。本阶段对该分支完成了编译接线、保守条件限制和日志落点，但直接运行态触发验证留到 NEXT-020。

---

## 3. 本次验证命令

x86 构建：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform x86 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_path_provider_strategy_019
```

x86 默认 smoke：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_path_provider_strategy_019/valet_parking_mvp/x86 \
  --domain-id 119 \
  --timeout-ms 20000 \
  --count 3 \
  --interval-ms 500
```

x86 辅助输入 all-valid：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_path_provider_strategy_019/valet_parking_mvp/x86 \
  --domain-id 120 \
  --timeout-ms 20000 \
  --count 3 \
  --interval-ms 500 \
  --with-aux-inputs \
  --aux-mode all-valid
```

x86 辅助输入 bad-obstacle-geometry：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_path_provider_strategy_019/valet_parking_mvp/x86 \
  --domain-id 121 \
  --timeout-ms 20000 \
  --count 3 \
  --interval-ms 500 \
  --with-aux-inputs \
  --aux-mode bad-obstacle-geometry
```

x86 关闭辅助输入订阅：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_path_provider_strategy_019/valet_parking_mvp/x86 \
  --domain-id 122 \
  --timeout-ms 20000 \
  --count 3 \
  --interval-ms 500 \
  --with-aux-inputs \
  --aux-mode all-valid \
  --disable-aux-input-topics
```

m57 交叉编译：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform m57 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_path_provider_strategy_019
```

---

## 4. 验收结果

| 验收项 | 结果 | 证据 |
|---|---|---|
| x86 构建 | PASS | `out/valet_parking_path_provider_strategy_019/valet_parking_mvp/x86/lib/libvalet_parking.so` |
| x86 默认 smoke | PASS | `domain_119_20260727_092611_1455` |
| PATH_PROVIDER 首帧生成 | PASS | 日志显示 `history=generated, replan=NO_VALID_PATH, warm_start=none` |
| PATH_PROVIDER 后续复用 | PASS | 日志显示 `history=reused, replan=NONE, warm_start=not_applied` |
| PATH_PROVIDER 轻量策略字段 | PASS | 日志显示 `strategy_init_move=0, strategy_kappa_cost=false, strategy_limit_steer=false` |
| x86 all-valid 辅助输入 | PASS | `domain_120_20260727_092631_1597`，`external_vehicle=true, external_obstacles=1` |
| x86 bad obstacle geometry | PASS | `domain_121_20260727_092649_1748`，`aux obstacles rejected`，`external_obstacles=0` |
| x86 disable aux input topics | PASS | `domain_122_20260727_092747_678`，`external_vehicle=false, external_obstacles=0` |
| x86 轨迹输出 | PASS | subscriber 收到 `points=179, length=7.64356, is_estop=false` |
| m57 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_path_provider_strategy_019/valet_parking_mvp/m57/lib/libvalet_parking.so` |
| m57 ELF 架构 | PASS | `ELF 64-bit LSB shared object, ARM aarch64` |
| m57 MagnaDDS 依赖 | PASS | `libmagna-dds-core.so.1`、`libmagna-dds-impl.so` |
| m57 板端运行 | NOT_RUN | 当前无 m57 板端 |

---

## 5. 本阶段修改文件

- `applications/source/valet_parking/src/valet_parking_stage_parking_adapter.cpp`
- `applications/source/valet_parking/README.md`
- `applications/source/Docs/valet_parking_magnadds/STATUS.yaml`
- `applications/source/Docs/valet_parking_magnadds/status_snapshots/019_项目状态快照_PATH_PROVIDER轻量策略与warm_start切片.md`

---

## 6. 下一步

进入 NEXT-020：继续沿用简化 DDS Topic，推进 `OpenSpacePathProvider` 下一层轻量切片。

建议优先级：

1. 评估 `PreCheck` 中不依赖完整 `Frame/DependencyInjector` 的输入有效性检查，优先迁入 adapter。
2. 给 mock/smoke 增加可控移动定位场景，直接触发 `TRACE_REPLAN` 与 `warm_start=history_splice`。
3. 继续保持 x86 smoke、m57 交叉编译、现有辅助输入边界全部通过。
4. 不直接引入完整线程管理、NLP smoother 或真实车端 Topic 协议。
