# 项目状态快照 031：PATH_PROVIDER路径ID变化重规划边界

- 快照编号：031
- 日期：2026-07-27
- 当前阶段：Fast-2 - `.so` + typed DDS + PATH_PROVIDER path_id update smoke
- 阶段状态：PASS_PATH_PROVIDER_PATH_ID_UPDATE_X86_M57_STATIC_BLOCKED_BOARD
- 创建原因：NEXT-030 已验证 SelectedSlot 目标位姿变化会触发 `TARGET_UPDATE`；本阶段继续验证目标几何基本不变但 `parking_seq/path_id` 变化时，PATH_PROVIDER 不会误复用上一 path_id 的历史路径，而是触发 `TARGET_UPDATE` 并重新生成。

---

## 1. 本次完成事项

- [x] `selected_slot_mock_publisher` 新增 `--mode=parking-seq-changes`：
  - 前 3 组样本发布 `parking_seq=1`。
  - 第 4 组开始发布 `parking_seq=2`。
  - 车位几何保持基本相同，专门隔离 `parking_seq/path_id` 变化，不混入目标位姿变化。
  - `SelectedSlot.opt_parking_seq` 与 `ParkingLot.parking_seq` 同步变化，保证 adapter 选中的就是当前发布的 lot。
  - 发布日志输出 `parking_seq=1|2`，方便 smoke 脚本验证样本顺序。
- [x] `smoke_valet_parking_x86.sh` 新增 `parking-seq-changes` 断言：
  - publish count 自动提高到至少 6，确保能覆盖初始 path_id、path_id 变化和新 path_id 稳定三个阶段。
  - publisher 必须显示前 3 组 `parking_seq=1`。
  - publisher 必须显示后 3 组 `parking_seq=2`。
  - runner 必须显示 `PATH_PROVIDER ok ... history=generated, replan=TARGET_UPDATE ... reason=target_update`。
  - runner 必须显示新 path_id 稳定后的 `PATH_PROVIDER ok ... history=reused, replan=NONE ... generated_count=2`。
  - subscriber 必须显示 `is_estop=false`。

---

## 2. 重要边界

本阶段没有修改 `ValetParkingStageParkingAdapter` 主业务逻辑。现有 `BuildPathProviderDecision` 已按以下条件触发 `TARGET_UPDATE`：

- `state.path_id != parking_seq`
- 或上一帧 `end_pose` 与当前目标位姿差异超过阈值

NEXT-030 已覆盖“目标位姿变化”这一半；本阶段覆盖“path_id 变化”这一半。由于本阶段保持车位几何基本不变，如果 runner 显示 `TARGET_UPDATE`，就能说明触发来源是 `parking_seq/path_id` 变化。

m57 仍只完成交叉编译、ELF 和依赖检查，尚未板端运行验证，不能写成板端通过。

---

## 3. 本次验证命令

x86 构建：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform x86 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_path_id_update_031
```

x86 path_id 变化 smoke：

```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_path_id_update_031/valet_parking_mvp/x86 \
  --domain-id 216 \
  --slot-mode parking-seq-changes \
  --interval-ms 650
```

x86 回归 smoke：

```bash
# default valid SelectedSlot
--domain-id 217

# all-valid aux input regression
--domain-id 218 --with-aux-inputs --aux-mode all-valid
```

m57 交叉编译：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform m57 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_path_id_update_031
```

---

## 4. 验收结果

| 验收项 | 结果 | 证据 |
|---|---|---|
| x86 构建 | PASS | `out/valet_parking_path_id_update_031/valet_parking_mvp/x86/lib/libvalet_parking.so` |
| x86 `parking-seq-changes` smoke | PASS | `domain_216_20260727_224642_718` |
| 初始 path_id 发布 | PASS | publisher 显示 sample 1-3 `parking_seq=1` |
| 变化后 path_id 发布 | PASS | publisher 显示 sample 4-6 `parking_seq=2` |
| 初始路径生成 | PASS | runner sample #1 显示 `history=generated, replan=NO_VALID_PATH` |
| 初始 path_id 稳定后历史复用 | PASS | runner sample #2 显示 `history=reused, replan=NONE` |
| path_id 变化触发重规划 | PASS | runner sample #3 显示 `history=generated, replan=TARGET_UPDATE, reason=target_update, generated_count=2` |
| 新 path_id 稳定后历史复用 | PASS | runner sample #4 显示 `history=reused, replan=NONE, generated_count=2` |
| x86 默认 valid 回归 | PASS | `domain_217_20260727_224716_725` |
| x86 all-valid 辅助输入回归 | PASS | `domain_218_20260727_224716_726` |
| m57 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_path_id_update_031/valet_parking_mvp/m57/lib/libvalet_parking.so` |
| m57 ELF 架构 | PASS | `ELF 64-bit LSB shared object, ARM aarch64` |
| m57 MagnaDDS 依赖 | PASS | `libmagna-dds-core.so.1`、`libmagna-dds-impl.so` |
| m57 板端运行 | NOT_RUN | 当前无 m57 板端 |

---

## 5. 本阶段修改文件

- `applications/source/valet_parking_tools/selected_slot_mock_publisher/main.cpp`
- `applications/source/valet_parking_tools/smoke_valet_parking_x86.sh`
- `applications/source/valet_parking/README.md`
- `applications/source/Docs/valet_parking_magnadds/STATUS.yaml`
- `applications/source/Docs/valet_parking_magnadds/status_snapshots/031_项目状态快照_PATH_PROVIDER路径ID变化重规划边界.md`

---

## 6. 下一步

进入 NEXT-032：泊车算法源码本地化。把当前 `valet_parking/CMakeLists.txt` 实际编译使用的 `parking_algorithm_standalone` 最小源码集合复制到 `applications/source/valet_parking/algorithm/parking_algorithm_standalone`，并修改 CMake 改用本地相对路径，解除 `applications` 对 `E:\APA\DDS\parking_algorithm_standalone` 外部绝对路径的构建依赖。

本地化只迁移当前已验证链路所需源码和头文件依赖，不全量复制 standalone，不复制 `proto/**/*.pb.*`、ROS2 节点、demo、out 或未验证第三方依赖。完成后必须保持 x86 构建、核心 smoke 回归和 m57 交叉编译通过。仍不引入完整 `Frame/DependencyInjector`、线程管理、NLP smoother 或真实车端 Topic 协议。
