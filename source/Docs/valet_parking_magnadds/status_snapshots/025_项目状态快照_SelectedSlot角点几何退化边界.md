# 项目状态快照 025：SelectedSlot角点几何退化边界

- 快照编号：025
- 日期：2026-07-27
- 当前阶段：Fast-2 - `.so` + typed DDS + SelectedSlot geometry boundary smoke
- 阶段状态：PASS_SELECTED_SLOT_GEOMETRY_BOUNDARY_X86_M57_STATIC_BLOCKED_BOARD
- 创建原因：NEXT-024 已验证 PATH_PROVIDER warm start/trace adjust 负向边界，但主输入 SelectedSlot 仍缺少一个“字段齐全但几何退化”的可控负向样本。

---

## 1. 本次完成事项

- [x] `selected_slot_mock_publisher` 新增 `--mode=degenerate-corners`：
  - `SelectedSlot.is_valid=true`。
  - `count=1`，`parking_lots.size=1`。
  - 四个角点位置标签齐全：TOP_LEFT、TOP_RIGHT、BOTTOM_LEFT、BOTTOM_RIGHT。
  - 四个角点坐标全部重合，用于模拟“字段看起来完整，但车位几何不存在”的坏输入。
- [x] `ValetParkingStageParkingAdapter::ConvertParkingLot` 新增角点几何退化检查：
  - 先保留原有角点数量、标签和 finite 检查。
  - 再检查四个有效角点的 AABB 跨度和排序后的多边形面积。
  - 退化时输出明确原因：`selected parking lot corner geometry is degenerate`。
- [x] `smoke_valet_parking_x86.sh` 新增 `--slot-mode`：
  - 默认仍为 `valid`，兼容已有 smoke 用法。
  - `degenerate-corners` 模式会断言 runner 日志出现退化角点拒绝原因和 `estop=true`。
  - subscriber 侧断言收到 `is_estop=true`。

---

## 2. 重要边界

本阶段没有搬完整 `OpenSpacePathProvider` 大类，也没有改变 PATH_PROVIDER、PATH_PARTITION、SPEED_OPTIMIZER 主算法。

本阶段只是把 SelectedSlot 的几何入口再硬化一层：当 DDS 消息的字段、数量、标签看起来都齐全，但车位四角点退化成零面积时，adapter 会在 ROI_DECIDER 前明确 estop，避免 ROI/PATH_PROVIDER 接收不存在的车位几何。

当前仍使用临时 IDL 契约和简化 Topic，仍未对齐真实车端 SelectedSlot 协议；m57 仍只完成交叉编译和 ELF/依赖检查，尚未板端运行。

---

## 3. 本次验证命令

x86 构建：
```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform x86 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_slot_geometry_025
```

x86 SelectedSlot 几何负向 smoke：
```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_slot_geometry_025/valet_parking_mvp/x86 \
  --domain-id 195 \
  --timeout-ms 30000 \
  --slot-mode degenerate-corners
```

x86 回归 smoke：
```bash
# default valid SelectedSlot
--domain-id 196

# all-valid aux input regression
--domain-id 197 --with-aux-inputs --aux-mode all-valid
```

m57 交叉编译：
```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform m57 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_slot_geometry_025
```

---

## 4. 验收结果

| 验收项 | 结果 | 证据 |
|---|---|---|
| x86 构建 | PASS | `out/valet_parking_slot_geometry_025/valet_parking_mvp/x86/lib/libvalet_parking.so` |
| x86 `degenerate-corners` smoke | PASS | `domain_195_20260727_135243_713` |
| SelectedSlot 退化角点拒绝原因 | PASS | runner 显示 `selected parking lot corner geometry is degenerate` |
| SelectedSlot 退化角点 estop | PASS | runner 显示 `estop=true`，subscriber 显示 `is_estop=true` |
| x86 默认 valid 回归 | PASS | `domain_196_20260727_135300_845` |
| x86 all-valid 辅助输入回归 | PASS | `domain_197_20260727_135337_711` |
| m57 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_slot_geometry_025/valet_parking_mvp/m57/lib/libvalet_parking.so` |
| m57 ELF 架构 | PASS | `ELF 64-bit LSB shared object, ARM aarch64` |
| m57 MagnaDDS 依赖 | PASS | `libmagna-dds-core.so.1`、`libmagna-dds-impl.so` |
| m57 板端运行 | NOT_RUN | 当前无 m57 板端 |

---

## 5. 本阶段修改文件

- `applications/source/valet_parking/src/valet_parking_stage_parking_adapter.cpp`
- `applications/source/valet_parking_tools/selected_slot_mock_publisher/main.cpp`
- `applications/source/valet_parking_tools/smoke_valet_parking_x86.sh`
- `applications/source/valet_parking/README.md`
- `applications/source/Docs/valet_parking_magnadds/STATUS.yaml`
- `applications/source/Docs/valet_parking_magnadds/status_snapshots/025_项目状态快照_SelectedSlot角点几何退化边界.md`

---

## 6. 下一步

进入 NEXT-026：继续沿用简化 DDS Topic，优先增加可控障碍物数量或障碍物线段规模类负向样本，验证 PATH_PROVIDER_PRECHECK 对 `too_many_obstacle_segments` 或局部几何过载的诊断能力。

仍不引入完整 `Frame/DependencyInjector`、线程管理、NLP smoother 或真实车端 Topic 协议。
