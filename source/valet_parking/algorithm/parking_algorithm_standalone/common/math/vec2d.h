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
 * @brief Defines the Vec2d class.
 */

#pragma once

#include <cmath>
#include <string>

#include "common/file/log.h"  // For LOG macro

namespace TL {
namespace common {
namespace math {

constexpr double kMathEpsilon = 1e-10;

/**
 * @class Vec2d
 * @brief 2-dimensional vector class
 */
class Vec2d {
 public:
  constexpr Vec2d(const double x, const double y) noexcept : x_(x), y_(y) {}
  constexpr Vec2d() noexcept : Vec2d(0.0, 0.0) {}

  static Vec2d CreateUnitVec2d(const double angle);

  double x() const { return x_; }
  double y() const { return y_; }
  void set_x(const double x) { x_ = x; }
  void set_y(const double y) { y_ = y; }

  double Length() const;
  double LengthSquare() const;
  double Angle() const;
  void Normalize();
  Vec2d rotate(const double angle) const;
  void SelfRotate(const double angle);
  double CrossProd(const Vec2d& other) const;
  double InnerProd(const Vec2d& other) const;
  double DistanceTo(const Vec2d& other) const;
  double DistanceSquareTo(const Vec2d& other) const;

  Vec2d operator+(const Vec2d& other) const;
  Vec2d operator-(const Vec2d& other) const;
  Vec2d operator*(const double ratio) const;
  Vec2d operator/(const double ratio) const;
  Vec2d& operator+=(const Vec2d& other);
  Vec2d& operator-=(const Vec2d& other);
  Vec2d& operator*=(const double ratio);
  Vec2d& operator/=(const double ratio);
  bool operator==(const Vec2d& other) const;

  std::string DebugString() const;

 protected:
  double x_ = 0.0;
  double y_ = 0.0;
};

Vec2d operator*(const double ratio, const Vec2d& vec);

}  // namespace math
}  // namespace common
}  // namespace TL
