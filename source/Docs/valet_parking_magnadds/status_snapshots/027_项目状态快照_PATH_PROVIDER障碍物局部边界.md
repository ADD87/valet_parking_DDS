# 项目状态快照 027：PATH_PROVIDER障碍物局部边界

- 快照编号：027
- 日期：2026-07-27
- 当前阶段：Fast-2 - `.so` + typed DDS + PATH_PROVIDER obstacle local bounds smoke
- 阶段状态：PASS_PATH_PROVIDER_OBSTACLE_LOCAL_BOUNDS_X86_M57_STATIC_BLOCKED_BOARD
- 创建原因：NEXT-026 已验证障碍物线段规模过载保护，本阶段继续硬化 PATH_PROVIDER 前的局部输入边界，验证合法但明显错坐标的远障碍物不会进入路径搜索。

---

## 1. 本次完成事项

- [x] `aux_input_mock_publisher` 新增 `--mode=far-obstacles`：
  - 继续发布合法 `LocalizationEstimate` 和 `ChassisState`。
  - 发布 1 个合法静态 `Obstacle`。
  - 障碍物中心点固定为 `(1000,1000)`，用于模拟障碍物 Topic 坐标系或 frame 明显错误。
- [x] `ValetParkingStageParkingAdapter::RunPathProviderPreCheck` 新增障碍物线段局部位置检查：
  - 先保留原有线段数量、线段长度、buffer 合法性检查。
  - 再把障碍物线段端点转换到 ROI local 坐标。
  - 当端点明显落在 `xy_bounds` 大 margin 外时，输出 `obstacle_segment_outside_xy_bounds[...]`。
  - 诊断中带 `local_start/local_end`，方便后续定位错 frame 或错坐标问题。
- [x] `smoke_valet_parking_x86.sh` 新增 `far-obstacles` 断言：
  - runner 必须显示 `aux obstacles ... (count=1)`。
  - runner 必须显示 `PATH_PROVIDER_PRECHECK failed: obstacle_segment_outside_xy_bounds[...]`。
  - runner 必须显示 `estop=true`。
  - subscriber 必须显示 `is_estop=true`。

---

## 2. 重要边界

本阶段没有搬完整 `OpenSpacePathProvider` 大类，也没有接入完整 `Frame/DependencyInjector`、线程管理、NLP smoother 或真实车端 Topic 协议。

本阶段只是增加轻量 PATH_PROVIDER_PRECHECK 的位置保护：障碍物字段和尺寸合法，但局部坐标明显远离当前 ROI 时，adapter 会在 PATH_PROVIDER 前明确 estop。

`all-valid` 模式中的普通障碍物仍通过，用于避免把局部 bounds 检查做得过窄。

m57 仍只完成交叉编译、ELF 和依赖检查，尚未板端运行验证，不能写成板端通过。

---

## 3. 本次验证命令

x86 构建：
```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform x86 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_obstacle_local_bounds_027
```

x86 障碍物局部位置负向 smoke：
```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_obstacle_local_bounds_027/valet_parking_mvp/x86 \
  --domain-id 202 \
  --with-aux-inputs \
  --aux-mode far-obstacles
```

x86 回归 smoke：
```bash
# default valid SelectedSlot
--domain-id 203

# all-valid aux input regression
--domain-id 204 --with-aux-inputs --aux-mode all-valid
```

m57 交叉编译：
```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform m57 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_obstacle_local_bounds_027
```

---

## 4. 验收结果

| 验收项 | 结果 | 证据 |
|---|---|---|
| x86 构建 | PASS | `out/valet_parking_obstacle_local_bounds_027/valet_parking_mvp/x86/lib/libvalet_parking.so` |
| x86 `far-obstacles` smoke | PASS | `domain_202_20260727_205509_1555` |
| 远障碍物实收 | PASS | runner 显示 `aux obstacles #1 (count=1)` |
| 局部边界拒绝原因 | PASS | runner 显示 `PATH_PROVIDER_PRECHECK failed: obstacle_segment_outside_xy_bounds[0]` |
| 局部坐标诊断 | PASS | runner 显示 `local_start=(-779.631,1167.6), local_end=(-780.121,1167.7)` |
| 远障碍物 estop | PASS | runner 显示 `estop=true`，subscriber 显示 `is_estop=true` |
| x86 默认 valid 回归 | PASS | `domain_203_20260727_205526_1704` |
| x86 all-valid 辅助输入回归 | PASS | `domain_204_20260727_205526_1712` |
| m57 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_obstacle_local_bounds_027/valet_parking_mvp/m57/lib/libvalet_parking.so` |
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
- `applications/source/Docs/valet_parking_magnadds/status_snapshots/027_项目状态快照_PATH_PROVIDER障碍物局部边界.md`

---

## 6. 下一步

进入 NEXT-028：继续沿用简化 DDS Topic，验证障碍物输入变化对 PATH_PROVIDER 运行态复用和 replan 标记的影响，例如外部障碍物从无到有或签名变化时应触发 `BLOCK_BY_STATIC_OBSTACLE`/重新生成路径，而不是误复用历史路径。

仍不引入完整 `Frame/DependencyInjector`、线程管理、NLP smoother 或真实车端 Topic 协议。
