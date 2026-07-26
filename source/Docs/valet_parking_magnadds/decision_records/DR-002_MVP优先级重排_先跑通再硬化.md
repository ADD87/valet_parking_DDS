# DR-002：MVP优先级重排（先跑通 `.so` 与通信，再硬化）

- 决策编号：DR-002
- 日期：2026-07-26
- 状态：ACTIVE
- 关联阶段：Fast-0 ~ Fast-4

---

## 1. 背景

用户指出当前计划“量产级流程前置过多”，与领导“优先出 `.so`、先跑通通信”的目标顺序不一致。

---

## 2. 决策

执行口径调整为 **MVP 快跑优先**：

1. 先完成 `libvalet_parking.so`、输入 Topic 打通、输出 Topic 打通、m57 构建打包。
2. 文档只保留最小集：`STATUS.yaml`、状态快照、必要决策记录。
3. 量产级治理文档和后续迁移细化延后，不阻塞 MVP。

---

## 3. 影响

- 阶段表从 Phase 0~8 调整为 Fast-0~Fast-4（MVP门禁）。
- 保留原详版内容作为“后续硬化计划”，不作为当前阻塞。
- Proto 仍要求语义可追溯，但只做最小闭环（两 Topic + 必需字段）。

---

## 4. 验收变化

MVP 仅保留 5 个硬验收：

1. `libvalet_parking.so` 产物存在且可 m57 构建。
2. `/selected_slot` 真订阅。
3. `/planning/trajectory` 真发布。
4. `PROTO_GAP_LIST.md` + `valet_parking_topics.idl` 可追溯。
5. `STATUS.yaml` + 最新快照可交接。

---

## 5. 结论

本决策立即生效。后续若新增范围（如完整 Stage 状态机或全量算法迁移），需再开新 DR，不得隐式扩展。