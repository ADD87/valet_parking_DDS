# 项目状态快照 030：PATH_PROVIDER目标变化重规划边界

- 快照编号：030
- 日期：2026-07-27
- 当前阶段：Fast-2 - `.so` + typed DDS + PATH_PROVIDER target update smoke
- 阶段状态：PASS_PATH_PROVIDER_TARGET_UPDATE_X86_M57_STATIC_BLOCKED_BOARD
- 创建原因：NEXT-029 已验证障碍物从有到无时会触发静态障碍物重规划；本阶段继续验证 SelectedSlot 目标位姿变化时，PATH_PROVIDER 不会误复用上一目标的历史路径，而是触发 `TARGET_UPDATE` 并重新生成。

---

## 1. 本次完成事项

- [x] `selected_slot_mock_publisher` 新增 `--mode=target-moves`：
  - 前 3 组样本发布基础车位目标，中心点为 `(8.5, 2.0)`。
  - 第 4 组开始发布移动后的车位目标，中心点为 `(9.3, 2.0)`。
  - `parking_seq` 保持为 `1`，本阶段只验证目标位姿变化，不混入 path_id 变化。
  - 发布日志输出 `target=base|moved`，方便 smoke 脚本验证样本顺序。
- [x] `smoke_valet_parking_x86.sh` 新增 `target-moves` 断言：
  - publish count 自动提高到至少 6，确保能覆盖基础目标、目标变化和新目标稳定三个阶段。
  - publisher 必须显示前 3 组 `target=base`。
  - publisher 必须显示后 3 组 `target=moved`。
  - runner 必须显示 `PATH_PROVIDER ok ... history=generated, replan=TARGET_UPDATE ... reason=target_update`。
  - runner 必须显示新目标稳定后的 `PATH_PROVIDER ok ... history=reused, replan=NONE ... generated_count=2`。
  - subscriber 必须显示 `is_estop=false`。

---

## 2. 重要边界

本阶段没有修改 `ValetParkingStageParkingAdapter` 主业务逻辑。现有 `BuildPathProviderDecision` 已按以下条件触发 `TARGET_UPDATE`：

- `state.path_id != parking_seq`
- 或上一帧 `end_pose` 与当前目标位姿差异超过阈值

本阶段只补齐可重复的 DDS 输入样本和 smoke 断言，专门覆盖“目标位姿变化”这一半。`parking_seq/path_id` 变化将在 NEXT-031 单独验证。

m57 仍只完成交叉编译、ELF 和依赖检查，尚未板端运行验证，不能写成板端通过。

---

## 3. 本次验证命令

x86 构建：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform x86 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_target_update_030
```

x86 SelectedSlot 目标变化 smoke：

```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_target_update_030/valet_parking_mvp/x86 \
  --domain-id 213 \
  --slot-mode target-moves \
  --interval-ms 650
```

x86 回归 smoke：

```bash
# default valid SelectedSlot
--domain-id 214

# all-valid aux input regression
--domain-id 215 --with-aux-inputs --aux-mode all-valid
```

m57 交叉编译：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform m57 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_target_update_030
```

---

## 4. 验收结果

| 验收项 | 结果 | 证据 |
|---|---|---|
| x86 构建 | PASS | `out/valet_parking_target_update_030/valet_parking_mvp/x86/lib/libvalet_parking.so` |
| x86 `target-moves` smoke | PASS | `domain_213_20260727_221535_1538` |
| 基础目标发布 | PASS | publisher 显示 sample 1-3 `target=base` |
| 移动目标发布 | PASS | publisher 显示 sample 4-6 `target=moved` |
| 初始路径生成 | PASS | runner sample #1 显示 `history=generated, replan=NO_VALID_PATH` |
| 基础目标稳定后历史复用 | PASS | runner sample #2 显示 `history=reused, replan=NONE` |
| 目标变化触发重规划 | PASS | runner sample #3 显示 `history=generated, replan=TARGET_UPDATE, reason=target_update, generated_count=2` |
| 新目标稳定后历史复用 | PASS | runner sample #4/#5 显示 `history=reused, replan=NONE, generated_count=2` |
| x86 默认 valid 回归 | PASS | `domain_214_20260727_221559_1757` |
| x86 all-valid 辅助输入回归 | PASS | `domain_215_20260727_221559_1760` |
| m57 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_target_update_030/valet_parking_mvp/m57/lib/libvalet_parking.so` |
| m57 ELF 架构 | PASS | `ELF 64-bit LSB shared object, ARM aarch64` |
| m57 MagnaDDS 依赖 | PASS | `libmagna-dds-core.so.1`、`libmagna-dds-impl.so` |
| m57 板端运行 | NOT_RUN | 当前无 m57 板端 |

---

## 5. 本阶段修改文件

- `applications/source/valet_parking_tools/selected_slot_mock_publisher/main.cpp`
- `applications/source/valet_parking_tools/smoke_valet_parking_x86.sh`
- `applications/source/valet_parking/README.md`
- `applications/source/Docs/valet_parking_magnadds/STATUS.yaml`
- `applications/source/Docs/valet_parking_magnadds/status_snapshots/030_项目状态快照_PATH_PROVIDER目标变化重规划边界.md`

---

## 6. 下一步

进入 NEXT-031：继续沿用简化 DDS Topic，验证 `parking_seq/path_id` 变化对 PATH_PROVIDER 运行态复用和 replan 标记的影响。当目标位姿基本不变但 `parking_seq/path_id` 变化时，应触发 `TARGET_UPDATE`/重新生成路径，而不是误复用上一 `path_id` 的历史路径；当 `path_id` 稳定后，应重新回到 `history=reused`。

仍不引入完整 `Frame/DependencyInjector`、线程管理、NLP smoother 或真实车端 Topic 协议。
