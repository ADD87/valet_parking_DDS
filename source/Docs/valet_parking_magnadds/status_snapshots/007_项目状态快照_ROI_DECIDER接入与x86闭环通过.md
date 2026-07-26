# 项目状态快照 007：ROI_DECIDER接入与x86闭环通过

- 快照编号：007
- 日期：2026-07-26
- 当前阶段：Fast-2 - `.so` + typed DDS + ROI_DECIDER 最小闭环
- 阶段状态：PASS_X86_BLOCKED_M57_TOOLCHAIN
- 创建原因：已按用户确认的 runner/component/adapter 分层接入 ValetParkingStageParking 第一阶段 ROI_DECIDER，并完成 x86 typed DDS 构建与本机运行验证；阶段完成后先报告再继续。

---

## 1. 本次完成事项

- [x] `valet_parking_runner/main.cpp` 保持为进程入口，只负责解析参数、创建 C API handle、启动/停止组件。
- [x] `ValetParkingComponent` 负责 MagnaDDS typed `SelectedSlot` 订阅、`PlanningTrajectory` 发布和 worker loop。
- [x] 新增 `ValetParkingStageParkingAdapter` 作为业务流程入口。
- [x] Adapter 已完成 `SelectedSlot` 到 standalone `TL::perception::ParkingLotOut` 的转换。
- [x] Adapter 已构造 standalone `TL::common::VehicleState`，暂使用配置中的 fake vehicle pose。
- [x] Adapter 已调用 `TL::planning::OpenSpaceRoiDecider::Process()`。
- [x] ROI 成功时发布 21 点 `PlanningTrajectory` seed，状态包含 `ROI_DECIDER ok`、scenario、lot_status 和 target pose。
- [x] 输入非法或 ROI 失败时发布 1 点 estop fallback trajectory，并保留失败原因。
- [x] `valet_parking` CMake 显式接入 standalone ROI 相关源文件与 `thirdparty::eigen`。

---

## 2. 构建与运行验证

| 验收项 | 结果 | 证据 |
|---|---|---|
| x86 CMake 配置 | PASS | `out/valet_parking_roi_stage_008/valet_parking_mvp/x86/build` |
| x86 构建与安装 | PASS | `out/valet_parking_roi_stage_008/valet_parking_mvp/x86/build/build.log` |
| runner 二进制加载 | PASS | `app/valet_parking_runner --help` 正常输出 |
| valid DDS 闭环 | PASS | subscriber 收到 `points=21, is_estop=false` |
| ROI_DECIDER 调用 | PASS | runner 输出 `ROI_DECIDER ok, scenario=1, lot_status=0, target=(7.314,1.760,0.200)` |
| empty fallback | PASS | subscriber 收到 `points=1, is_estop=true`，原因 `selected_slot.is_valid is false` |
| overflow fallback | PASS | subscriber 收到 `points=1, is_estop=true`，原因 `selected_slot count exceeds parking_lots size` |
| nan fallback | PASS | subscriber 收到 `points=1, is_estop=true`，原因 `selected parking lot has insufficient or invalid corner points` |
| m57 配置探针 | BLOCKED | 缺少 `/usr/local/ARM-toolchain/.../aarch64-none-linux-gnu-gcc/g++` |

---

## 3. 本次环境修复

- `thirdparty/x86/magnadds/lib` 中已有真实 `libmagna-dds-core.so.1.0.0` 和 `libmagna-dds-impl.so`。
- 为匹配工程 `thirdparty.cmake` 查找和运行时 ELF `SONAME`，补齐 x86 符号链接链：
  - `libmagna-dds-core.so -> libmagna-dds-core.so.1`
  - `libmagna-dds-core.so.1 -> libmagna-dds-core.so.1.0.0`
- `thirdparty/m57/magnadds/lib` 已确认存在真实 aarch64 `libmagna-dds-core.so.1.0.0` 与 `libmagna-dds-impl.so`，旧的 m57 实库缺失 blocker 可关闭。

---

## 4. 变更文件

| 文件 | 状态 | 说明 |
|---|---|---|
| `applications/source/valet_parking/CMakeLists.txt` | 修改 | 接入 standalone ROI 源文件、compat include、`thirdparty::eigen` |
| `applications/source/valet_parking/src/valet_parking_component.h` | 修改 | 持有 `ValetParkingStageParkingAdapter` |
| `applications/source/valet_parking/src/valet_parking_component.cpp` | 修改 | DDS 输入样本交给 Adapter 处理 |
| `applications/source/valet_parking/src/valet_parking_stage_parking_adapter.h` | 新增 | 业务流程入口声明 |
| `applications/source/valet_parking/src/valet_parking_stage_parking_adapter.cpp` | 新增 | SelectedSlot 转换、ROI_DECIDER 调用、trajectory seed/estop 输出 |
| `applications/source/valet_parking/src/compat/**` | 新增 | standalone ROI 依赖的最小兼容头 |
| `applications/source/Docs/valet_parking_magnadds/STATUS.yaml` | 修改 | 更新当前状态、blocker、next_action |
| `applications/source/Docs/valet_parking_magnadds/status_snapshots/007_项目状态快照_ROI_DECIDER接入与x86闭环通过.md` | 新增 | 本快照 |

---

## 5. 当前阻塞项

| 编号 | 状态 | 描述 | 下一步 |
|---|---|---|---|
| B-001 | OPEN | 当前无 m57 板端，不能做板端运行态 DDS 验收 | 板端可用后再跑 runtime |
| B-002 | OPEN | WSL 中缺少 m57 交叉工具链路径 `/usr/local/ARM-toolchain/gcc-arm-9.2-2019.12-x86_64-aarch64-none-linux-gnu` | 补齐工具链或确认新路径后修订 toolchain 配置 |
| B-003 | RESOLVED | m57 MagnaDDS 实库已补齐 | 无 |
| B-004 | RESOLVED | x86 MagnaDDS typed API/实库已可构建运行 | 无 |

---

## 6. 下一步唯一动作

阶段报告后等待用户确认再继续。建议优先级：

1. 若目标是 m57 交付：先恢复 m57 交叉工具链，再执行 m57 静态构建与依赖检查。
2. 若目标是继续算法链路：在当前 Adapter 后接入 PATH_PROVIDER，保持 DDS Topic 契约和 runner/component 分层不变。

