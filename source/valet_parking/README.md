# valet_parking (Fast-2 Real DDS MVP)

本模块是 `ValetParkingStageParking` 适配 MagnaDDS 的首版共享库：

- 目标产物：`libvalet_parking.so`
- 当前能力：生命周期 C API（create/start/stop/destroy）
- 当前状态：已接入真实 MagnaDDS Participant/Subscriber/Publisher/DataReader/DataWriter
- 输入路径：订阅 `/selected_slot`（当前使用 key-value raw payload）
- 输出路径：发布 `/planning/trajectory`（当前输出 JSON 字符串，包含轨迹 allowlist 字段）
- 异常行为：非法输入（如 NaN/empty/overflow）发布显式 `estop` 输出

后续阶段：

1. 接入 `valet_parking_topics.idl` 生成类型，替换 raw payload
2. 将 mock 输入/输出从字符串协议迁移到正式 IDL 类型
3. 补齐 m57 工具链后执行 m57 静态构建验收
