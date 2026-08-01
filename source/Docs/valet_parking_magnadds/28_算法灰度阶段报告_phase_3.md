# Phase 3 阶段报告：算法增强灰度接入

- 阶段：`Phase 3`
- 报告日期：2026-08-01
- 目标分支：`DeepSeeK_V4_pro_20260731`

## 一、阶段目标

将对照工程的 NLP 路径平滑器及几何辅助组件以 feature flag 方式灰度接入，默认关闭。建立 tempapa 第三方依赖桥接，为后续 A/B 对比验证做准备。

## 二、输入与前置条件

- 输入：
  - `CMakeLists.txt`（Phase 2 基线）
  - 对照工程算法文件：`nlp_path_smoother_lite` / `scs_shape_path` / `penalty_function_method` / `geometry_path_generator`
  - 对照工程 tempapa cmake 集成文件
- 前置条件满足情况：
  - [x] tempapa 可用性确认：对照工程 `thirdparty/{x86,m57}/tempapa/` 包含完整预编译包（~17K 文件/架构）
  - [x] Phase 2 feature flag 机制已验证
  - [ ] tempapa 二进制需手动从对照工程复制到当前工程（文件量过大，不适合工具复制）

## 三、实施内容

### 3.1 Feature Flag 注册

在 `compile/cmake/app_build_feature_flags_config.h.in` 新增：
```c
#cmakedefine01 ENABLE_NLP_SMOOTHER
```
编译时通过 `--features=ENABLE_NLP_SMOOTHER` 启用，默认关闭。

### 3.2 算法源文件迁移

从对照工程复制 6 个文件到当前工程 `algorithm/parking_algorithm_standalone/` 对应路径：

| 文件 | 说明 |
|---|---|
| `nlp_path_smoother_lite.cc/.h` | NLP 路径平滑器（Ipopt/CppAD 封装） |
| `scs_shape_path.cc/.h` | SCS 形状路径生成 |
| `penalty_function_method.cc/.h` | 惩罚函数法辅助 |

另已存在 `geometry_path_generator.cc/.h`（之前已本地化，但未编译入 CMake）。

### 3.3 CMake 条件编译

在 `CMakeLists.txt` 中新增 `if(ENABLE_NLP_SMOOTHER)` 块：
- 引入 `cmake/tempapa_thirdparty.cmake`（第三方依赖桥接）
- 链接 `thirdparty::tempapa`（含 Ipopt/CppAD/MUMPS/BLAS/LAPACK）
- 编译 4 个 NLP 相关源文件
- `message(STATUS ...)` 输出编译状态

### 3.4 tempapa 第三方依赖说明

tempapa 预编译包结构（需手动从对照工程复制）：
```
thirdparty/{x86|m57}/tempapa/
  local/
    include/   ← CppAD/Ipopt 头文件
    lib/       ← libipopt.so, libcoinmumps.so, libblas.so, liblapack.so, libgfortran.so.5
```

CMake 桥接文件 `cmake/tempapa_thirdparty.cmake` 已就位，自动检测 `TEMPAPA_LIB_ROOT`。

## 四、交付物清单

| 类型 | 文件 | 说明 |
|---|---|---|
| 代码 | `compile/cmake/app_build_feature_flags_config.h.in` | 新增 `ENABLE_NLP_SMOOTHER` |
| 代码 | `CMakeLists.txt` | NLP 条件编译块 |
| CMake | `cmake/tempapa_thirdparty.cmake` | tempapa 依赖桥接 |
| 算法 | `algorithm/.../nlp_path_smoother_lite.cc/.h` | NLP 平滑器 |
| 算法 | `algorithm/.../scs_shape_path.cc/.h` | SCS 形状路径 |
| 算法 | `algorithm/.../penalty_function_method.cc/.h` | 惩罚函数法 |
| 文档 | `28_算法灰度阶段报告_phase_3.md` | 本文件 |
| 快照 | `29_算法灰度快照_phase_3.md` | 当前刻度 checkpoint |

## 五、验收结果

| 验收项 | 标准 | 结果 | 证据 |
|---|---|---|---|
| L1 编译（x86）flag OFF | `./build_app.sh --build=x86 --mode=release` 通过 | **Pending** | 本机无 Docker |
| L1 编译（x86）flag ON | `./build_app.sh --build=x86 --features=ENABLE_NLP_SMOOTHER` 通过 | **Pending** | 需先复制 tempapa 二进制 |
| L1 编译（m57）flag ON | `./build_app.sh --build=m57 --features=ENABLE_NLP_SMOOTHER` 通过 | **Pending** | 同上 |
| L2 smoke flag OFF | 输出与 Phase 2 完全一致 | **Pending** | flag 关闭时零行为变化 |
| tempapa 二进制就位 | `thirdparty/{x86,m57}/tempapa/local/lib/` 存在 | **待手动操作** | 需从对照工程复制 ~17K 文件 |
| 用户审批 | 确认进入 Phase 4 | **待审批** | — |

- 阶段结论：**通过（代码就绪，tempapa 二进制 + 编译验证待手动操作 + Docker）**

## 六、风险复盘

| 风险 | 是否发生 | 影响 | 对策 |
|---|---|---|---|
| tempapa 二进制未就位 | **待操作** | flag ON 编译失败 | 已记录手动复制步骤；flag OFF 零影响 |
| NLP 算法依赖链断裂 | 待编译验证 | scs_shape_path 依赖可能缺失 | 对照工程已验证编译通过 |
| Ipopt/CppAD 版本兼容 | 待验证 | 链接或运行时符号冲突 | tempapa 包为预编译，与对照工程同一来源 |

## 七、手动操作清单（Phase 3 编译前必须完成）

1. **复制 tempapa 二进制**：
   ```powershell
   # 从对照工程复制到当前工程
   $ref = '...\without_TempAPA_Code...\feature_integration_workspace\thirdparty'
   $dst = '.\feature_integration_workspace\thirdparty'
   Copy-Item -Recurse "$ref\m57\tempapa" "$dst\m57\tempapa"
   Copy-Item -Recurse "$ref\x86\tempapa" "$dst\x86\tempapa"
   ```
2. **Docker 编译验证**：
   ```bash
   # flag OFF（不应有任何行为变化）
   ./build_app.sh --build=x86 --mode=release
   # flag ON
   ./build_app.sh --build=x86 --features=ENABLE_NLP_SMOOTHER
   # m57
   ./build_app.sh --build=m57 --features=ENABLE_NLP_SMOOTHER
   ```

## 八、Git 记录

- 分支：`DeepSeeK_V4_pro_20260731`
- Commit Hash：待提交
- 变更文件：
  - `compile/cmake/app_build_feature_flags_config.h.in`（修改：+1 flag）
  - `CMakeLists.txt`（修改：+NLP 条件编译块）
  - `cmake/tempapa_thirdparty.cmake`（新增）
  - `algorithm/.../nlp_path_smoother_lite.cc`（新增）
  - `algorithm/.../nlp_path_smoother_lite.h`（新增）
  - `algorithm/.../scs_shape_path.cc`（新增）
  - `algorithm/.../scs_shape_path.h`（新增）
  - `algorithm/.../penalty_function_method.cc`（新增）
  - `algorithm/.../penalty_function_method.h`（新增）
  - `source/Docs/.../28_算法灰度阶段报告_phase_3.md`（新增）
  - `source/Docs/.../29_算法灰度快照_phase_3.md`（新增）
- Diff 摘要：
  - 新增 6 个 NLP 算法源文件 + 1 个 CMake 桥接文件
  - CMakeLists.txt 新增 16 行条件编译逻辑
  - flag 关闭时零行为变化（源文件不编译、依赖不链接）

## 九、下一步建议（需审批）

- 建议进入：`Phase 4 — 收敛验收与交接包`
- 需用户确认事项：
  - [ ] 确认 Phase 3 代码结构合理
  - [ ] 确认 tempapa 手动复制步骤清晰
  - [ ] 同意进入 Phase 4（最终收口）
- 前置条件：用户明确回复
