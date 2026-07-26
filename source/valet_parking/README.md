# valet_parking (Fast-2 Real DDS MVP)

本模块是 `ValetParkingStageParking` 适配 MagnaDDS 的首版共享库。

- 目标产物：`libvalet_parking.so`
- 输入：订阅 typed DDS Topic `/selected_slot`
- 输出：发布 typed DDS Topic `/planning/trajectory`
- 当前主链路：`SelectedSlot -> ROI_DECIDER -> PATH_PROVIDER -> PATH_PARTITION -> SPEED_OPTIMIZER -> PlanningTrajectory`
- 异常行为：非法输入，如 NaN、empty、overflow，会发布显式 `estop` 输出

## 当前算法状态

已接入内容：

- `ROI_DECIDER`：从选中车位计算 ROI、目标位姿、目标区域。
- `PATH_PROVIDER`：使用 standalone 中已独立化的 `OpenSpacePathGenerator + HybridAStar + PathPartition` 生成粗路径。
- `PATH_PARTITION`：使用 standalone 中已独立化的 `OpenSpacePathPartition::Execute` 做任务级路径仲裁，输出当前应执行的 `chosen_partitioned_path`。
- `SPEED_OPTIMIZER`：使用 standalone 中已独立化的 `OpenSpaceSpeedOptimizer::Execute` 为 `chosen_partitioned_path` 生成速度和时间采样。
- `RuntimeContext`：`ValetParkingStageParkingAdapter` 内部复用 `PATH_PARTITION` 和 `SPEED_OPTIMIZER` 对象，保存上一帧发布档位、speed collision/replan 状态和 last frame 时间信息。
- `PlanningTrajectory`：把 SPEED_OPTIMIZER 输出的轨迹转换成 DDS 输出；若 SPEED_OPTIMIZER 失败，则回退到 PATH_PARTITION 的 nominal speed 轨迹。

暂未接入内容：

- 完整 `OpenSpacePathProvider` 大类。
- NLP smoother。
- 完整原车 `Frame/DependencyInjector` history。
- 真实定位/底盘输入 Topic。
- 真实静态/动态障碍物输入 Topic。

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

## 最近验证

RuntimeContext 运行态硬化后，已验证：

- x86：生成 x86-64 `libvalet_parking.so`，链接 `libmagna-dds-core.so.1`。
- m57：生成 ARM aarch64 `libvalet_parking.so`，链接 `libmagna-dds-core.so.1` 和 `libmagna-dds-impl.so`。
- x86 DDS 冒烟：mock `SelectedSlot` 输入后，subscriber 收到 179 点 `PlanningTrajectory`，`is_estop=false`；runner 第一帧显示 `last_frame=false`，第二帧显示 `last_frame=true`。

m57 目前只完成交叉编译和依赖检查，尚未做真实板端运行验证。
