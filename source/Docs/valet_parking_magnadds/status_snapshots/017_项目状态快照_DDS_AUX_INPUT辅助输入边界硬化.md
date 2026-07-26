# 项目状态快照 017：DDS_AUX_INPUT辅助输入边界硬化

- 快照编号：017
- 日期：2026-07-27
- 当前阶段：Fast-2 - `.so` + typed DDS + ROI_DECIDER/PATH_PROVIDER/PATH_PARTITION/SPEED_OPTIMIZER 辅助输入异常与局部输入边界硬化
- 阶段状态：PASS_DDS_AUX_INPUT_HARDENING_X86_M57_STATIC_BLOCKED_BOARD
- 创建原因：016 已证明三类辅助 DDS 输入能被 runner 实收。本阶段继续验证异常辅助输入和局部输入不会污染 `ValetParkingStageParkingAdapter` 的外部车辆状态与障碍物状态。

---

## 1. 本次完成事项

- [x] `ValetParkingComponent` 新增辅助样本有限性和合法性检查。
- [x] invalid 或 NaN/Inf `LocalizationEstimate` 会清理外部车辆状态，并打印 `aux localization invalid ... (cleared_vehicle_state)`。
- [x] invalid 或 NaN/Inf `ChassisState` 会清理速度、加速度、档位补充状态，并打印 `aux chassis invalid ... (cleared_chassis_state)`。
- [x] invalid `ObstacleArray` 会清空外部障碍物，并打印 `aux obstacles invalid ... (cleared_obstacles)`。
- [x] 非法障碍物几何，例如长度或宽度小于等于 0，会拒绝该组障碍物、清空外部障碍物，并打印 `aux obstacles rejected ... (cleared_obstacles)`。
- [x] `aux_input_mock_publisher` 新增 `--mode` 参数，用于生成有效、异常和局部辅助输入。
- [x] `smoke_valet_parking_x86.sh` 新增 `--aux-mode` 和 `--disable-aux-input-topics`，并按模式校验 runner 日志。
- [x] smoke 日志目录改为 `smoke_latest/domain_<domain>_<timestamp>_<pid>/`，避免并行运行时多个 smoke 覆盖同一组日志。

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

## 2. 关键语义

本阶段没有改变主输入 `/selected_slot` 的异常策略：主输入坏了仍然输出显式 `estop`。

辅助输入的策略不同：辅助输入坏了不直接让主链路 `estop`，而是清理对应外部状态，避免把不可信的定位、底盘或障碍物带入后续路径和速度计算。

重要边界：

- `LocalizationEstimate` 是外部车辆状态生效门槛。
- `ChassisState` 只补充速度、加速度和档位；只有 chassis 不会让 `external_vehicle=true`。
- `ObstacleArray` 无效或障碍物几何非法时，外部障碍物会被清空。
- `--disable-aux-input-topics` 启动 runner 时，即使有辅助 publisher 发数据，runner 也不消费这些 Topic。
- 当前辅助 Topic 仍是 MVP 临时 IDL 契约，不代表真实车端协议已对齐。

---

## 3. 本次验证命令

x86 smoke 脚本语法检查：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash -n applications/source/valet_parking_tools/smoke_valet_parking_x86.sh
```

x86 正常辅助输入：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_aux_hardening_017/valet_parking_mvp/x86 \
  --domain-id 32 \
  --timeout-ms 20000 \
  --count 3 \
  --interval-ms 500 \
  --with-aux-inputs \
  --aux-mode all-valid \
  --aux-count 3 \
  --aux-interval-ms 200
```

x86 异常辅助输入矩阵：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_aux_hardening_017/valet_parking_mvp/x86 \
  --domain-id <domain> \
  --timeout-ms 20000 \
  --count 3 \
  --interval-ms 500 \
  --with-aux-inputs \
  --aux-mode <mode> \
  --aux-count 3 \
  --aux-interval-ms 200
```

本阶段使用的 `<mode>`：

```text
invalid-localization
nan-localization
chassis-only
invalid-obstacles
bad-obstacle-geometry
```

x86 关闭辅助输入订阅：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_aux_hardening_017/valet_parking_mvp/x86 \
  --domain-id 34 \
  --timeout-ms 20000 \
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
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_aux_hardening_017
```

---

## 4. 验收结果

| 验收项 | 结果 | 证据 |
|---|---|---|
| smoke 脚本语法 | PASS | `bash -n smoke_valet_parking_x86.sh` |
| `all-valid` | PASS | `domain_32_20260727_072220_761`，`external_vehicle=true`，`external_obstacles=1` |
| `bad-obstacle-geometry` | PASS | `domain_33_20260727_072243_938`，`aux obstacles rejected`，`external_obstacles=0` |
| `--disable-aux-input-topics` | PASS | `domain_34_20260727_072307_1116`，无 aux 消费日志，`external_vehicle=false`，`external_obstacles=0` |
| `invalid-localization` | PASS | `domain_35_20260727_072808_775`，`aux localization invalid`，`external_vehicle=false` |
| `nan-localization` | PASS | `domain_36_20260727_072828_941`，`aux localization invalid`，`external_vehicle=false` |
| `chassis-only` | PASS | `domain_37_20260727_072849_1105`，只有 `aux chassis`，`external_vehicle=false`，`external_obstacles=0` |
| `invalid-obstacles` | PASS | `domain_38_20260727_072913_1283`，`aux obstacles invalid`，`external_obstacles=0` |
| x86 轨迹输出 | PASS | 各 smoke 中 subscriber 收到 `points=179, length=7.64356, is_estop=false` |
| m57 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_aux_hardening_017/valet_parking_mvp/m57/lib/libvalet_parking.so` |
| m57 ELF 架构 | PASS | `ELF 64-bit LSB shared object, ARM aarch64` |
| m57 MagnaDDS 依赖 | PASS | `libmagna-dds-core.so.1`、`libmagna-dds-impl.so` |
| m57 板端运行 | NOT_RUN | 当前无 m57 板端 |

---

## 5. 本阶段修改文件

- `applications/source/valet_parking/src/valet_parking_component.cpp`
- `applications/source/valet_parking_tools/aux_input_mock_publisher/main.cpp`
- `applications/source/valet_parking_tools/smoke_valet_parking_x86.sh`
- `applications/source/valet_parking/README.md`
- `applications/source/Docs/valet_parking_magnadds/STATUS.yaml`
- `applications/source/Docs/valet_parking_magnadds/02_零基础解释_本阶段代码与DDS术语.md`
- `applications/source/Docs/valet_parking_magnadds/status_snapshots/017_项目状态快照_DDS_AUX_INPUT辅助输入边界硬化.md`

---

## 6. 下一步

进入 NEXT-018：评估并分阶段接入完整 `OpenSpacePathProvider` 大类。

建议先做依赖审计，不直接整块搬迁：

- 梳理 standalone `OpenSpacePathProvider` 的输入输出。
- 对比当前已经接入的 `OpenSpacePathGenerator + HybridAStar + PathPartition`。
- 标记必须接入、可延后、不可在当前 MVP 引入的依赖。
- 保持现有 DDS 对外结构不变。
- 真实车端 Topic 契约确认前，继续把当前 IDL 标注为临时契约。
