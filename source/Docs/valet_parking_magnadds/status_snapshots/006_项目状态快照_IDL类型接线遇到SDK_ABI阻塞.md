# 项目状态快照 006：IDL类型接线遇到SDK ABI阻塞

- 快照编号：006
- 日期：2026-07-26
- 当前阶段：Fast-2 - `.so` 与真实 DDS 最小闭环
- 阶段状态：BLOCKED
- 创建原因：已把 generated IDL 类型接入 valet_parking 与 mock 工具，但 x86 typed DDS 链接被 MagnaDDS SDK 头文件/实库 ABI namespace 不匹配阻塞；按要求先记录并报告。

---

## 1. 本次完成事项

- [x] `valet_parking` 组件 CMake 已编入 `generated/valet_parking_topics*.cpp`。
- [x] `valet_parking` 内部 Reader/Writer 已从 `TopicDataType_raw` 切换到 `SelectedSlotTopicDataType` 与 `PlanningTrajectoryTopicDataType`。
- [x] `selected_slot_mock_publisher` 已从字符串 raw payload 改为构造并发布 `SelectedSlot`。
- [x] `planning_trajectory_mock_subscriber` 已从 JSON key 检查改为订阅并结构化校验 `PlanningTrajectory`。
- [x] valid/empty/overflow/nan 的测试语义已迁移到 typed 样本构造与组件输入校验。
- [x] x86 构建已推进到链接阶段，证明新增源码语法编译通过。

---

## 2. 本次未完成事项

- [ ] x86 链接未通过，未产出可运行 typed DDS 二进制。
- [ ] valid/nan/estop 端到端运行验证未执行。
- [ ] m57 构建与板端 runtime 验证未执行，仍受既有 m57 环境 blocker 限制。

---

## 3. 当前交付物

| 文件/产物 | 状态 | 说明 |
|---|---|---|
| `applications/source/valet_parking/CMakeLists.txt` | 已修改 | 编入 generated IDL 代码并补充 MagnaDDS impl 链接依赖 |
| `applications/source/valet_parking/src/valet_parking_component.h` | 已修改 | 私有成员从 raw TopicDataType 切换为 typed TopicDataType |
| `applications/source/valet_parking/src/valet_parking_component.cpp` | 已修改 | typed register/take/write 与 `SelectedSlot` 到 `PlanningTrajectory` MVP转换 |
| `applications/source/valet_parking_tools/selected_slot_mock_publisher/main.cpp` | 已修改 | 发布 typed `SelectedSlot` 样本 |
| `applications/source/valet_parking_tools/planning_trajectory_mock_subscriber/main.cpp` | 已修改 | 订阅 typed `PlanningTrajectory` 并做结构化校验 |
| `out/valet_parking_mvp_next006/valet_parking_mvp/x86/build/build.log` | 已生成 | x86 构建日志，链接失败证据 |

---

## 4. 最近执行动作或命令

| 时间 | 动作/命令 | 结果 | 日志/证据 |
|---|---|---|---|
| 2026-07-26 | `bash ./build_app.sh --build=x86 --product-bom-file=.../valet_parking_mvp_bom.yaml --mode=release --j=8` | BLOCKED | 旧 `out/valet_parking_mvp/x86/build` 中 CMakeCache 仍指向上一台电脑 `/mnt/c/Users/...` |
| 2026-07-26 | 同上，改用 `--out-dir=/mnt/e/.../out/valet_parking_mvp_next006` | FAIL | 编译进入链接阶段后失败 |
| 2026-07-26 | `nm -D .../libmagna-dds-impl.so \| c++filt` | OBSERVED | 实库导出 `dds::domain::DomainParticipantFactory::get_instance()` 与 `gstone::rtps::DdsCdr::serialize(double)` |
| 2026-07-26 | 构建 SDK 自带 `demo/hello_world` typed demo | FAIL | 同样 undefined `magna::dds::DomainParticipantFactory::get_instance()` / `magna::dds::DdsCdr::*` |

---

## 5. 当前阻塞项

| 编号 | 状态 | 描述 | 负责人 | 下一步 |
|---|---|---|---|---|
| B-004 | OPEN | x86 MagnaDDS typed API 头文件声明 `magna::dds::*`，但当前实库导出旧 ABI：`dds::domain::*`、`dds::pub::*`、`dds::sub::*`、`dds::topic::*`、`gstone::rtps::DdsCdr::*`。SDK typed demo 同样无法链接。 | env/sdk | 优先获取匹配 `magna::dds::*` 导出的 SDK/thirdparty 库；若拿不到，再由项目确认是否允许维护受控 shim。 |
| B-001 | OPEN | 当前无 m57 板端，运行态通信验收暂无法闭环。 | user/project | 板端可用后执行 runtime 验证。 |
| B-002 | OPEN | WSL 中标准 m57 交叉工具链目录不存在，未找到 `aarch64-none-linux-gnu-gcc`。 | env/workstation | 恢复工具链。 |
| B-003 | OPEN | `thirdparty/m57/magnadds/lib` 疑似缺少真实 m57 MagnaDDS 实库。 | env/workstation | 补齐 m57 MagnaDDS 实库。 |

---

## 6. 阶段验收清单

| 验收项 | 结果 | 证据/备注 |
|---|---|---|
| generated 类型接入 CMake | PASS | 三个目标均已编入 generated `.cpp` |
| `valet_parking` typed Reader/Writer 接线 | PASS_CODE | 源码已切换到 `take_next_sample(&SelectedSlot, ...)` 与 `write(&PlanningTrajectory, HANDLE_NIL)` |
| mock publisher typed 发布 | PASS_CODE | `BuildSample()` 构造 `SelectedSlot` |
| mock subscriber typed 订阅校验 | PASS_CODE | `ValidateTrajectorySample()` 结构化检查输出 |
| x86 编译 | PASS_TO_LINK | 对象文件已生成，失败点在链接 |
| x86 链接 | BLOCKED | MagnaDDS SDK ABI namespace mismatch |
| x86 runtime valid/nan/estop | NOT_RUN | 链接失败，无可运行二进制 |
| m57 静态构建 | BLOCKED | 既有工具链/库 blocker |

---

## 7. 变更文件列表

| 文件 | 类型 | 说明 |
|---|---|---|
| `applications/source/valet_parking/CMakeLists.txt` | 修改 | 编入 generated IDL 源并补充 impl 库 |
| `applications/source/valet_parking/src/valet_parking_component.h` | 修改 | typed TopicDataType 成员与 typed 构建接口 |
| `applications/source/valet_parking/src/valet_parking_component.cpp` | 修改 | typed DDS register/take/write 与轨迹构造 |
| `applications/source/valet_parking_tools/selected_slot_mock_publisher/CMakeLists.txt` | 修改 | 编入 generated IDL 源并补充 impl 库 |
| `applications/source/valet_parking_tools/selected_slot_mock_publisher/main.cpp` | 修改 | typed `SelectedSlot` publisher |
| `applications/source/valet_parking_tools/planning_trajectory_mock_subscriber/CMakeLists.txt` | 修改 | 编入 generated IDL 源并补充 impl 库 |
| `applications/source/valet_parking_tools/planning_trajectory_mock_subscriber/main.cpp` | 修改 | typed `PlanningTrajectory` subscriber |
| `applications/source/Docs/valet_parking_magnadds/STATUS.yaml` | 修改 | 更新到 B-004/NEXT-007 |
| `applications/source/Docs/valet_parking_magnadds/status_snapshots/006_项目状态快照_IDL类型接线遇到SDK_ABI阻塞.md` | 新增 | 本快照 |

---

## 8. 决策记录

| 决策编号 | 文件 | 状态 |
|---|---|---|
| DR-003 | `applications/source/Docs/valet_parking_magnadds/decision_records/DR-003_IDL线协议与生成代码管理策略.md` | ACTIVE |

---

## 9. 下一步唯一动作

> 先解决 MagnaDDS SDK x86 typed API 头文件/实库 ABI namespace 不匹配；优先替换为匹配 `magna::dds::*` 导出的 SDK/thirdparty 库，若无法获取，再确认是否允许在 `applications/source` 内维护最小受控兼容 shim。

---

## 10. 当前禁止动作

- 禁止把本阶段写成 x86 runtime 已通过。
- 禁止把 SDK demo 或历史 raw 产物当作 typed IDL 通信验收。
- 禁止修改 `compile/` 或 `thirdparty/` 绕过问题。
- 禁止继续扩展完整 Stage 状态机或全量算法迁移。

---

## 11. 给下一个 AI 的一句话

NEXT-006 的 typed IDL 接线代码已落地并编译到链接阶段，但 x86 与 SDK 自带 typed demo 都被 MagnaDDS `magna::dds` 头文件和旧 ABI 实库 namespace 不匹配阻塞；下一步先解决 SDK/库匹配或确认受控 shim 策略。
