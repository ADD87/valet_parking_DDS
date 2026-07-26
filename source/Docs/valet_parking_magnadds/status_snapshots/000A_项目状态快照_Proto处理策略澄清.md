# 项目状态快照 000A：Proto 处理策略澄清

- 快照编号：000A
- 日期：2026-07-26
- 当前阶段：Phase 0 — 文档计划与基线冻结
- 阶段状态：WAITING_USER_CONFIRMATION
- 创建原因：用户询问“计划中 Proto 怎么处理”，本次补充了可执行的 Proto 三层策略与 `proto_convert` 复用矩阵。

---

## 1. 本次完成事项

- [x] 核查 `TempAPA_Code` 原始 proto（`perception_parking_lot.proto`、`pnc_point.proto`、`planning.proto`）。
- [x] 核查 `parking_algorithm_standalone/proto_convert/*` 关键文件（`parking_lot_convert.h`、`pnc_point_convert.h`、`planning_internal_convert.h`、`header_convert.h`）。
- [x] 识别 standalone `*.pb.h` 的版本约束（protoc 3.12.x）与当前工程 Thirdparty protobuf 版本风险。
- [x] 在执行计划书新增：
  - Proto 三层处理策略（权威语义层/线协议层/内部模型层）
  - `proto_convert` 复用矩阵
  - `PROTO_GAP_LIST.md` 作为 Phase 2 新交付物
  - Phase 2 新验收项（差异清单闭环）
- [x] 新增决策记录 `DR-001_Proto处理与proto_convert复用策略.md`。
- [x] 更新 `STATUS.yaml` 引用 DR-001。

---

## 2. 关键结论（本次）

1. `proto_convert` **可以复用**，但只能作为内部模型/转换层，不是线协议。
2. MagnaDDS 线上消息必须由 `IDL` 定义并生成类型。
3. 所有字段语义回溯到 `TempAPA_Code/proto/**`，冲突时以原始 proto 为准。
4. standalone `*.pb.*` 不直接搬到当前工程产品链路。

---

## 3. 当前阻塞项

| 编号 | 状态 | 描述 | 负责人 |
|---|---|---|---|
| B-000 | OPEN | 等待用户确认更新后的执行计划书 | user |
| B-001 | KNOWN_LIMITATION | 当前无 m57 板端，runtime 验证后置 | user/project |

---

## 4. 本次新增文件

| 文件 | 类型 | 说明 |
|---|---|---|
| `Doc/valet_parking_magnadds/decision_records/DR-001_Proto处理与proto_convert复用策略.md` | 新增 | 锁定 Proto 与 `proto_convert` 复用决策 |
| `Doc/valet_parking_magnadds/status_snapshots/000A_项目状态快照_Proto处理策略澄清.md` | 新增 | 本次澄清快照 |

---

## 5. 下一步唯一动作

> 用户确认更新后的 `00_执行计划书_ValetParkingStageParking_MagnaDDS.md`；确认后进入 Phase 1（代码库基线与环境审计）。

---

## 6. 当前禁止动作

- 禁止在用户确认前进入源码实现。
- 禁止把 `proto_convert` 直接当线协议。
- 禁止直接复用 standalone `*.pb.*` 进入产品链路。
