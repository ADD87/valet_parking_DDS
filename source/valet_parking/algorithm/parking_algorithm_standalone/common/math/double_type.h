/******************************************************************************
 * Copyright 2017 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

/**
 * @file
 * @brief Defines double type comparison utilities.
 */

#pragma once

#include <cmath>

namespace TL {
namespace common {
namespace math {
namespace double_type {

//! Checks if a double is zero (within epsilon)
inline bool IsZero(double value, double epsilon = 1e-10) {
  return std::abs(value) < epsilon;
}

//! Compares two doubles for equality (within epsilon)
inline bool IsEqual(double a, double b, double epsilon = 1e-10) {
  return IsZero(a - b, epsilon);
}

//! Compares two doubles: returns -1, 0, or 1
inline int Compare(double a, double b, double epsilon = 1e-10) {
  if (IsEqual(a, b, epsilon)) {
    return 0;
  }
  return a < b ? -1 : 1;
}

//! Checks if a > b (definitely greater)
inline bool DefinitelyGreater(double a, double b, double epsilon = 1e-10) {
  return a - b > epsilon;
}

//! Checks if a < b (definitely less)
inline bool DefinitelyLess(double a, double b, double epsilon = 1e-10) {
  return b - a > epsilon;
}

//! Checks if a >= b (definitely greater or equal)
inline bool DefinitelyGreaterEqual(double a, double b, double epsilon = 1e-10) {
  return !DefinitelyLess(a, b, epsilon);
}

//! Checks if a <= b (definitely less or equal)
// 独立编译改造：补充 open_space_speed_optimizer 所需函数
inline bool DefinitelyLessEqual(double a, double b, double epsilon = 1e-10) {
  return !DefinitelyGreater(a, b, epsilon);
}

//! Checks if a ~= b (approximately equal, looser than IsEqual)
inline bool SeemsEqual(double a, double b, double epsilon = 1e-6) {
  return std::fabs(a - b) <= epsilon;
}

//! Checks if a != b (approximately not equal)
inline bool SeemsNotEqual(double a, double b, double epsilon = 1e-6) {
  return !SeemsEqual(a, b, epsilon);
}

}  // namespace double_type
}  // namespace math
}  // namespace common
}  // namespace TL
