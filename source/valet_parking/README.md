# valet_parking (Fast-2 Real DDS MVP)

本模块是 `ValetParkingStageParking` 适配 MagnaDDS 的首版共享库。

- 目标产物：`libvalet_parking.so`
- 输入：订阅 typed DDS Topic `/selected_slot`
- 输出：发布 typed DDS Topic `/planning/trajectory`
- 可选辅助输入：默认订阅临时 typed DDS Topic `/localization/estimate`、`/chassis/state`、`/perception/obstacles`
- 当前主链路：`SelectedSlot -> ROI_DECIDER -> PATH_PROVIDER -> PATH_PARTITION -> SPEED_OPTIMIZER -> PlanningTrajectory`
- 异常行为：非法主输入，如 NaN、empty、overflow，会发布显式 `estop` 输出；非法辅助输入会清理对应外部状态，避免污染后续规划
- NEXT-021 后，外部定位与选中车位几何明显不在同一 envelope 时，会在 ROI_DECIDER 前输出 `estop`；ROI 成功后进入 PATH_PROVIDER 前会执行轻量 `PATH_PROVIDER_PRECHECK`
- NEXT-022 后，`TRACE_REPLAN` 且历史 warm start 被接受时，会启用轻量 `trace_adjust` 策略切片，并在状态日志中输出可验证字段。
- NEXT-023 后，PATH_PROVIDER 的失败和策略状态日志会输出更完整的 `warm_start`/`trace_adjust` 诊断，`moving-localization` mock 也固定为稳定小偏移，避免测试样本自己漂出 warm start 接受阈值。
- NEXT-024 后，新增 `moving-localization-large` 负向 smoke，稳定验证 `warm_start_reject=lateral_offset_large` 与 `trace_adjust_reject=no_trace_path`。
- NEXT-025 后，新增 SelectedSlot `degenerate-corners` 负向 smoke，验证角点标签齐全但几何退化的车位会被 adapter 明确 estop 拒绝。
- NEXT-026 后，新增辅助输入 `many-obstacles` 负向 smoke，验证 128 个合法障碍物产生 512 条线段时，会在 PATH_PROVIDER 前被 `PATH_PROVIDER_PRECHECK failed: too_many_obstacle_segments=512` 拒绝。
- NEXT-027 后，新增辅助输入 `far-obstacles` 负向 smoke，验证合法但明显远离 ROI local `xy_bounds` 的障碍物会被 `PATH_PROVIDER_PRECHECK failed: obstacle_segment_outside_xy_bounds[...]` 拒绝。
- NEXT-028 后，新增辅助输入 `obstacle-appears` smoke，验证障碍物从无到有时 PATH_PROVIDER 必须 `replan=BLOCK_BY_STATIC_OBSTACLE` 并重新生成路径；障碍物签名稳定后应回到 `history=reused, replan=NONE`。
- NEXT-029 后，新增辅助输入 `obstacle-disappears` smoke，验证合法空 `ObstacleArray` 会清空外部障碍物；障碍物从有到无时必须 `replan=BLOCK_BY_STATIC_OBSTACLE` 并重新生成路径，稳定为空后再复用历史路径。
- NEXT-030 后，新增 SelectedSlot `target-moves` smoke，验证选中车位目标位姿变化时 PATH_PROVIDER 必须 `replan=TARGET_UPDATE` 并重新生成路径；目标稳定后应回到 `history=reused, replan=NONE`。
- NEXT-031 后，新增 SelectedSlot `parking-seq-changes` smoke，验证目标位姿基本不变但 `parking_seq/path_id` 变化时，PATH_PROVIDER 也必须 `replan=TARGET_UPDATE` 并重新生成路径；path_id 稳定后应回到 `history=reused, replan=NONE`。
- NEXT-032 后，泊车算法源码已本地化到 `source/valet_parking/algorithm/parking_algorithm_standalone`，当前 MVP 构建不再依赖外部 `E:\APA\DDS\parking_algorithm_standalone` 绝对路径。
- NEXT-033 后，新增 SelectedSlot `multi-lot-seq-switch` smoke，验证同一条 DDS 输入中包含多个 `ParkingLot` 时，Adapter 会按 `opt_parking_seq` 选择对应车位；切到 `parking_seq=2` 后 PATH_PROVIDER 必须 `replan=TARGET_UPDATE`，稳定后回到 `history=reused`。

## 当前算法状态

已接入内容：

- `ROI_DECIDER`：从选中车位计算 ROI、目标位姿、目标区域。
- `PATH_PROVIDER`：使用本模块本地化的 `algorithm/parking_algorithm_standalone` 中已独立化的 `OpenSpacePathGenerator + HybridAStar + PathPartition` 生成粗路径；adapter 已新增轻量运行态，能在目标、障碍物、起点和速度重规划状态不变时复用上一帧有效路径；NEXT-019 接入轻量 `warm_start`/`path_strategy` 切片；NEXT-020 改为基于历史路径几何累计距离截取 warm start，并用 `moving-localization` smoke 直接验证 `TRACE_REPLAN -> history_splice`；NEXT-021 在 PATH_PROVIDER 前增加轻量 PreCheck，检查 ROI bounds、起点/终点、目标区域和障碍物线段几何；NEXT-022 在 `TRACE_REPLAN + history_splice` 的保守条件下接入轻量 `trace_adjust` 策略；NEXT-023 补齐 trace adjust 拒绝原因、路径长度和失败路径诊断；NEXT-024 新增可控大偏移负向样本，覆盖 warm start/trace adjust 拒绝原因；NEXT-026 将可进入路径搜索的障碍物线段上限收紧为 500，并新增 128 障碍物过载 smoke；NEXT-027 新增障碍物线段 ROI local bounds 检查，拦截明显错坐标的远障碍物；NEXT-028 新增障碍物从无到有 smoke；NEXT-029 新增障碍物从有到无 smoke；NEXT-030 新增 SelectedSlot 目标位姿变化 smoke；NEXT-031 新增 `parking_seq/path_id` 变化 smoke；NEXT-032 将当前 MVP 实际编译使用的算法源码本地化，确保 `applications` 仓库可独立携带当前算法基线；NEXT-033 新增同一 DDS 输入多车位 `opt_parking_seq` 选择验证，并在 PATH_PROVIDER 日志中输出 `parking_seq` 作为 runner 实际选中车位的证据。
- `PATH_PARTITION`：使用 standalone 中已独立化的 `OpenSpacePathPartition::Execute` 做任务级路径仲裁，输出当前应执行的 `chosen_partitioned_path`。
- `SPEED_OPTIMIZER`：使用 standalone 中已独立化的 `OpenSpaceSpeedOptimizer::Execute` 为 `chosen_partitioned_path` 生成速度和时间采样。
- `RuntimeContext`：`ValetParkingStageParkingAdapter` 内部复用 `PATH_PARTITION` 和 `SPEED_OPTIMIZER` 对象，保存上一帧发布档位、speed collision/replan 状态和 last frame 时间信息。
- `PathProviderRuntimeState`：`RuntimeContext` 内部保存上一帧 PATH_PROVIDER 输出、目标点、路径 id、障碍物签名和复用计数；runner 状态日志会显示 `history=generated|reused` 与 `replan=...`。
- 外部输入边界：C API 已提供 `vehicle_state` 与 `obstacles` 的 update/clear 入口；未调用时继续使用 fake vehicle 和空障碍物。
- DDS 辅助输入 reader：`ValetParkingComponent` 已默认订阅临时 `LocalizationEstimate`、`ChassisState`、`ObstacleArray` typed Topic，并把样本转入外部输入边界。
- 辅助输入边界硬化：invalid/nan localization 会清理外部车辆状态；invalid chassis 会清理速度、加速度和档位；invalid obstacle array 或非法障碍物几何会清空外部障碍物；chassis-only 不会误置 `external_vehicle=true`。
- 定位/车位 envelope 保护：当外部定位和选中车位 AABB 明显不一致时，adapter 会在 ROI_DECIDER 前输出 `estop`，避免把远距离错坐标喂给 ROI。
- SelectedSlot 角点几何保护：当车位四角点标签齐全但几何退化为零面积或近零跨度时，adapter 会在 ROI_DECIDER 前输出 `estop`，避免 ROI/PATH_PROVIDER 接收不存在的车位几何。
- 障碍物线段过载保护：当前外部障碍物数量上限仍是 128；当 128 个合法箱体障碍物生成 512 条线段时，adapter 会在 PATH_PROVIDER 前输出 `estop`，避免把过载输入交给路径搜索。
- 障碍物局部位置保护：当外部障碍物本身合法，但线段端点转换到 ROI local 坐标后明显远离 `xy_bounds` 时，adapter 会在 PATH_PROVIDER 前输出 `estop`，避免错 frame/错坐标障碍物污染路径搜索。
- 障碍物变化重规划保护：当外部障碍物从无到有、从有到无、或障碍物签名变化时，PATH_PROVIDER 会拒绝直接复用旧路径，输出 `replan=BLOCK_BY_STATIC_OBSTACLE` 并重新生成；当障碍物签名保持稳定时，才允许 `history=reused`。
- 目标变化重规划保护：当 SelectedSlot 的目标位姿或 `parking_seq/path_id` 变化时，PATH_PROVIDER 会拒绝直接复用旧路径，输出 `replan=TARGET_UPDATE` 并重新生成；当目标保持稳定时，才允许 `history=reused`。
- `PlanningTrajectory`：把 SPEED_OPTIMIZER 输出的轨迹转换成 DDS 输出；若 SPEED_OPTIMIZER 失败，则回退到 PATH_PARTITION 的 nominal speed 轨迹。

暂未接入内容：

- 完整 `OpenSpacePathProvider` 大类。
- 完整 `OpenSpacePathProvider` 的线程管理、完整 PreCheck、完整 path strategy、完整 warm start/splice path。
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
- `moving-localization`：先发布原点定位，再稳定发布小幅横向移动定位 `(0.80,0.70)`，期望触发 `TRACE_REPLAN`，生成 `warm_start=history_splice`，并启用 `trace_adjust=true`。
- `moving-localization-large`：先发布原点定位，再稳定发布较大横向移动定位 `(1.40,1.30)`，期望触发 `TRACE_REPLAN`，但拒绝历史 warm start，并输出 `warm_start_reject=lateral_offset_large`、`trace_adjust_reject=no_trace_path`。
- `far-localization`：发布远离车位的定位，期望触发 `vehicle_lot_precheck failed` 并输出 `is_estop=true`，用于验证错坐标/错 frame 不会进入 ROI。
- `far-obstacles`：发布 1 个合法但坐标为 `(1000,1000)` 的静态障碍物，期望触发 `PATH_PROVIDER_PRECHECK failed: obstacle_segment_outside_xy_bounds[...]` 并输出 `is_estop=true`。
- `many-obstacles`：发布 128 个合法静态障碍物，期望触发 `PATH_PROVIDER_PRECHECK failed: too_many_obstacle_segments=512` 并输出 `is_estop=true`，用于验证障碍物线段规模不会进入路径搜索。
- `obstacle-appears`：前 3 组只发布定位/底盘、不发布障碍物，之后发布 1 个稳定静态障碍物；期望先看到 `external_obstacles=0`，障碍物出现时看到 `history=generated, replan=BLOCK_BY_STATIC_OBSTACLE, reason=obstacles_changed`，障碍物稳定后看到 `history=reused, replan=NONE`。
- `obstacle-disappears`：前 3 组发布 1 个稳定静态障碍物，之后发布合法空 `ObstacleArray`；期望先看到 `external_obstacles=1`，障碍物清空时看到 `history=generated, replan=BLOCK_BY_STATIC_OBSTACLE, reason=obstacles_changed, external_obstacles=0`，稳定为空后看到 `history=reused, replan=NONE`。

如需直接验证 `TRACE_REPLAN/history_splice`：

```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_quick_build/valet_parking_mvp/x86 \
  --with-aux-inputs \
  --aux-mode moving-localization \
  --count 6 \
  --aux-count 8 \
  --interval-ms 650 \
  --aux-interval-ms 650
```

该模式会让辅助发布者和 `SelectedSlot` 发布者并发运行，并等待 runner 处理到 trace adjust 分支；脚本会校验 `replan=TRACE_REPLAN`、`warm_start=history_splice`、`warm_start_points>0`、`strategy_kappa_cost=true`、`trace_adjust=true`、`trace_adjust_reject=accepted`、`trace_adjust_points>0`、`trace_adjust_path_length>0`。

如需验证远定位保护：
```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_quick_build/valet_parking_mvp/x86 \
  --with-aux-inputs \
  --aux-mode far-localization
```

该模式会发布 `(1000,1000)` 的定位样本。runner 应显示 `vehicle_lot_precheck failed`，subscriber 应显示 `is_estop=true`。

如需验证障碍物线段过载保护：
```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_quick_build/valet_parking_mvp/x86 \
  --with-aux-inputs \
  --aux-mode many-obstacles
```

该模式会发布 128 个合法静态障碍物。runner 应显示 `aux obstacles ... (count=128)`、`PATH_PROVIDER_PRECHECK failed: too_many_obstacle_segments=512` 和 `estop=true`，subscriber 应显示 `is_estop=true`。

如需验证障碍物局部位置保护：
```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_quick_build/valet_parking_mvp/x86 \
  --with-aux-inputs \
  --aux-mode far-obstacles
```

该模式会发布一个位于 `(1000,1000)` 的合法静态障碍物。runner 应显示 `aux obstacles ... (count=1)`、`PATH_PROVIDER_PRECHECK failed: obstacle_segment_outside_xy_bounds[...]`、`local_start/local_end` 和 `estop=true`，subscriber 应显示 `is_estop=true`。

如需直接验证 warm start/trace adjust 拒绝原因：
```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_quick_build/valet_parking_mvp/x86 \
  --with-aux-inputs \
  --aux-mode moving-localization-large
```

该模式会在第二帧产生 `replan=TRACE_REPLAN`，但由于横向偏移超过 warm start 接受阈值，runner 应显示 `warm_start_reject=lateral_offset_large`、`warm_start_points=0`、`trace_adjust=false`、`trace_adjust_reject=no_trace_path`。

如需验证 SelectedSlot 车位角点退化保护：
```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_quick_build/valet_parking_mvp/x86 \
  --slot-mode degenerate-corners
```

该模式会发布一个 `is_valid=true`、角点位置标签齐全，但四个角点全部重合的车位。runner 应显示 `selected parking lot corner geometry is degenerate` 和 `estop=true`，subscriber 应显示 `is_estop=true`。

如需验证 SelectedSlot 目标变化会触发重规划：

```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_quick_build/valet_parking_mvp/x86 \
  --slot-mode target-moves \
  --interval-ms 650
```

该模式会先发布 3 组基础车位目标，再发布 3 组中心点移动后的车位目标。runner 应先显示基础目标下的 `history=generated` 与 `history=reused`，目标变化时显示 `history=generated, replan=TARGET_UPDATE, reason=target_update`，新目标稳定后显示 `history=reused, replan=NONE, generated_count=2`，subscriber 应保持 `is_estop=false`。

如需验证 SelectedSlot `parking_seq/path_id` 变化会触发重规划：

```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_quick_build/valet_parking_mvp/x86 \
  --slot-mode parking-seq-changes \
  --interval-ms 650
```

该模式会先发布 3 组 `parking_seq=1` 的车位目标，再发布 3 组几何基本相同但 `parking_seq=2` 的车位目标。runner 应在 path_id 变化时显示 `history=generated, replan=TARGET_UPDATE, reason=target_update, generated_count=2`，`parking_seq=2` 稳定后显示 `history=reused, replan=NONE, generated_count=2`，subscriber 应保持 `is_estop=false`。

如需验证同一条 SelectedSlot 内多车位选择：

```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_quick_build/valet_parking_mvp/x86 \
  --slot-mode multi-lot-seq-switch \
  --interval-ms 650
```

该模式每帧都发布两个 `ParkingLot`。前 3 组 `opt_parking_seq=1`，后 3 组切到 `opt_parking_seq=2`。runner 应显示 `input_count=2`，PATH_PROVIDER 先使用 `parking_seq=1`，切换后使用 `parking_seq=2` 并输出 `history=generated, replan=TARGET_UPDATE, reason=target_update`，稳定后输出 `history=reused, replan=NONE, generated_count=2`，subscriber 应保持 `is_estop=false`。

如需验证 runner 完全不订阅辅助输入 Topic：

```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_quick_build/valet_parking_mvp/x86 \
  --with-aux-inputs \
  --disable-aux-input-topics
```

## PATH_PROVIDER 运行态与策略日志

NEXT-019 后，runner 的规划状态里会出现 PATH_PROVIDER 运行态和轻量策略字段：

```text
PATH_PROVIDER ok, ..., history=generated, replan=NO_VALID_PATH, reason=no_history, warm_start=none, warm_start_points=0, strategy_init_move=0
PATH_PROVIDER ok, ..., history=reused, replan=NONE, warm_start=not_applied
PATH_PROVIDER ok, ..., history=generated, replan=TARGET_UPDATE, reason=target_update
PATH_PROVIDER ok, ..., replan=TRACE_REPLAN, warm_start=history_splice, trace_adjust=true, trace_adjust_source=history_warm_start, trace_adjust_reject=accepted
PATH_PROVIDER_PRECHECK ok, xy_bounds_span=16.000x23.283, dest_points=4, obstacle_segments=4
PATH_PROVIDER_PRECHECK failed: too_many_obstacle_segments=512
PATH_PROVIDER_PRECHECK failed: obstacle_segment_outside_xy_bounds[0] local_start=(-779.631,1167.6), local_end=(-780.121,1167.7)
```

含义：

- `history=generated`：本帧重新调用 `OpenSpacePathGenerator` 生成路径。
- `history=reused`：本帧复用上一帧有效路径，避免同目标、同起点、同障碍物时重复搜索。
- `replan=NO_VALID_PATH`：没有可复用历史路径，通常出现在第一帧。
- `replan=TARGET_UPDATE`：目标车位、目标位姿或 `parking_seq/path_id` 变化。NEXT-030 的 `target-moves` smoke 已验证目标中心移动时会触发该状态；NEXT-031 的 `parking-seq-changes` smoke 已验证目标几何基本不变但 path_id 变化时也会触发该状态；NEXT-033 的 `multi-lot-seq-switch` smoke 已验证同一条 DDS 输入包含多个车位时，`opt_parking_seq` 切换会让 Adapter 选择新的 `ParkingLot`，而不是误复用上一目标。
- `replan=BLOCK_BY_STATIC_OBSTACLE`：外部障碍物签名变化。
- `replan=TRACE_REPLAN`：当前起点已经无法匹配历史路径。
- `replan=REPLAN_FOR_SPEED_WARN` 或 `DYNAMIC_REPLAN`：速度层或碰撞风险要求重新规划。
- `warm_start=history_splice`：本帧重新搜索前，adapter 从上一帧历史路径截取了一段可用尾段作为 `OpenSpacePathGenerator` 的 warm start。
- `warm_start=none`：本帧没有可用 warm start，或因为目标/障碍物/速度重规划/碰撞风险等原因不允许使用旧路径。
- `warm_start_reject=...`：当 warm start 未被采用时记录拒绝原因；当采用成功时显示 `accepted`。
- `warm_start_history_points`、`warm_start_s`、`warm_start_l`、`warm_start_path_s=[a,b]`：用于确认当前起点投影到历史路径的位置。NEXT-020 后这些值来自历史路径 x/y 的几何累计距离，不依赖 standalone 输出点里可能未填充的 `PathPoint.s`。
- `strategy_init_move`、`strategy_kappa_cost`、`strategy_limit_steer`：对照完整 `SetPathStrategy` 提取的轻量策略字段，用于让当前简化 adapter 的搜索输入更接近原始 OpenSpacePathProvider 的语义。
- `trace_adjust=true`：NEXT-022 新增，表示本帧在 `TRACE_REPLAN` 且历史 warm start 可用时启用了轻量 `LocalTraceAdjustSearchStrategy` 切片。
- `trace_adjust_source=history_warm_start`：表示 trace adjust 使用的是上一帧路径截取出的历史 warm start。
- `trace_adjust_reject=...`：NEXT-023 新增，说明 trace adjust 未启用或已接受的原因，常见值包括 `not_trace_replan`、`unsafe_replan_status`、`no_trace_path`、`trace_path_too_short`、`trace_path_length_short`、`accepted`。
- `trace_adjust_points`、`trace_adjust_target_s`、`trace_adjust_finish_l`、`trace_adjust_finish_theta`、`trace_adjust_bounds`：用于核对 trace adjust 输入规模和阈值；当前固定对照原始逻辑使用 `target_s=8`、`finish_l_threshold=0.05`、`finish_theta_threshold=0.02`、`xy_bounds=[-30,30,-30,30]`。
- `trace_adjust_path_length`、`trace_adjust_min_length`：NEXT-023 新增，用于确认 trace adjust 使用的历史 warm start 尾段长度是否足够。
- `PATH_PROVIDER_PRECHECK ok`：NEXT-021 新增，表示 ROI origin、`xy_bounds`、start/end pose、目标区域 polygon/角度和障碍物线段几何已通过轻量输入检查；它不是完整原车 `OpenSpacePathProvider::PreCheck`。
- `too_many_obstacle_segments=512`：NEXT-026 新增验证，表示外部障碍物线段规模超过当前轻量 precheck 允许的 500 条，adapter 会在 PATH_PROVIDER 前发布 estop。
- `obstacle_segment_outside_xy_bounds[...]`：NEXT-027 新增验证，表示障碍物线段端点转换到 ROI local 坐标后明显远离当前 `xy_bounds`，adapter 会在 PATH_PROVIDER 前发布 estop，并输出 `local_start/local_end` 方便定位坐标系问题。

这仍只是完整 `OpenSpacePathProvider` 的轻量切片，不包含完整线程管理、NLP smoother、完整 `PreCheck` 或完整 `Frame/DependencyInjector`。当前 `target-moves` mock 已能验证 SelectedSlot 目标位姿变化会触发 `TARGET_UPDATE`；`parking-seq-changes` mock 已能验证 `parking_seq/path_id` 变化会触发 `TARGET_UPDATE`；`multi-lot-seq-switch` mock 已能验证同一 DDS 输入内多车位选择；`moving-localization` mock 已能直接触发并验证 `TRACE_REPLAN -> history_splice -> trace_adjust=true` 分支；`far-localization` mock 已能验证错 frame/远定位会被前置 estop；`many-obstacles` mock 已能验证障碍物线段过载会在 PATH_PROVIDER 前被 estop；`far-obstacles` mock 已能验证远障碍物会在 PATH_PROVIDER 前被 estop；但真实车端定位/底盘/障碍物 Topic 仍未对齐。

## 最近验证

NEXT-033 多车位 `opt_parking_seq` DDS 验证后，已验证：

- x86：生成 x86-64 `libvalet_parking.so`，链接 `libmagna-dds-core.so.1`。
- m57：生成 ARM aarch64 `libvalet_parking.so`，链接 `libmagna-dds-core.so.1` 和 `libmagna-dds-impl.so`。
- x86 多车位选择 smoke：domain_225 `multi-lot-seq-switch` 每帧发布两个 `ParkingLot`；前 3 组选择 `parking_seq=1`，后 3 组选择 `parking_seq=2`；runner 显示 `input_count=2`，切到 `parking_seq=2` 后 `history=generated, replan=TARGET_UPDATE, reason=target_update, generated_count=2`，稳定后 `history=reused, replan=NONE`，subscriber 收到 `is_estop=false`。
- x86 默认 valid 回归：domain_226 默认 mock `SelectedSlot` 输入后仍输出非 estop `PlanningTrajectory`，并保持 `PATH_PROVIDER_PRECHECK ok`、`history=generated` 后 `history=reused`。
- x86 all-valid 辅助输入回归：domain_227 显示 `aux localization`、`aux chassis`、`aux obstacles`，规划状态保持 `external_vehicle=true`、`external_obstacles=1`，并输出非 estop 轨迹。
- 源码本地化：当前 CMake 实际编译使用的 91 个 standalone 最小源码/头文件依赖已复制到 `source/valet_parking/algorithm/parking_algorithm_standalone`，未复制 `proto/**/*.pb.*`、demo、out、launch 或 ROS 相关产物。
- CMake 本地路径验证：x86/m57 编译日志中 standalone `.cc` 均来自 `applications/source/valet_parking/algorithm/parking_algorithm_standalone/...`，不再来自外部 `E:\APA\DDS\parking_algorithm_standalone`。
- x86 默认 valid 回归：domain_219 默认 mock `SelectedSlot` 输入后仍输出非 estop `PlanningTrajectory`，并保持 `PATH_PROVIDER_PRECHECK ok`、`history=generated` 后 `history=reused`。
- x86 `parking_seq/path_id` 变化回归：domain_220 `parking-seq-changes` 仍显示 path_id 变化时 `history=generated, replan=TARGET_UPDATE, reason=target_update, generated_count=2`，`parking_seq=2` 稳定后显示 `history=reused, replan=NONE, generated_count=2`，subscriber 收到 `is_estop=false`。
- x86 all-valid 辅助输入回归：domain_221 显示 `aux localization`、`aux chassis`、`aux obstacles`，规划状态保持 `external_vehicle=true`、`external_obstacles=1`，并输出非 estop 轨迹。
- x86 SelectedSlot `parking_seq/path_id` 变化 smoke：domain_216 `parking-seq-changes` 前 3 组发布 `parking_seq=1`，后 3 组发布几何基本相同的 `parking_seq=2`；runner 显示 path_id 变化时 `history=generated, replan=TARGET_UPDATE, reason=target_update, generated_count=2`，`parking_seq=2` 稳定后显示 `history=reused, replan=NONE, generated_count=2`，subscriber 收到 `is_estop=false`。
- x86 默认 valid 回归：domain_217 默认 mock `SelectedSlot` 输入后仍输出非 estop `PlanningTrajectory`，并保持 `PATH_PROVIDER_PRECHECK ok`、`history=generated` 后 `history=reused`。
- x86 all-valid 辅助输入回归：domain_218 显示 `aux localization`、`aux chassis`、`aux obstacles`，规划状态保持 `external_vehicle=true`、`external_obstacles=1`，并输出非 estop 轨迹。
- x86 SelectedSlot 目标变化 smoke：domain_213 `target-moves` 前 3 组发布基础目标，后 3 组发布移动后的目标；runner 显示目标变化时 `history=generated, replan=TARGET_UPDATE, reason=target_update, generated_count=2`，新目标稳定后显示 `history=reused, replan=NONE, generated_count=2`，subscriber 收到 `is_estop=false`。
- x86 障碍物局部位置负向 smoke：domain_202 `far-obstacles` 发布 1 个位于 `(1000,1000)` 的合法静态障碍物，runner 显示 `aux obstacles #1 (count=1)`、`PATH_PROVIDER_PRECHECK failed: obstacle_segment_outside_xy_bounds[0]`、`local_start/local_end`、`estop=true`，subscriber 收到 `is_estop=true`。
- x86 障碍物线段过载负向 smoke：`many-obstacles` 已验证发布 128 个合法静态障碍物时，runner 显示 `PATH_PROVIDER_PRECHECK failed: too_many_obstacle_segments=512`、`estop=true`，subscriber 收到 `is_estop=true`。
- x86 SelectedSlot 几何负向 smoke：`degenerate-corners` 已验证发布端 `is_valid=true`、`count=1`、`lots=1`，runner 显示 `selected parking lot corner geometry is degenerate`、`estop=true`，subscriber 收到 `is_estop=true`。
- x86 DDS 冒烟：domain_203 默认 mock `SelectedSlot` 输入后，subscriber 收到 179 点 `PlanningTrajectory`，`is_estop=false`；runner 第一帧显示 `last_frame=false`，第二帧显示 `last_frame=true`，默认无辅助发布者时输入状态显示 `external_vehicle=false, external_obstacles=0`。
- x86 DDS 辅助输入回归：domain_204 `all-valid` 显示 `aux localization`、`aux chassis`、`aux obstacles`，规划状态保持 `external_vehicle=true`、`external_obstacles=1`，`PATH_PROVIDER_PRECHECK ok`、`obstacle_segments=4`。
- x86 PATH_PROVIDER 运行态与策略字段：默认 smoke 中第一帧显示 `history=generated, replan=NO_VALID_PATH, warm_start=none, strategy_init_move=0`，第二帧显示 `history=reused, replan=NONE`。
- x86 trace warm start + trace adjust：`moving-localization` smoke 中第二帧显示 `replan=TRACE_REPLAN, warm_start=history_splice, warm_start_reject=accepted, warm_start_points=96, strategy_kappa_cost=true, strategy_limit_steer=true, trace_adjust=true, trace_adjust_source=history_warm_start, trace_adjust_reject=accepted, trace_adjust_points=96, trace_adjust_path_length=7.24982`。
- x86 warm start 拒绝诊断：`moving-localization-large` smoke 中第二帧显示 `replan=TRACE_REPLAN, warm_start=none, warm_start_reject=lateral_offset_large, warm_start_points=0, trace_adjust=false, trace_adjust_reject=no_trace_path, trace_adjust_path_length=0`。
- x86 PATH_PROVIDER_PRECHECK：默认和辅助输入 smoke 中均显示 `PATH_PROVIDER_PRECHECK ok`，包含 `xy_bounds_span`、`dest_points`、`obstacle_segments`。
- x86 远定位保护：`far-localization` smoke 显示 `vehicle_lot_precheck failed`，subscriber 收到 `is_estop=true`，runner 不再崩溃。
- x86 DDS 辅助输入冒烟：`aux_input_mock_publisher` 发布三类辅助样本后，runner 显示 `aux localization`、`aux chassis`、`aux obstacles`，规划状态显示 `external_vehicle=true, external_obstacles=1`。
- x86 DDS 辅助输入边界硬化：`all-valid`、`invalid-localization`、`nan-localization`、`chassis-only`、`invalid-obstacles`、`bad-obstacle-geometry`、`--disable-aux-input-topics` 场景已验证；关键复验中 `bad-obstacle-geometry` 显示 `aux obstacles rejected` 且 `external_obstacles=0`。
- runner 启动日志已显示默认订阅 `/localization/estimate`、`/chassis/state`、`/perception/obstacles`。
- x86/m57：`nm -D libvalet_parking.so` 可看到 `valet_parking_update_vehicle_state`、`valet_parking_clear_vehicle_state`、`valet_parking_update_obstacles`、`valet_parking_clear_obstacles`。
- x86/m57：`aux_input_mock_publisher` 已纳入 `valet_parking_mvp_bom.yaml`，可随快捷脚本一起构建。

m57 目前只完成交叉编译和依赖检查，尚未做真实板端运行验证。
