# DR-003：IDL线协议与生成代码管理策略

- 决策编号：DR-003
- 日期：2026-07-26
- 状态：ACTIVE
- 关联阶段：Fast-2（`.so` 与真实 DDS 最小闭环）

---

## 1. 背景

当前 `valet_parking` 已经用 MagnaDDS raw payload 跑通最小闭环：

- `/selected_slot`：key-value 字符串输入。
- `/planning/trajectory`：JSON 字符串输出。

该方式适合快速验证 Reader/Writer 真收发，但不是稳定的 DDS 线协议。后续接真实泊车算法时，需要明确字段、枚举、数组、嵌套结构和序列化规则，避免各端对字符串字段理解不一致。

---

## 2. 原计划

原计划要求最终以 `idl/valet_parking_topics.idl` 作为 MagnaDDS Topic 类型来源，但 Fast-2 前半段为抢先跑通通信，临时采用 raw payload。

---

## 3. 新决定

本项目正式采用以下三层边界：

1. **语义来源**：原始 `TempAPA_Code/proto/**`，用于确认字段含义、枚举顺序和单位。
2. **DDS线协议来源**：`applications/source/valet_parking/idl/valet_parking_topics.idl`。
3. **工程编译来源**：SDK `idlparser` 生成的 `applications/source/valet_parking/generated/*`。

当前生成代码纳入 `applications` 本地 Git，作为可追溯产物。后续如果修改 IDL，必须重新运行 `idlparser`，并同时提交 IDL 与生成代码。

---

## 4. 决策原因

| 原因 | 说明 |
|---|---|
| 稳定线协议 | IDL 明确字段类型、数组、枚举和嵌套结构，避免 raw 字符串隐式协议漂移 |
| 对齐 MagnaDDS 工具链 | MagnaDDS SDK 通过 `idlparser` 生成 `TopicDataType`，C++ Reader/Writer 需要这些类型接入 |
| 降低后续算法迁移风险 | 真实算法接入时可以面向结构体字段适配，不需要解析 JSON/key-value |
| 便于验收 | 可以单独验证 IDL 生成、生成代码语法、DDS 类型收发、x86 闭环和 m57 静态构建 |

---

## 5. 被拒绝的替代方案

| 方案 | 拒绝原因 |
|---|---|
| 继续长期使用 raw payload | 字段无强约束，后续接真实算法容易出现字段缺失、枚举错位和解析错误 |
| 直接使用 Protobuf 作为线上协议 | 当前中间件目标是 MagnaDDS，且本工程 Thirdparty protobuf 版本存在兼容风险 |
| 只提交 IDL、不提交生成代码 | 后续换机或 SDK 版本变化时难以复现当前生成物，影响交接和构建可追溯 |
| 手写 TopicDataType | 容易和 SDK 序列化规则不一致，维护成本高 |

---

## 6. 影响范围

| 影响对象 | 影响 |
|---|---|
| `valet_parking` | 后续 Reader/Writer 将从 `TopicDataType_raw` 切换为 `SelectedSlotTopicDataType` 和 `PlanningTrajectoryTopicDataType` |
| mock publisher/subscriber | 后续从字符串构造/校验切换为 generated 类型字段构造/校验 |
| CMake | 后续需要把 `generated/*.cpp` 纳入 `valet_parking` 与工具目标编译 |
| 文档状态 | `STATUS.yaml` 与快照必须区分“IDL生成通过”和“DDS类型通信通过” |
| m57 验收 | IDL 化不解决 m57 工具链和 m57 MagnaDDS 实库缺失，仍需单独补齐 |

---

## 7. 风险与缓解

| 风险 | 影响 | 缓解 |
|---|---|---|
| IDL 字段与原 proto 语义偏离 | 后续真实算法适配返工 | 以 `PROTO_GAP_LIST.md` 和原始 proto 为字段语义基准 |
| SDK `idlparser` 生成风格限制 | C++ 接入方式需要调整 | 先做生成探针和 `g++ -fsyntax-only`，再接入业务代码 |
| 生成代码过大 | 代码审查噪声增加 | 生成代码与 IDL 同步提交，人工重点审 IDL 和接线代码 |
| m57 依赖仍缺失 | 无法完成 m57 静态构建 | 保持 B-002/B-003 打开，不把 x86/IDL 结果写成 m57 通过 |

---

## 8. 回滚或重新评估条件

- MagnaDDS SDK 要求使用不同 IDL 语法或生成工具版本。
- 真实上下游 Topic 契约与当前 MVP 字段范围不一致。
- 后续项目明确要求切换到非 IDL 的线协议。
- m57 平台生成代码编译出现 SDK 或 ABI 兼容问题。

---

## 9. 结论

本决策生效：`valet_parking_topics.idl` 是当前 MagnaDDS 线上协议源，`generated/*` 是当前工程编译输入。下一步只能在用户确认后，把 generated 类型接入 CMake 和 C++ Reader/Writer，替换 raw payload 的 x86 通信路径。

