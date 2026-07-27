# 项目状态快照 023：PATH_PROVIDER诊断增强与TraceAdjust边界修订

- 快照编号：023
- 日期：2026-07-27
- 当前阶段：Fast-2 - `.so` + typed DDS + ROI_DECIDER/PATH_PROVIDER/PATH_PARTITION/SPEED_OPTIMIZER path diagnostics slice
- 阶段状态：PASS_PATH_PROVIDER_DIAGNOSTICS_X86_M57_STATIC_BLOCKED_BOARD
- 创建原因：NEXT-022 已能触发轻量 trace adjust，但日志里缺少“为什么启用/为什么没启用”的原因；同时 moving-localization mock 的递增位移会让测试样本偶发漂出 warm start 接受阈值，需要把 smoke 变成稳定验证。

---

## 1. 本次完成事项

- [x] 新增 `TraceAdjustDecision` 和 `EvaluateTraceAdjust()`，把 trace adjust 判定原因显式化：
  - `not_trace_replan`
  - `unsafe_replan_status`
  - `no_trace_path`
  - `trace_path_too_short`
  - `trace_path_length_short`
  - `accepted`
- [x] PATH_PROVIDER 成功日志新增字段：
  - `trace_adjust_reject`
  - `trace_adjust_path_length`
  - `trace_adjust_min_length`
- [x] PATH_PROVIDER 失败路径统一追加输入摘要：
  - exception
  - generator `error_msg`
  - insufficient path points
- [x] 失败路径摘要包含：
  - `replan`
  - `reason`
  - `warm_start`
  - `warm_start_reject`
  - `warm_start_points`
  - `warm_start_history_points`
  - `trace_adjust`
  - `trace_adjust_reject`
  - `trace_adjust_path_length`
  - `external_vehicle`
  - `external_obstacles`
- [x] `smoke_valet_parking_x86.sh` 的 moving-localization 校验新增：
  - `trace_adjust_reject=accepted`
  - `trace_adjust_path_length>0`
- [x] 修订 `aux_input_mock_publisher --mode=moving-localization`：
  - 前两组定位保持 `(0,0)`，用于生成第一帧历史路径。
  - 后续定位固定为 `(0.80,0.70)`，用于稳定触发 `TRACE_REPLAN` 且保持 warm start 可接受。

---

## 2. 本次发现并修复的问题

第一次 NEXT-023 moving-localization smoke 使用 domain `181` 时失败。日志显示 runner 第二帧读到了较晚的 aux 样本，例如 `(1.40,1.30)`，导致 warm start 被 `lateral_offset_large` 拒绝：

```text
warm_start=none
warm_start_reject=lateral_offset_large
trace_adjust=false
trace_adjust_reject=no_trace_path
```

这不是 PATH_PROVIDER 算法失败，而是 mock 的递增位移把测试目标从“验证 trace adjust”变成了“验证 warm start 拒绝”。因此本阶段把 moving-localization mock 改为稳定小偏移，后续 smoke 不再依赖 runner 与 aux publisher 的偶然时序。

---

## 3. 本次验证命令

x86 构建：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform x86 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_diagnostics_023
```

x86 moving-localization smoke：

```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_diagnostics_023/valet_parking_mvp/x86 \
  --domain-id 182 \
  --timeout-ms 30000 \
  --count 6 \
  --interval-ms 650 \
  --with-aux-inputs \
  --aux-mode moving-localization \
  --aux-count 8 \
  --aux-interval-ms 650
```

x86 默认和辅助输入回归：

```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_diagnostics_023/valet_parking_mvp/x86 \
  --domain-id 183 \
  --timeout-ms 30000 \
  --count 3 \
  --interval-ms 650

bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_diagnostics_023/valet_parking_mvp/x86 \
  --domain-id 184 \
  --timeout-ms 30000 \
  --count 3 \
  --interval-ms 650 \
  --with-aux-inputs \
  --aux-mode all-valid \
  --aux-count 3 \
  --aux-interval-ms 250

bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_diagnostics_023/valet_parking_mvp/x86 \
  --domain-id 185 \
  --timeout-ms 30000 \
  --count 3 \
  --interval-ms 650 \
  --with-aux-inputs \
  --aux-mode bad-obstacle-geometry \
  --aux-count 3 \
  --aux-interval-ms 250

bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_diagnostics_023/valet_parking_mvp/x86 \
  --domain-id 186 \
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
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_diagnostics_023/valet_parking_mvp/x86 \
  --domain-id 187 \
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
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_diagnostics_023
```

---

## 4. 验收结果

| 验收项 | 结果 | 证据 |
|---|---|---|
| x86 构建 | PASS | `out/valet_parking_diagnostics_023/valet_parking_mvp/x86/lib/libvalet_parking.so` |
| x86 moving-localization smoke | PASS | `domain_182_20260727_122335_979` |
| trace adjust 诊断 | PASS | 第二帧显示 `trace_adjust_reject=accepted, trace_adjust_points=96, trace_adjust_path_length=7.24982, trace_adjust_min_length=0.5` |
| x86 默认 smoke | PASS | `domain_183_20260727_122358_1179` |
| trace adjust 未启用原因 | PASS | 默认第一帧显示 `trace_adjust_reject=not_trace_replan` |
| x86 all-valid 辅助输入 | PASS | `domain_184_20260727_122416_1314` |
| x86 bad obstacle geometry | PASS | `domain_185_20260727_122441_1498` |
| x86 disable aux input topics | PASS | `domain_186_20260727_122500_1656` |
| x86 far-localization | PASS | `domain_187_20260727_122524_1842` |
| m57 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_diagnostics_023/valet_parking_mvp/m57/lib/libvalet_parking.so` |
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
- `applications/source/Docs/valet_parking_magnadds/status_snapshots/023_项目状态快照_PATH_PROVIDER诊断增强与TraceAdjust边界修订.md`

---

## 6. 下一步

进入 NEXT-024：继续沿用简化 DDS Topic，优先增加可控负向样本或更细的边界 smoke，用来直接验证：

1. PATH_PROVIDER 失败路径诊断是否能说明输入状态。
2. warm start 拒绝原因是否能被稳定触发和断言。
3. trace adjust 拒绝原因是否能被稳定触发和断言。
4. x86 smoke 与 m57 交叉编译继续保持通过。

仍不引入完整 `Frame/DependencyInjector`、线程管理、NLP smoother 或真实车端 Topic 协议。
