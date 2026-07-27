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
 * @brief Defines the AABox2d class (Axis-Aligned Box).
 */

#pragma once

#include <string>
#include <vector>

#include "common/file/log.h"  // For LOG macro
#include "common/math/vec2d.h"

namespace TL {
namespace common {
namespace math {

/**
 * @class AABox2d
 * @brief Axis-aligned 2-dimensional box
 */
class AABox2d {
 public:
  //! Default constructor
  AABox2d() = default;

  //! Constructor with center, length, and width
  AABox2d(const Vec2d& center, const double length, const double width);

  //! Constructor with two opposite corners
  AABox2d(const Vec2d& one_corner, const Vec2d& opposite_corner);

  //! Constructor from a vector of points
  explicit AABox2d(const std::vector<Vec2d>& points);

  //! Getter for center
  const Vec2d& center() const { return center_; }

  //! Getter for center x-coordinate
  double center_x() const { return center_.x(); }

  //! Getter for center y-coordinate
  double center_y() const { return center_.y(); }

  //! Getter for length
  double length() const { return length_; }

  //! Getter for width
  double width() const { return width_; }

  //! Getter for half length
  double half_length() const { return half_length_; }

  //! Getter for half width
  double half_width() const { return half_width_; }

  //! Getter for area
  double area() const { return length_ * width_; }

  //! Getter for minimum x
  double min_x() const { return center_.x() - half_length_; }

  //! Getter for maximum x
  double max_x() const { return center_.x() + half_length_; }

  //! Getter for minimum y
  double min_y() const { return center_.y() - half_width_; }

  //! Getter for maximum y
  double max_y() const { return center_.y() + half_width_; }

  //! Gets all four corners
  void GetAllCorners(std::vector<Vec2d>* const corners) const;

  //! Checks if a point is inside the box
  bool IsPointIn(const Vec2d& point) const;

  //! Checks if a point is on the boundary
  bool IsPointOnBoundary(const Vec2d& point) const;

  //! Computes the distance to a point
  double DistanceTo(const Vec2d& point) const;

  //! Computes the distance to another box
  double DistanceTo(const AABox2d& box) const;

  //! Checks if this box overlaps with another box
  bool HasOverlap(const AABox2d& box) const;

  //! Shifts the box by a vector
  void Shift(const Vec2d& shift_vec);

  //! Merges with another box
  void MergeFrom(const AABox2d& other_box);

  //! Merges with a point
  void MergeFrom(const Vec2d& other_point);

  //! Returns a human-readable string for debugging
  std::string DebugString() const;

 protected:
  Vec2d center_;
  double length_ = 0.0;
  double width_ = 0.0;
  double half_length_ = 0.0;
  double half_width_ = 0.0;
};

}  // namespace math
}  // namespace common
}  // namespace TL
