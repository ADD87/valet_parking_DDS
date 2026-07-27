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
 * @brief Defines angle classes and trigonometric functions.
 */

#pragma once

#include <cmath>
#include <cstdint>

namespace TL {
namespace common {
namespace math {

/**
 * @class Angle16
 * @brief 16-bit angle representation
 */
class Angle16 {
 public:
  static constexpr int16_t RAW_PI_2 = 16384;
  static constexpr int16_t RAW_PI = 32768;
  static constexpr double RAW_TO_RAD = M_PI / RAW_PI;
  static constexpr double RAD_TO_RAW = RAW_PI / M_PI;

  Angle16() = default;
  explicit Angle16(int16_t value) : raw_(value) {}

  int16_t raw() const { return raw_; }
  double to_rad() const { return raw_ * RAW_TO_RAD; }

  static Angle16 from_rad(double rad) {
    return Angle16(static_cast<int16_t>(std::lround(rad * RAD_TO_RAW)));
  }

 private:
  int16_t raw_ = 0;
};

/**
 * @class Angle8
 * @brief 8-bit angle representation
 */
class Angle8 {
 public:
  Angle8() = default;
  explicit Angle8(int8_t value) : raw_(value) {}

  int8_t raw() const { return raw_; }

 private:
  int8_t raw_ = 0;
};

// Trigonometric functions for Angle16
float sin(Angle16 a);
float cos(Angle16 a);
float tan(Angle16 a);

// Trigonometric functions for Angle8
float sin(Angle8 a);
float cos(Angle8 a);
float tan(Angle8 a);

// Trigonometric functions for double
float sin(double rad);
float cos(double rad);

// atan2 function
float atan2(float y, float x);

}  // namespace math
}  // namespace common
}  // namespace TL
