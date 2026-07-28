# 两版 PathProvider 复用价值评估与接入切口

- 文档编号：DOC-006
- 日期：2026-07-28
- 触发原因：用户询问 `Repair_ValetParkingStageParking_260430-main` 和 `parking_algorithm_standalone` 两版已修代码，能否减少当前 MagnaDDS 适配工作量。
- 当前结论：两版代码很有价值，主要节省“算法源码剥离、类型替换、独立编译”的工作量；但不直接等于已经完成 MagnaDDS Adapter 接入，也不能替代 DDS 输入输出验证。

## 1. 本次参考的两版代码

| 参考源 | 当前作用 | 是否作为主工程代码 |
|---|---|---|
| `E:\APA\DDS\parking_algorithm_standalone` | 已剥离中间件的泊车算法独立化参考，包含 proto_convert、OpenSpacePathGenerator、PathPartition、SpeedOptimizer 等可复用源码 | 否，只作为参考源；NEXT-032 已把当前 MVP 真实使用的最小源码闭包复制到 `applications/source/valet_parking/algorithm/parking_algorithm_standalone` |
| `E:\APA\DDS\Repair_ValetParkingStageParking_260430-main` | 修订剥离中间件后的另一版参考，额外包含 stage_runner adapter plan，可作为原始流程差距清单 | 否，只作为参考源；不整目录复制到当前工程 |
| `applications/source/valet_parking/algorithm/parking_algorithm_standalone` | 当前 MagnaDDS MVP 实际编译和链接的本地算法源码 | 是，当前主工程使用这里 |

## 2. 已经节省的工作量

| 工作项 | 如果从 TempAPA_Code 原始工程开始 | 两版参考代码节省点 | 当前状态 |
|---|---|---|---|
| proto 类型剥离 | 需要逐个剥离 Apollo/ROS2/protobuf 依赖，替换 Header、VehicleState、PathPoint、ParkingLot、OpenSpaceStatus 等类型 | 两版都已有 `proto_convert/*`，很多类型替代和字段映射已经整理过 | 当前已复用其思路和部分类型闭包 |
| OpenSpacePathGenerator 独立化 | 需要从大量 planning 依赖中拆出 HybridAStar、PathGenerator、车辆参数、数学库、配置对象 | `parking_algorithm_standalone` 已提供可独立编译路径生成闭包 | 当前 MVP 已接入本地化 `OpenSpacePathGenerator + HybridAStar` |
| PATH_PARTITION / SPEED_OPTIMIZER 编译闭包 | 原始工程依赖 Frame、OpenSpaceInfo、配置、调试信息和规划上下文 | standalone 已把任务级 Execute 边界剥离出来 | 当前已接入并通过 x86 smoke/m57 build |
| 原始 Stage 流程理解 | 需要人工从 `stage.cc`、`stage_valet_parking_parking.cc`、各 Task 大类反推执行顺序 | Repair 版保留 Stage 和 adapter plan，能直接看到 unsupported 差距 | 已形成 DOC-005 差距表 |
| PathProvider 后续迁移边界 | 从原始大类直接搬会遇到线程、Frame、PlanningContext、smoother、状态机等混杂依赖 | Repair 版 `open_space_path_provider_adapter_plan.cpp` 已列出 unsupported 项，可作为任务拆分依据 | 已纳入后续计划 |

粗略判断：这两版参考代码让“算法剥离/类型替换/独立编译”的工作量减少约 50%-70%。尤其是粗路径、路径分段、速度采样这类纯算法或半纯算法部分，已经避免了从原始工程重新拆依赖。

## 3. 没有被节省的工作量

| 工作项 | 为什么仍要做 |
|---|---|
| DDS IDL 契约设计 | 当前 MagnaDDS 要靠 `.idl -> generated typed API` 收发，参考代码不是 DDS Topic 契约 |
| Component/Adapter 生命周期 | 需要创建 DomainParticipant、Reader、Writer、worker loop，并保证进程启动和退出可控 |
| SelectedSlot 到算法输入的边界 | DDS 输入可能为空、NaN、count 溢出、多车位、错坐标、障碍物异常，必须在 Adapter 层硬化 |
| RuntimeContext 映射 | 原始工程靠 Frame/OpenSpaceInfo/PlanningContext 保存历史，当前工程要用轻量 runtime state 显式承接 |
| smoke 验证 | 参考代码能编译不代表当前 DDS 收发链路可运行，必须用 mock publisher/subscriber 验证 |
| m57 交叉编译 | x86 能跑不等于 m57 能链接，必须检查 ARM aarch64 ELF 和 MagnaDDS 依赖 |
| 板端运行 | 当前没有 m57 板端，不能把交叉编译写成板端通过 |

一句话：参考代码节省的是“算法搬得动”的工作；DDS 适配要证明的是“消息收得进来、算法吃得下、结果发得出去”。

## 4. 对 PathProvider 的工作量修正

之前看起来 PathProvider 很大，是因为如果目标是完整复现原始 `OpenSpacePathProvider::Process + OpenSpaceThreadManager + smoother + Frame`，它确实是大块。

但当前不需要从零做。更准确的拆分是：

| 切片 | 工作量判断 | 原因 |
|---|---|---|
| `OpenSpacePathGenerator/HybridAStar` 粗路径 | 已基本省掉 | standalone 已有可编译闭包，当前已接入 |
| 轻量 replan/history/warm_start/trace_adjust | 中等，已完成多个 smoke 切片 | 需要当前 Adapter 自己保存 runtime state，不直接搬 Frame |
| 多车位 `opt_parking_seq` 选择验证 | 小到中等，适合当前阶段 | 不改 IDL，只补 DDS 样本和断言 |
| `DIRECT_FORWARD/DIRECT_BACKWARD` 直行分支 | 中等，适合下一批 | `OpenSpaceStraightPathProvider` 源码存在，但需要轻量控制输入表达 |
| 完整 PathProvider 大类和 ThreadManager | 大，不建议立刻做 | 牵涉线程生命周期、PrePlan/TargetPlan、Frame/OpenSpaceInfo、smoother 和状态回写 |
| NLP smoother | 大且高风险 | 依赖 solver、配置和稳定性验证，当前 MVP 不应混入 |

## 5. 本阶段建议的接入切口

本阶段选择 `NEXT-033：SelectedSlot 多车位 opt_parking_seq DDS 输入输出验证`，原因如下：

1. 不修改 IDL ABI，风险低。
2. 直接验证当前 MagnaDDS typed 输入，不是只验证 C++ 函数。
3. 可以确认 Adapter 在同一条 `SelectedSlot.parking_lots` 列表中按 `opt_parking_seq` 选择对应车位。
4. 可以继续验证 `PATH_PROVIDER` 在选中目标切换后触发 `TARGET_UPDATE`，稳定后回到 `history=reused`。
5. 输出仍由 `/planning/trajectory` mock subscriber 验证，覆盖 DDS 输入到 DDS 输出闭环。

本阶段不做：

- 不新增 `ParkingCommand` IDL。
- 不接完整 `FunctionManager`。
- 不接完整 `OpenSpacePathProvider` 大类。
- 不接 `OpenSpaceThreadManager`。
- 不接 NLP smoother。

## 6. 后续建议

完成 NEXT-033 后，下一步更专业的路线是：

| 阶段 | 内容 | 原因 |
|---|---|---|
| NEXT-034 | 轻量 Stage 控制输入设计，定义 `ParkingCommand` 草案 | 为 pause/brake/direct/finish 提供输入表达，避免滥用 `SelectedSlot.is_valid` |
| NEXT-035 | 接入 `DIRECT_FORWARD/DIRECT_BACKWARD` 直行分支 | 原始流程 P0 差距，边界比完整 PathProvider 大类更清楚 |
| NEXT-036 | pause/brake/finish 行为 smoke | 补 Stage 可观察状态，不急于搬大框架 |
| NEXT-037+ | 再评估完整 PathProvider 大类、ThreadManager、smoother | 需要单独验证依赖和线程行为 |

## 7. 当前判断

用户之前修过的两版代码非常有用，应该继续作为参考和局部源码来源。它们能让我们少做大量算法剥离工作。

但当前 MagnaDDS 适配工程的主战场仍在：

```text
DDS 输入契约
-> Component 收发
-> Adapter 输入边界
-> RuntimeContext 状态映射
-> 算法调用
-> PlanningTrajectory 输出
-> x86/m57 验证
```

所以最稳的做法不是整目录复制，也不是马上塞进完整原始大类，而是继续按可验证切片推进。
