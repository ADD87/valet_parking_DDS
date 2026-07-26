# 项目状态快照 013：RUNTIME_CONTEXT运行态硬化

- 快照编号：013
- 日期：2026-07-26
- 当前阶段：Fast-2 - `.so` + typed DDS + ROI_DECIDER/PATH_PROVIDER/PATH_PARTITION/SPEED_OPTIMIZER 运行态最小闭环
- 阶段状态：PASS_RUNTIME_CONTEXT_X86_M57_STATIC_BLOCKED_BOARD
- 创建原因：在 SPEED_OPTIMIZER 接入后，继续把 adapter 从“一帧一次性创建任务对象”推进到“进程生命周期内保留最小运行态”。

---

## 1. 本次完成事项

- [x] 在 `ValetParkingStageParkingAdapter` 中新增 `RuntimeContext`。
- [x] `RuntimeContext` 创建时初始化并复用：
  - `OpenSpacePathPartition`
  - `OpenSpaceSpeedOptimizer`
  - EP30 `VehicleParam`
  - `OpenSpacePathPartitionConfig`
  - `OpenSpaceSpeedOptimizerConfig`
- [x] `PATH_PARTITION` 不再每帧新建并 reset，而是复用 runtime context 内的对象。
- [x] `SPEED_OPTIMIZER` 不再每帧新建并 reset，而是复用 runtime context 内的对象，使 `last_curve_` 能跨帧保留。
- [x] 将上一帧运行态回灌给下一帧：
  - 上一帧发布档位 `last_published_gear`
  - 上一帧 speed collision info
  - 上一帧 speed replan 标志
  - 上一帧 `last_frame_timestamp`
  - 上一帧 `last_planning_start_relative_time`
- [x] 非法 SelectedSlot、ROI 失败、PATH_PARTITION 失败、PATH_PROVIDER 失败时会 reset runtime state，避免陈旧状态污染下一次规划。
- [x] component 调用链从 `const Process()` 调整为可更新运行态的 `Process()`。

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

本阶段是运行态硬化，不是新增算法层。

已改善：

- adapter 进程生命周期内保留 PATH_PARTITION history_path。
- adapter 进程生命周期内保留 SPEED_OPTIMIZER last_curve。
- 第二帧开始可看到 `last_frame=true`。
- PATH_PARTITION 输入可收到上一帧 speed collision/replan 状态。

仍未完成：

- 真实定位/底盘 Topic 尚未接入，`VehicleState` 仍来自 fake vehicle config。
- 真实静态/动态障碍物 Topic 尚未接入。
- standalone `OpenSpaceSpeedOptimizer::UpdateSampleParams` 里对 `last_curve_` 的时间平移当前仍使用 0.1s fallback，尚未真正使用 `CalDiffTimeFromLast(input)`。
- 未接完整原车 `Frame/DependencyInjector`。
- 未接完整 `OpenSpacePathProvider` 大类。
- 未接 NLP smoother。
- m57 板端 runtime 未验证。

---

## 3. 本次验证命令

x86：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform x86 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_runtime_context_013
```

x86 DDS 冒烟：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_runtime_context_013/valet_parking_mvp/x86 \
  --domain-id 15 \
  --timeout-ms 20000 \
  --count 3 \
  --interval-ms 500
```

m57：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform m57 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_runtime_context_013
```

---

## 4. 验收结果

| 验收项 | 结果 | 证据 |
|---|---|---|
| x86 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_runtime_context_013/valet_parking_mvp/x86/lib/libvalet_parking.so` |
| x86 ELF 架构 | PASS | `ELF 64-bit LSB shared object, x86-64` |
| x86 MagnaDDS 依赖 | PASS | `readelf -d` 显示 `libmagna-dds-core.so.1` |
| x86 DDS 冒烟 | PASS | subscriber 收到 `points=179, length=7.64356, is_estop=false` |
| runtime context 是否生效 | PASS | runner 第一帧 `last_frame=false`，第二帧 `last_frame=true` |
| m57 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_runtime_context_013/valet_parking_mvp/m57/lib/libvalet_parking.so` |
| m57 ELF 架构 | PASS | `ELF 64-bit LSB shared object, ARM aarch64` |
| m57 MagnaDDS 依赖 | PASS | `readelf -d` 显示 `libmagna-dds-core.so.1` 和 `libmagna-dds-impl.so` |
| m57 板端运行 | NOT_RUN | 当前无 m57 板端 |

---

## 5. 下一步

继续下一阶段：真实输入接口梳理。

建议先做：

- 定义真实定位 Topic 到 `VehicleState` 的字段映射，替换当前 fake vehicle pose。
- 定义底盘/档位/车速输入如何进入 `RuntimeContext`。
- 定义静态/动态障碍物 Topic 如何进入 `SpeedOptimizerInput.static_obstacles` 和 `moving_obstacles`。
- 保留当前 mock SelectedSlot 冒烟脚本，作为每次改动后的最小回归。
