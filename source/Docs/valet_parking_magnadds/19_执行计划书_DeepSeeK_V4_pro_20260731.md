# 《执行计划书》

- 项目：feature_integration / valet_parking 能力增量接入
- 计划版本：v2.0（修订版）
- 编制日期：2026-08-01
- 目标分支：`DeepSeeK_V4_pro_20260731`
- 审批规则：**未经用户明确同意，不得进入下一阶段**

---

## 0. 环境与工具链

### 0.1 编译环境（Docker）

本项目编译在标准 Docker 容器内进行，**不在宿主机直接编译**。

| 项 | 值 |
|---|---|
| Docker 镜像 | `elc-artifactory-hoo.magna.global:443/84244-cn-sw-docker/l5i4550_m57_mcm6c_jammy_env:latest` |
| 启动命令 | `docker run -it --rm -u $(id -u):$(id -g) -v "$(pwd):/workspace" <镜像>` |
| 容器内编译入口 | `./build_app.sh --build=<x86|m57>` |

`build_app.sh` 实际调用 `compile/build.sh`，自动传入 `--app-root`、`--out-dir`、`--pack-name`。

### 0.2 构建命令速查

| 场景 | 命令（在 Docker 容器内执行） |
|---|---|
| m57 release 全量编译 | `./build_app.sh --build=m57 --mode=release` |
| x86 release 编译+验证 | `./build_app.sh --build=x86 --mode=release` |
| 单模块编译 | `./build_app.sh --build=x86 --module=valet_parking` |
| 启用 feature flag 编译 | `./build_app.sh --build=x86 --features=ENABLE_PRKVINBUS,ENABLE_NLP_SMOOTHER` |
| 打包产物 | `./build_app.sh --build=m57 --pack` |
| 清理重建 | `./build_app.sh --build=x86 --clean` |

Feature flag 定义文件：`compile/cmake/app_build_feature_flags_config.h.in`。
使用方式：`#include "app_build_feature_flags_config.h"`，通过 `#if ENABLE_XXX` 做条件编译。
详见 `compile/docs/cmake_build_guide.md`。

### 0.3 工具链

| 平台 | 工具链文件 | 目标架构 |
|---|---|---|
| x86 | 内置（默认） | x86_64 |
| m57 | `compile/toolchains/m57_toolchain.cmake` | ARM aarch64 (Cortex-M57) |

### 0.4 参考源码路径

| 用途 | 路径 |
|---|---|
| **原始 APA 源码（权威语义源）** | `C:\Users\jasezhan\Desktop\00_git\APA\Local_task\TempAPA_Code` |
| standalone 算法参考 | `C:\Users\jasezhan\Desktop\00_git\APA\Local_task\parking_algorithm_standalone` |
| 中间件剥离/修订参考 | `C:\Users\jasezhan\Desktop\00_git\APA\Local_task\Repair_ValetParkingStageParking_260430` |
| MagnaDDS SDK（仅用于 IDL 生成） | `C:\Users\jasezhan\Desktop\00_git\20260718\02_middleware\DDS\MagnaDDS-SDK-v0.0.4` |
| 对照工程（差异分析用） | `C:\Users\jasezhan\Downloads\feature_integration_workspace_without_TempAPA_Code_20260801.tar\...` |

### 0.5 回归验证分层定义

| 层级 | 名称 | 触发条件 | 验证内容 |
|---|---|---|---|
| L1 | 编译回归 | 每阶段必须 | m57 + x86 release 编译通过，无新增 `-Werror` 警告 |
| L2 | x86 smoke 回归 | Phase 1 起，行为路径变更时 | runner + mock pub/sub 最小闭环；flag OFF 时输出与前一阶段完全一致；flag ON 时不崩溃且输出合法 |
| L3 | m57 板端回归 | 需 m57 硬件 | DDS discovery + topic 收发验证；当前阻塞：`BLOCKED_NO_M57_BOARD` |

---

## 1. 总体目标与原则

### 1.1 目标
在不替换当前主干架构的前提下，按"协议层 → 组件层 → 算法层"的顺序，分阶段吸收对照工程优势能力，实现：
1. 可灰度接入
2. 可回滚
3. 可暂停和可交接
4. 每阶段有明确交付物与验收门槛

### 1.2 核心原则
1. **禁止全量替换** `valet_parking_component.cpp` 与 `valet_parking_stage_parking_adapter.cpp`
2. 仅做"增量接入 + 开关化"
3. 每阶段结束必须提交：
   - 本地「项目状态快照」Markdown（带序号）
   - 阶段结果文档（完成项 / 未完成项 / 下一步）
   - application git 分支提交到 `DeepSeeK_V4_pro_20260731`
4. 未通过验收不得进入下一阶段

---

## 2. 阶段划分（Phase Gate）

### Phase 0：基线冻结与交接治理

#### 输入
- 当前工程基线代码（workspace 当前 HEAD）
- 对照工程差异分析结论
- 本计划书 v2.0

#### 输出
- `22_基线冻结阶段报告_phase_0.md`
- `23_基线冻结快照_phase_0.md`
- 基线 commit（分支：`DeepSeeK_V4_pro_20260731`）

#### 工作内容
1. 冻结当前基线版本与关键文件清单
2. 记录对比结论与"禁止动作"（整替 component/adapter）
3. 建立阶段文档编号规则

#### 验收标准
- 基线 commit 可复现
- 阶段文档齐全且路径固定
- L1 编译回归：Docker 内 `./build_app.sh --build=x86 --mode=release` 通过
- L1 编译回归：Docker 内 `./build_app.sh --build=m57 --mode=release` 通过
- 用户确认"进入 Phase 1"

#### 风险点
- 风险：基线未冻结导致后续 diff 污染
- 缓解：Phase 0 结束后仅允许按计划清单修改

#### 交付物
- 基线冻结阶段报告（`22_基线冻结阶段报告_phase_0.md`）
- 快照（`23_基线冻结快照_phase_0.md`）
- diff 摘要 + commit hash

---

### Phase 1：IDL 模块化兼容接入（低风险）

#### 输入
- 当前 `idl/valet_parking_topics.idl`
- 对照 `idl/topics/*.idl` 模块化组织方式

#### 输出
- 兼容式 IDL 组织（保留向后兼容）
- `24_IDL兼容接入阶段报告_phase_1.md`
- `25_IDL兼容接入快照_phase_1.md`
- Phase 1 commit（目标分支）

#### 工作内容
1. 引入模块化 IDL 结构（聚合入口 + 子 topic 拆分）
2. 保留现有 topic 的兼容映射，不破坏当前消费者
3. 生成/校验相关 generated 代码

#### 验收标准
- L1 编译回归：`./build_app.sh --build=x86 --mode=release` 通过，无新增 `-Werror` 警告
- L1 编译回归：`./build_app.sh --build=m57 --mode=release` 通过
- L2 smoke 回归：现有 topic 消费端（`/planning/trajectory`）行为无变化
- 新增 topic 处于"可选未启用"状态（无 feature flag 时不编译对应代码）
- 用户确认"进入 Phase 2"

#### 风险点
- 风险：IDL 变更引发序列化兼容问题
- 缓解：保留原字段与名称，做双向兼容验证

#### 交付物
- IDL 变更阶段报告（`24_IDL兼容接入阶段报告_phase_1.md`）
- 快照（`25_IDL兼容接入快照_phase_1.md`）
- diff 摘要（文件级 + 接口级）

---

### Phase 2：组件层可选输入支路接入（中风险）

#### 输入
- `src/valet_parking_component.cpp`
- Phase 1 IDL 兼容成果

#### 输出
- 可开关输入支路（如 `PrkVinBus`/FKM/坐标转换）
- `26_组件可选支路阶段报告_phase_2.md`
- `27_组件可选支路快照_phase_2.md`
- Phase 2 commit（目标分支）

#### 工作内容
1. 新输入支路通过 feature flag 接入（flag：`ENABLE_PRKVINBUS`）
2. 默认关闭，保证关闭时"零行为变化"
3. 增加输入合法性与输出保护（如 waypoint 上限防护）

#### 验收标准
- L1 编译回归：`./build_app.sh --build=x86 --mode=release` 通过（flag 关闭 + 开启均通过）
- L1 编译回归：`./build_app.sh --build=m57 --mode=release` 通过
- L2 smoke 回归（flag 关闭）：编译后运行 runner + mock pub/sub，输出与 Phase 1 完全一致
- L2 smoke 回归（flag 开启）：`./build_app.sh --build=x86 --features=ENABLE_PRKVINBUS` 编译后运行，新链路不崩溃且输出合法
- 用户确认"进入 Phase 3"

#### 风险点
- 风险：坐标系转换误差影响轨迹
- 缓解：加入转换一致性检查与日志诊断字段

#### 交付物
- 组件改造阶段报告（`26_组件可选支路阶段报告_phase_2.md`）
- 快照（`27_组件可选支路快照_phase_2.md`）
- diff 摘要（逻辑路径 + 开关说明）

---

### Phase 3：算法增强灰度接入（中高风险）

#### 输入
- `CMakeLists.txt`
- `nlp_path_smoother_lite.*` / `scs_shape_path.*` / `penalty_function_method.*`（来自对照工程）
- **前置条件（阻塞级）**：tempapa 源码/二进制已确认可用。对照工程目录名为 `without_TempAPA_Code`，仅保留 CMake 集成引用（`cmake/tempapa_thirdparty.cmake` + `thirdparty::tempapa`），实际 tempapa 源码极可能缺失。本 Phase 启动前必须先从 `C:\Users\jasezhan\Desktop\00_git\APA\Local_task\TempAPA_Code` 原始工程确认 tempapa 来源，否则本阶段标记为 `BLOCKED_TEMPAPA_MISSING` 并暂停。

#### 输出
- 算法与依赖开关化接入（默认关闭，flag：`ENABLE_NLP_SMOOTHER`）
- `28_算法灰度阶段报告_phase_3.md`
- `29_算法灰度快照_phase_3.md`
- Phase 3 commit（目标分支）

#### 工作内容
1. 确认 tempapa 可用性（从 `TempAPA_Code` 提取或标注为阻塞）
2. 依赖接入（含 tempapa）与 CMake 开关（`ENABLE_NLP_SMOOTHER`）
3. 新算法路径灰度开关
4. A/B 结果对比（时延、可行率、平滑性）

#### 验收标准
- L1 编译回归：`./build_app.sh --build=x86 --mode=release` 通过（flag 关闭，即默认）
- L1 编译回归：`./build_app.sh --build=x86 --features=ENABLE_NLP_SMOOTHER` 通过（flag 开启）
- L1 编译回归：`./build_app.sh --build=m57 --features=ENABLE_NLP_SMOOTHER` 通过
- L2 smoke 回归：flag 关闭时输出与 Phase 2 完全一致
- L2 smoke 回归：flag 开启时 NLP smoother 输出轨迹合法且指标记录齐全
- 用户确认"进入 Phase 4"

#### 风险点
- **阻塞级风险**：对照工程不包含 tempapa 源码（目录名 `without_TempAPA_Code`），仅保留 CMake 集成引用。Phase 3 启动前必须先从原始 `TempAPA_Code` 工程或其他渠道确认 tempapa 来源，否则本阶段标记为 `BLOCKED_TEMPAPA_MISSING` 并暂停。
- 风险：依赖版本或数值不稳定导致退化
- 缓解：保留原路径作为 fallback，失败可一键回滚（关闭 `ENABLE_NLP_SMOOTHER` flag 重新编译）

#### 交付物
- 算法灰度阶段报告（`28_算法灰度阶段报告_phase_3.md`）
- 快照（`29_算法灰度快照_phase_3.md`）
- diff 摘要（构建项 + 运行策略）

---

### Phase 4：收敛验收与交接包

#### 输入
- Phase 0~3 全部交付物

#### 输出
- `30_最终验收阶段报告_phase_4.md`
- `31_最终验收快照_phase_4.md`
- 最终 commit（目标分支）

#### 工作内容
1. 汇总阶段成果与遗留项
2. 形成可交接包（文档、验证记录、风险清单）
3. 给出下一轮迭代建议

#### 验收标准
- L1 编译回归：`./build_app.sh --build=x86 --mode=release` + `./build_app.sh --build=m57 --mode=release` 均通过
- 所有阶段文档可追溯（22~31 编号连续）
- 每阶段 diff 摘要完整
- 可在任意阶段中断并由新设备续跑（按第 3 节交接流程验证）
- 用户确认"项目收口完成"

#### 风险点
- 风险：状态只在聊天记录中导致断点丢失
- 缓解：强制本地快照 + 阶段报告 + 分支提交

#### 交付物
- 最终验收阶段报告（`30_最终验收阶段报告_phase_4.md`）
- 快照（`31_最终验收快照_phase_4.md`）
- 总体 diff 摘要

---

## 3. 中断/换电脑无缝交接机制

1. **状态唯一真源 = 本地文档 + Git 分支**，聊天仅辅助说明。
2. 每阶段结束必须更新（路径统一在 `applications/source/Docs/valet_parking_magnadds/`）：
   - 最新状态快照（按 `NN_描述_phase_Y.md` 命名）
   - 阶段报告（按 `NN_描述_phase_Y.md` 命名）
3. 每阶段必须提交到 `DeepSeeK_V4_pro_20260731`，并在文档写入：
   - commit hash
   - 本阶段修改文件列表
   - diff 摘要
4. 新设备接手流程：
   - 拉取 `DeepSeeK_V4_pro_20260731`
   - 进入 `applications/source/Docs/valet_parking_magnadds/`
   - 读取 `STATUS.yaml` → 打开最新编号快照 → 从"下一步动作"继续

---

## 4. 阶段暂停与审批闸门

每阶段结束后执行"暂停点"流程：
1. 产出本阶段文档与 snapshot
2. 提交分支并生成 diff 摘要
3. 向用户汇报"完成内容 / 风险 / 下一步"
4. **等待用户明确同意后**才进入下一阶段

审批口令建议（示例）：
- "同意进入 Phase 1"
- "同意进入 Phase 2"
- ...

---

## 5. 文档与命名规范

### 5.1 命名规则（对齐历史惯例）

所有文档统一放在 `applications/source/Docs/valet_parking_magnadds/`，采用 `NN_描述.md` 格式：
- `NN`：两位递增序号（22, 23, 24, ...），续接现有 00~21
- `描述`：中文简短描述，含阶段号以便识别

示例：
- Phase 0 阶段报告：`22_基线冻结阶段报告_phase_0.md`
- Phase 0 快照：`23_基线冻结快照_phase_0.md`
- Phase 1 阶段报告：`24_IDL兼容接入阶段报告_phase_1.md`
- Phase 1 快照：`25_IDL兼容接入快照_phase_1.md`

### 5.2 最低必填字段（每份阶段产出文档）
- 本阶段目标
- 已完成事项（带文件路径）
- 未完成事项
- 风险与阻塞
- 验收结果（含具体编译命令及结果）
- 分支/commit hash
- diff 摘要
- 下一步动作（不得跨阶段）

---

## 6. 回滚策略

每个 Phase 必须支持独立回滚。回滚目标为上一 Phase 的最终 commit。

| 回滚场景 | 操作 |
|---|---|
| Phase N 引入回归 | `git revert <Phase N commit>` 或 `git reset --hard <Phase N-1 commit>` |
| Feature flag 开启后不稳定 | 关闭 flag 重新编译：去掉 `--features=ENABLE_XXX`，行为应退回 Phase N-1 |
| 需完全回退到基线 | `git reset --hard <Phase 0 baseline commit>` |

### 回滚验证步骤
1. 回滚到目标 commit
2. Docker 内执行 `./build_app.sh --build=x86 --mode=release --clean`
3. 运行 L2 smoke 回归，确认输出与目标 Phase 一致
4. 在快照中记录回滚操作和原因

---

## 7. 执行边界（防偏离）

1. 未经审批不得跨阶段
2. 非计划文件修改需在阶段报告中说明原因
3. 禁止引入"不可回滚"变更
4. 所有新增能力必须可开关

---

## 8. 你确认后的启动动作

收到你确认后，将执行：
1. 进入 Phase 0
2. 创建 `22_基线冻结阶段报告_phase_0.md`
3. 创建 `23_基线冻结快照_phase_0.md`
4. 提交到 `DeepSeeK_V4_pro_20260731` 并输出 diff 摘要
5. **等待你批准后才进入 Phase 1**
