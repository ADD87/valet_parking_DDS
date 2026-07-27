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
 * @brief Defines the Box2d class.
 */

#pragma once

#include <string>
#include <vector>

#include "common/file/log.h"  // Must be before other includes for LOG macro
#include "common/math/aabox2d.h"
#include "common/math/line_segment2d.h"
#include "common/math/vec2d.h"

namespace TL {
namespace common {
namespace math {

/**
 * @class Box2d
 * @brief Oriented 2-dimensional box
 */
class Box2d {
 public:
  //! Default constructor
  Box2d() = default;

  //! Constructor with center, heading, length, and width
  Box2d(const Vec2d& center, const double heading, const double length,
        const double width);

  //! Constructor from an axis and width
  Box2d(const LineSegment2d& axis, const double width);

  //! Constructor from AABox2d
  Box2d(const AABox2d& aabox);

  //! Initializes the corners
  void InitCorners();

  //! Creates an axis-aligned box
  static Box2d CreateAABox(const Vec2d& one_corner,
                           const Vec2d& opposite_corner);

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

  //! Getter for heading
  double heading() const { return heading_; }

  //! Getter for cos(heading)
  double cos_heading() const { return cos_heading_; }

  //! Getter for sin(heading)
  double sin_heading() const { return sin_heading_; }

  //! Getter for area
  double area() const { return length_ * width_; }

  //! Getter for corners
  const std::vector<Vec2d>& corners() const { return corners_; }

  //! Getter for minimum x
  double min_x() const { return min_x_; }

  //! Getter for maximum x
  double max_x() const { return max_x_; }

  //! Getter for minimum y
  double min_y() const { return min_y_; }

  //! Getter for maximum y
  double max_y() const { return max_y_; }

  //! Gets all four corners
  void GetAllCorners(std::vector<Vec2d>* const corners) const;

  //! Checks if a point is inside the box
  bool IsPointIn(const Vec2d& point) const;

  //! Checks if a point is on the boundary
  bool IsPointOnBoundary(const Vec2d& point) const;

  //! Computes the distance to a point
  double DistanceTo(const Vec2d& point) const;

  //! Checks if this box overlaps with a line segment
  bool HasOverlap(const LineSegment2d& line_segment) const;

  //! Computes the distance to a line segment
  double DistanceTo(const LineSegment2d& line_segment) const;

  //! Computes the distance to another box
  double DistanceTo(const Box2d& box) const;

  //! Checks if this box overlaps with another box
  bool HasOverlap(const Box2d& box) const;

  //! Gets the axis-aligned bounding box
  AABox2d GetAABox() const;

  //! Rotates the box from its center
  void RotateFromCenter(const double rotate_angle);

  //! Shifts the box by a vector
  void Shift(const Vec2d& shift_vec);

  //! Extends the box longitudinally
  void LongitudinalExtend(const double extension_length);

  //! Extends the box laterally
  void LateralExtend(const double extension_length);

  //! Returns a human-readable string for debugging
  std::string DebugString() const;

 protected:
  Vec2d center_;
  double length_ = 0.0;
  double width_ = 0.0;
  double half_length_ = 0.0;
  double half_width_ = 0.0;
  double heading_ = 0.0;
  double cos_heading_ = 1.0;
  double sin_heading_ = 0.0;
  std::vector<Vec2d> corners_;
  double max_x_ = std::numeric_limits<double>::lowest();
  double min_x_ = std::numeric_limits<double>::max();
  double max_y_ = std::numeric_limits<double>::lowest();
  double min_y_ = std::numeric_limits<double>::max();
};

}  // namespace math
}  // namespace common
}  // namespace TL
