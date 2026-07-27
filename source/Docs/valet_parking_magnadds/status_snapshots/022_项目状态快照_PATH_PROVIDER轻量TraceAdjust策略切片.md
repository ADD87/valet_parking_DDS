# 项目状态快照 022：PATH_PROVIDER轻量TraceAdjust策略切片

- 快照编号：022
- 日期：2026-07-27
- 当前阶段：Fast-2 - `.so` + typed DDS + ROI_DECIDER/PATH_PROVIDER/PATH_PARTITION/SPEED_OPTIMIZER light trace adjust slice
- 阶段状态：PASS_PATH_PROVIDER_TRACE_ADJUST_SLICE_X86_M57_STATIC_BLOCKED_BOARD
- 创建原因：继续沿用简化 DDS Topic，对照完整 `OpenSpacePathProvider::LocalTraceAdjustSearchStrategy`，迁入不依赖完整 `Frame/DependencyInjector`、线程管理或 NLP smoother 的轻量策略切片。

---

## 1. 本次完成事项

- [x] 在 `ValetParkingStageParkingAdapter` 的 PATH_PROVIDER 策略构造中新增轻量 `trace_adjust` 分支。
- [x] `trace_adjust` 只在保守条件下启用：
  - 当前重规划状态为 `TRACE_REPLAN`。
  - history warm start 已被接受并生成可用路径。
  - warm start 路径点数大于 1，路径长度不小于 0.50m。
- [x] 对照原始 `OpenSpacePathProvider::LocalTraceAdjustSearchStrategy` 设置轻量参数：
  - `trace_adjust=true`
  - `target_s=8`
  - `finish_l_threshold=0.05`
  - `finish_theta_threshold=0.02`
  - `xy_bounds=[-30,30,-30,30]`
  - `cut_off_strategy=0`
  - `is_plan_from_start=true`
- [x] PATH_PROVIDER 状态日志新增可验收字段：
  - `trace_adjust=true|false`
  - `trace_adjust_source=history_warm_start|none`
  - `trace_adjust_points`
  - `trace_adjust_target_s`
  - `trace_adjust_finish_l`
  - `trace_adjust_finish_theta`
  - `trace_adjust_bounds`
- [x] 批量修订 `moving-localization` mock：
  - 前两组定位保持 `(0,0)`，用于生成第一帧历史路径。
  - 后续定位改为小幅横向移动，例如 `(0.80,0.70)`、`(1.00,0.90)`，用于稳定触发 `TRACE_REPLAN`。
  - 移动幅度仍控制在 warm start 接受阈值内，避免把本阶段变成异常输入测试。
- [x] 批量硬化 `smoke_valet_parking_x86.sh`：
  - moving-localization 默认有效发布窗口提升到主输入至少 6 帧、aux 至少 8 组。
  - moving-localization 等待 `trace_adjust=true` 后再收尾。
  - far-localization 等待 `vehicle_lot_precheck failed`。
  - moving-localization 新增 `trace_adjust=true` 和 `trace_adjust_points>0` 断言。

当前主链路保持不变：

```text
SelectedSlot
  -> ROI_DECIDER
  -> PATH_PROVIDER_PRECHECK
  -> PATH_PROVIDER
       -> history warm_start
       -> trace_adjust strategy
  -> PATH_PARTITION
  -> SPEED_OPTIMIZER
  -> PlanningTrajectory
```

---

## 2. 重要边界

本阶段不是完整 `OpenSpacePathProvider` 大类接入完成。

已迁入的是轻量 `trace_adjust` 策略参数与输入路径切片，目的是让当前 DDS adapter 在 `TRACE_REPLAN + history_splice` 分支下更接近原始路径搜索策略语义。

仍未接入：

- 完整 `OpenSpacePathProvider` 线程管理。
- 完整 `OpenSpacePathProvider::PreCheck`。
- NLP smoother。
- 完整 `Frame/DependencyInjector` history。
- 完整原车 path strategy 全量配置。
- 真实车端定位/底盘/障碍物 Topic 协议。

m57 仍只完成交叉编译、ELF 和依赖检查，尚未做真实板端运行验证。

---

## 3. 本次验证命令

x86 构建：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform x86 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_trace_adjust_022
```

x86 moving-localization smoke：

```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_trace_adjust_022/valet_parking_mvp/x86 \
  --domain-id 226 \
  --timeout-ms 30000 \
  --count 6 \
  --interval-ms 650 \
  --with-aux-inputs \
  --aux-mode moving-localization \
  --aux-count 8 \
  --aux-interval-ms 650
```

x86 默认 smoke：

```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_trace_adjust_022/valet_parking_mvp/x86 \
  --domain-id 227 \
  --timeout-ms 30000 \
  --count 3 \
  --interval-ms 650
```

x86 辅助输入回归：

```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_trace_adjust_022/valet_parking_mvp/x86 \
  --domain-id 228 \
  --timeout-ms 30000 \
  --count 3 \
  --interval-ms 650 \
  --with-aux-inputs \
  --aux-mode all-valid \
  --aux-count 3 \
  --aux-interval-ms 250

bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_trace_adjust_022/valet_parking_mvp/x86 \
  --domain-id 229 \
  --timeout-ms 30000 \
  --count 3 \
  --interval-ms 650 \
  --with-aux-inputs \
  --aux-mode bad-obstacle-geometry \
  --aux-count 3 \
  --aux-interval-ms 250

bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_trace_adjust_022/valet_parking_mvp/x86 \
  --domain-id 230 \
  --timeout-ms 30000 \
  --count 3 \
  --interval-ms 650 \
  --with-aux-inputs \
  --aux-mode all-valid \
  --aux-count 3 \
  --aux-interval-ms 250 \
  --disable-aux-input-topics
```

x86 far-localization smoke：

```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_trace_adjust_022/valet_parking_mvp/x86 \
  --domain-id 180 \
  --timeout-ms 30000 \
  --count 3 \
  --interval-ms 650 \
  --with-aux-inputs \
  --aux-mode far-localization \
  --aux-count 3 \
  --aux-interval-ms 250
```

m57 交叉编译：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform m57 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_trace_adjust_022
```

---

## 4. 验收结果

| 验收项 | 结果 | 证据 |
|---|---|---|
| x86 构建 | PASS | `out/valet_parking_trace_adjust_022/valet_parking_mvp/x86/lib/libvalet_parking.so` |
| x86 moving-localization smoke | PASS | `domain_226_20260727_120239_766` |
| TRACE_REPLAN/history_splice | PASS | 第二帧显示 `replan=TRACE_REPLAN, warm_start=history_splice, warm_start_reject=accepted, warm_start_points=73` |
| trace_adjust 策略 | PASS | 第二帧显示 `trace_adjust=true, trace_adjust_source=history_warm_start, trace_adjust_points=73` |
| trace_adjust 参数 | PASS | 第二帧显示 `trace_adjust_target_s=8, trace_adjust_finish_l=0.05, trace_adjust_finish_theta=0.02, trace_adjust_bounds=[-30,30,-30,30]` |
| x86 默认 smoke | PASS | `domain_227_20260727_120313_728` |
| x86 all-valid 辅助输入 | PASS | `domain_228_20260727_120333_858` |
| x86 bad obstacle geometry | PASS | `domain_229_20260727_120356_1033` |
| x86 disable aux input topics | PASS | `domain_230_20260727_120429_719` |
| x86 far-localization | PASS | `domain_180_20260727_120639_730` |
| 远定位保护 | PASS | runner 显示 `vehicle_lot_precheck failed`，subscriber 显示 `is_estop=true` |
| m57 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_trace_adjust_022/valet_parking_mvp/m57/lib/libvalet_parking.so` |
| m57 ELF 架构 | PASS | `ELF 64-bit LSB shared object, ARM aarch64` |
| m57 MagnaDDS 依赖 | PASS | `libmagna-dds-core.so.1`、`libmagna-dds-impl.so` |
| m57 板端运行 | NOT_RUN | 当前无 m57 板端 |

补充说明：一次使用 domain `231` 时 MagnaDDS `DomainParticipant` 创建失败，判断属于 DDS domain 或本机运行环境限制，不作为本阶段代码回归。后续本机 smoke 优先使用已验证可用的 domain 段，例如 `180`、`226`-`230` 或更低范围。

---

## 5. 本阶段修改文件

- `applications/source/valet_parking/src/valet_parking_stage_parking_adapter.cpp`
- `applications/source/valet_parking_tools/aux_input_mock_publisher/main.cpp`
- `applications/source/valet_parking_tools/smoke_valet_parking_x86.sh`
- `applications/source/valet_parking/README.md`
- `applications/source/Docs/valet_parking_magnadds/STATUS.yaml`
- `applications/source/Docs/valet_parking_magnadds/status_snapshots/022_项目状态快照_PATH_PROVIDER轻量TraceAdjust策略切片.md`

---

## 6. 下一步

进入 NEXT-023：继续沿用简化 DDS Topic，在不搬完整 `OpenSpacePathProvider` 大类的前提下，筛选下一批可迁移 PATH_PROVIDER 逻辑。

建议优先级：

1. 继续评估 path strategy 参数诊断、warm_start/trace_adjust 边界保护和路径生成失败解释。
2. 保持现有默认、all-valid、bad-obstacle-geometry、disable aux、moving-localization、far-localization smoke 全部通过。
3. 保持 m57 交叉编译通过，但不写成板端运行通过。
4. 不直接引入完整线程管理、NLP smoother、完整 `Frame/DependencyInjector` 或真实车端 Topic 协议。
