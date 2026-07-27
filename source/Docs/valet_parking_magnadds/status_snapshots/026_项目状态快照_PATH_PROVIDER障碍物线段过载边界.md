# 项目状态快照 026：PATH_PROVIDER障碍物线段过载边界

- 快照编号：026
- 日期：2026-07-27
- 当前阶段：Fast-2 - `.so` + typed DDS + PATH_PROVIDER obstacle segment overload smoke
- 阶段状态：PASS_PATH_PROVIDER_OBSTACLE_SEGMENT_OVERLOAD_X86_M57_STATIC_BLOCKED_BOARD
- 创建原因：NEXT-025 已验证 SelectedSlot 角点退化保护，本阶段继续硬化 PATH_PROVIDER 前的局部输入边界，增加可控障碍物线段规模负向样本。

---

## 1. 本次完成事项

- [x] `aux_input_mock_publisher` 新增 `--mode=many-obstacles`：
  - 继续发布合法 `LocalizationEstimate` 和 `ChassisState`。
  - 发布 128 个合法静态 `Obstacle`，不放宽现有外部障碍物数量上限。
  - 每个障碍物是一个箱体，进入 precheck 后贡献 4 条线段。
- [x] `ValetParkingStageParkingAdapter::RunPathProviderPreCheck` 收紧障碍物线段上限：
  - 当前可进入 PATH_PROVIDER 的线段上限为 500。
  - 128 个合法障碍物生成 512 条线段时，输出 `PATH_PROVIDER_PRECHECK failed: too_many_obstacle_segments=512`。
  - 拒绝发生在 PATH_PROVIDER 前，避免把过载输入交给路径搜索。
- [x] `smoke_valet_parking_x86.sh` 新增 `many-obstacles` 断言：
  - runner 必须显示 `aux obstacles ... (count=128)`。
  - runner 必须显示 `PATH_PROVIDER_PRECHECK failed: too_many_obstacle_segments=512`。
  - runner 必须显示 `estop=true`。
  - subscriber 必须显示 `is_estop=true`。

---

## 2. 重要边界

本阶段没有搬完整 `OpenSpacePathProvider` 大类，也没有接入完整 `Frame/DependencyInjector`、线程管理、NLP smoother 或真实车端 Topic 协议。

本阶段只是增加轻量 PATH_PROVIDER_PRECHECK 的过载保护：障碍物本身合法，但线段规模超过当前 MVP 可接受范围时，adapter 会在 PATH_PROVIDER 前明确 estop。

m57 仍只完成交叉编译、ELF 和依赖检查，尚未板端运行验证，不能写成板端通过。

---

## 3. 本次验证命令

x86 构建：
```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform x86 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_obstacle_precheck_026
```

x86 障碍物线段过载负向 smoke：
```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_obstacle_precheck_026/valet_parking_mvp/x86 \
  --domain-id 199 \
  --with-aux-inputs \
  --aux-mode many-obstacles
```

x86 回归 smoke：
```bash
# default valid SelectedSlot
--domain-id 200

# all-valid aux input regression
--domain-id 201 --with-aux-inputs --aux-mode all-valid
```

m57 交叉编译：
```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform m57 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_obstacle_precheck_026
```

---

## 4. 验收结果

| 验收项 | 结果 | 证据 |
|---|---|---|
| x86 构建 | PASS | `out/valet_parking_obstacle_precheck_026/valet_parking_mvp/x86/lib/libvalet_parking.so` |
| x86 `many-obstacles` smoke | PASS | `domain_199_20260727_204042_1013` |
| 障碍物数量实收 | PASS | runner 显示 `aux obstacles #1 (count=128)` |
| 线段过载拒绝原因 | PASS | runner 显示 `PATH_PROVIDER_PRECHECK failed: too_many_obstacle_segments=512` |
| 线段过载 estop | PASS | runner 显示 `estop=true`，subscriber 显示 `is_estop=true` |
| x86 默认 valid 回归 | PASS | `domain_200_20260727_204112_723` |
| x86 all-valid 辅助输入回归 | PASS | `domain_201_20260727_204112_724` |
| m57 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_obstacle_precheck_026/valet_parking_mvp/m57/lib/libvalet_parking.so` |
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
- `applications/source/Docs/valet_parking_magnadds/status_snapshots/026_项目状态快照_PATH_PROVIDER障碍物线段过载边界.md`

---

## 6. 下一步

进入 NEXT-027：继续沿用简化 DDS Topic，增加 PATH_PROVIDER_PRECHECK 对外部障碍物局部几何位置的保护，例如障碍物线段明显落在 ROI local `xy_bounds` 外时输出可诊断 estop。

仍不引入完整 `Frame/DependencyInjector`、线程管理、NLP smoother 或真实车端 Topic 协议。
