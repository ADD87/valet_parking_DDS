# 项目状态快照 014：EXTERNAL_INPUT_BOUNDARY外部输入边界

- 快照编号：014
- 日期：2026-07-26
- 当前阶段：Fast-2 - `.so` + typed DDS + ROI_DECIDER/PATH_PROVIDER/PATH_PARTITION/SPEED_OPTIMIZER 外部输入边界
- 阶段状态：PASS_EXTERNAL_INPUT_BOUNDARY_X86_M57_STATIC_BLOCKED_BOARD
- 创建原因：为后续真实定位、底盘、障碍物 DDS reader 接入建立稳定入口，避免继续把 fake vehicle 写死在 adapter 里。

---

## 1. 本次完成事项

- [x] 扩展 `valet_parking_c_api.h`，新增外部输入结构：
  - `valet_parking_vehicle_state_t`
  - `valet_parking_obstacle_t`
  - `valet_parking_gear_position_t`
  - `valet_parking_obstacle_type_t`
- [x] 扩展 `.so` C API：
  - `valet_parking_update_vehicle_state`
  - `valet_parking_clear_vehicle_state`
  - `valet_parking_update_obstacles`
  - `valet_parking_clear_obstacles`
- [x] `ValetParkingComponent` 和 `ValetParkingStageParkingAdapter` 增加对应 update/clear 转发。
- [x] adapter runtime context 新增外部车辆状态和障碍物快照。
- [x] `Process()` 与外部 update/clear 共用 mutex，避免 worker loop 和外部输入更新之间的数据竞争。
- [x] 外部车辆状态已可替换：
  - ROI_DECIDER 的 `VehicleState`
  - PATH_PROVIDER 的 start point
  - PATH_PARTITION 的 `VehicleState` 和 planning start point
  - SPEED_OPTIMIZER 的 `VehicleState`、start point、start velocity、start acceleration
- [x] 外部障碍物已可进入：
  - PATH_PROVIDER/PATH_PARTITION 的 obstacle line segments
  - SPEED_OPTIMIZER 的 static/moving obstacle input
- [x] 未提供外部输入时，仍回退到 fake vehicle 和空障碍物。
- [x] API version 更新为 `0.2.0-mvp`。

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

本阶段不是 DDS 多 Topic 接入完成。

当前状态是：

- `/selected_slot` 和 `/planning/trajectory` 仍然是真实 MagnaDDS typed Topic。
- 车辆状态和障碍物目前通过 C API 外部输入边界进入。
- 现有 `valet_parking_runner` 尚未订阅 localization/chassis/obstacle Topic，也不会主动调用外部输入 C API。
- 没有外部输入时，x86 冒烟应继续显示 `external_vehicle=false, external_obstacles=0`。

这样做的原因：

- 当前还没有真实车端定位、底盘、障碍物 Topic 契约。
- 先把 adapter 内的替换点固定住，后续 DDS reader 只需要把收到的数据转成 C API/adapter 输入即可。
- 避免凭空定义一个可能和真实车端不一致的 DDS 协议。

仍未完成：

- 真实 localization/chassis/obstacle DDS reader。
- 真实 Topic 名称、字段、单位、坐标系确认。
- m57 板端 runtime 验证。
- 完整原车 `Frame/DependencyInjector`。
- 完整 `OpenSpacePathProvider` 大类。
- NLP smoother。

---

## 3. 本次验证命令

x86：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform x86 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_external_inputs_014
```

x86 DDS 冒烟：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_external_inputs_014/valet_parking_mvp/x86 \
  --domain-id 16 \
  --timeout-ms 20000 \
  --count 3 \
  --interval-ms 500
```

x86 C API 符号检查：

```bash
nm -D out/valet_parking_external_inputs_014/valet_parking_mvp/x86/lib/libvalet_parking.so \
  | grep -E 'valet_parking_(update|clear)_(vehicle_state|obstacles)'
```

m57：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform m57 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_external_inputs_014
```

m57 C API 符号检查：

```bash
nm -D out/valet_parking_external_inputs_014/valet_parking_mvp/m57/lib/libvalet_parking.so \
  | grep -E 'valet_parking_(update|clear)_(vehicle_state|obstacles)'
```

---

## 4. 验收结果

| 验收项 | 结果 | 证据 |
|---|---|---|
| x86 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_external_inputs_014/valet_parking_mvp/x86/lib/libvalet_parking.so` |
| x86 ELF 架构 | PASS | `ELF 64-bit LSB shared object, x86-64` |
| x86 MagnaDDS 依赖 | PASS | `ldd` 显示 `libmagna-dds-core.so.1` 和 `libmagna-dds-impl.so` |
| x86 DDS 冒烟 | PASS | subscriber 收到 `points=179, length=7.64356, is_estop=false` |
| 默认外部输入状态 | PASS | runner 显示 `external_vehicle=false, external_obstacles=0` |
| runtime context 保持有效 | PASS | runner 第一帧 `last_frame=false`，第二帧 `last_frame=true` |
| x86 C API 符号导出 | PASS | `nm -D` 显示 update/clear vehicle_state/obstacles |
| m57 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_external_inputs_014/valet_parking_mvp/m57/lib/libvalet_parking.so` |
| m57 ELF 架构 | PASS | `ELF 64-bit LSB shared object, ARM aarch64` |
| m57 MagnaDDS 依赖 | PASS | `readelf -d` 显示 `libmagna-dds-core.so.1` 和 `libmagna-dds-impl.so` |
| m57 C API 符号导出 | PASS | `nm -D` 显示 update/clear vehicle_state/obstacles |
| m57 板端运行 | NOT_RUN | 当前无 m57 板端 |

---

## 5. 下一步

继续下一阶段：DDS 多输入接入。

建议先做：

- 在现有 IDL 中新增临时 `LocalizationEstimate`、`ChassisState`、`ObstacleArray` 类型，或等待真实车端 Topic 契约后再生成。
- 给 `valet_parking_runner` 增加可选输入 Topic 参数。
- 在 `ValetParkingComponent` 中新增 optional readers，把收到的定位/底盘/障碍物样本转入 014 的外部输入边界。
- 保留当前只有 `/selected_slot` 的冒烟链路，作为最小回归。
