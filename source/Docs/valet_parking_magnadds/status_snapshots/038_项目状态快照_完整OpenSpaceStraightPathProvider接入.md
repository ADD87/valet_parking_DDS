# 项目状态快照 038：完整 OpenSpaceStraightPathProvider / OPEN_SPACE_STRAIGHT_PATH 接入

日期：2026-07-28

## 本阶段结论

NEXT-038 已完成。

`DIRECT_FORWARD / DIRECT_BACKWARD` 已从旧的 adapter 轻量短直线分支，升级为：

```text
ParkingCommand
-> OPEN_SPACE_STRAIGHT_PATH
-> SPEED_OPTIMIZER
-> PlanningTrajectory
```

普通泊车主链路保持：

```text
SelectedSlot
-> ROI_DECIDER
-> PATH_PROVIDER
-> PATH_PARTITION
-> SPEED_OPTIMIZER
-> PlanningTrajectory
```

## 代码变更

新增：

- `applications/source/valet_parking/algorithm/parking_algorithm_standalone/planning/tasks/optimizers/open_space_straight_path/open_space_straight_path_provider.h`
- `applications/source/valet_parking/algorithm/parking_algorithm_standalone/planning/tasks/optimizers/open_space_straight_path/open_space_straight_path_provider.cc`

修改：

- `applications/source/valet_parking/CMakeLists.txt`
- `applications/source/valet_parking/src/compat/planning/common/planning_gflags.h`
- `applications/source/valet_parking/src/valet_parking_stage_parking_adapter.cpp`
- `applications/source/valet_parking_tools/aux_input_mock_publisher/main.cpp`
- `applications/source/valet_parking_tools/smoke_valet_parking_x86.sh`
- `applications/source/Docs/valet_parking_magnadds/08_完整OpenSpaceStraightPathProvider与038接入说明.md`
- `applications/source/Docs/valet_parking_magnadds/STATUS.yaml`

## 已对齐原始语义

- direct 模式跳过 ROI / PathProvider / PathPartition。
- direct 模式执行 `OPEN_SPACE_STRAIGHT_PATH`。
- direct 输出继续进入 `SPEED_OPTIMIZER`。
- 目标挡位不一致时生成 stop path。
- 速度方向冲突且车辆非静止时生成 stop path。
- direct 模式切换时重置原点和方向。
- direct 模式稳定时复用原始方向生成直线路径。
- 直线路径按 0.1m 步长生成。

## 仍未恢复的原车框架

- 未接入完整 `Frame`。
- 未接入完整 `DependencyInjector`。
- 未接入完整 `PlanningContext`。
- 未接入完整 `OpenSpaceInfo`。
- 未接入完整 `FunctionManager`。
- 未接入真实车端 command 协议。
- m57 未做板端运行。

## 验证证据

静态检查：

```text
git diff --check: PASS
bash -n smoke_valet_parking_x86.sh: PASS
```

x86 构建：

```text
out/valet_parking_straight_provider_038/valet_parking_mvp/x86/lib/libvalet_parking.so
ELF: x86-64
```

x86 smoke：

```text
domain_220_20260728_192215_1636  valid 普通主链路 PASS
domain_221_20260728_192231_1762  direct-forward 默认 P 档 stop_path PASS
domain_222_20260728_192249_1921  direct-backward 默认 P 档 stop_path PASS
domain_223_20260728_192613_952   direct-forward + DRIVE 挡位 direct_moving PASS
domain_224_20260728_192628_1127  direct-backward + REVERSE 挡位 direct_moving PASS
```

direct moving 关键日志：

```text
OPEN_SPACE_STRAIGHT_PATH direct_moving
points=31
direct_move_length=3
step=0.1
SPEED_OPTIMIZER ok
```

m57 交叉编译：

```text
out/valet_parking_straight_provider_038/valet_parking_mvp/m57/lib/libvalet_parking.so
ELF: ARM aarch64
NEEDED: libmagna-dds-core.so.1
NEEDED: libmagna-dds-impl.so
```

## 状态

```text
PASS_OPEN_SPACE_STRAIGHT_PATH_X86_M57_STATIC_BLOCKED_BOARD
```

板端状态仍是：

```text
NOT_RUN / BLOCKED_NO_M57_BOARD
```

不能写成 m57 板端运行通过。

## 下一步

建议进入 NEXT-039：

```text
继续硬化 OPEN_SPACE_STRAIGHT_PATH direct 分支：
1. 评估 direct_speed_mps 是否需要映射到速度优化器配置。
2. 增加反向速度冲突 smoke。
3. 补 direct 分支 target gear / trajectory_type / parking status 的输出差距表。
4. 保持 x86 smoke 与 m57 交叉编译验证。
```
