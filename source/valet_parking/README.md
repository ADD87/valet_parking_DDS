# valet_parking (Fast-2 Real DDS MVP)

本模块是 `ValetParkingStageParking` 适配 MagnaDDS 的首版共享库。

- 目标产物：`libvalet_parking.so`
- 输入：订阅 typed DDS Topic `/selected_slot`
- 输出：发布 typed DDS Topic `/planning/trajectory`
- 可选辅助输入：默认订阅临时 typed DDS Topic `/localization/estimate`、`/chassis/state`、`/perception/obstacles`
- 当前主链路：`SelectedSlot -> ROI_DECIDER -> PATH_PROVIDER -> PATH_PARTITION -> SPEED_OPTIMIZER -> PlanningTrajectory`
- 异常行为：非法主输入，如 NaN、empty、overflow，会发布显式 `estop` 输出；非法辅助输入会清理对应外部状态，避免污染后续规划

## 当前算法状态

已接入内容：

- `ROI_DECIDER`：从选中车位计算 ROI、目标位姿、目标区域。
- `PATH_PROVIDER`：使用 standalone 中已独立化的 `OpenSpacePathGenerator + HybridAStar + PathPartition` 生成粗路径。
- `PATH_PARTITION`：使用 standalone 中已独立化的 `OpenSpacePathPartition::Execute` 做任务级路径仲裁，输出当前应执行的 `chosen_partitioned_path`。
- `SPEED_OPTIMIZER`：使用 standalone 中已独立化的 `OpenSpaceSpeedOptimizer::Execute` 为 `chosen_partitioned_path` 生成速度和时间采样。
- `RuntimeContext`：`ValetParkingStageParkingAdapter` 内部复用 `PATH_PARTITION` 和 `SPEED_OPTIMIZER` 对象，保存上一帧发布档位、speed collision/replan 状态和 last frame 时间信息。
- 外部输入边界：C API 已提供 `vehicle_state` 与 `obstacles` 的 update/clear 入口；未调用时继续使用 fake vehicle 和空障碍物。
- DDS 辅助输入 reader：`ValetParkingComponent` 已默认订阅临时 `LocalizationEstimate`、`ChassisState`、`ObstacleArray` typed Topic，并把样本转入外部输入边界。
- 辅助输入边界硬化：invalid/nan localization 会清理外部车辆状态；invalid chassis 会清理速度、加速度和档位；invalid obstacle array 或非法障碍物几何会清空外部障碍物；chassis-only 不会误置 `external_vehicle=true`。
- `PlanningTrajectory`：把 SPEED_OPTIMIZER 输出的轨迹转换成 DDS 输出；若 SPEED_OPTIMIZER 失败，则回退到 PATH_PARTITION 的 nominal speed 轨迹。

暂未接入内容：

- 完整 `OpenSpacePathProvider` 大类。
- NLP smoother。
- 完整原车 `Frame/DependencyInjector` history。
- 真实车端定位/底盘/障碍物 Topic 契约对齐。
- 真实车端辅助输入的异常语义、单位和坐标系对齐。

## 外部输入 C API

当前 `.so` 已导出以下可选输入接口：

```c
int valet_parking_update_vehicle_state(
    valet_parking_handle_t* handle,
    const valet_parking_vehicle_state_t* vehicle_state);
int valet_parking_clear_vehicle_state(valet_parking_handle_t* handle);
int valet_parking_update_obstacles(
    valet_parking_handle_t* handle,
    const valet_parking_obstacle_t* obstacles,
    uint32_t obstacle_count);
int valet_parking_clear_obstacles(valet_parking_handle_t* handle);
```

这些接口是给后续真实 DDS reader 或工程集成层调用的。现有 runner 不调用它们，因此默认行为仍然是 fake vehicle + 空障碍物。

## DDS 辅助输入 Topic

当前 IDL 中新增了三个临时输入类型：

- `LocalizationEstimate`：车辆位置和朝向，字段包含 `x/y/z/heading`。
- `ChassisState`：底盘运动状态，字段包含 `speed_mps`、`acceleration_mps2`、`gear`。
- `ObstacleArray`：障碍物列表，字段包含障碍物中心点、尺寸、朝向、速度和类型。

runner 默认订阅：

```text
/localization/estimate
/chassis/state
/perception/obstacles
```

可通过参数改名或关闭：

```bash
valet_parking_runner \
  --localization-topic=/your/localization \
  --chassis-topic=/your/chassis \
  --obstacle-topic=/your/obstacles

valet_parking_runner --disable-aux-input-topics
```

重要边界：

- 这些 Topic 是当前 MVP 的临时 typed DDS 契约，不代表已经对齐真实车端协议。
- 没有辅助发布者时，主链路仍回退到 fake vehicle 和空障碍物。
- 车辆外部状态以 `LocalizationEstimate` 为生效门槛；`ChassisState` 只补充速度、加速度和档位，避免底盘先到时用默认原点覆盖车辆位姿。
- `LocalizationEstimate` 为 invalid 或含 NaN/Inf 时会清理外部车辆状态。
- `ChassisState` 为 invalid 或含 NaN/Inf 时会清理底盘补充状态。
- `ObstacleArray` 为 invalid、或其中障碍物尺寸/坐标非法时会清空外部障碍物。

## WSL 快捷编译

在 WSL Ubuntu 中进入 workspace 后运行：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh
```

常用参数：

```bash
bash applications/source/valet_parking_tools/build_valet_parking.sh --platform x86
bash applications/source/valet_parking_tools/build_valet_parking.sh --platform m57 --jobs 8
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform both \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_quick_build
```

脚本默认编译 x86 和 m57，并在构建结束后检查：

- `libvalet_parking.so` 是否生成。
- ELF 架构是否符合目标平台。
- `readelf -d` 是否能看到 MagnaDDS 动态依赖。

## x86 DDS 冒烟

编译完成后，可以在 WSL Ubuntu 中运行：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_quick_build/valet_parking_mvp/x86
```

该脚本会自动启动 `valet_parking_runner`、`planning_trajectory_mock_subscriber` 和 `selected_slot_mock_publisher`，并在结束时清理后台进程。

如需验证辅助 DDS 输入真正进入 reader，可以加 `--with-aux-inputs`：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_quick_build/valet_parking_mvp/x86 \
  --with-aux-inputs
```

该模式会先运行 `aux_input_mock_publisher`，发布 `LocalizationEstimate`、`ChassisState` 和 `ObstacleArray`，再发布 `SelectedSlot`。脚本会校验 runner 日志中出现：

- `aux localization`
- `aux chassis`
- `aux obstacles`
- `external_vehicle=true`
- `external_obstacles>0`

如需验证辅助输入异常和局部输入边界，可以用 `--aux-mode`：

```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_quick_build/valet_parking_mvp/x86 \
  --with-aux-inputs \
  --aux-mode bad-obstacle-geometry
```

当前支持的模式：

- `all-valid`：三类辅助样本都有效，期望 `external_vehicle=true` 且 `external_obstacles>0`。
- `invalid-localization`：最后一组定位样本 invalid，期望清理外部车辆状态。
- `nan-localization`：最后一组定位样本含 NaN，期望清理外部车辆状态。
- `chassis-only`：只发布底盘，不发布定位和障碍物，期望 `external_vehicle=false`。
- `invalid-obstacles`：最后一组障碍物数组 invalid，期望清空外部障碍物。
- `bad-obstacle-geometry`：最后一组障碍物尺寸非法，期望拒绝并清空外部障碍物。

如需验证 runner 完全不订阅辅助输入 Topic：

```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_quick_build/valet_parking_mvp/x86 \
  --with-aux-inputs \
  --disable-aux-input-topics
```

## 最近验证

NEXT-017 辅助输入边界硬化后，已验证：

- x86：生成 x86-64 `libvalet_parking.so`，链接 `libmagna-dds-core.so.1`。
- m57：生成 ARM aarch64 `libvalet_parking.so`，链接 `libmagna-dds-core.so.1` 和 `libmagna-dds-impl.so`。
- x86 DDS 冒烟：mock `SelectedSlot` 输入后，subscriber 收到 179 点 `PlanningTrajectory`，`is_estop=false`；runner 第一帧显示 `last_frame=false`，第二帧显示 `last_frame=true`，默认无辅助发布者时输入状态显示 `external_vehicle=false, external_obstacles=0`。
- x86 DDS 辅助输入冒烟：`aux_input_mock_publisher` 发布三类辅助样本后，runner 显示 `aux localization`、`aux chassis`、`aux obstacles`，规划状态显示 `external_vehicle=true, external_obstacles=1`。
- x86 DDS 辅助输入边界硬化：`all-valid`、`invalid-localization`、`nan-localization`、`chassis-only`、`invalid-obstacles`、`bad-obstacle-geometry`、`--disable-aux-input-topics` 场景已验证；关键复验中 `bad-obstacle-geometry` 显示 `aux obstacles rejected` 且 `external_obstacles=0`。
- runner 启动日志已显示默认订阅 `/localization/estimate`、`/chassis/state`、`/perception/obstacles`。
- x86/m57：`nm -D libvalet_parking.so` 可看到 `valet_parking_update_vehicle_state`、`valet_parking_clear_vehicle_state`、`valet_parking_update_obstacles`、`valet_parking_clear_obstacles`。
- x86/m57：`aux_input_mock_publisher` 已纳入 `valet_parking_mvp_bom.yaml`，可随快捷脚本一起构建。

m57 目前只完成交叉编译和依赖检查，尚未做真实板端运行验证。
