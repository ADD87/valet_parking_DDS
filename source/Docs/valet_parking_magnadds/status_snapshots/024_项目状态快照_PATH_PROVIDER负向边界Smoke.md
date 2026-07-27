# 项目状态快照 024：PATH_PROVIDER负向边界Smoke

- 快照编号：024
- 日期：2026-07-27
- 当前阶段：Fast-2 - `.so` + typed DDS + ROI_DECIDER/PATH_PROVIDER/PATH_PARTITION/SPEED_OPTIMIZER negative boundary smoke
- 阶段状态：PASS_PATH_PROVIDER_NEGATIVE_BOUNDARY_X86_M57_STATIC_BLOCKED_BOARD
- 创建原因：NEXT-023 已补齐 PATH_PROVIDER 诊断字段，但缺少一个稳定的负向样本来证明 `warm_start` 拒绝原因和 `trace_adjust` 拒绝原因可以被 smoke 直接断言。

---

## 1. 本次完成事项

- [x] `aux_input_mock_publisher` 新增 `moving-localization-large` 模式：
  - 前两组定位保持 `(0,0)`，用于生成第一帧历史路径。
  - 后续定位固定为 `(1.40,1.30)`，用于稳定触发 `TRACE_REPLAN`。
  - 障碍物 id 与普通 `moving-localization` 一样固定，避免障碍物签名变化触发 `BLOCK_BY_STATIC_OBSTACLE` 干扰 warm start 判定。
- [x] `smoke_valet_parking_x86.sh` 新增 `moving-localization-large` 支持：
  - 纳入后台并发发布模式。
  - 自动把主输入发布窗口提升到至少 6 帧、aux 发布窗口提升到至少 8 组。
  - 等待 `warm_start_reject=lateral_offset_large` 出现后收尾。
- [x] `moving-localization-large` 新增断言：
  - `replan=TRACE_REPLAN`
  - `warm_start=none`
  - `warm_start_reject=lateral_offset_large`
  - `warm_start_points=0`
  - `trace_adjust=false`
  - `trace_adjust_reject=no_trace_path`
  - `trace_adjust_path_length=0`

---

## 2. 重要边界

本阶段没有改变 PATH_PROVIDER 主算法。

新增的是可控负向样本和 smoke 断言，用于证明当车辆起点相对历史路径横向偏移过大时：

- 历史 warm start 会被拒绝。
- trace adjust 不会在没有 trace path 的情况下启用。
- 日志能明确解释拒绝原因。

本阶段仍不引入完整 `Frame/DependencyInjector`、线程管理、NLP smoother 或真实车端 Topic 协议。

m57 仍只完成交叉编译、ELF 和依赖检查，尚未做真实板端运行验证。

---

## 3. 本次验证命令

x86 构建：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform x86 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_negative_024
```

x86 moving-localization-large 负向 smoke：

```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_negative_024/valet_parking_mvp/x86 \
  --domain-id 188 \
  --timeout-ms 30000 \
  --count 6 \
  --interval-ms 650 \
  --with-aux-inputs \
  --aux-mode moving-localization-large \
  --aux-count 8 \
  --aux-interval-ms 650
```

x86 回归 smoke：

```bash
# moving-localization
--domain-id 189 --with-aux-inputs --aux-mode moving-localization

# default
--domain-id 190

# far-localization
--domain-id 191 --with-aux-inputs --aux-mode far-localization

# all-valid
--domain-id 192 --with-aux-inputs --aux-mode all-valid

# bad-obstacle-geometry
--domain-id 193 --with-aux-inputs --aux-mode bad-obstacle-geometry

# disable aux input topics
--domain-id 194 --with-aux-inputs --aux-mode all-valid --disable-aux-input-topics
```

m57 交叉编译：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform m57 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_negative_024
```

---

## 4. 验收结果

| 验收项 | 结果 | 证据 |
|---|---|---|
| x86 构建 | PASS | `out/valet_parking_negative_024/valet_parking_mvp/x86/lib/libvalet_parking.so` |
| x86 moving-localization-large | PASS | `domain_188_20260727_130916_1650` |
| warm start 拒绝原因 | PASS | 第二帧显示 `warm_start_reject=lateral_offset_large, warm_start_points=0` |
| trace adjust 拒绝原因 | PASS | 第二帧显示 `trace_adjust=false, trace_adjust_reject=no_trace_path, trace_adjust_path_length=0` |
| x86 moving-localization 回归 | PASS | `domain_189_20260727_130950_717` |
| x86 默认 smoke | PASS | `domain_190_20260727_131006_877` |
| x86 far-localization | PASS | `domain_191_20260727_131022_1013` |
| x86 all-valid 辅助输入 | PASS | `domain_192_20260727_131338_732` |
| x86 bad obstacle geometry | PASS | `domain_193_20260727_131411_724` |
| x86 disable aux input topics | PASS | `domain_194_20260727_131411_721` |
| m57 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_negative_024/valet_parking_mvp/m57/lib/libvalet_parking.so` |
| m57 ELF 架构 | PASS | `ELF 64-bit LSB shared object, ARM aarch64` |
| m57 MagnaDDS 依赖 | PASS | `libmagna-dds-core.so.1`、`libmagna-dds-impl.so` |
| m57 板端运行 | NOT_RUN | 当前无 m57 板端 |

---

## 5. 本阶段修改文件

- `applications/source/valet_parking_tools/aux_input_mock_publisher/main.cpp`
- `applications/source/valet_parking_tools/smoke_valet_parking_x86.sh`
- `applications/source/valet_parking/README.md`
- `applications/source/Docs/valet_parking_magnadds/STATUS.yaml`
- `applications/source/Docs/valet_parking_magnadds/status_snapshots/024_项目状态快照_PATH_PROVIDER负向边界Smoke.md`

---

## 6. 下一步

进入 NEXT-025：继续沿用简化 DDS Topic，优先考虑增加 PATH_PROVIDER_PRECHECK 或 SelectedSlot 几何类负向样本，直接验证 PATH_PROVIDER 失败/前置拦截诊断。

仍不引入完整 `Frame/DependencyInjector`、线程管理、NLP smoother 或真实车端 Topic 协议。
