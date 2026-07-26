# 项目状态快照 015：DDS_MULTI_INPUT辅助输入Topic接入

- 快照编号：015
- 日期：2026-07-27
- 当前阶段：Fast-2 - `.so` + typed DDS + ROI_DECIDER/PATH_PROVIDER/PATH_PARTITION/SPEED_OPTIMIZER DDS多输入接入
- 阶段状态：PASS_DDS_MULTI_INPUT_X86_M57_STATIC_BLOCKED_BOARD
- 创建原因：把 014 阶段的外部输入边界接到 MagnaDDS typed reader，让 localization/chassis/obstacle 可以通过 DDS Topic 进入泊车规划链路。

---

## 1. 本次完成事项

- [x] 扩展 `valet_parking_topics.idl`，新增临时 typed DDS 输入类型：
  - `LocalizationEstimate`
  - `ChassisState`
  - `Obstacle`
  - `ObstacleArray`
  - `ObstacleType`
- [x] 使用当前 MagnaDDS SDK `idlparser` 重新生成 typed topic 代码。
- [x] 扩展 `valet_parking_config_t`：
  - `localization_topic_name`
  - `chassis_topic_name`
  - `obstacle_topic_name`
  - `enable_aux_input_topics`
- [x] `ValetParkingComponent` 默认订阅三个辅助输入 Topic：
  - `/localization/estimate`
  - `/chassis/state`
  - `/perception/obstacles`
- [x] 将 DDS 样本转换到 014 阶段已有的外部输入边界：
  - `LocalizationEstimate` -> `UpdateVehicleState` 的位姿部分
  - `ChassisState` -> `UpdateVehicleState` 的速度、加速度、档位部分
  - `ObstacleArray` -> `UpdateObstacles`
- [x] 增加 `valet_parking_runner` 参数：
  - `--localization-topic=<name>`
  - `--chassis-topic=<name>`
  - `--obstacle-topic=<name>`
  - `--disable-aux-input-topics`
- [x] 保持 `/selected_slot` 主链路可回归：没有辅助发布者时仍回退 fake vehicle 和空障碍物。
- [x] 增加两处运行态防护：
  - localization 是外部车辆状态生效门槛，避免只有 chassis 时用默认原点覆盖车辆位姿。
  - auxiliary reader 每轮最多排空固定批次，避免高频辅助 Topic 饿住 `/selected_slot` 主输入。

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

本阶段完成的是 DDS 多输入的临时 typed reader 接线，不是最终车端协议验收。

当前状态是：

- `/selected_slot` 和 `/planning/trajectory` 仍然是真实 MagnaDDS typed Topic。
- localization/chassis/obstacle 三个辅助输入 Topic 已由 `ValetParkingComponent` 默认订阅。
- 三个辅助输入 Topic 的 IDL 是 MVP 临时契约，字段、单位、坐标系、Topic 名称还需要和真实车端确认。
- 无辅助发布者时，x86 冒烟必须继续显示 `external_vehicle=false, external_obstacles=0`。
- m57 只完成交叉编译、ELF 架构和 MagnaDDS 依赖检查，尚未做真实板端运行。

仍未完成：

- 辅助输入 mock publisher 与实收样本 smoke。
- 真实车端 localization/chassis/obstacle Topic 契约对齐。
- m57 板端 runtime 验证。
- 完整原车 `Frame/DependencyInjector`。
- 完整 `OpenSpacePathProvider` 大类。
- NLP smoother。

---

## 3. 本次验证命令

x86 构建：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform x86 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_multi_input_015
```

x86 DDS 冒烟：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_multi_input_015/valet_parking_mvp/x86 \
  --domain-id 17 \
  --timeout-ms 20000 \
  --count 3 \
  --interval-ms 500
```

m57 构建：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform m57 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_multi_input_015
```

x86 C API 符号检查：

```bash
nm -D out/valet_parking_multi_input_015/valet_parking_mvp/x86/lib/libvalet_parking.so \
  | grep valet_parking_update_vehicle_state
nm -D out/valet_parking_multi_input_015/valet_parking_mvp/x86/lib/libvalet_parking.so \
  | grep valet_parking_clear_vehicle_state
nm -D out/valet_parking_multi_input_015/valet_parking_mvp/x86/lib/libvalet_parking.so \
  | grep valet_parking_update_obstacles
nm -D out/valet_parking_multi_input_015/valet_parking_mvp/x86/lib/libvalet_parking.so \
  | grep valet_parking_clear_obstacles
```

m57 C API 符号检查：

```bash
/usr/local/ARM-toolchain/gcc-arm-9.2-2019.12-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-nm \
  -D out/valet_parking_multi_input_015/valet_parking_mvp/m57/lib/libvalet_parking.so \
  | grep valet_parking_update_vehicle_state
```

---

## 4. 验收结果

| 验收项 | 结果 | 证据 |
|---|---|---|
| IDL 重新生成 | PASS | `generated/valet_parking_topics*` 已包含 `LocalizationEstimateTopicDataType`、`ChassisStateTopicDataType`、`ObstacleArrayTopicDataType` |
| x86 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_multi_input_015/valet_parking_mvp/x86/lib/libvalet_parking.so` |
| x86 ELF 架构 | PASS | `ELF 64-bit LSB shared object, x86-64` |
| x86 MagnaDDS 依赖 | PASS | `readelf -d` 显示 `libmagna-dds-core.so.1` |
| x86 DDS 冒烟 | PASS | subscriber 收到 `points=179, length=7.64356, is_estop=false` |
| 默认无辅助发布者回退 | PASS | runner 显示 `external_vehicle=false, external_obstacles=0` |
| 辅助 Topic 启动日志 | PASS | runner 启动日志包含 `/localization/estimate`、`/chassis/state`、`/perception/obstacles` |
| x86 C API 符号导出 | PASS | `nm -D` 显示 update/clear vehicle_state/obstacles |
| m57 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_multi_input_015/valet_parking_mvp/m57/lib/libvalet_parking.so` |
| m57 ELF 架构 | PASS | `ELF 64-bit LSB shared object, ARM aarch64` |
| m57 MagnaDDS 依赖 | PASS | `readelf -d` 显示 `libmagna-dds-core.so.1` 和 `libmagna-dds-impl.so` |
| m57 C API 符号导出 | PASS | cross `nm -D` 显示 update/clear vehicle_state/obstacles |
| m57 板端运行 | NOT_RUN | 当前无 m57 板端 |

---

## 5. 下一步

继续下一阶段：DDS 多输入实收样本验证。

建议先做：

- 新增或扩展 x86 mock publisher，发布 `LocalizationEstimate`、`ChassisState`、`ObstacleArray`。
- 扩展 smoke，让辅助样本先进入 reader，再发送 `/selected_slot`。
- 验证 runner 日志出现 `aux localization`、`aux chassis`、`aux obstacles`。
- 验证规划状态显示 `external_vehicle=true`、`external_obstacles>0`。
- 在真实车端 Topic 契约确认前，继续把当前 IDL 标注为临时契约。
