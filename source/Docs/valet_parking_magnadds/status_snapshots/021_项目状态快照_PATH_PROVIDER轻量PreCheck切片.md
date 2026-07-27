# 项目状态快照 021：PATH_PROVIDER轻量PreCheck切片

- 快照编号：021
- 日期：2026-07-27
- 当前阶段：Fast-2 - `.so` + typed DDS + ROI_DECIDER/PATH_PROVIDER/PATH_PARTITION/SPEED_OPTIMIZER light PreCheck slice
- 阶段状态：PASS_PATH_PROVIDER_PRECHECK_SLICE_X86_M57_STATIC_BLOCKED_BOARD
- 创建原因：继续沿用简化 DDS Topic，对照完整 `OpenSpacePathProvider::PreCheck`，先迁入不依赖完整 `Frame/DependencyInjector` 的输入几何有效性检查。

---

## 1. 本次完成事项

- [x] 在 `ValetParkingStageParkingAdapter` 中新增轻量 `PATH_PROVIDER_PRECHECK`：
  - 检查 ROI `origin_point/origin_heading` 是否有限。
  - 检查 `xy_bounds` 是否至少 4 个值、有限、顺序正确、span 合理。
  - 检查 PATH_PROVIDER start/end pose 是否有限。
  - 将 start/end 按 ROI origin/heading 转到局部坐标，对明显越界输入做保护。
  - 检查 `dest_region` polygon 点数、面积、角度和顶点有限性。
  - 检查 ROI + 外部障碍物线段是否有限、非零长度、buffer 合法、数量不过大。
- [x] `PATH_PROVIDER_PRECHECK` 通过时写入状态日志：
  - `PATH_PROVIDER_PRECHECK ok`
  - `xy_bounds_span`
  - `dest_points`
  - `obstacle_segments`
- [x] `PATH_PROVIDER_PRECHECK` 失败时 reset runtime 并输出 `estop`，不再继续 PATH_PROVIDER。
- [x] 新增 `vehicle_lot_precheck`：
  - 仅在收到外部定位时生效。
  - 若外部定位与选中车位 AABB envelope 明显不一致，在 ROI_DECIDER 前输出 `estop`。
  - 本项用于避免错 frame/远定位把 ROI_DECIDER 带入异常输入状态。
- [x] `aux_input_mock_publisher` 新增 `far-localization` 模式，发布 `(1000,1000)` 定位。
- [x] `smoke_valet_parking_x86.sh` 新增 `far-localization` 校验：
  - 要求 runner 日志出现 `vehicle_lot_precheck failed`。
  - 要求 subscriber 收到 `is_estop=true`。

当前主链路保持不变：

```text
SelectedSlot
  -> ROI_DECIDER
  -> PATH_PROVIDER_PRECHECK
  -> PATH_PROVIDER
  -> PATH_PARTITION
  -> SPEED_OPTIMIZER
  -> PlanningTrajectory
```

`vehicle_lot_precheck` 位于 `ROI_DECIDER` 之前，只处理明显错 frame 的外部定位保护。

---

## 2. 重要边界

本阶段不是完整 `OpenSpacePathProvider` 大类接入完成。

已迁入的是轻量输入有效性切片：
- ROI bounds/目标区域/障碍线段几何检查。
- 外部定位与选中车位 envelope 一致性检查。
- 状态日志和 smoke 可验证分支。

仍未接入：
- 完整 `OpenSpacePathProvider::PreCheck` 中依赖 `Frame/OpenSpaceInfo`、low_fs 障碍物集合和完整配置的检查。
- 完整线程管理。
- NLP smoother。
- 完整 `Frame/DependencyInjector`。
- 真实车端 Topic 协议。

m57 仍只完成交叉编译、ELF 和依赖检查，尚未做真实板端运行验证。

---

## 3. 本次验证命令

x86 构建：
```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform x86 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_precheck_021
```

x86 默认 smoke：
```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_precheck_021/valet_parking_mvp/x86 \
  --domain-id 158 \
  --timeout-ms 30000 \
  --count 3 \
  --interval-ms 650
```

x86 moving-localization smoke：
```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_precheck_021/valet_parking_mvp/x86 \
  --domain-id 159 \
  --timeout-ms 30000 \
  --count 4 \
  --interval-ms 650 \
  --with-aux-inputs \
  --aux-mode moving-localization \
  --aux-count 5 \
  --aux-interval-ms 650
```

x86 far-localization smoke：
```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_precheck_021/valet_parking_mvp/x86 \
  --domain-id 157 \
  --timeout-ms 30000 \
  --count 3 \
  --interval-ms 650 \
  --with-aux-inputs \
  --aux-mode far-localization \
  --aux-count 3 \
  --aux-interval-ms 250
```

x86 辅助输入回归：
```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_precheck_021/valet_parking_mvp/x86 \
  --domain-id 160 \
  --timeout-ms 30000 \
  --count 3 \
  --interval-ms 650 \
  --with-aux-inputs \
  --aux-mode all-valid \
  --aux-count 3 \
  --aux-interval-ms 250

bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_precheck_021/valet_parking_mvp/x86 \
  --domain-id 161 \
  --timeout-ms 30000 \
  --count 3 \
  --interval-ms 650 \
  --with-aux-inputs \
  --aux-mode bad-obstacle-geometry \
  --aux-count 3 \
  --aux-interval-ms 250

bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_precheck_021/valet_parking_mvp/x86 \
  --domain-id 162 \
  --timeout-ms 30000 \
  --count 3 \
  --interval-ms 650 \
  --with-aux-inputs \
  --aux-mode all-valid \
  --aux-count 3 \
  --aux-interval-ms 250 \
  --disable-aux-input-topics
```

m57 交叉编译：
```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform m57 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_precheck_021
```

---

## 4. 验收结果

| 验收项 | 结果 | 证据 |
|---|---|---|
| x86 构建 | PASS | `out/valet_parking_precheck_021/valet_parking_mvp/x86/lib/libvalet_parking.so` |
| x86 默认 smoke | PASS | `domain_158_20260727_113418_870` |
| PATH_PROVIDER_PRECHECK 正常分支 | PASS | 默认 smoke 显示 `PATH_PROVIDER_PRECHECK ok, xy_bounds_span=16.000x23.283, dest_points=4, obstacle_segments=0` |
| x86 all-valid 辅助输入 | PASS | `domain_160_20260727_113457_719`，显示 `obstacle_segments=4` |
| x86 bad obstacle geometry | PASS | `domain_161_20260727_113457_720`，显示 `aux obstacles rejected` 且 `external_obstacles=0` |
| x86 disable aux input topics | PASS | `domain_162_20260727_113457_718` |
| x86 moving-localization smoke | PASS | `domain_159_20260727_113418_884` |
| TRACE_REPLAN/history_splice | PASS | 第二帧显示 `replan=TRACE_REPLAN, warm_start=history_splice, warm_start_points=96` |
| x86 far-localization | PASS | `domain_157_20260727_113359_715` |
| 远定位保护 | PASS | runner 显示 `vehicle_lot_precheck failed`，subscriber 显示 `is_estop=true` |
| m57 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_precheck_021/valet_parking_mvp/m57/lib/libvalet_parking.so` |
| m57 ELF 架构 | PASS | `ELF 64-bit LSB shared object, ARM aarch64` |
| m57 MagnaDDS 依赖 | PASS | `libmagna-dds-core.so.1`、`libmagna-dds-impl.so` |
| m57 板端运行 | NOT_RUN | 当前无 m57 板端 |

---

## 5. 本阶段修改文件

- `applications/source/valet_parking/src/valet_parking_stage_parking_adapter.cpp`
- `applications/source/valet_parking_tools/aux_input_mock_publisher/main.cpp`
- `applications/source/valet_parking_tools/smoke_valet_parking_x86.sh`
- `applications/source/valet_parking/README.md`
- `applications/source/Docs/valet_parking_magnadds/STATUS.yaml`
- `applications/source/Docs/valet_parking_magnadds/status_snapshots/021_项目状态快照_PATH_PROVIDER轻量PreCheck切片.md`

---

## 6. 下一步

进入 NEXT-022：继续沿用简化 DDS Topic，对照完整 `OpenSpacePathProvider` 大类筛选下一批可迁移切片。

建议优先级：

1. 继续评估不依赖完整 `Frame/DependencyInjector` 的 path strategy、trace adjust、边界诊断逻辑。
2. 保持现有默认、all-valid、bad-obstacle-geometry、disable aux、moving-localization、far-localization smoke 全部通过。
3. 保持 m57 交叉编译通过，但不写成板端运行通过。
4. 不直接引入完整线程管理、NLP smoother 或真实车端 Topic 协议。
