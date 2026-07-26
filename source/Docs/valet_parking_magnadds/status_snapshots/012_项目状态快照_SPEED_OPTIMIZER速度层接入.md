# 项目状态快照 012：SPEED_OPTIMIZER速度层接入

- 快照编号：012
- 日期：2026-07-26
- 当前阶段：Fast-2 - `.so` + typed DDS + ROI_DECIDER/PATH_PROVIDER/PATH_PARTITION/SPEED_OPTIMIZER 最小闭环
- 阶段状态：PASS_SPEED_OPTIMIZER_X86_M57_STATIC_BLOCKED_BOARD
- 创建原因：用户要求继续按计划从 `PATH_PARTITION` 往后接 `SPEED_OPTIMIZER`，并在阶段完成后更新文档和推送 `applications` Git。

---

## 1. 本次完成事项

- [x] 在 `ValetParkingStageParkingAdapter` 中接入 standalone `OpenSpaceSpeedOptimizer::Execute`。
- [x] 当前主链路更新为：

```text
SelectedSlot
  -> ROI_DECIDER
  -> PATH_PROVIDER
  -> PATH_PARTITION
  -> SPEED_OPTIMIZER
  -> PlanningTrajectory
```

- [x] 为 SPEED_OPTIMIZER 构造最小 `SpeedOptimizerInput`：
  - `discretized_path` 来自 PATH_PARTITION 的 `chosen_partitioned_path.first`
  - `gear` 来自 PATH_PARTITION 的 `chosen_partitioned_path.second`
  - `is_gear_changed`、`is_stop_path`、`partitioned_paths` 来自 PATH_PARTITION 输出
  - `vehicle_state` 和 `start_point` 暂时仍来自 fake vehicle 配置
  - `parking_scenario_type`、`is_parking_inwards` 来自 ROI_DECIDER 输出
  - 暂不注入真实静态/动态障碍物
- [x] 将 SPEED_OPTIMIZER 输出的 `DiscretizedTrajectory + gear` 转换为 DDS `PlanningTrajectory`。
- [x] SPEED_OPTIMIZER 失败时保留回退：
  - 回退到 PATH_PARTITION 的 nominal speed 轨迹
  - `replan_reason` 写明 SPEED_OPTIMIZER 失败原因和 fallback
- [x] 补齐速度层最小 compat：
  - `FLAGS_fallback_time_unit`
  - `FLAGS_numerical_epsilon`
- [x] CMake 纳入 standalone 速度层源文件：
  - `SpeedData`
  - `DiscretizedTrajectory`
  - `PiecewiseAccelerationTrajectory1d`
  - `PathHandle`
  - `StSampleCost`
  - `StSampleCurves`
  - `OpenSpaceSpeedOptimizer`

---

## 2. 重要边界

本阶段接入的是已经独立化的 `OpenSpaceSpeedOptimizer`，不是完整原车 `Frame/DependencyInjector` 运行环境。

当前仍未接入：

- 完整 `OpenSpacePathProvider` 大类。
- NLP smoother。
- 跨帧 `history_path` 和 speed optimizer `last_curve` 持久化；当前 adapter 每次处理时创建并 reset PATH_PARTITION/SPEED_OPTIMIZER。
- 真实定位/底盘输入 Topic。
- 真实障碍物输入 Topic。
- m57 板端运行验证。

---

## 3. 本次验证命令

x86：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform x86 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_speed_optimizer_012
```

x86 DDS 冒烟：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_speed_optimizer_012/valet_parking_mvp/x86 \
  --domain-id 14 \
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
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_speed_optimizer_012
```

---

## 4. 验收结果

| 验收项 | 结果 | 证据 |
|---|---|---|
| x86 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_speed_optimizer_012/valet_parking_mvp/x86/lib/libvalet_parking.so` |
| x86 ELF 架构 | PASS | `ELF 64-bit LSB shared object, x86-64` |
| x86 MagnaDDS 依赖 | PASS | `readelf -d` 显示 `libmagna-dds-core.so.1` |
| m57 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_speed_optimizer_012/valet_parking_mvp/m57/lib/libvalet_parking.so` |
| m57 ELF 架构 | PASS | `ELF 64-bit LSB shared object, ARM aarch64` |
| m57 MagnaDDS 依赖 | PASS | `readelf -d` 显示 `libmagna-dds-core.so.1` 和 `libmagna-dds-impl.so` |
| x86 DDS 冒烟 | PASS | subscriber 收到 `points=179, length=7.64356, is_estop=false` |
| SPEED_OPTIMIZER 是否生效 | PASS | runner 日志显示 `SPEED_OPTIMIZER ok, points=179, duration=17.800, distance=7.644` |
| m57 板端运行 | NOT_RUN | 当前无 m57 板端 |

---

## 5. 下一步

继续下一阶段：运行态硬化。

建议先做：

- 把 PATH_PARTITION 和 SPEED_OPTIMIZER 需要的跨帧状态放进 adapter/component 的 runtime context。
- 梳理真实定位、底盘、障碍物输入 Topic，明确哪些字段替换当前 fake vehicle 和空障碍物。
- 保留当前 mock SelectedSlot 冒烟能力，作为每次改动后的最小回归。
