# 项目状态快照 003：Fast-2 真实DDS最小闭环与x86运行验证

- 快照编号：003
- 日期：2026-07-26
- 当前阶段：Fast-2 — `.so` 与真实 DDS 最小闭环
- 阶段状态：IN_PROGRESS
- 创建原因：库内 Reader/Writer 已真实接线，需要记录 x86 端到端运行结果与剩余阻塞。

---

## 1. 本次完成事项

- [x] `valet_parking` 组件从占位 worker 升级为真实 MagnaDDS 闭环：
  - 创建 `DomainParticipant / Subscriber / Publisher`
  - 创建 `/selected_slot` 的 `DataReader`
  - 创建 `/planning/trajectory` 的 `DataWriter`
  - 消费输入后生成轨迹并发布输出
- [x] `selected_slot_mock_publisher` 升级为真实 DDS 发布器（支持 `valid|empty|overflow|nan`）。
- [x] `planning_trajectory_mock_subscriber` 升级为真实 DDS 订阅器，并执行输出字段校验。
- [x] 保留并增强异常输入路径：非法输入发布显式 `estop` 输出。

---

## 2. x86 构建与运行验证

### 2.1 构建

- 使用 `applications/config/valet_parking_mvp_bom.yaml` 进行 x86 白名单构建。
- 结果：PASS（`libvalet_parking.so` + 三工具全部成功编译并安装）。

### 2.2 运行闭环（valid 场景）

- 进程组合：`valet_parking_runner` + `selected_slot_mock_publisher(mode=valid)` + `planning_trajectory_mock_subscriber(strict)`。
- 结果：PASS
  - publisher 成功发布
  - runner 日志显示已 bridge 输入到输出
  - subscriber 收到 `/planning/trajectory` 且字段校验通过

### 2.3 运行闭环（nan 场景）

- 输入：`selected_slot_mock_publisher(mode=nan)`。
- 结果：PASS
  - runner 识别非有限值并发布 `estop`
  - subscriber 收到输出并识别 `is_estop=true`
  - 关键 allowlist 字段仍完整

---

## 3. 当前阻塞项

| 编号 | 状态 | 描述 | 负责人 |
|---|---|---|---|
| B-001 | KNOWN_LIMITATION | 当前无 m57 板端，runtime 验证后置 | user/project |
| B-002 | OPEN | m57 交叉编译工具链目录不存在：`/usr/local/ARM-toolchain/gcc-arm-9.2-2019.12-x86_64-aarch64-none-linux-gnu` | env/workstation |

---

## 4. 当前验收状态（MVP门禁）

| 验收项 | 状态 | 说明 |
|---|---|---|
| `libvalet_parking.so` 可编译产出 | PASS(x86) / BLOCKED(m57) | x86 通过；m57 仍受工具链阻塞 |
| `/selected_slot` 真实订阅 | PASS(x86) | 库内 Reader 已消费 mock 发布数据 |
| `/planning/trajectory` 真实发布 | PASS(x86) | 库内 Writer 已发布并被 subscriber 接收 |
| 异常输入显式 `estop` | PASS(x86) | `mode=nan` 场景验证通过 |
| m57 静态验收 | BLOCKED | 待工具链补齐后执行 |

---

## 5. 下一步唯一动作

> 保持当前 x86 闭环稳定，开始将 raw 字符串 payload 升级为 `idl/valet_parking_topics.idl` 生成类型；并在补齐 m57 工具链后重跑 m57 BOM 构建。

---

## 6. 当前禁止动作

- 禁止把 x86 通过写成 m57 通过。
- 禁止在无板端情况下写 runtime 全通过结论。
- 禁止修改 `compile/` 或 `thirdparty/` 绕过工具链问题。
- 禁止擅自扩展到完整 Stage 状态机或完整算法迁移。
