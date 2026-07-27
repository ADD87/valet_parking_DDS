# 项目状态快照 029：PATH_PROVIDER障碍物清空重规划边界

- 快照编号：029
- 日期：2026-07-27
- 当前阶段：Fast-2 - `.so` + typed DDS + PATH_PROVIDER obstacle clear replan smoke
- 阶段状态：PASS_PATH_PROVIDER_OBSTACLE_CLEAR_REPLAN_X86_M57_STATIC_BLOCKED_BOARD
- 创建原因：NEXT-028 已验证障碍物从无到有时会触发静态障碍物重规划；本阶段继续验证障碍物从有到无、或收到合法空障碍物数组时，不会残留旧障碍物状态，也不会误复用旧路径。

---

## 1. 本次完成事项

- [x] `aux_input_mock_publisher` 新增 `--mode=obstacle-disappears`：
  - 前 3 组样本发布合法 `LocalizationEstimate`、`ChassisState` 和 1 个合法静态 `Obstacle`。
  - 第 4 组开始发布合法空 `ObstacleArray`。
  - 该障碍物 ID 固定为 `400000`，用于避免前半段稳定障碍物自己触发签名变化。
- [x] `smoke_valet_parking_x86.sh` 新增 `obstacle-disappears` 断言：
  - aux publisher 必须显示前 3 组 `obstacles=present-valid obstacle_count=1`。
  - aux publisher 必须显示后续 `obstacles=cleared-valid obstacle_count=0`。
  - runner 必须先收到 `aux obstacles ... (count=1)`，之后收到 `aux obstacles ... (count=0)`。
  - runner 必须先显示 `external_obstacles=1`。
  - 障碍物清空时，runner 必须显示 `PATH_PROVIDER ok ... history=generated, replan=BLOCK_BY_STATIC_OBSTACLE ... reason=obstacles_changed ... external_obstacles=0`。
  - 障碍物保持为空后，runner 必须显示 `PATH_PROVIDER ok ... history=reused, replan=NONE ... external_obstacles=0`。

---

## 2. 重要边界

本阶段没有修改 `ValetParkingStageParkingAdapter` 主业务逻辑。现有 `BuildPathProviderDecision` 已按障碍物签名比较触发 `BLOCK_BY_STATIC_OBSTACLE`；本阶段只是补齐可重复的 DDS 输入样本和 smoke 断言。

本阶段验证的是合法空 `ObstacleArray` 的语义：

- 空数组且 `is_valid=true` 表示“当前没有外部障碍物”。
- component 会把它转成 `UpdateObstacles(nullptr, 0)`。
- adapter 内部障碍物状态会被更新为空。
- PATH_PROVIDER 下一帧看到障碍物签名变化后必须重新生成路径。

m57 仍只完成交叉编译、ELF 和依赖检查，尚未板端运行验证，不能写成板端通过。

---

## 3. 本次验证命令

x86 构建：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform x86 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_obstacle_clear_029
```

x86 障碍物从有到无 smoke：

```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_obstacle_clear_029/valet_parking_mvp/x86 \
  --domain-id 210 \
  --with-aux-inputs \
  --aux-mode obstacle-disappears \
  --interval-ms 650 \
  --aux-interval-ms 650
```

x86 回归 smoke：

```bash
# default valid SelectedSlot
--domain-id 211

# all-valid aux input regression
--domain-id 212 --with-aux-inputs --aux-mode all-valid
```

m57 交叉编译：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform m57 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_obstacle_clear_029
```

---

## 4. 验收结果

| 验收项 | 结果 | 证据 |
|---|---|---|
| x86 构建 | PASS | `out/valet_parking_obstacle_clear_029/valet_parking_mvp/x86/lib/libvalet_parking.so` |
| x86 `obstacle-disappears` smoke | PASS | `domain_210_20260727_213523_1768` |
| 初始有障碍物状态 | PASS | runner 显示 `aux obstacles #1 (count=1)` 与 `external_obstacles=1` |
| 合法空障碍物数组实收 | PASS | runner 显示 `aux obstacles #3 (count=0)` |
| 障碍物清空触发重规划 | PASS | runner 显示 `history=generated, replan=BLOCK_BY_STATIC_OBSTACLE, reason=obstacles_changed, external_obstacles=0` |
| 障碍物稳定为空后历史复用 | PASS | runner 显示 `history=reused, replan=NONE, external_obstacles=0` |
| x86 默认 valid 回归 | PASS | `domain_211_20260727_213546_1988` |
| x86 all-valid 辅助输入回归 | PASS | `domain_212_20260727_213546_1996` |
| m57 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_obstacle_clear_029/valet_parking_mvp/m57/lib/libvalet_parking.so` |
| m57 ELF 架构 | PASS | `ELF 64-bit LSB shared object, ARM aarch64` |
| m57 MagnaDDS 依赖 | PASS | `libmagna-dds-core.so.1`、`libmagna-dds-impl.so` |
| m57 板端运行 | NOT_RUN | 当前无 m57 板端 |

---

## 5. 本阶段修改文件

- `applications/source/valet_parking_tools/aux_input_mock_publisher/main.cpp`
- `applications/source/valet_parking_tools/smoke_valet_parking_x86.sh`
- `applications/source/valet_parking/README.md`
- `applications/source/Docs/valet_parking_magnadds/STATUS.yaml`
- `applications/source/Docs/valet_parking_magnadds/status_snapshots/029_项目状态快照_PATH_PROVIDER障碍物清空重规划边界.md`

---

## 6. 下一步

进入 NEXT-030：继续沿用简化 DDS Topic，验证 SelectedSlot 目标变化对 PATH_PROVIDER 运行态复用和 replan 标记的影响。当选中车位目标位姿变化时，应触发 `TARGET_UPDATE`/重新生成路径，而不是误复用上一目标的历史路径；当目标位姿稳定后，应重新回到 `history=reused`。

仍不引入完整 `Frame/DependencyInjector`、线程管理、NLP smoother 或真实车端 Topic 协议。
