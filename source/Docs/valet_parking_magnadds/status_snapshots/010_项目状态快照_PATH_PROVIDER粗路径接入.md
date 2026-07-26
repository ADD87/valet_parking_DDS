# 项目状态快照 010：PATH_PROVIDER粗路径接入

- 快照编号：010
- 日期：2026-07-26
- 当前阶段：Fast-2 - `.so` + typed DDS + ROI_DECIDER/PATH_PROVIDER 最小闭环
- 阶段状态：PASS_PATH_PROVIDER_X86_M57_STATIC_BLOCKED_BOARD
- 创建原因：用户要求继续按 `SelectedSlot -> ROI_DECIDER -> PATH_PROVIDER -> PlanningTrajectory` 计划执行，并在每个阶段更新文档和 Git。

---

## 1. 本次完成事项

- [x] 在 `ValetParkingStageParkingAdapter` 中接入 PATH_PROVIDER 粗路径生成。
- [x] 保持 DDS 外部契约不变：
  - 输入仍是 typed DDS `SelectedSlot`
  - 输出仍是 typed DDS `PlanningTrajectory`
- [x] 接入 standalone 中已独立化的路径生成核心：
  - `OpenSpacePathGenerator`
  - `HybridAStar`
  - `ReedShepp`
  - `PathGenerator::PathPartition`
- [x] 将 PATH_PROVIDER 分段路径压平成 `PlanningTrajectory.trajectory_point`。
- [x] PATH_PROVIDER 失败时保留回退：
  - 回退到原 ROI seed 轨迹
  - `replan_reason` 写明 PATH_PROVIDER 失败原因和 fallback
- [x] 补齐最小 compat：
  - `planning_gflags.h`
  - `glog/logging.h`
  - `common/status/status.h` 的比较与输出接口
  - `proto/planning/sl_boundary.pb.h` 的轻量 `SLBoundary`

---

## 2. 重要边界

本阶段接入的是 PATH_PROVIDER 的“粗路径核心”，不是完整原车工程里的 `OpenSpacePathProvider` 大类。

原因：

- 完整 `OpenSpacePathProvider` 依赖 `Frame`、`DependencyInjector`、历史帧、规划上下文等大框架。
- 当前阶段的目标是把 DDS 进程内的业务链路先打通，不把整套规划框架一次性搬进来。
- NLP smoother 还依赖 IPOPT/OSQP/gflags/glog 等外部栈，暂不进入本阶段。

因此当前链路是：

```text
SelectedSlot
  -> ROI_DECIDER
  -> OpenSpacePathGenerator / HybridAStar / PathPartition
  -> PlanningTrajectory
```

---

## 3. 本次验证命令

x86：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform x86 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_path_provider_010
```

m57：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform m57 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_path_provider_010
```

x86 DDS 冒烟：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_path_provider_010/valet_parking_mvp/x86
export LD_LIBRARY_PATH="$PWD/lib:${LD_LIBRARY_PATH:-}"
./app/valet_parking_runner --domain-id=10
./app/planning_trajectory_mock_subscriber --domain-id=10 --timeout-ms=20000 --strict
./app/selected_slot_mock_publisher --domain-id=10 --mode=valid --count=3 --interval-ms=500
```

---

## 4. 验收结果

| 验收项 | 结果 | 证据 |
|---|---|---|
| x86 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_path_provider_010/valet_parking_mvp/x86/lib/libvalet_parking.so` |
| x86 ELF 架构 | PASS | `ELF 64-bit LSB shared object, x86-64` |
| x86 MagnaDDS 依赖 | PASS | `readelf -d` 显示 `libmagna-dds-core.so.1` |
| m57 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_path_provider_010/valet_parking_mvp/m57/lib/libvalet_parking.so` |
| m57 ELF 架构 | PASS | `ELF 64-bit LSB shared object, ARM aarch64` |
| m57 MagnaDDS 依赖 | PASS | `readelf -d` 显示 `libmagna-dds-core.so.1` 和 `libmagna-dds-impl.so` |
| x86 DDS 冒烟 | PASS | subscriber 收到 `points=99, length=7.64359, is_estop=false` |
| PATH_PROVIDER 是否生效 | PASS | runner 日志显示 `PATH_PROVIDER ok, partitions=1, points=99, path_type=1, smoothed=false` |
| m57 板端运行 | NOT_RUN | 当前无 m57 板端 |

---

## 5. 下一步

继续下一阶段：`PATH_PARTITION`。

执行原则：

- 继续沿用当前结构，不改变 DDS Topic。
- 优先复用 standalone 中已经能独立编译的代码。
- 如果完整 `OpenSpacePathPartition` 依赖过重，先抽出最小可运行的路径分割、重采样、分段状态能力。
- 每完成一个阶段继续更新文档并提交推送 `applications` Git。
