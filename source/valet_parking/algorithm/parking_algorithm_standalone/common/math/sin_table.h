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
 * @brief Defines sine lookup table for fast trigonometric calculations.
 */

#pragma once

#include <cmath>

namespace TL {
namespace common {
namespace math {

constexpr int SIN_TABLE_SIZE = 16384;

// Pre-computed sine table for angles (16385 entries: 0 to 16384 inclusive)
extern const float SIN_TABLE[16385];

}  // namespace math
}  // namespace common
}  // namespace TL
