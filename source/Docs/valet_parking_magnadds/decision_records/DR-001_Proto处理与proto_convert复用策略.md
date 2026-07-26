# DR-001：Proto 处理与 `proto_convert` 复用策略

- 决策编号：DR-001
- 日期：2026-07-26
- 状态：ACTIVE
- 关联阶段：Phase 2（Topic 契约冻结），Phase 3（IDL 生成验证）

---

## 1. 背景

用户已在 `parking_algorithm_standalone/proto_convert` 中剥离了大量 protobuf 依赖，形成可独立编译的结构体转换层。当前任务需要把泊车能力接入 MagnaDDS，并保证后续能逐步迁移真实算法。

冲突点：

1. 当前工程通信协议是 MagnaDDS，不是 Protobuf。
2. standalone 的 `*.pb.*` 生成代码存在 protoc 版本约束（3.12.x），与当前工程 Thirdparty `protobuf-3.5.1` 存在直接兼容风险。
3. `proto_convert` 某些结构与原始 proto 存在字段或命名差异。

---

## 2. 原计划

原计划中已写“可参考 `proto_convert`，但不原样复制”，但缺少明确的三层边界与修订清单，容易在执行时产生歧义。

---

## 3. 新决定

采用“三层分离”并固化到计划：

1. **权威语义层**：`TempAPA_Code/proto/**` 为唯一字段语义来源。
2. **线协议层**：MagnaDDS 仅走 `idl/valet_parking_topics.idl` 生成类型。
3. **内部模型层**：`proto_convert` 允许复用，但必须先做字段 gap 清单并按原始 proto 修订。

同时新增交付物 `PROTO_GAP_LIST.md`，用于记录原始 proto 与 `proto_convert` 的逐字段差异和修订动作。

---

## 4. 决策原因

| 原因 | 说明 |
|---|---|
| 保持中间件一致性 | MagnaDDS 线上协议必须由 IDL 定义，避免混入 protobuf 线协议 |
| 保护后续迁移成本 | 先冻结字段矩阵，后续接真实算法不反复改 Topic |
| 降低版本风险 | 避免直接搬入 standalone `*.pb.*` 导致 protobuf 版本冲突 |
| 提升可追溯性 | 用 `PROTO_GAP_LIST.md` 明确每个差异和处理结论 |

---

## 5. 被拒绝的替代方案

| 方案 | 拒绝原因 |
|---|---|
| 直接使用 standalone `*.pb.*` 作为通信消息 | 与 MagnaDDS 线协议不一致，且 protobuf 版本风险高 |
| `proto_convert` 不做对齐直接复用 | 容易出现字段缺失/命名漂移，后续调试代价大 |
| 先忽略字段差异，等算法迁移时再补 | 会把协议债务后移，导致中后期连锁返工 |

---

## 6. 影响范围

| 影响对象 | 影响 |
|---|---|
| Phase 2 | 新增 `PROTO_GAP_LIST.md` 与逐字段差异审查 |
| Phase 3 | IDL 生成前必须完成字段对齐闭环 |
| 源码实现 | `proto_convert` 只做内部适配，不做线协议定义 |
| 验收 | 新增“差异清单已闭环”的门禁 |

---

## 7. 风险与缓解

| 风险 | 影响 | 缓解 |
|---|---|---|
| 差异清单维护不完整 | 运行时出现隐性字段 bug | 在 Phase 2 验收中强制检查 gap 清单闭环 |
| 开发者误把 IDL 与内部结构混用 | 通信/业务语义错位 | 文档中固定三层边界并加禁止动作 |

---

## 8. 回滚或重新评估条件

- 若项目要求切回“完整 Protobuf 线协议”（非 MagnaDDS IDL）再评估。
- 若 Thirdparty protobuf 升级并明确要求复用原始 pb 代码，再评估。
- 若 MagnaDDS IDL 工具链无法表达必要字段，再评估并出新决策。

---

## 9. 结论

本决策生效：`proto_convert` 可复用，但必须修订；原始 proto 为语义基准；MagnaDDS IDL 为唯一线协议。
