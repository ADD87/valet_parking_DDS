# NEXT-040 完整 OpenSpacePathProvider 线程体系接入说明

本文记录本阶段把普通泊车主链里的 `PATH_PROVIDER` 从“Adapter 同步调用 `OpenSpacePathGenerator::Plan()`”升级为“本地化 `OpenSpacePathProvider` + `OpenSpaceThreadManager` 线程体系”。

## 一句话解释

以前普通链是：

```text
ROI_DECIDER
-> Adapter 直接 new OpenSpacePathGenerator
-> OpenSpacePathGenerator::Plan()
-> PATH_PARTITION
-> SPEED_OPTIMIZER
```

现在普通链是：

```text
ROI_DECIDER
-> OpenSpacePathProvider
-> OpenSpaceThreadManager
   -> search threads: PrePlan 候选车位
   -> target thread: TargetPlan 当前选中车位
-> PATH_PARTITION
-> SPEED_OPTIMIZER
```

也就是说，路径搜索不再只是一个同步函数调用，已经有了和原车 `OpenSpacePathProvider / OpenSpaceThreadManager` 对齐的线程外壳。

## 本阶段新增文件

```text
applications/source/valet_parking/algorithm/parking_algorithm_standalone/planning/open_space/open_space_thread/open_space_thread_manager.h
applications/source/valet_parking/algorithm/parking_algorithm_standalone/planning/open_space/open_space_thread/open_space_thread_manager.cc
applications/source/valet_parking/algorithm/parking_algorithm_standalone/planning/tasks/optimizers/open_space_path_generation/open_space_path_provider.h
applications/source/valet_parking/algorithm/parking_algorithm_standalone/planning/tasks/optimizers/open_space_path_generation/open_space_path_provider.cc
```

并已加入：

```text
applications/source/valet_parking/CMakeLists.txt
```

## Adapter 中的新流程

普通入位链路仍然由 `ValetParkingStageParkingAdapter::Process()` 统一编排：

```text
SelectedSlot
-> 选择 opt_parking_seq 对应 ParkingLot
-> ConvertParkingLot
-> BuildVehicleState
-> ROI_DECIDER
-> PATH_PROVIDER_PRECHECK
-> BuildPathProviderPrePlanInputs
-> OpenSpacePathProvider::PrePlan(non-selected lots)
-> OpenSpacePathProvider::Plan(selected lot)
-> PATH_PARTITION
-> SPEED_OPTIMIZER
-> PlanningTrajectory
```

`PrePlan` 只预热非选中车位，不改变本帧输出。正式输出仍只来自当前选中车位的 `TargetPlan`。

## 日志诊断字段

`PATH_PROVIDER ok` 现在会带这些字段：

```text
threaded=true
provider_status=TARGET_READY
target_plan=submitted
target_output=ready
target_source=target_thread 或 preplan_candidate
target_timeout=false
wait_s=<本次等待时间>
preplan_candidates=<候选数>
thread_path_ids=[...]
```

这些字段用于确认当前确实进入线程 Provider，而不是静默退回同步路径生成。

## 本阶段修过的问题

第一次接入后，`valid` smoke 暴露了超时：

```text
PATH_PROVIDER failed: OpenSpacePathProvider target plan timeout
provider_status=TARGET_TIMEOUT
```

原因是 target 线程先等待不存在的 PrePlan 候选结果，等满 `8.5s` 后才准备自己生成路径，Adapter 已经超时。

修订后：

```text
有候选结果 -> target 线程复用候选结果
无候选结果 -> target 线程立即调用 OpenSpacePathGenerator::Plan()
```

同时修正了 `TargetPlan` 与 target completion 的锁顺序，避免 manager mutex 和 target mutex 反向持锁造成理论死锁。

复验时继续补了一个 PrePlan 队列硬化点：

```text
已排队但还没被 search worker 取走的候选任务，也视为已存在任务；
选择 worker 时避免覆盖 has_task=true 的候选输入。
```

这样可以降低多车位快速切换时，候选车位输入被后续候选覆盖的概率。

## 验证结果

x86 构建：

```text
out/valet_parking_thread_provider_040/valet_parking_mvp/x86/lib/libvalet_parking.so
```

x86 smoke：

```text
domain_145_20260728_224300_1009  valid 普通主链 PASS
domain_150_20260728_225158_651   multi-lot PrePlan 候选复用 PASS
domain_148_20260728_224359_929   direct-forward 回归 PASS
domain_151_20260728_225932_643   PrePlan 队列硬化后 valid 普通主链复验 PASS
domain_152_20260728_225947_831   PrePlan 队列硬化后 multi-lot 候选复用复验 PASS
domain_153_20260728_230057_653   PrePlan 队列硬化后 direct-forward 回归复验 PASS
```

关键证据：

```text
PATH_PROVIDER ok ... threaded=true ... provider_status=TARGET_READY ... target_source=target_thread
PATH_PROVIDER ok ... preplan_candidates=1 ... thread_path_ids=[2,-1,-1,-1]
PATH_PROVIDER ok ... preplan_candidates=1 ... target_source=preplan_candidate ... thread_path_ids=[2,1,-1,-1]
```

m57 交叉编译：

```text
out/valet_parking_thread_provider_040/valet_parking_mvp/m57/lib/libvalet_parking.so
```

PrePlan 队列硬化后已重新执行 x86 与 m57 构建，两个平台均通过。

静态检查：

```text
ELF: ARM aarch64
NEEDED: libmagna-dds-core.so.1
NEEDED: libmagna-dds-impl.so
```

## 和原始工程仍有差异

本阶段接入的是本地化线程体系，不是把原车 `Frame/DependencyInjector/PlanningContext/OpenSpaceInfo` 整套框架搬进来。

仍未完整接入：

- 原车 `PathOptimizer` 基类和 `TaskConfig` 框架。
- 原车 `PARKSTART -> PrePlan` 的完整状态机。
- 原车 `PlanningOnPathThread()` 的所有状态回写。
- 原车 `OpenSpacePathSmoother::Smooth()` 的 NLP/IPOPT 平滑链。
- 原车 `OpenSpaceDebug` 全量调试字段。

当前可以准确说：

```text
OpenSpacePathProvider / OpenSpaceThreadManager 的线程执行骨架已接入，
TargetPlan 与多车位 PrePlan 候选已进入 x86 DDS smoke 和 m57 交叉编译验证。
```

当前不能说：

```text
完整原车 OpenSpacePathProvider + NLP smoother + Frame 状态回写已经 100% 复现。
```

## 下一步

进入 NEXT-041：继续硬化 `OpenSpacePathProvider` 线程生命周期。

重点：

- reset/stop/destructor 的线程退出验证。
- target timeout 的负向 smoke。
- `target_source=preplan_candidate` 已在 multi-lot smoke 出现，后续继续补生命周期和负向场景。
- 是否接入真正 `OpenSpacePathSmoother/NLP`，需要先单独审依赖，不能混进当前已通过链路。
