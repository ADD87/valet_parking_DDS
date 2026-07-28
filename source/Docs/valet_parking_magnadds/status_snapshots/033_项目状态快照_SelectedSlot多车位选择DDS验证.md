# 项目状态快照 033：SelectedSlot多车位选择DDS验证

- 快照编号：033
- 日期：2026-07-28
- 当前阶段：Fast-2 - `.so` + typed DDS + SelectedSlot multi-lot select smoke
- 阶段状态：PASS_SELECTED_SLOT_MULTI_LOT_SELECT_X86_M57_STATIC_BLOCKED_BOARD
- 创建原因：用户要求先评估两版已修 PathProvider 代码节省的工作量，再继续执行当前 MagnaDDS Adapter + DDS 输入输出验证。评估结论已写入 DOC-006；本阶段代码验证选择不改 IDL ABI 的多车位 `opt_parking_seq` 切片。

---

## 1. 本次完成事项

- [x] 新增 `06_两版PathProvider复用价值评估与接入切口.md`：
  - 明确 `parking_algorithm_standalone` 和 `Repair_ValetParkingStageParking_260430-main` 主要节省算法剥离、类型替换、独立编译工作。
  - 明确 DDS IDL、Component/Adapter、RuntimeContext、mock 输入输出和 smoke 验证仍必须在当前工程完成。
  - 建议继续按可验证切片推进，而不是整目录复制或直接接完整 `OpenSpacePathProvider` 大类。
- [x] `selected_slot_mock_publisher` 新增 `--mode=multi-lot-seq-switch`：
  - 每帧发布两个 `ParkingLot`。
  - 前 3 组样本 `opt_parking_seq=1`，选择 `parking_seq=1`。
  - 第 4 组开始 `opt_parking_seq=2`，选择 `parking_seq=2`。
  - publisher 日志输出 `count=2`、`lots=2`、`parking_seq=1|2`、`target=slot1|slot2`。
- [x] `ValetParkingStageParkingAdapter` 的 PATH_PROVIDER 状态日志新增 `parking_seq=<id>`：
  - smoke 可以看到 runner 实际传给 PATH_PROVIDER 的 selected lot id。
  - 避免只验证 publisher 发了什么，而没有验证 Adapter 选了什么。
- [x] `smoke_valet_parking_x86.sh` 新增 `multi-lot-seq-switch` 断言：
  - 自动把 SelectedSlot 发布数量提高到至少 6。
  - 校验前 3 组选择 `parking_seq=1`。
  - 校验后 3 组选择 `parking_seq=2`。
  - 校验 runner 显示 `parking_seq=2` 后 `history=generated, replan=TARGET_UPDATE, reason=target_update`。
  - 校验 `parking_seq=2` 稳定后 `history=reused, replan=NONE, generated_count=2`。
  - 校验 subscriber 收到非 estop `PlanningTrajectory`。

---

## 2. 本次代码边界

本阶段没有修改 IDL，没有新增 Topic，也没有改变现有 `/selected_slot` 或 `/planning/trajectory` 字段语义。

本阶段验证的是当前 Adapter 已存在的选择逻辑：

```text
SelectedSlot.parking_lots
+ SelectedSlot.opt_parking_seq
-> SelectParkingLot()
-> ConvertParkingLot()
-> ROI_DECIDER
-> PATH_PROVIDER
-> PATH_PARTITION
-> SPEED_OPTIMIZER
-> PlanningTrajectory
```

这个切片补齐了 NEXT-031 没覆盖的部分：

- NEXT-031 验证的是单个车位消息中 `parking_seq/path_id` 变化。
- NEXT-033 验证的是同一条消息中有多个 `ParkingLot`，Adapter 会按 `opt_parking_seq` 选择对应车位。

---

## 3. 本次验证命令

x86 构建：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform x86 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_multi_lot_select_033
```

x86 多车位选择 smoke：

```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_multi_lot_select_033/valet_parking_mvp/x86 \
  --domain-id 225 \
  --slot-mode multi-lot-seq-switch \
  --interval-ms 650
```

x86 回归 smoke：

```bash
# default valid SelectedSlot
--domain-id 226

# all-valid aux input regression
--domain-id 227 --with-aux-inputs --aux-mode all-valid
```

m57 交叉编译：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform m57 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_multi_lot_select_033
```

---

## 4. 验收结果

| 验收项 | 结果 | 证据 |
|---|---|---|
| DOC-006 复用价值评估 | PASS | `source/Docs/valet_parking_magnadds/06_两版PathProvider复用价值评估与接入切口.md` |
| x86 构建 | PASS | `out/valet_parking_multi_lot_select_033/valet_parking_mvp/x86/lib/libvalet_parking.so` |
| x86 多车位选择 smoke | PASS | `domain_225_20260728_183228_920` |
| publisher 多车位输入 | PASS | 每帧显示 `count=2 lots=2` |
| 初始选择 lot 1 | PASS | publisher sample 1-3 显示 `parking_seq=1 target=slot1`，runner 显示 `PATH_PROVIDER ok ... parking_seq=1 ... history=generated` |
| 切换选择 lot 2 | PASS | publisher sample 4-6 显示 `parking_seq=2 target=slot2`，runner 显示 `PATH_PROVIDER ok ... parking_seq=2 ... history=generated, replan=TARGET_UPDATE, reason=target_update` |
| lot 2 稳定后复用 | PASS | runner 显示 `PATH_PROVIDER ok ... parking_seq=2 ... history=reused, replan=NONE, generated_count=2` |
| DDS 输出 | PASS | subscriber 显示 `received sample points=179`、`is_estop=false`、`validation: required trajectory fields valid` |
| x86 默认 valid 回归 | PASS | `domain_226_20260728_183254_1150` |
| x86 all-valid 辅助输入回归 | PASS | `domain_227_20260728_183254_1155` |
| m57 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_multi_lot_select_033/valet_parking_mvp/m57/lib/libvalet_parking.so` |
| m57 ELF 架构 | PASS | `ELF 64-bit LSB shared object, ARM aarch64` |
| m57 MagnaDDS 依赖 | PASS | `libmagna-dds-core.so.1`、`libmagna-dds-impl.so` |
| m57 板端运行 | NOT_RUN | 当前无 m57 板端，不能标记为板端通过 |

---

## 5. 本阶段修改文件

- `applications/source/Docs/valet_parking_magnadds/06_两版PathProvider复用价值评估与接入切口.md`
- `applications/source/Docs/valet_parking_magnadds/README.md`
- `applications/source/Docs/valet_parking_magnadds/STATUS.yaml`
- `applications/source/Docs/valet_parking_magnadds/status_snapshots/033_项目状态快照_SelectedSlot多车位选择DDS验证.md`
- `applications/source/valet_parking/README.md`
- `applications/source/valet_parking/src/valet_parking_stage_parking_adapter.cpp`
- `applications/source/valet_parking_tools/selected_slot_mock_publisher/main.cpp`
- `applications/source/valet_parking_tools/smoke_valet_parking_x86.sh`

---

## 6. 下一步

进入 NEXT-034：轻量 Stage 控制输入设计。

目标是先设计而不是立刻大改 IDL：定义一个不搬完整 `FunctionManager` 的轻量 `ParkingCommand` 契约草案，用于表达原始流程里的 `PARKING_IN`、`PARKING_OUT`、`DIRECT_FORWARD`、`DIRECT_BACKWARD`、`PAUSE`、`BRAKE`、`FINISH` 等可观察语义。

完成设计后再进入 NEXT-035，优先接入 `DIRECT_FORWARD/DIRECT_BACKWARD` 直行分支。仍不直接迁移完整 `OpenSpacePathProvider` 大类、`OpenSpaceThreadManager`、NLP smoother 或完整 `Frame/DependencyInjector`。
