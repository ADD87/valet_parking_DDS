# 零基础解释：本阶段代码与 DDS 术语

> 依据：`E:\APA\DDS\MagnaDDS_SDK_培训材料_修订版_v1.1.md` 中关于 DDS、Domain、Topic、Type、QoS、IDL、Publisher/Subscriber、DataWriter/DataReader、`idlparser` 的培训内容。  
> 目的：你不需要先懂 DDS，也不需要先懂这些代码。先把它当成“有人寄包裹、有人收包裹、中间有一个泊车小工厂”来理解。

---

## 1. 先用一个小故事理解 DDS

把 DDS 想成一个小区里的“自动广播和投递系统”。

有人想发消息，就像把包裹放进一个固定邮箱。  
有人想收消息，就像每天去同一个邮箱拿包裹。  
DDS 负责让发包裹的人和收包裹的人自动找到对方。

在培训材料里，这几个词最重要：

| DDS 术语 | 像什么 | 在我们代码里是什么意思 |
|---|---|---|
| Domain | 一个小区 | 只有在同一个 domain id 里的程序才能互相发现 |
| Topic | 一个频道或邮箱名 | `/selected_slot` 是输入频道，`/planning/trajectory` 是输出频道 |
| Type | 包裹盒子的形状 | `SelectedSlot` 和 `PlanningTrajectory` 的字段格式 |
| QoS | 快递规则 | 保留几条历史数据、队列深度、可靠性等规则 |
| Publisher | 发包裹的管理者 | 创建 `DataWriter` |
| Subscriber | 收包裹的管理者 | 创建 `DataReader` |
| DataWriter | 真正把包裹放进邮箱的人 | 调用 `write()` 发数据 |
| DataReader | 真正从邮箱取包裹的人 | 调用 `take_next_sample()` 收数据 |

一句话记住：  
**Domain 要一样，Topic 要一样，Type 要兼容，QoS 要能匹配，数据才能通。**

当前项目状态补充：

- 主输入仍是 `/selected_slot`。
- 主输出仍是 `/planning/trajectory`。
- 辅助输入已经增加三个临时 Topic：`/localization/estimate`、`/chassis/state`、`/perception/obstacles`。
- 这些辅助 Topic 目前是 MVP 临时协议，还不是最终车端协议。

---

## 2. 我们这次做出来的系统像什么

现在这个泊车 DDS 小系统可以画成这样：

```text
+------------------------------+
| selected_slot_mock_publisher |
| 假装感知/上游模块发一个车位     |
+---------------+--------------+
                |
                | DDS Topic: /selected_slot
                v
+---------------+--------------+
| valet_parking_runner          |
| 只负责启动，不做泊车计算         |
+---------------+--------------+
                |
                v
+---------------+--------------+
| ValetParkingComponent         |
| DDS 收发员：收 SelectedSlot，发 PlanningTrajectory |
+---------------+--------------+
                |
                v
+---------------+--------------+
| ValetParkingStageParkingAdapter |
| 业务翻译员：把 DDS 数据翻译给 ROI_DECIDER |
+---------------+--------------+
                |
                v
+---------------+--------------+
| ROI_DECIDER                   |
| 算出泊车 ROI 和目标位姿         |
+---------------+--------------+
                |
                | DDS Topic: /planning/trajectory
                v
+---------------+--------------+
| planning_trajectory_mock_subscriber |
| 假装下游模块，检查收到的轨迹          |
+--------------------------------------+
```

这还不是完整量产版泊车算法。
但当前已经不只是第一段 ROI_DECIDER，而是接到了：

```text
ROI_DECIDER -> PATH_PROVIDER -> PATH_PARTITION -> SPEED_OPTIMIZER
```

你可以把它理解为：

1. 上游说：“我看到了一个车位。”
2. 辅助上游可以说：“这是车辆位置、底盘状态、障碍物。”
3. DDS 把这些消息送进我们的泊车组件。
4. 组件把数据交给 Adapter。
5. Adapter 把 DDS 数据翻译成 standalone 算法能看懂的结构。
6. ROI_DECIDER 圈出泊车区域和目标姿态。
7. PATH_PROVIDER 找粗路径。
8. PATH_PARTITION 选择当前应该执行的路径段。
9. SPEED_OPTIMIZER 给路径配速度和时间。
10. 最后通过 DDS 发出 `PlanningTrajectory`。

---

## 3. 为什么要有 IDL 文件

文件：

```text
applications/source/valet_parking/idl/valet_parking_topics.idl
```

IDL 可以理解成“包裹盒子的设计图”。

如果没有设计图，发送方可能说：

```text
我发的是：车位编号、四个角点、车位宽度
```

接收方却以为：

```text
我收到的是：轨迹点、速度、加速度
```

这样就乱了。

所以我们写了 `valet_parking_topics.idl`，告诉 DDS：

- 输入包裹叫 `SelectedSlot`，表示“选中的车位”。
- 输出包裹叫 `PlanningTrajectory`，表示“规划轨迹”。
- 每个包裹里面有哪些字段、字段是什么类型、字段顺序是什么。

培训材料里的标准流程是：

```text
IDL -> idlparser -> 生成 C++ 类型代码 -> register_type() -> create_topic()
```

我们的项目也是这样做的。

---

## 4. generated 文件是什么，为什么不要手改

目录：

```text
applications/source/valet_parking/generated/
```

里面主要有：

```text
valet_parking_topics.h
valet_parking_topics.cpp
valet_parking_topicsTopicDataType.h
valet_parking_topicsTopicDataType.cpp
```

这些文件不是人工慢慢写出来的，而是由 MagnaDDS SDK 的 `idlparser` 从 IDL 生成的。

你可以理解成：

```text
IDL 设计图
  -> idlparser 机器
    -> C++ 包裹类
    -> DDS 打包/拆包工具
```

它们的作用是：

| 文件 | 作用 |
|---|---|
| `valet_parking_topics.h/.cpp` | 定义 `SelectedSlot`、`PlanningTrajectory` 等 C++ 数据类 |
| `valet_parking_topicsTopicDataType.h/.cpp` | 告诉 DDS 怎么序列化、反序列化这些数据 |

注意：  
**以后如果要改消息字段，应该先改 IDL，再重新运行 `idlparser`，不要直接手改 generated 文件。**

---

## 5. 每个代码文件在干什么

### 5.1 `valet_parking_runner/main.cpp`

路径：

```text
applications/source/valet_parking_tools/valet_parking_runner/main.cpp
```

它像一个“电源开关”。

它负责：

- 读命令行参数，比如 `--domain-id=79`；
- 准备 `valet_parking_config_t` 配置；
- 调用 `valet_parking_create()` 创建组件；
- 调用 `valet_parking_start()` 启动组件；
- 等 Ctrl+C；
- 最后 stop 和 destroy。

它不负责：

- 不收 DDS 数据；
- 不算 ROI；
- 不生成轨迹。

这符合你定的分层：  
**runner 只负责启动组件。**

---

### 5.2 `valet_parking_c_api.h/.cpp`

路径：

```text
applications/source/valet_parking/include/valet_parking_c_api.h
applications/source/valet_parking/src/valet_parking_c_api.cpp
```

它像“组件门口的几个按钮”。

外部程序只需要知道这些按钮：

```text
create
start
stop
destroy
get_last_error
```

为什么要做 C API？

因为 `libvalet_parking.so` 是一个共享库。  
C API 比 C++ 类接口更稳定，更适合作为 `.so` 对外入口。

---

### 5.3 `valet_parking_component.h/.cpp`

路径：

```text
applications/source/valet_parking/src/valet_parking_component.h
applications/source/valet_parking/src/valet_parking_component.cpp
```

它像“DDS 收发室”。

它按照培训材料里的标准 DDS 流程做事：

```text
get_instance()
  -> create_participant()
    -> register_type()
      -> create_topic()
        -> create_subscriber()
          -> create_datareader()
        -> create_publisher()
          -> create_datawriter()
```

收到数据时，它做：

```text
take_next_sample(&SelectedSlot)
  -> 交给 ValetParkingStageParkingAdapter
  -> write(&PlanningTrajectory)
```

也就是说：

- 它负责 DDS；
- 它负责 worker loop；
- 它负责把输入交给业务层；
- 它不应该承载完整泊车算法。

---

### 5.4 `valet_parking_stage_parking_adapter.h/.cpp`

路径：

```text
applications/source/valet_parking/src/valet_parking_stage_parking_adapter.h
applications/source/valet_parking/src/valet_parking_stage_parking_adapter.cpp
```

它像“翻译员 + 第一阶段泊车工人”。

DDS 收到的是 `SelectedSlot`。  
standalone 的 ROI_DECIDER 想要的是另一套 C++ 结构，例如：

```text
TL::perception::ParkingLotOut
TL::common::VehicleState
```

所以 Adapter 做三件事：

1. 把 DDS 的 `SelectedSlot` 翻译成 standalone 的车位结构。
2. 用当前配置里的 fake vehicle pose 构造车辆状态。
3. 调用 ROI_DECIDER，拿到目标位姿，再生成一条简单轨迹。

核心流程可以理解为：

```text
SelectedSlot
  -> ParkingLotOut
  -> VehicleState
  -> OpenSpaceRoiDecider::Process()
  -> PlanningTrajectory
```

如果输入不对，例如：

- 没有车位；
- 车位数量异常；
- 角点里有 NaN；
- ROI_DECIDER 失败；

Adapter 不会硬算轨迹，而是输出：

```text
estop = true
reason = 具体失败原因
```

这就是我们验证过的 `empty / overflow / nan` fallback。

---

### 5.5 `CMakeLists.txt`

路径：

```text
applications/source/valet_parking/CMakeLists.txt
```

它像“做饭菜谱”。

告诉 CMake：

- 这个模块叫 `valet_parking`；
- 要编译哪些 `.cpp`；
- 要包含哪些头文件目录；
- 要链接哪些库；
- 要把 standalone ROI_DECIDER 哪些源码一起编进来。

这次关键变化是：

```text
thirdparty::magna-dds-core
thirdparty::magna-dds-impl
thirdparty::eigen
standalone ROI source files
```

如果没有 `thirdparty::eigen`，standalone 里的 `math_utils.cc` 会找不到 `Eigen/Dense`。

---

### 5.6 `src/compat/**`

路径：

```text
applications/source/valet_parking/src/compat/
```

它像“临时小字典”。

standalone ROI_DECIDER 里面引用了一些原工程常见依赖，例如：

```text
absl
common/status
planning/common/path
```

为了先接通 ROI_DECIDER，而不是一次性搬完整原工程，我们放了少量兼容头。

注意：  
这些不是最终大而全的基础库迁移，只是为了让 ROI_DECIDER 这一段能先编译、先跑通。

---

### 5.7 mock publisher 和 mock subscriber

路径：

```text
applications/source/valet_parking_tools/selected_slot_mock_publisher/main.cpp
applications/source/valet_parking_tools/aux_input_mock_publisher/main.cpp
applications/source/valet_parking_tools/planning_trajectory_mock_subscriber/main.cpp
```

它们是测试用的小工具。

`selected_slot_mock_publisher` 像一个“假上游”：

```text
我假装发一个选中的车位 SelectedSlot
```

`aux_input_mock_publisher` 像另一个“假上游”：

```text
我假装发车辆定位、底盘状态、障碍物
```

`planning_trajectory_mock_subscriber` 像一个“假下游”：

```text
我等着收 PlanningTrajectory，并检查字段是不是基本正常
```

没有这些工具，就很难证明 DDS 通路真的跑通。

---

## 6. 一条 valid 数据实际怎么跑

当我们运行 valid 测试时，发生的是：

```text
1. subscriber 先启动，等 /planning/trajectory
2. runner 启动 valet_parking 组件
3. publisher 发 /selected_slot
4. component 的 DataReader 取到 SelectedSlot
5. component 调 Adapter
6. Adapter 调 ROI_DECIDER
7. Adapter 构造 PlanningTrajectory
8. component 的 DataWriter 写出 /planning/trajectory
9. subscriber 收到轨迹
```

验证结果是：

```text
received sample points=179, length=7.64356, is_estop=false
ROI_DECIDER ok, scenario=1, lot_status=0, target=(7.314,1.760,0.200)
```

这说明：

- DDS 输入通了；
- ROI_DECIDER、PATH_PROVIDER、PATH_PARTITION、SPEED_OPTIMIZER 都被调用了；
- DDS 输出也通了。

当运行带辅助输入的测试时：

```bash
bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh \
  --run-root <x86运行目录> \
  --with-aux-inputs
```

runner 日志里应该能看到：

```text
aux localization
aux chassis
aux obstacles
external_vehicle=true
external_obstacles=1
```

这说明辅助 Topic 不只是创建出来了，而是真的被 component 读到了。

---

## 7. 为什么异常输入要输出 estop

如果输入坏了，不能假装一切正常。

例如：

| 输入模式 | 问题 | 输出 |
|---|---|---|
| `empty` | 没有车位 | 1 点 estop 轨迹 |
| `overflow` | `count` 比实际车位数量大很多 | 1 点 estop 轨迹 |
| `nan` | 角点坐标有非法数值 | 1 点 estop 轨迹 |

这就像：

```text
如果地图不清楚，不能乱开车。
应该停住，并说明为什么停。
```

所以输出里会有：

```text
estop.is_estop = true
estop.reason = 失败原因
```

---

## 8. 你现在最需要记住的 10 个词

| 词 | 最简单解释 |
|---|---|
| DDS | 自动帮不同程序传数据的系统 |
| MagnaDDS | 本项目使用的 DDS SDK |
| Domain | 通信小区，id 一样才互相看得见 |
| Topic | 数据频道，例如 `/selected_slot` |
| Type | 数据格式，例如 `SelectedSlot` |
| IDL | 数据格式设计图 |
| Publisher | 发布端管理者 |
| Subscriber | 订阅端管理者 |
| DataWriter | 真正发送数据 |
| DataReader | 真正接收数据 |

再加 3 个本项目词：

| 词 | 最简单解释 |
|---|---|
| runner | 启动程序的入口 |
| component | DDS 收发室 |
| adapter | 把 DDS 数据翻译给泊车算法 |

---

## 9. 以后排查 DDS 不通，先看这 6 件事

培训材料里也强调：DDS 不通时，不要一上来改算法。

先检查：

1. 两边 `domain_id` 是否一样。
2. 两边 topic name 是否一样。
3. 两边 type 是否来自同一份 IDL。
4. 修改 IDL 后是否重新生成 generated 代码。
5. QoS 是否兼容，尤其是 history depth。
6. subscriber 是否启动得太晚，discovery 是否有足够时间完成。

我们测试时故意让 publisher 多发几条，就是为了给 DDS discovery 留时间。

---

## 10. 下一阶段你应该怎么理解

现在我们已经接了：

```text
ROI_DECIDER
  -> PATH_PROVIDER
  -> PATH_PARTITION
  -> SPEED_OPTIMIZER
```

后面继续推进时，重点会从“能跑通”转向“更像真实车端”：

```text
临时辅助 Topic
  -> 异常输入验证
  -> 真实车端 Topic 契约对齐
  -> 板端运行验证
```

你可以把它理解成：

| 阶段 | 像什么 |
|---|---|
| ROI_DECIDER | 先圈出“车应该在哪个区域里停车” |
| PATH_PROVIDER | 找一条能开进去的大致路线 |
| PATH_PARTITION | 把路线切成更好处理的小段 |
| SPEED_OPTIMIZER | 给路线配速度，决定什么时候快、什么时候慢、什么时候停 |

现在这一步的意义是：  
**我们已经证明 DDS 真的能把车位送进来，也能把 ROI 结果变成轨迹发出去。**
