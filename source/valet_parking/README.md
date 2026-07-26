# valet_parking (Fast-2 Real DDS MVP)

本模块是 `ValetParkingStageParking` 适配 MagnaDDS 的首版共享库：

- 目标产物：`libvalet_parking.so`
- 当前能力：生命周期 C API（create/start/stop/destroy）
- 当前状态：已接入真实 MagnaDDS Participant/Subscriber/Publisher/DataReader/DataWriter
- 输入路径：订阅 typed DDS Topic `/selected_slot`
- 输出路径：发布 typed DDS Topic `/planning/trajectory`
- 异常行为：非法输入（如 NaN/empty/overflow）发布显式 `estop` 输出

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
bash applications/source/valet_parking_tools/build_valet_parking.sh --platform both --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_quick_build
```

默认会编译 x86 和 m57，并在构建结束后检查 `libvalet_parking.so` 的 ELF 架构和 MagnaDDS 动态依赖。

后续阶段：

1. 在现有 `SelectedSlot -> ROI_DECIDER -> PlanningTrajectory` 链路后接入 `PATH_PROVIDER`
2. 保持 DDS Topic 契约不变
3. 有 m57 板端后执行板端 runtime 验收
