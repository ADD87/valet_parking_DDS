# 项目状态快照 040：OpenSpacePathProvider 线程体系接入

## 阶段结论

NEXT-040 已完成。

本阶段把普通泊车主链中的 `PATH_PROVIDER` 从 Adapter 内部同步调用 `OpenSpacePathGenerator::Plan()`，升级为本地化 `OpenSpacePathProvider + OpenSpaceThreadManager` 线程体系。

当前普通链路为：

```text
SelectedSlot
-> ROI_DECIDER
-> OpenSpacePathProvider
   -> PrePlan(non-selected lots)
   -> TargetPlan(selected lot)
-> PATH_PARTITION
-> SPEED_OPTIMIZER
-> PlanningTrajectory
```

## 本阶段修订

- 新增本地化线程管理：

```text
planning/open_space/open_space_thread/open_space_thread_manager.h
planning/open_space/open_space_thread/open_space_thread_manager.cc
```

- 新增本地化 Provider 包装：

```text
planning/tasks/optimizers/open_space_path_generation/open_space_path_provider.h
planning/tasks/optimizers/open_space_path_generation/open_space_path_provider.cc
```

- `PathProviderRuntimeState` 持有线程版 `OpenSpacePathProvider`。
- `RunPathProvider()` 不再直接构造 `OpenSpacePathGenerator`，改为：

```text
OpenSpacePathProvider::PrePlan(candidate_inputs)
OpenSpacePathProvider::Plan(target_input)
```

- 多车位 `SelectedSlot` 中的非选中车位会转换为 PrePlan 候选输入，交给 search threads 预热。
- 正式输出仍只来自当前选中车位的 TargetPlan。
- smoke 脚本新增普通主链线程断言：

```text
PATH_PROVIDER ok
threaded=true
provider_status=TARGET_READY
target_source=target_thread
```

- multi-lot smoke 新增 PrePlan 候选断言：

```text
preplan_candidates=1
thread_path_ids=[2,...]
thread_path_ids=[1,...]
```

## 调试修订

首次接入后，`valid` smoke 出现：

```text
PATH_PROVIDER failed: OpenSpacePathProvider target plan timeout
provider_status=TARGET_TIMEOUT
```

根因：

```text
target 线程先等待不存在的 PrePlan 候选结果，等待满 8.5s 后才开始自己生成路径；
Adapter 的 Plan() 同样等待 8.5s，因此先超时回退到 ROI seed。
```

修订：

```text
有候选结果 -> 复用候选结果；
无候选结果 -> target 线程立即调用 OpenSpacePathGenerator::Plan()。
```

另外修正了 `TargetPlan` 与 target completion 的锁顺序，避免 manager mutex 和 target mutex 反向持锁。

复验阶段又补充了 PrePlan 队列硬化：

```text
has_task=true 的候选任务也会被识别为已有任务；
worker 选择时不覆盖尚未被消费的候选输入。
```

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

关键日志：

```text
PATH_PROVIDER ok ... threaded=true ... provider_status=TARGET_READY ... target_source=target_thread
PATH_PROVIDER ok ... preplan_candidates=1 ... thread_path_ids=[2,-1,-1,-1]
PATH_PROVIDER ok ... preplan_candidates=1 ... target_source=preplan_candidate ... thread_path_ids=[2,1,-1,-1]
```

m57 交叉编译：

```text
out/valet_parking_thread_provider_040/valet_parking_mvp/m57/lib/libvalet_parking.so
```

PrePlan 队列硬化后已重新执行 x86 与 m57 构建，均通过。

m57 静态检查：

```text
ELF: ARM aarch64
NEEDED: libmagna-dds-core.so.1
NEEDED: libmagna-dds-impl.so
```

## 当前边界

本阶段不是完整搬入原车 `Frame/DependencyInjector/PlanningContext/OpenSpaceInfo`。

仍未完成：

- 原车 `PathOptimizer` 基类和 `TaskConfig` 框架。
- 原车 `PARKSTART -> PrePlan` 的完整状态机。
- 原车 `PlanningOnPathThread()` 的全部状态回写。
- 原车 `OpenSpacePathSmoother::Smooth()` 的 NLP/IPOPT 平滑链。
- m57 板端运行验证。

## 当前状态

```text
PASS_PATH_PROVIDER_THREAD_MANAGER_X86_M57_STATIC_BLOCKED_BOARD
```

## 下一步

进入 NEXT-041：继续硬化 `OpenSpacePathProvider` 线程生命周期和候选复用。

重点：

- reset/stop/destructor 的线程退出验证。
- target timeout 负向场景。
- `target_source=preplan_candidate` 已在 multi-lot smoke 中出现，后续继续补生命周期和负向验证。
- 单独评估 NLP smoother 依赖，不混入当前已通过链路。
