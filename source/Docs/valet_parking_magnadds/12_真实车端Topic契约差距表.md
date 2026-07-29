# 真实车端 Topic 契约差距表

日期：2026-07-29

## 目的

本文记录 `NEXT-047` 的结果：当前泊车 MagnaDDS MVP 使用的是临时 IDL 和临时 Topic 名，已经能跑通 x86 DDS smoke 和 m57 交叉编译，但还不能直接等同于真实车端协议。

培训材料给出的判断标准很明确：DDS 通信要同时匹配 `Domain`、`Topic name`、`Type name/IDL` 和 `QoS`。因此，后续接真实车端前，不能只问“Topic 名一样吗”，还要确认两端 IDL 字段顺序、类型名、枚举语义、QoS 和重新生成代码是否一致。

## 当前临时 Topic 总览

| 方向 | 默认 Topic | IDL Type | 当前生产者 | 当前消费者 | 状态 |
|---|---|---|---|---|---|
| 输入 | `/selected_slot` | `SelectedSlot` | `selected_slot_mock_publisher` 或真实上游 | `valet_parking_runner/libvalet_parking.so` | MVP 已验证 |
| 输出 | `/planning/trajectory` | `PlanningTrajectory` | `libvalet_parking.so` | `planning_trajectory_mock_subscriber` 或真实下游 | MVP 已验证 |
| 输入 | `/parking/command` | `ParkingCommand` | `parking_command_mock_publisher` 或真实 HMI/状态机 | `libvalet_parking.so` | MVP 已验证 |
| 输入 | `/localization/estimate` | `LocalizationEstimate` | `aux_input_mock_publisher` 或真实定位 | `libvalet_parking.so` | MVP 已验证 |
| 输入 | `/chassis/state` | `ChassisState` | `aux_input_mock_publisher` 或真实底盘 | `libvalet_parking.so` | MVP 已验证 |
| 输入 | `/perception/obstacles` | `ObstacleArray` | `aux_input_mock_publisher` 或真实感知 | `libvalet_parking.so` | MVP 已验证 |

当前这些 Topic 都支持 runner 参数覆盖，例如 `--in-topic`、`--out-topic`、`--command-topic`、`--localization-topic`、`--chassis-topic`、`--obstacle-topic`。

## 差距表

| Topic | 当前临时字段 | 真实车端必须确认 | 当前风险 |
|---|---|---|---|
| `/selected_slot` | `count`、`opt_parking_seq`、`ParkingLot`、`pts_vrf`、`pts_enu`、`traced_path`、`enable_trigger` | 真实车位来源、Topic 名、坐标系、角点顺序、`parking_seq/path_id` 语义、泊车类型枚举、HMI 字段来源 | 当前只覆盖泊车入位目标车位，不等于完整 APA/AVP 车位协议 |
| `/planning/trajectory` | `trajectory_point`、`gear`、`estop`、`trajectory_type`、`replan_reason` | 真实下游是否使用 ADCTrajectory 等已有协议、轨迹点字段单位、gear 枚举、estop 语义、replan 类型 | `parking_status` 目前只是 reason 文本，不是正式 DDS 字段 |
| `/parking/command` | `mode`、`parking_seq`、`direct_distance_m`、`direct_speed_mps`、`reset_history`、`reason` | FunctionManager/HMI 的真实命令 Topic、命令生命周期、暂停/刹停/完成/出库枚举、重复命令去抖 | 当前出库命令只做 unsupported stop，不是出库算法 |
| `/localization/estimate` | `x/y/z/heading/is_valid` | 真实定位 Topic、坐标系、时间戳、速度/加速度是否另取、协方差/质量状态 | 当前字段很轻，只够 PATH_PROVIDER 局部转换和边界 smoke |
| `/chassis/state` | `speed_mps`、`acceleration_mps2`、`gear` | 真实底盘 Topic、档位枚举映射、速度正负号、车辆状态有效位 | direct 方向冲突已硬化，但真实车端枚举未确认 |
| `/perception/obstacles` | 障碍物中心、长宽、朝向、速度、类型、动态标志 | 真实感知 Topic、障碍物多边形/轮廓、预测轨迹、坐标系、过滤规则 | 当前只把障碍物转为局部线段边界，未接完整感知语义 |

## 接真实协议前的硬门禁

1. 拿到真实车端 Topic 清单：Domain、Topic name、Type name、QoS。
2. 拿到真实 IDL 或正式接口头文件，确认字段顺序和枚举值。
3. 决定是替换当前临时 IDL，还是新增真实协议适配层。
4. 修改 IDL 后必须重新运行 `idlparser`，重新生成 `generated/*TopicDataType*`。
5. x86 和 m57 都必须重新编译。
6. x86 smoke 必须覆盖至少一条真实协议映射样例。
7. m57 只能在板端实测后标记 runtime PASS。

## 推荐接入策略

不要直接把当前临时 IDL 改成大而全的正式协议。更稳的做法是：

```text
真实车端 IDL / Topic
  -> protocol adapter / converter
  -> 当前 ValetParkingStageParkingAdapter 内部结构
  -> PlanningTrajectory 输出
```

这样做的好处是，泊车算法内部流程不用被车端协议频繁变化牵着走；真实 Topic 变化时主要修 converter。

## 当前结论

当前计划和培训材料介绍的 MagnaDDS 开发流程一致：使用 IDL、生成 typed API、注册类型、创建 Topic、创建 DataReader/DataWriter。但当前 IDL 是 MVP 临时协议，不能当作真实车端最终协议。下一步如果要接车端，需要优先补齐真实 Topic/IDL 材料，而不是继续猜字段。
