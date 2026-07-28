# NEXT-038 完整 OpenSpaceStraightPathProvider / OPEN_SPACE_STRAIGHT_PATH 接入说明

本文记录本阶段把 `DIRECT_FORWARD / DIRECT_BACKWARD` 从旧的轻量短直线 shortcut，升级为本地化 `OpenSpaceStraightPathProvider` 语义链路。

## 一句话解释

以前 direct 命令像是“adapter 自己画一条短直线然后直接发出去”。

现在 direct 命令变成：

```text
ParkingCommand DIRECT_FORWARD / DIRECT_BACKWARD
-> OPEN_SPACE_STRAIGHT_PATH
-> SPEED_OPTIMIZER
-> PlanningTrajectory
```

也就是说，直行分支不再绕开速度优化器。它会先生成原始 `OpenSpaceStraightPathProvider` 语义里的直线路径或停车路径，再交给现有 `OpenSpaceSpeedOptimizer` 生成最终可发布轨迹。

## 为什么不是直接复制原车类

原车 `OpenSpaceStraightPathProvider` 的源码依赖：

- `Frame`
- `DependencyInjector`
- `PlanningContext`
- `OpenSpaceInfo`
- `TaskConfig`
- 原始 `Stage::ExecuteTaskOnOpenSpace`

当前 MagnaDDS MVP 的架构刻意不搬完整 `Frame/DependencyInjector`，因为这样会把半个原车 planning 框架带进来，风险比收益大。

所以本阶段采用“行为语义本地化”：

- 保留原始 provider 的关键判断和输出语义。
- 使用当前 adapter 已经有的轻量输入结构构造 provider 输入。
- 输出复用现有 `PartitionOutput` / `SpeedOptimizerInput` 这条本地算法闭包。

## 这次真正接入了什么

新增本地算法文件：

```text
applications/source/valet_parking/algorithm/parking_algorithm_standalone/planning/tasks/optimizers/open_space_straight_path/open_space_straight_path_provider.h
applications/source/valet_parking/algorithm/parking_algorithm_standalone/planning/tasks/optimizers/open_space_straight_path/open_space_straight_path_provider.cc
```

它保留的原始语义包括：

- `DIRECT_FORWARD` 目标挡位为 `GEAR_DRIVE`。
- `DIRECT_BACKWARD` 目标挡位为 `GEAR_REVERSE`。
- 当前挡位和目标挡位不一致时，生成 `stop_path`。
- 当前速度方向和移动方向冲突且车辆非静止时，生成 `stop_path`。
- direct 模式变化时，重新记录 `original_point_` 和 `original_direction_`。
- direct 模式不变时，沿首次 direct 时记录的方向继续生成直线路径。
- 直线路径按 0.1m 间隔插值。
- 生成结果写入 `chosen_partitioned_path`，再转成 `PartitionOutput`。

## Adapter 中的新流程

文件：

```text
applications/source/valet_parking/src/valet_parking_stage_parking_adapter.cpp
```

DIRECT 命令现在这样走：

```text
收到 ParkingCommand DIRECT_FORWARD / DIRECT_BACKWARD
-> 构造 OpenSpaceStraightPathInput
-> OpenSpaceStraightPathProvider::Process()
-> 转成 PartitionOutput
-> RunSpeedOptimizer()
-> BuildTrajectoryFromSpeedOptimizer()
-> 发布 PlanningTrajectory
```

普通泊车主链路仍然不变：

```text
SelectedSlot
-> ROI_DECIDER
-> PATH_PROVIDER
-> PATH_PARTITION
-> SPEED_OPTIMIZER
-> PlanningTrajectory
```

PAUSE / BRAKE / FINISH 也保持上一阶段行为：输出非 estop 单点停止轨迹。

## 和原始 Stage 的关系

原始 `Stage::ExecuteTaskOnOpenSpace` 在 direct 模式下会：

```text
跳过 OPEN_SPACE_ROI_DECIDER
跳过 OPEN_SPACE_PATH_PROVIDER
跳过 OPEN_SPACE_PATH_PARTITION
执行 OPEN_SPACE_STRAIGHT_PATH
继续执行后续 OPEN_SPACE_SPEED_OPTIMIZER
```

本阶段已对齐这个关键顺序：

```text
skip=ROI_PATH_PROVIDER_PATH_PARTITION
task=OPEN_SPACE_STRAIGHT_PATH
SPEED_OPTIMIZER ok
```

日志中会看到这些字段，作为 smoke 验证证据。

## 需要诚实保留的边界

这次不是完整恢复原车 `Frame/DependencyInjector` 框架，因此仍有边界：

- `PlanningContext.avp_status.parking_type` 暂时由 `ParkingCommand.mode` 映射。
- `Frame::PlanningStartPoint()` 由 adapter 内部的 `BuildStartPathPoint()` 构造。
- `Frame::vehicle_state()` 由 DDS 辅助输入或 fake vehicle 构造。
- `OpenSpaceInfo` 没有整类接入，而是映射到 `PartitionOutput` 和 `SpeedOptimizerInput`。
- `direct_distance_m` 已映射为 straight path 长度。
- NEXT-038 完成时，`direct_speed_mps` 只进入日志诊断；NEXT-039 已把它映射到 direct 分支的速度优化器 speed bound。正式车端 command 协议明确前，这仍属于临时 Topic 下的本地化行为。
- m57 只完成交叉编译、ELF 和依赖检查，没有板端运行验证。

## 新增验证能力

`aux_input_mock_publisher` 新增：

```bash
--chassis-gear parking|drive|reverse|neutral
```

`smoke_valet_parking_x86.sh` 新增：

```bash
--aux-chassis-gear parking|drive|reverse|neutral
```

这样可以验证两种 direct 行为：

1. 默认 P 档时，direct 命令触发挡位保护，输出 `OPEN_SPACE_STRAIGHT_PATH stop_path`。
2. 显式喂正确挡位时，direct 命令输出 `OPEN_SPACE_STRAIGHT_PATH direct_moving`。

## 本阶段通过的关键验证

x86 构建：

```text
out/valet_parking_straight_provider_038/valet_parking_mvp/x86/lib/libvalet_parking.so
```

x86 smoke：

```text
domain_220_20260728_192215_1636  valid 普通主链路
domain_221_20260728_192231_1762  direct-forward 默认 P 档 stop_path
domain_222_20260728_192249_1921  direct-backward 默认 P 档 stop_path
domain_223_20260728_192613_952   direct-forward + DRIVE 挡位 direct_moving
domain_224_20260728_192628_1127  direct-backward + REVERSE 挡位 direct_moving
```

m57 交叉编译：

```text
out/valet_parking_straight_provider_038/valet_parking_mvp/m57/lib/libvalet_parking.so
```

检查结果：

```text
ELF: ARM aarch64
NEEDED: libmagna-dds-core.so.1
NEEDED: libmagna-dds-impl.so
```

## 下一步建议

下一阶段优先做 direct 分支的细节硬化，而不是立刻扩大到完整 Stage 框架：

- 已进入 NEXT-039：把 `direct_speed_mps` 映射到速度优化器配置。
- 已进入 NEXT-039：增加速度方向冲突的 smoke，用非零反向速度验证 `velocity_direction_conflict`。
- 评估是否把 `OPEN_SPACE_STRAIGHT_PATH` 的输出接入更完整的 Stage 输出字段，例如 trajectory_type、target gear、parking status。
- 继续保持普通主链路和 direct 分支的 x86 smoke + m57 交叉编译双验证。
