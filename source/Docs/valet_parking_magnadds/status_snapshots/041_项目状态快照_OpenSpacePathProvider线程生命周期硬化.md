# 项目状态快照 041：OpenSpacePathProvider 线程生命周期硬化

## 阶段结论

NEXT-041 已完成。

本阶段没有继续扩大到完整 `Frame/DependencyInjector/OpenSpaceInfo/NLP smoother`，而是在 NEXT-040 已接入的本地化线程 Provider 上做硬化：

```text
OpenSpacePathProvider
-> OpenSpaceThreadManager
   -> search threads
   -> target thread
```

当前结果：

```text
线程 stop/destructor 可观测
target timeout 可取消
multi-lot PrePlan 复用保持可用
x86 smoke 通过
m57 交叉编译通过
NLP smoother 已评估，暂不接入产品链路
```

## 代码修订

- `SearchWorker` 和 `TargetWorker` 新增 shared `early_stop`。
- `StopThreads()` 会设置 stop 和 early_stop，并 join search/target 线程。
- `StartThreads()` 会在重建 target 线程前清回 `TargetWorker::stop=false`，避免 `Reset()` 后 target 线程立即退出。
- 新增 `CancelTargetPlan(uint64_t target_plan_id)`。
- `OpenSpacePathProvider::Plan()` 超时后调用 `CancelTargetPlan()`。
- `PATH_PROVIDER` 诊断新增 `target_cancel=true/false`。
- `smoke_valet_parking_x86.sh` 新增：

```text
--expect-thread-provider-stop
--expect-path-provider-timeout
--path-provider-timeout-s
```

- `build_valet_parking.sh` 修正相对 `--out-dir`，统一转换为 workspace 绝对路径。

## 验证结果

x86 构建：

```text
out/valet_parking_thread_lifecycle_041/valet_parking_mvp/x86/lib/libvalet_parking.so
```

x86 smoke：

```text
domain_160_20260728_232904_1307  valid + destructor stop evidence PASS
domain_161_20260728_232928_1477  target timeout + cancel + ROI seed fallback PASS
domain_162_20260728_232943_1607  multi-lot PrePlan candidate reuse regression PASS
domain_164_20260728_233018_1946  direct-forward regression PASS
```

说明：direct-forward 分支按设计跳过 threaded `OpenSpacePathProvider`，因此不要求线程停止日志，只验证 direct 输出契约不回归。

关键证据：

```text
OpenSpaceThreadManager stopped, search_threads=4, target_thread_joined=true
PATH_PROVIDER failed: OpenSpacePathProvider target plan timeout
provider_status=TARGET_TIMEOUT
target_timeout=true
target_cancel=true
fallback to ROI seed
target_source=preplan_candidate
```

m57 交叉编译：

```text
out/valet_parking_thread_lifecycle_041/valet_parking_mvp/m57/lib/libvalet_parking.so
```

静态检查：

```text
ELF: ARM aarch64
NEEDED: libmagna-dds-core.so.1
NEEDED: libmagna-dds-impl.so
```

## NLP smoother 评估结论

当前不接入 NLP smoother。

原因：

```text
外部 standalone 有 open_space_path_smoother 与 smoothing math 源码；
当前 DDS 本地算法子集没有纳入这些文件；
WSL x86 系统有 IPOPT 3.11.9，但 OSQP pkg-config 缺失；
thirdparty/x86 与 thirdparty/m57 未发现随包 IPOPT/OSQP 实库；
m57 产品链路不能依赖 WSL 系统库。
```

因此，当前阶段继续保持：

```text
OpenSpacePathGenerator 粗路径
-> OpenSpacePathPartition
-> OpenSpaceSpeedOptimizer
```

不把未验证的 smoother/NLP 依赖混入已通过链路。

## 当前边界

- m57 仍只是交叉编译通过，未做板端运行验证。
- `parking_status` 仍是诊断文本，不是正式 DDS 字段。
- timeout 后当前策略仍是 Adapter 的 `fallback to ROI seed`，不是最终量产安全策略。
- 完整 NLP smoother 需要单独补依赖和验证闭包。

## 下一步

进入 NEXT-042：direct 分支输出契约补强。

重点：

```text
把 direct 分支 replan_reason 诊断字段标准化；
明确 target_gear / trajectory_type / parking_status 哪些只是文本诊断；
保持 x86 direct smoke 与 m57 交叉编译通过。
```
