# 项目状态快照 011：PATH_PARTITION任务级收口接入

- 快照编号：011
- 日期：2026-07-26
- 当前阶段：Fast-2 - `.so` + typed DDS + ROI_DECIDER/PATH_PROVIDER/PATH_PARTITION 最小闭环
- 阶段状态：PASS_PATH_PARTITION_X86_M57_STATIC_BLOCKED_BOARD
- 创建原因：用户要求继续按计划从 `PATH_PROVIDER` 往后接 `PATH_PARTITION`，并在阶段完成后更新文档和推送 `applications` Git。

---

## 1. 本次完成事项

- [x] 在 `ValetParkingStageParkingAdapter` 中接入 standalone `OpenSpacePathPartition::Execute`。
- [x] 当前主链路更新为：

```text
SelectedSlot
  -> ROI_DECIDER
  -> PATH_PROVIDER
  -> PATH_PARTITION
  -> PlanningTrajectory
```

- [x] 将 PATH_PROVIDER 输出的分段路径先正规化：
  - 为每段 `DiscretizedPath` 重建递增 `s`
  - 保留 x/y/theta/kappa/gear
  - 重建 `DiscretizedPath` 内部累计距离和线段缓存
- [x] 构造最小 `PartitionInput`：
  - `vehicle_state` 和 `planning_start_point` 暂时仍来自 fake vehicle 配置
  - `path_result.path_set` 来自正规化后的 PATH_PROVIDER 分段路径
  - `parking_scenario_type`、`is_parking_inwards`、`end_pose`、`dest_region_with_angle`、`obstacles_segments_vec` 来自 ROI_DECIDER 输出
- [x] PATH_PARTITION 成功时使用 `chosen_partitioned_path` 生成 DDS `PlanningTrajectory`。
- [x] PATH_PARTITION 失败时保留回退：
  - 回退到 PATH_PROVIDER 全路径
  - `replan_reason` 写明 PATH_PARTITION 失败原因和 fallback
- [x] 新增 x86 DDS 冒烟脚本：
  - `applications/source/valet_parking_tools/smoke_valet_parking_x86.sh`
  - 自动编排 runner、subscriber、publisher
  - 自动清理后台进程并输出关键日志

---

## 2. 重要边界

本阶段接入的是 `OpenSpacePathPartition` 的独立编译版本，用来做任务级路径仲裁和当前执行段选择。

它和 PATH_PROVIDER 内部的 `PathGenerator::PathPartition` 不是同一个层级：

- `PathGenerator::PathPartition`：把 Hybrid A* 粗路径按挡位切成若干段。
- `OpenSpacePathPartition::Execute`：从分段路径里选择当前应执行的 `chosen_partitioned_path`，同时产出 path decision、finish status、stop path、gear change 等任务级状态。

当前仍未接入：

- 完整 `OpenSpacePathProvider` 大类。
- NLP smoother。
- `SPEED_OPTIMIZER`。
- 跨帧 `history_path` 持久化；当前 adapter 每次处理时创建并 reset `OpenSpacePathPartition`，所以本阶段只验证当前帧的新路径仲裁。
- 真实定位/底盘输入 Topic。
- m57 板端运行验证。

---

## 3. 本次验证命令

x86：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform x86 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_path_partition_011
```

m57：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform m57 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_path_partition_011
```

x86 DDS 冒烟：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_path_partition_011/valet_parking_mvp/x86 \
  --domain-id 13 \
  --timeout-ms 20000 \
  --count 3 \
  --interval-ms 500
```

---

## 4. 验收结果

| 验收项 | 结果 | 证据 |
|---|---|---|
| x86 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_path_partition_011/valet_parking_mvp/x86/lib/libvalet_parking.so` |
| x86 ELF 架构 | PASS | `ELF 64-bit LSB shared object, x86-64` |
| x86 MagnaDDS 依赖 | PASS | `readelf -d` 显示 `libmagna-dds-core.so.1` |
| m57 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_path_partition_011/valet_parking_mvp/m57/lib/libvalet_parking.so` |
| m57 ELF 架构 | PASS | `ELF 64-bit LSB shared object, ARM aarch64` |
| m57 MagnaDDS 依赖 | PASS | `readelf -d` 显示 `libmagna-dds-core.so.1` 和 `libmagna-dds-impl.so` |
| x86 DDS 冒烟 | PASS | subscriber 收到 `points=99, length=7.64359, is_estop=false` |
| PATH_PARTITION 是否生效 | PASS | runner 日志显示 `PATH_PARTITION ok, decision=4, chosen_points=99, stop_path=false` |
| m57 板端运行 | NOT_RUN | 当前无 m57 板端 |

---

## 5. 下一步

继续下一阶段：`SPEED_OPTIMIZER`。

执行原则：

- 继续沿用当前 DDS Topic 和 adapter 分层。
- SPEED_OPTIMIZER 输入优先来自 `PATH_PARTITION` 的 `chosen_partitioned_path`。
- 如果完整速度层依赖过重，先接入可独立编译的最小速度规划能力，并保留当前 nominal speed fallback。
- 每完成一个阶段继续更新文档并提交推送 `applications` Git。
