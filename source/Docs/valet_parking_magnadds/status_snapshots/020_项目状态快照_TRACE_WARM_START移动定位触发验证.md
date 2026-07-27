# 项目状态快照 020：TRACE_WARM_START移动定位触发验证

- 快照编号：020
- 日期：2026-07-27
- 当前阶段：Fast-2 - `.so` + typed DDS + ROI_DECIDER/PATH_PROVIDER/PATH_PARTITION/SPEED_OPTIMIZER trace warm_start smoke
- 阶段状态：PASS_TRACE_WARM_START_SMOKE_X86_M57_STATIC_BLOCKED_BOARD
- 创建原因：NEXT-019 已完成 `warm_start`/`path_strategy` 轻量切片，但默认 smoke 只能验证编译接线与保守条件，不能直接触发 `TRACE_REPLAN -> history_splice`。本阶段新增可控移动定位 mock/smoke，补齐该分支的运行态验证。

---

## 1. 本次完成事项

- [x] `aux_input_mock_publisher` 新增 `moving-localization` 模式：
  - 前期发布原点定位。
  - 后续发布小幅移动定位，触发历史起点不匹配。
  - 该模式下障碍物 id 固定，避免障碍物签名变化误触发 `BLOCK_BY_STATIC_OBSTACLE`。
- [x] `smoke_valet_parking_x86.sh` 新增 `moving-localization` 验证：
  - aux publisher 与 SelectedSlot publisher 并发运行。
  - 等待 runner 处理到第二帧，避免 subscriber 收到第一条轨迹后过早收尾。
  - 校验 `replan=TRACE_REPLAN`、`warm_start=history_splice`、`warm_start_points>0`、`strategy_kappa_cost=true`、`strategy_limit_steer=true`。
- [x] `ValetParkingStageParkingAdapter` 增加 warm start 诊断字段：
  - `warm_start_reject`
  - `warm_start_history_points`
  - `warm_start_s`
  - `warm_start_l`
  - `warm_start_path_s=[front,back]`
- [x] 修复 warm start 截尾逻辑：
  - 问题：standalone `partitioned_path` 中的 `PathPoint.s` 可能全部为 0，原先用 `path_point.s` 找历史尾段会得到 `no_tail_after_start`。
  - 修订：adapter 改为基于历史路径点的 `x/y` 计算几何累计距离和最近段投影，再按投影后的点索引截取尾段。
  - 保守条件不变：仅在 `TRACE_REPLAN` 且无目标、障碍物、速度、碰撞类 replan 原因时使用历史 warm start。

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

本阶段仍不是完整 `OpenSpacePathProvider` 大类接入完成。

已验证的是轻量 trace warm start 分支：

- 受控移动定位触发 `TRACE_REPLAN`。
- adapter 从上一帧路径截取历史尾段作为 `OpenSpacePathInput::warm_start_path`。
- 轻量策略字段按 warm start 启用 `strategy_kappa_cost` 和 `strategy_init_move`。

仍未接入：

- 完整 `OpenSpacePathProvider` 线程管理。
- 完整 `PreCheck`。
- 完整 trace adjust 策略。
- 完整 splice trajectory 与上一帧发布轨迹的时间拼接。
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
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_trace_warm_start_020
```

x86 moving-localization smoke：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_trace_warm_start_020/valet_parking_mvp/x86 \
  --domain-id 141 \
  --timeout-ms 30000 \
  --count 4 \
  --interval-ms 650 \
  --with-aux-inputs \
  --aux-mode moving-localization \
  --aux-count 5 \
  --aux-interval-ms 650
```

x86 回归：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_trace_warm_start_020/valet_parking_mvp/x86 \
  --domain-id 142 \
  --timeout-ms 25000 \
  --count 3 \
  --interval-ms 500

bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_trace_warm_start_020/valet_parking_mvp/x86 \
  --domain-id 143 \
  --timeout-ms 25000 \
  --count 3 \
  --interval-ms 500 \
  --with-aux-inputs \
  --aux-mode all-valid \
  --aux-count 3 \
  --aux-interval-ms 200

bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_trace_warm_start_020/valet_parking_mvp/x86 \
  --domain-id 144 \
  --timeout-ms 25000 \
  --count 3 \
  --interval-ms 500 \
  --with-aux-inputs \
  --aux-mode bad-obstacle-geometry \
  --aux-count 3 \
  --aux-interval-ms 200

bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_trace_warm_start_020/valet_parking_mvp/x86 \
  --domain-id 145 \
  --timeout-ms 25000 \
  --count 3 \
  --interval-ms 500 \
  --with-aux-inputs \
  --aux-mode all-valid \
  --aux-count 3 \
  --aux-interval-ms 200 \
  --disable-aux-input-topics
```

m57 交叉编译：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform m57 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_trace_warm_start_020
```

---

## 4. 验收结果

| 验收项 | 结果 | 证据 |
|---|---|---|
| x86 构建 | PASS | `out/valet_parking_trace_warm_start_020/valet_parking_mvp/x86/lib/libvalet_parking.so` |
| x86 moving-localization smoke | PASS | `domain_141_20260727_105040_1017` |
| TRACE_REPLAN 触发 | PASS | 第二帧显示 `replan=TRACE_REPLAN, reason=history_start_mismatch` |
| history_splice warm start | PASS | 第二帧显示 `warm_start=history_splice, warm_start_reject=accepted, warm_start_points=96` |
| path strategy 字段 | PASS | 第二帧显示 `strategy_init_move=1, strategy_kappa_cost=true, strategy_limit_steer=true` |
| x86 默认 smoke | PASS | `domain_142_20260727_105243_726` |
| x86 all-valid 辅助输入 | PASS | `domain_143_20260727_105252_803` |
| x86 bad obstacle geometry | PASS | `domain_144_20260727_105303_918` |
| x86 disable aux input topics | PASS | `domain_145_20260727_105315_1036` |
| m57 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_trace_warm_start_020/valet_parking_mvp/m57/lib/libvalet_parking.so` |
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
- `applications/source/Docs/valet_parking_magnadds/status_snapshots/020_项目状态快照_TRACE_WARM_START移动定位触发验证.md`

---

## 6. 下一步

进入 NEXT-021：轻量 `PreCheck`/输入有效性切片。

建议优先级：

1. 对照完整 `OpenSpacePathProvider::PreCheck`，筛选不依赖完整 `Frame/DependencyInjector` 的几何/边界检查。
2. 先接入能用现有 adapter 输入表达的检查，例如起点、障碍物、目标区域的局部边界。
3. 保持现有默认 smoke、辅助输入 smoke、moving-localization trace warm start smoke、m57 交叉编译全部通过。
4. 不直接引入完整线程管理、NLP smoother 或真实车端 Topic 协议。
