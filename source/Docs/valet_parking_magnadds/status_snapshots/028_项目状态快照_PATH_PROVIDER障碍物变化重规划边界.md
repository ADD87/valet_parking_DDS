# 项目状态快照 028：PATH_PROVIDER障碍物变化重规划边界

- 快照编号：028
- 日期：2026-07-27
- 当前阶段：Fast-2 - `.so` + typed DDS + PATH_PROVIDER obstacle change replan smoke
- 阶段状态：PASS_PATH_PROVIDER_OBSTACLE_CHANGE_REPLAN_X86_M57_STATIC_BLOCKED_BOARD
- 创建原因：NEXT-027 已验证明显错坐标的远障碍物会在 PATH_PROVIDER 前被拒绝；本阶段继续验证障碍物状态从无到有时，PATH_PROVIDER 不应误复用历史路径，而应明确触发静态障碍物重规划。

---

## 1. 本次完成事项

- [x] `aux_input_mock_publisher` 新增 `--mode=obstacle-appears`：
  - 前 3 组样本只发布合法 `LocalizationEstimate` 和 `ChassisState`。
  - 第 4 组开始发布 1 个合法静态 `Obstacle`。
  - 该障碍物 ID 固定为 `300000`，用于避免后续每帧因 ID 变化而持续误触发 `BLOCK_BY_STATIC_OBSTACLE`。
- [x] `smoke_valet_parking_x86.sh` 新增 `obstacle-appears` 断言：
  - aux publisher 必须显示前 3 组 `obstacles=skipped obstacle_count=0`。
  - runner 必须先显示 `external_obstacles=0`。
  - runner 必须收到 `aux obstacles ... (count=1)`。
  - 障碍物出现时，runner 必须显示 `PATH_PROVIDER ok ... history=generated, replan=BLOCK_BY_STATIC_OBSTACLE ... reason=obstacles_changed`。
  - 障碍物签名稳定后，runner 必须显示 `PATH_PROVIDER ok ... history=reused, replan=NONE ... external_obstacles=1`。

---

## 2. 重要边界

本阶段没有搬完整 `OpenSpacePathProvider` 大类，也没有接入完整 `Frame/DependencyInjector`、线程管理、NLP smoother 或真实车端 Topic 协议。

本阶段只验证轻量 `PathProviderRuntimeState` 的障碍物签名变化语义：

- 无障碍物 -> 有障碍物：必须重新生成路径。
- 障碍物签名稳定：允许复用历史路径。
- 不把“障碍物出现”误判为同一环境下的历史路径复用。
- 不把稳定障碍物误判为每帧都需要重规划。

m57 仍只完成交叉编译、ELF 和依赖检查，尚未板端运行验证，不能写成板端通过。

---

## 3. 本次验证命令

x86 构建：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform x86 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_obstacle_replan_028
```

x86 障碍物从无到有 smoke：

```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_obstacle_replan_028/valet_parking_mvp/x86 \
  --domain-id 209 \
  --with-aux-inputs \
  --aux-mode obstacle-appears \
  --interval-ms 650 \
  --aux-interval-ms 650
```

x86 回归 smoke：

```bash
# default valid SelectedSlot
--domain-id 207

# all-valid aux input regression
--domain-id 208 --with-aux-inputs --aux-mode all-valid
```

m57 交叉编译：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform m57 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_obstacle_replan_028
```

---

## 4. 验收结果

| 验收项 | 结果 | 证据 |
|---|---|---|
| x86 构建 | PASS | `out/valet_parking_obstacle_replan_028/valet_parking_mvp/x86/lib/libvalet_parking.so` |
| x86 `obstacle-appears` smoke | PASS | `domain_209_20260727_212329_716` |
| 初始无障碍物状态 | PASS | runner 显示 `external_obstacles=0` |
| 障碍物实收 | PASS | runner 显示 `aux obstacles #1 (count=1)` |
| 障碍物出现触发重规划 | PASS | runner 显示 `history=generated, replan=BLOCK_BY_STATIC_OBSTACLE, reason=obstacles_changed` |
| 障碍物稳定后历史复用 | PASS | runner 显示 `history=reused, replan=NONE, external_obstacles=1` |
| x86 默认 valid 回归 | PASS | `domain_207_20260727_212414_716` |
| x86 all-valid 辅助输入回归 | PASS | `domain_208_20260727_212414_711` |
| m57 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_obstacle_replan_028/valet_parking_mvp/m57/lib/libvalet_parking.so` |
| m57 ELF 架构 | PASS | `ELF 64-bit LSB shared object, ARM aarch64` |
| m57 MagnaDDS 依赖 | PASS | `libmagna-dds-core.so.1`、`libmagna-dds-impl.so` |
| m57 板端运行 | NOT_RUN | 当前无 m57 板端 |

---

## 5. 本阶段修改文件

- `applications/source/valet_parking_tools/aux_input_mock_publisher/main.cpp`
- `applications/source/valet_parking_tools/smoke_valet_parking_x86.sh`
- `applications/source/valet_parking/README.md`
- `applications/source/Docs/valet_parking_magnadds/STATUS.yaml`
- `applications/source/Docs/valet_parking_magnadds/status_snapshots/028_项目状态快照_PATH_PROVIDER障碍物变化重规划边界.md`

---

## 6. 下一步

进入 NEXT-029：继续沿用简化 DDS Topic，验证障碍物清空/消失边界。当外部障碍物状态从有变为无、或障碍物数组被显式清空时，PATH_PROVIDER 应识别障碍物签名变化并重新生成路径，不能残留旧障碍物状态，也不能把旧路径当作同一障碍物环境直接复用。

仍不引入完整 `Frame/DependencyInjector`、线程管理、NLP smoother 或真实车端 Topic 协议。
