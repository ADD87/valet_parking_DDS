/**
 * @file math_util_convert.h
 * @brief 独立编译改造：数学工具函数
 * @description 提供常用的数学工具函数，用于替代原有的 common::util 和 common::math 中的函数
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <limits>

namespace TL {
namespace common {
namespace util {

/**
 * @brief 将值限制在指定范围内
 * @param value 输入值
 * @param bound1 边界1
 * @param bound2 边界2
 * @return 限制后的值（在 [min(bound1, bound2), max(bound1, bound2)] 范围内）
 */
template <typename T>
inline T BoundedValue(T value, T bound1, T bound2) {
  T lower = std::min(bound1, bound2);
  T upper = std::max(bound1, bound2);
  return std::max(lower, std::min(upper, value));
}

/**
 * @brief 将值限制在 [lower, upper] 范围内（简化版）
 */
template <typename T>
inline T Clamp(T value, T lower, T upper) {
  return std::max(lower, std::min(upper, value));
}

}  // namespace util

namespace math {
namespace double_type {

/**
 * @brief 比较浮点数与零的大小关系
 * @param value 待比较的值
 * @param epsilon 精度阈值，默认为 1e-9
 * @return 返回值：> 0 表示 value > 0，< 0 表示 value < 0，= 0 表示 value ≈ 0
 */
inline int ComparedToZero(double value, double epsilon = 1e-9) {
  if (std::fabs(value) < epsilon) {
    return 0;  // 近似等于 0
  } else if (value > 0.0) {
    return 1;  // 大于 0
  } else {
    return -1; // 小于 0
  }
}

// IsZero 和 IsEqual 已在 common/math/double_type.h 中定义，此处不重复定义

}  // namespace double_type
}  // namespace math
}  // namespace common
}  // namespace TL
