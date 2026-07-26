# valet_parking (Fast-2 Real DDS MVP)

本模块是 `ValetParkingStageParking` 适配 MagnaDDS 的首版共享库。

- 目标产物：`libvalet_parking.so`
- 输入：订阅 typed DDS Topic `/selected_slot`
- 输出：发布 typed DDS Topic `/planning/trajectory`
- 当前主链路：`SelectedSlot -> ROI_DECIDER -> PATH_PROVIDER -> PlanningTrajectory`
- 异常行为：非法输入，如 NaN、empty、overflow，会发布显式 `estop` 输出

## 当前算法状态

已接入内容：

- `ROI_DECIDER`：从选中车位计算 ROI、目标位姿、目标区域。
- `PATH_PROVIDER`：使用 standalone 中已独立化的 `OpenSpacePathGenerator + HybridAStar + PathPartition` 生成粗路径。
- `PlanningTrajectory`：把 PATH_PROVIDER 的分段路径压平成 DDS 输出轨迹点。

暂未接入内容：

- 完整 `OpenSpacePathProvider` 大类。
- NLP smoother。
- 后续独立 `PATH_PARTITION` 阶段的更完整路径分割、重采样、状态管理。
- `SPEED_OPTIMIZER`。

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

## 最近验证

PATH_PROVIDER 接入后，已验证：

- x86：生成 x86-64 `libvalet_parking.so`，链接 `libmagna-dds-core.so.1`。
- m57：生成 ARM aarch64 `libvalet_parking.so`，链接 `libmagna-dds-core.so.1` 和 `libmagna-dds-impl.so`。
- x86 DDS 冒烟：mock `SelectedSlot` 输入后，subscriber 收到 99 点 `PlanningTrajectory`，`is_estop=false`，runner 日志显示 `PATH_PROVIDER ok`。

m57 目前只完成交叉编译和依赖检查，尚未做真实板端运行验证。
