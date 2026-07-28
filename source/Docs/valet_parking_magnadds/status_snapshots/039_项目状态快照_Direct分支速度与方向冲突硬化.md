# 项目状态快照 039：Direct 分支速度与方向冲突硬化

## 阶段结论

NEXT-039 已完成。

本阶段继续硬化 `OPEN_SPACE_STRAIGHT_PATH` direct 分支，没有扩大到完整 `Stage/Frame/DependencyInjector`。核心变化是：

```text
ParkingCommand DIRECT_FORWARD / DIRECT_BACKWARD
-> OPEN_SPACE_STRAIGHT_PATH
-> direct speed bound config
-> SPEED_OPTIMIZER
-> PlanningTrajectory
```

## 本阶段修订

- `direct_speed_mps` 不再只是日志字段。
- direct 分支会基于 `direct_speed_mps` 构造本次局部 `OpenSpaceSpeedOptimizerConfig`：
  - `direct_speed_bound_max = SelectDirectSpeed(direct_speed_mps)`
  - `direct_speed_bound_min = max(0.05, direct_speed_bound_max * 0.6)`
  - 同步写入 `apa_speed_bound_info` 和 `rpa_direct_speed_bound_info`
- `SpeedOptimizerInput.is_rpa_direct_mode` 在 direct 分支置为 `true`。
- direct 输出原因中新增：
  - `target_gear`
  - `direct_speed_bound_min`
  - `direct_speed_bound_max`
  - `trajectory_type=NORMAL`
  - `parking_status=direct_moving/direct_stop_path`
- `aux_input_mock_publisher` 新增：

```bash
--chassis-speed-mps=<value>
```

- `smoke_valet_parking_x86.sh` 新增：

```bash
--aux-chassis-speed-mps VALUE
```

并在 direct smoke 中自动断言：

- 已出现 `direct_speed_bound_max`
- 速度方向冲突时出现 `velocity_direction_conflict`
- 速度方向冲突时发布 0 长度 stop path

## 后续 10 个任务预测

新增文档：

```text
applications/source/Docs/valet_parking_magnadds/09_NEXT039到048后续任务预测与执行节奏.md
```

该文档明确后续 NEXT-039 到 NEXT-048 的路线，并固定“先修订、再调试排查”的节奏。

## 验证结果

x86/m57 构建：

```text
out/valet_parking_direct_hardening_039/valet_parking_mvp/x86/lib/libvalet_parking.so
out/valet_parking_direct_hardening_039/valet_parking_mvp/m57/lib/libvalet_parking.so
```

m57 静态检查：

```text
ELF: ARM aarch64
NEEDED: libmagna-dds-core.so.1
NEEDED: libmagna-dds-impl.so
```

x86 smoke：

```text
domain_130_20260728_212316_2588  valid 普通主链路 PASS
domain_131_20260728_212336_2729  direct-forward + DRIVE + direct_speed=1.2 direct_moving PASS
domain_132_20260728_212407_647   direct-forward + DRIVE + speed=-0.2 velocity_direction_conflict PASS
domain_133_20260728_212423_875   direct-backward + REVERSE + speed=0.2 velocity_direction_conflict PASS
domain_134_20260728_212439_1049  direct-backward + REVERSE + direct_speed=1.2 direct_moving PASS
```

关键日志证据：

```text
direct_speed=1.200
direct_speed_bound_min=0.720
direct_speed_bound_max=1.200
parking_status=direct_moving
parking_status=direct_stop_path
velocity_direction_conflict velocity=-0.2, moving_direction=1
velocity_direction_conflict velocity=0.2, moving_direction=-1
```

`direct_speed=1.2` 后，direct moving 轨迹从 NEXT-038 的 87 点变为 79 点、duration 从 8.6s 变为 7.8s，说明速度边界确实进入了速度优化器，而不是只写进日志。

## 输出契约边界

当前 IDL `PlanningTrajectory` 没有正式 `parking_status` 字段。

因此本阶段的 `parking_status=direct_moving/direct_stop_path` 仍写在 `replan_reason` / `estop.reason` 诊断文本中，不能把它说成已经对齐真实车端协议。后续 NEXT-040 需要继续把 direct 分支输出契约标准化。

`trajectory_type=NORMAL` 也是当前 direct 分支经 `SPEED_OPTIMIZER` 输出后的诊断文本说明；正式字段仍是 `PlanningTrajectory.trajectory_type`，subscriber 中显示为 `trajectory_type=1`。

## 当前状态

```text
PASS_DIRECT_HARDENING_X86_M57_STATIC_BLOCKED_BOARD
```

m57 仍只完成交叉编译、ELF 和依赖检查。当前没有 m57 板端，不能标记板端运行通过。

## 下一步

进入 NEXT-040：继续 direct 分支输出契约补强。

重点是把 `target_gear / trajectory_type / parking_status / direct speed bound / stop reason` 的诊断格式稳定下来，并继续明确哪些只是临时 Topic 的诊断文本，哪些才是正式 DDS 字段。
