# NEXT-041 OpenSpacePathProvider 线程生命周期与 NLP 依赖评估

本文记录 NEXT-041 的代码硬化、负向 smoke 和 NLP smoother 依赖判断。

## 一句话结论

本阶段继续硬化 `OpenSpacePathProvider / OpenSpaceThreadManager`：

```text
正常规划成功 -> target/search 线程能随 runner 退出被析构回收
规划超时 -> target plan 会被 cancel，Hybrid A* 收到 early_stop
PrePlan 候选 -> multi-lot 切换后仍能复用
NLP smoother -> 当前不接入，先记录依赖缺口
```

## 线程生命周期是什么意思

`OpenSpaceThreadManager` 里面有两类线程：

```text
search threads
  提前给非选中的停车位做 PrePlan 候选搜索

target thread
  给当前真正选中的停车位做 TargetPlan
```

线程生命周期就是确认这几件事：

```text
启动：Provider 创建时线程能正常启动
运行：有任务时线程能拿到输入并产出路径
取消：超时时能通知正在搜索的算法提前停止
停止：runner 退出、Provider reset、析构时线程能 join 回收
```

如果生命周期没做好，常见后果是：

```text
runner 退出卡住
后台线程访问已经释放的数据
timeout 后旧任务继续跑，影响下一次规划
多车位候选被错误覆盖
```

## 本阶段代码修订

### 1. early_stop 从局部变量升级为 worker 状态

之前：

```text
SearchLoop/TargetLoop 内部创建局部 atomic<bool> early_stop(false)
StopThreads() 看不到这个对象
```

现在：

```text
SearchWorker::early_stop
TargetWorker::early_stop
```

每个正在执行的规划任务都有自己的 shared early-stop 标志。`StopThreads()`、`CancelTargetPlan()` 和新 target plan 覆盖旧 target plan 时，都可以把旧任务的 `early_stop` 置为 `true`。

补充边界：`Reset()` 会先停止旧线程再重建线程，`StartThreads()` 现在会把 `TargetWorker::stop` 清回 `false`，避免 reset 后 target 线程一启动就退出。

### 2. 新增 CancelTargetPlan

新增接口：

```cpp
bool OpenSpaceThreadManager::CancelTargetPlan(uint64_t target_plan_id);
```

用途：

```text
OpenSpacePathProvider::Plan() 等待超过 target_plan_timeout_s
-> 标记 TARGET_TIMEOUT
-> 调用 CancelTargetPlan()
-> target 线程中的 Hybrid A* 收到 early_stop
-> Adapter 能尽快 reset Provider，而不是长时间等旧搜索自然结束
```

### 3. 增加 target_cancel 诊断

`PATH_PROVIDER` 诊断里新增：

```text
target_cancel=true/false
```

timeout 负向场景现在应看到：

```text
provider_status=TARGET_TIMEOUT
target_timeout=true
target_cancel=true
fallback to ROI seed
```

### 4. 增加线程停止日志

`StopThreads()` 完成 join 后输出：

```text
OpenSpaceThreadManager stopped, search_threads=4, target_thread_joined=true
```

smoke 脚本可用它验证 runner 退出或 Provider reset 时线程确实被回收。

### 5. build 脚本修正相对 out-dir

本阶段发现：

```text
build_valet_parking.sh --out-dir out/xxx
```

使用新目录时，底层 install 可能把相对路径展开到 build 目录内部，导致包装脚本最终找不到：

```text
out/xxx/valet_parking_mvp/x86/lib/libvalet_parking.so
```

已修正为：

```text
如果 --out-dir 是相对路径，先归一化为 workspace 下面的绝对路径
```

这样 x86/m57 的产物路径稳定。

## smoke 脚本新增能力

`smoke_valet_parking_x86.sh` 新增：

```text
--expect-thread-provider-stop
```

要求 runner log 中出现线程停止证据。

```text
--expect-path-provider-timeout
```

要求 valid/no-command 场景看到 `TARGET_TIMEOUT`，而不是正常 `TARGET_READY`。

```text
--path-provider-timeout-s N
```

通过环境变量：

```text
VALET_PARKING_PATH_PROVIDER_TIMEOUT_S
```

临时覆盖 Provider timeout。默认不设置时仍是 `8.5s`。

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

说明：direct-forward 分支按设计跳过 `ROI/PATH_PROVIDER/PATH_PARTITION`，走 `OPEN_SPACE_STRAIGHT_PATH -> SPEED_OPTIMIZER`，因此不要求 `OpenSpaceThreadManager stopped` 日志。

关键日志：

```text
PATH_PROVIDER ok ... provider_status=TARGET_READY ... target_cancel=false
PATH_PROVIDER failed: OpenSpacePathProvider target plan timeout ... provider_status=TARGET_TIMEOUT ... target_cancel=true ... fallback to ROI seed
OpenSpaceThreadManager stopped, search_threads=4, target_thread_joined=true
PATH_PROVIDER ok ... target_source=preplan_candidate
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

说明：这仍是交叉编译和依赖检查，不是 m57 板端运行通过。

## NLP smoother 依赖评估

原车或外部 standalone 参考里存在：

```text
E:\APA\DDS\parking_algorithm_standalone\planning\tasks\optimizers\open_space_path_generation\open_space_path_smoother.cc
E:\APA\DDS\parking_algorithm_standalone\planning\tasks\optimizers\open_space_path_generation\open_space_path_smoother.h
E:\APA\DDS\parking_algorithm_standalone\planning\math\discretized_points_smoothing\*
E:\APA\DDS\parking_algorithm_standalone\proto_convert\planner_open_space_config_convert.h
```

`open_space_path_smoother.cc` 的核心调用是：

```text
NlpPathSmoother::XYRoadPreprocessor()
NlpPathSmoother::NlpSolver()
```

相关数学平滑模块涉及：

```text
IPOPT
OSQP
ADOL-C
MUMPS/Coin-OR 相关头文件和库
```

当前 DDS 适配工程本地算法子集里没有纳入：

```text
open_space_path_smoother.cc/h
planning/math/discretized_points_smoothing/*
planner_open_space_config_convert.h
```

当前依赖环境检查：

```text
WSL x86 系统: pkg-config ipopt = 3.11.9
WSL x86 系统: pkg-config osqp 缺失
thirdparty/x86: 未发现随包 ipopt/osqp 实库
thirdparty/m57: 未发现随包 ipopt/osqp 实库
```

因此当前判断：

```text
不能在 NEXT-041 直接把 NLP smoother 接入产品链路。
```

原因不是 x86 不能临时编过，而是：

```text
1. m57 构建链路没有随包 NLP/QP 求解器依赖；
2. 依赖不能靠 WSL 系统库，否则 m57 无法交付；
3. 引入 smoother 会扩大到 config/proto_convert/math solver/链接库/RPATH 多个边界；
4. 当前已通过的 PATH_PROVIDER 粗路径 + PATH_PARTITION + SPEED_OPTIMIZER 链路不应被未验证依赖污染。
```

## 下一步建议

进入 NEXT-042：direct 分支输出契约补强。

NLP smoother 后续建议单独开阶段：

```text
先补 thirdparty/x86 与 thirdparty/m57 的 OSQP/IPOPT 依赖包
-> 再本地化 planner_open_space_config_convert 与 smoothing math 最小闭包
-> 再做独立 smoother 单元/工具验证
-> 最后再接入 OpenSpacePathProvider target/search 线程
```
