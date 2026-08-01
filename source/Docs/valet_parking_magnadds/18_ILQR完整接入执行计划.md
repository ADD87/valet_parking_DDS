# ILQR 完整接入执行计划

- 日期：2026-07-31
- 状态：计划阶段
- 分支：DeepSeeK_V4_pro_20260731

---

## 0. 现状分析

### 0.1 已导入文件（在本地 standalone 中）

```
planning/open_space/
├── coarse_path_generator/
│   ├── geometric_path.cc/.h          ✅ 文件存在，MISSING scs_shape_path.h
│   ├── ilqr_path.cc/.h              ✅ 文件存在，有 API 差异
│   ├── geometry_path_generator.cc/.h ✅ 文件存在，MISSING geometry_path.h(shape/)
│   └── utils/
│       └── penalty_function_method.cc/.h ✅ 文件存在
│
├── ilqr_smoother/                    ✅ 全部 13 文件存在，include 全部 OK
│   ├── al_constrain.h
│   ├── al_cost.cc/.h
│   ├── al_ilqr.cc/.h
│   ├── al_ilqr_interface.cc/.h
│   ├── control_constrain.h
│   ├── endpose_constrain.h
│   ├── kinematic_model.cc/.h
│   ├── macros.h
│   └── obs_constrain.h
```

### 0.2 编译错误清单（共 3 类）

#### 类型 A：依赖链断裂 (2 个文件)
| 文件 | 缺失依赖 | 影响 |
|---|---|---|
| `geometric_path.h:20` | `shape/scs_shape_path.h` | FATAL, 编译终止 |
| `geometry_path_generator.h:18` | `shape/geometry_path.h` | FATAL, 编译终止 |

#### 类型 B：Proto-style API 未适配
| 文件 | 问题数 | 具体问题 |
|---|---|---|
| `ilqr_path.cc` | 11 处 | `.x()`×3, `.y()`×3, `.theta()`×3, `VehicleConfigHelper`×2, `FLAGS_`×2 |
| `geometry_path_generator.cc` | 12 处 | `.x()`×4, `.y()`×2, `.theta()`×6 |

#### 类型 C：构造函数签名不匹配
| 文件 | 问题 |
|---|---|
| `ilqr_path.cc:29` | `ILQR(WarmStartConfig)` 调用 `PathGenerator(WarmStartConfig)` 但本地是 `PathGenerator(HybridAStarConfig, VehicleParam)` |

### 0.3 关键发现：PathGenerator 构造函数相同！

原始 standalone 和本地 standalone 的 PathGenerator 构造函数**完全一致**：
```cpp
PathGenerator(const HybridAStarConfig& hybrid_a_star_config,
              const VehicleParam& vehicle_param)
```

但 ILQR 类试图用 `WarmStartConfig` 构造基类，这是一个设计差异。原始 TempAPA_Code 中的 PathGenerator 可能有不同的重载。

---

## 1. 执行步骤

### Step 1: 移除 shape/ 依赖链 (geometric_path)

**文件**: `geometric_path.h`, `geometric_path.cc`

**策略**: `geometric_path` 的 `#include "scs_shape_path.h"` 仅用于 SCS 辅助函数。本地 standalone 不需要 SCS 路径生成。删除该 include 并移除对 SCS 的引用。

**文件**: `geometry_path_generator.h`, `geometry_path_generator.cc`

**策略**: `#include "shape/geometry_path.h"` 用于 Geometry 几何计算。需要将 shape/geometry_path 的函数直接内联或使用本地 equivalent。

### Step 2: 修复 Proto-style API (ilqr_path.cc, geometry_path_generator.cc)

**批量替换**:
- `start_point.x()` → `start_point.x`
- `start_point.y()` → `start_point.y`
- `start_point.theta()` → `start_point.theta`
- `end_point.x()` → `end_point.x`
- `end_point.y()` → `end_point.y`
- `end_point.theta()` → `end_point.theta`

### Step 3: 修复 PathGenerator 构造 (ilqr_path.cc)

**问题**: `ILQR` 继承 `PathGenerator` 但构造参数不匹配

**方案**: 
- ILQR 构造函数改为 `ILQR(const HybridAStarConfig&, const VehicleParam&, const WarmStartConfig&)`
- 或者 ILQR 不继承 PathGenerator，改为持有 PathGenerator 实例

### Step 4: 替换 FLAGS 和 VehicleConfigHelper (ilqr_path.cc)

**`FLAGS_avp_ego_inflated_buffer_for_checking_collision`**:
→ 硬编码为合理默认值（如 0.3m），或添加到 WarmStartConfig

**`VehicleConfigHelper::GetConfig()`**:
→ 使用本地 `vehicle_config_helper_simple.cc` 的等效函数
→ 或从 `VehicleParam` 结构直接获取 `back_edge_to_center` 等

### Step 5: CMakeLists.txt 更新

从编译清单移除 shape/文件（已不需要），保留 ILQR + geometric 文件。

---

## 2. 风险点

| 风险 | 缓解 |
|---|---|
| shape/ 移除后 geometric_path 功能缺失 | geometric_path 的核心逻辑不依赖 SCS，移除不影响 |
| ILQR 构造器改动影响 ilqr_smoother | ilqr_smoother 通过 ALILQR_INTERFACE 间接使用，不直接调用 ILQR 构造器 |
| VehicleConfigHelper 替换后行为不一致 | 对照原始 TempAPA_Code 验证数值 |

---

## 3. 验收标准

- [ ] `geometric_path.*` 编译通过
- [ ] `ilqr_path.*` 编译通过
- [ ] `geometry_path_generator.*` 编译通过
- [ ] `ilqr_smoother/*` 所有文件编译通过
- [ ] x86 `libvalet_parking.so` 生成成功
- [ ] 不影响已有链路（ROI→PathProvider→PathPartition→SpeedOptimizer）
