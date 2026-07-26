# PROTO_GAP_LIST（MVP最小闭环）

- 日期：2026-07-26
- 范围：仅覆盖 `/selected_slot` 与 `/planning/trajectory`
- 目标：先支撑 MagnaDDS 骨架跑通，不做全量 protobuf 迁移

---

## 1. 权威来源

- 输入：`TempAPA_Code/proto/perception/perception_parking_lot.proto`
- 轨迹点：`TempAPA_Code/proto/common/pnc_point.proto`
- 输出：`TempAPA_Code/proto/planning/planning.proto`
- 档位枚举：`TempAPA_Code/proto/soc/chassis.proto`

`parking_algorithm_standalone/proto_convert/*` 作为内部模型参考，不是线协议权威。

---

## 2. 输入 Topic（`/selected_slot`）差异清单

### 2.1 可直接沿用（MVP）

- `ParkingLotOutArray.count/locSeq/opt_parking_seq/parking_lots/is_valid/path_point_size/traced_path/hpp_cruising_to_parking/enable_trigger`
- `ParkingLotOut` 基本字段与枚举数值
- `PSPoint` 与 `Point3D` 基本结构

### 2.2 必须修订（MVP前）

1. **Header 语义对齐**
   - 现状：`parking_lot_convert.h` 内部 `Header` 是简化版。
   - 动作：在 IDL 中保留 MVP 必需 header 字段（`seq/frame_id/publish_stamp/data_stamp`），并在文档标注其余字段 deferred。

2. **ParkStatus 命名兼容**
   - 原始 proto：`UNKOWN=2`（拼写即如此）。
   - 动作：内部实现允许 `UNKNOWN` 别名，但线协议枚举值保持 `2` 不变。

3. **一致性校验**
   - 动作：发布前校验 `count == parking_lots.size()`、`path_point_size == traced_path.size()`（若不一致，以向量实际长度为准并记录告警）。

---

## 3. 输出 Topic（`/planning/trajectory`）差异清单

### 3.1 首版字段范围（allowlist）

- `header`
- `total_path_length`
- `total_path_time`
- `trajectory_point`
- `is_replan`
- `replan_type`
- `replan_reason`
- `longitudinal_diff`
- `lateral_diff`
- `gear`
- `estop`
- `trajectory_type`

### 3.2 嵌套结构必须修订项（MVP前）

1. **PathPoint 缺字段**
   - 现状：`pnc_point_convert.h::PathPoint` 缺 `lane_id`。
   - 动作：IDL/内部模型补 `lane_id`（MVP 可默认空字符串）。

2. **GaussianInfo 字段不完整**
   - 现状：缺 `area_probability`，且名称用 `ellipse_theta`。
   - 动作：补 `area_probability`；将角度字段对齐为 `theta_a`（可内部兼容别名映射）。

3. **TrajectoryPoint 语义控制**
   - 现状：存在 `steer_rate` 扩展字段（原 proto 无）。
   - 动作：MVP 输出不走 `steer_rate`，避免同名不同义扩展漂移。

4. **EStop 结构对齐**
   - 动作：保留 `is_estop + reason`，非法输入必须显式 estop。

### 3.3 枚举/数值对齐（必须固定）

- `gear` 按 `TL.soc.Chassis.GearPosition`：
  - `GEAR_NEUTRAL=0`
  - `GEAR_DRIVE=1`
  - `GEAR_REVERSE=2`
  - `GEAR_PARKING=3`
  - `GEAR_LOW=4`
  - `GEAR_INVALID=5`
  - `GEAR_NONE=6`

- `trajectory_type` 按 `planning.proto`：
  - `UNKNOWN=0`
  - `NORMAL=1`
  - `PATH_FALLBACK=2`
  - `SPEED_FALLBACK=3`
  - ...

---

## 4. MVP 立即执行项（最小集合）

1. 产出 `idl/valet_parking_topics.idl`（仅两 Topic + allowlist 字段）。
2. 在内部模型补齐：`lane_id`、`area_probability`、`theta_a` 映射。
3. 固定 `gear` 与 `trajectory_type` 枚举数值。
4. mock 输入只造 1 个车位，但结构不裁剪。
5. fake 输出 21 点轨迹，`relative_time=0.0~2.0 step 0.1`。

---

## 5. Deferred（MVP后处理）

- `ADCTrajectory` 其余外围字段（function_manager/hmi/path/decision/debug 等）
- 完整 Header 全字段语义透传
- 全量 protobuf 兼容层/回放链路

---

## 6. 完成判据

- 可从本文件追溯每个 MVP 字段来源。
- 所有“必须修订项”在 IDL/代码中有落地或有明确 deferred 记录。
- 不直接搬运 standalone `*.pb.*` 进入产品构建链路。