# 项目状态快照 018：PATH_PROVIDER轻量运行态复用

- 快照编号：018
- 日期：2026-07-27
- 当前阶段：Fast-2 - `.so` + typed DDS + ROI_DECIDER/PATH_PROVIDER/PATH_PARTITION/SPEED_OPTIMIZER PATH_PROVIDER轻量运行态接入
- 阶段状态：PASS_PATH_PROVIDER_RUNTIME_REUSE_X86_M57_STATIC_BLOCKED_BOARD
- 创建原因：用户确认当前继续使用简化 DDS Topic，本阶段开始推进完整 `OpenSpacePathProvider` 的分阶段代码适配。第一步不直接搬完整大类，而是先接入可验证的历史路径复用与 replan 状态切片。

---

## 1. 本次完成事项

- [x] 对照 `E:\APA\DDS\Repair_ValetParkingStageParking_260430-main\planning` 中完整 `OpenSpacePathProvider` 的 `HasValidHistoryPath`、`CheckPathValid`、`UpdatePathStatus` 思路。
- [x] 在 `ValetParkingStageParkingAdapter::RuntimeContext` 中新增 `PathProviderRuntimeState`。
- [x] 保存上一帧有效 PATH_PROVIDER 输出、`path_id`、目标点、起点、障碍物签名、生成次数和复用次数。
- [x] `RunPathProvider()` 新增轻量 replan 决策：
  - 无历史路径 -> `NO_VALID_PATH`
  - 目标变化 -> `TARGET_UPDATE`
  - 障碍物签名变化 -> `BLOCK_BY_STATIC_OBSTACLE`
  - 当前起点无法匹配历史路径 -> `TRACE_REPLAN`
  - SPEED_OPTIMIZER 触发重规划 -> `REPLAN_FOR_SPEED_WARN`
  - 当前路径存在碰撞风险 -> `DYNAMIC_REPLAN`
- [x] 当目标、障碍物、起点和速度层状态均未变化时，复用上一帧有效 `OpenSpacePathOutput`。
- [x] 复用历史路径时清零本帧 `replan_status`，避免把上一帧 replan 状态误传给 `PATH_PARTITION`。
- [x] runner 状态日志新增：
  - `history=generated|reused`
  - `replan=...`
  - `reason=...`
  - `generated_count=...`
  - `reused_count=...`

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

本阶段不是完整 `OpenSpacePathProvider` 大类接入完成。

已接入的是轻量运行态切片：

- 历史路径缓存。
- 历史路径可复用判断。
- replan 状态位标记。
- 可观测状态日志。

仍未接入：

- 完整 `OpenSpacePathProvider` 线程管理。
- 完整 `PreCheck`。
- 完整 `SetPathStrategy`。
- 完整 warm start / splice path。
- NLP smoother。
- 完整 `Frame/DependencyInjector`。
- 真实车端 Topic 协议。

当前仍使用临时简化 Topic：

```text
/selected_slot
/planning/trajectory
/localization/estimate
/chassis/state
/perception/obstacles
```

---

## 3. 本次验证命令

x86 构建：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform x86 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_path_provider_runtime_018
```

x86 默认 smoke：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_path_provider_runtime_018/valet_parking_mvp/x86 \
  --domain-id 44 \
  --timeout-ms 20000 \
  --count 3 \
  --interval-ms 500
```

x86 辅助输入 all-valid：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_path_provider_runtime_018/valet_parking_mvp/x86 \
  --domain-id 43 \
  --timeout-ms 20000 \
  --count 3 \
  --interval-ms 500 \
  --with-aux-inputs \
  --aux-mode all-valid \
  --aux-count 3 \
  --aux-interval-ms 200
```

x86 辅助输入边界复验：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_path_provider_runtime_018/valet_parking_mvp/x86 \
  --domain-id 41 \
  --timeout-ms 20000 \
  --count 3 \
  --interval-ms 500 \
  --with-aux-inputs \
  --aux-mode bad-obstacle-geometry \
  --aux-count 3 \
  --aux-interval-ms 200
```

x86 关闭辅助输入订阅：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_path_provider_runtime_018/valet_parking_mvp/x86 \
  --domain-id 42 \
  --timeout-ms 20000 \
  --count 3 \
  --interval-ms 500 \
  --with-aux-inputs \
  --aux-mode all-valid \
  --aux-count 3 \
  --aux-interval-ms 200 \
  --disable-aux-input-topics
```

m57 交叉编译：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform m57 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_path_provider_runtime_018
```

---

## 4. 验收结果

| 验收项 | 结果 | 证据 |
|---|---|---|
| x86 构建 | PASS | `out/valet_parking_path_provider_runtime_018/valet_parking_mvp/x86/lib/libvalet_parking.so` |
| x86 默认 smoke | PASS | `domain_44_20260727_080556_1854` |
| PATH_PROVIDER 首帧生成 | PASS | 日志显示 `history=generated, replan=NO_VALID_PATH` |
| PATH_PROVIDER 后续复用 | PASS | 日志显示 `history=reused, replan=NONE` |
| x86 all-valid 辅助输入 | PASS | `domain_43_20260727_080248_1044`，`external_vehicle=true, external_obstacles=1` |
| x86 bad obstacle geometry | PASS | `domain_41_20260727_080226_750`，`aux obstacles rejected`，`external_obstacles=0` |
| x86 disable aux input topics | PASS | `domain_42_20260727_080226_743`，`external_vehicle=false, external_obstacles=0` |
| x86 轨迹输出 | PASS | subscriber 收到 `points=179, length=7.64356, is_estop=false` |
| m57 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_path_provider_runtime_018/valet_parking_mvp/m57/lib/libvalet_parking.so` |
| m57 ELF 架构 | PASS | `ELF 64-bit LSB shared object, ARM aarch64` |
| m57 MagnaDDS 依赖 | PASS | `libmagna-dds-core.so.1`、`libmagna-dds-impl.so` |
| m57 板端运行 | NOT_RUN | 当前无 m57 板端 |

---

## 5. 本阶段修改文件

- `applications/source/valet_parking/src/valet_parking_stage_parking_adapter.cpp`
- `applications/source/valet_parking/README.md`
- `applications/source/Docs/valet_parking_magnadds/STATUS.yaml`
- `applications/source/Docs/valet_parking_magnadds/status_snapshots/018_项目状态快照_PATH_PROVIDER轻量运行态复用.md`

---

## 6. 下一步

进入 NEXT-019：继续沿用简化 DDS Topic，接入 `OpenSpacePathProvider` 的下一层轻量切片。

建议优先级：

1. 对照剥离版 `open_space_path_provider.cc` 的 `PreCheck`。
2. 对照 `SetPathStrategy`，提取不依赖完整 `Frame/DependencyInjector` 的策略字段。
3. 梳理 warm start / splice path 能否在当前 `RuntimeContext` 中安全表达。
4. 继续保持 x86 smoke 和 m57 交叉编译通过。
5. 不直接引入完整线程管理、NLP smoother 或真实车端 Topic 协议。
