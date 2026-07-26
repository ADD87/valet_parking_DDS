# 项目状态快照 016：DDS_AUX_INPUT实收样本验证

- 快照编号：016
- 日期：2026-07-27
- 当前阶段：Fast-2 - `.so` + typed DDS + ROI_DECIDER/PATH_PROVIDER/PATH_PARTITION/SPEED_OPTIMIZER DDS多输入实收样本验证
- 阶段状态：PASS_DDS_AUX_INPUT_SMOKE_X86_M57_STATIC_BLOCKED_BOARD
- 创建原因：015 阶段已经接入辅助 DDS reader，但只验证了无辅助发布者时不破坏主链路；本阶段补齐 localization/chassis/obstacle 三类样本真实进入 reader 的 x86 smoke。

---

## 1. 本次完成事项

- [x] 新增 `aux_input_mock_publisher` 工具。
- [x] 该工具使用 MagnaDDS typed API 同时发布：
  - `LocalizationEstimate` -> `/localization/estimate`
  - `ChassisState` -> `/chassis/state`
  - `ObstacleArray` -> `/perception/obstacles`
- [x] `valet_parking_mvp_bom.yaml` 已纳入 `aux_input_mock_publisher`。
- [x] `smoke_valet_parking_x86.sh` 新增可选参数：
  - `--with-aux-inputs`
  - `--aux-count`
  - `--aux-interval-ms`
- [x] 带 `--with-aux-inputs` 时，smoke 会先发布辅助样本，再发布 `/selected_slot`。
- [x] 带 `--with-aux-inputs` 时，smoke 会检查 runner 日志：
  - `aux localization`
  - `aux chassis`
  - `aux obstacles`
  - `external_vehicle=true`
  - `external_obstacles>0`
- [x] 默认 smoke 行为保持不变：不加 `--with-aux-inputs` 时不运行辅助 publisher，仍验证无辅助发布者回退路径。

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

本阶段验证的是当前临时 IDL 的 DDS 实收样本闭环，不是最终车端协议对齐。

当前状态是：

- `aux_input_mock_publisher` 发布的是 MVP 临时样本。
- localization 样本使用默认 fake vehicle 对齐的 `x=0, y=0, heading=0`，用于验证外部车辆状态能生效但不改变现有成功路径。
- obstacle 样本放在远离路径的位置，目标是验证输入进入链路，而不是制造碰撞场景。
- x86 已验证 runner 实际消费三类辅助样本，并输出正常 `PlanningTrajectory`。
- m57 只完成交叉编译、ELF 架构和 MagnaDDS 依赖检查，尚未做真实板端运行。

仍未完成：

- 辅助输入异常模式验证，例如 invalid localization、chassis-only、invalid obstacles。
- 真实车端 localization/chassis/obstacle Topic 契约对齐。
- m57 板端 runtime 验证。
- 完整原车 `Frame/DependencyInjector`。
- 完整 `OpenSpacePathProvider` 大类。
- NLP smoother。

---

## 3. 本次验证命令

x86 构建：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform x86 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_aux_smoke_016
```

x86 带辅助输入 smoke：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_aux_smoke_016/valet_parking_mvp/x86 \
  --domain-id 18 \
  --timeout-ms 20000 \
  --count 3 \
  --interval-ms 500 \
  --with-aux-inputs \
  --aux-count 3 \
  --aux-interval-ms 200
```

x86 默认无辅助输入 smoke：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_aux_smoke_016/valet_parking_mvp/x86 \
  --domain-id 19 \
  --timeout-ms 20000 \
  --count 3 \
  --interval-ms 500
```

m57 构建：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform m57 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_aux_smoke_016
```

---

## 4. 验收结果

| 验收项 | 结果 | 证据 |
|---|---|---|
| `aux_input_mock_publisher` x86 构建 | PASS | `out/valet_parking_aux_smoke_016/valet_parking_mvp/x86/app/aux_input_mock_publisher` |
| x86 带辅助输入 smoke | PASS | runner 日志出现 `aux localization`、`aux chassis`、`aux obstacles` |
| 外部车辆状态生效 | PASS | runner 状态显示 `external_vehicle=true` |
| 外部障碍物生效 | PASS | runner 状态显示 `external_obstacles=1` |
| 带辅助输入时轨迹输出 | PASS | subscriber 收到 `points=179, length=7.64356, is_estop=false` |
| 默认无辅助输入 smoke | PASS | runner 状态显示 `external_vehicle=false, external_obstacles=0` |
| m57 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_aux_smoke_016/valet_parking_mvp/m57/lib/libvalet_parking.so` |
| m57 `aux_input_mock_publisher` 生成 | PASS | `out/valet_parking_aux_smoke_016/valet_parking_mvp/m57/app/aux_input_mock_publisher` |
| m57 ELF 架构 | PASS | `ELF 64-bit LSB executable, ARM aarch64` |
| m57 MagnaDDS 依赖 | PASS | build output 显示 `libmagna-dds-core.so.1` 和 `libmagna-dds-impl.so` |
| m57 板端运行 | NOT_RUN | 当前无 m57 板端 |

---

## 5. 下一步

继续下一阶段：辅助输入边界硬化。

建议先做：

- 扩展 `aux_input_mock_publisher` 的模式参数。
- 验证 invalid localization 会清除外部车辆状态并回退 fake vehicle。
- 验证 chassis-only 不会误置 `external_vehicle=true`。
- 验证 invalid obstacle array 会清空外部障碍物。
- 真实车端 Topic 契约确认前，继续把当前 IDL 标注为临时契约。
