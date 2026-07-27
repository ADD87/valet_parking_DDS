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
 * @brief Defines the Polygon2d class.
 */

#pragma once

#include <string>
#include <vector>

#include "common/math/aabox2d.h"
#include "common/math/box2d.h"
#include "common/math/line_segment2d.h"
#include "common/math/vec2d.h"
#include "common/geometry_types.h"

namespace TL {
namespace common {
namespace math {

// Forward declaration
class AABox2d;

/**
 * @class Polygon2d
 * @brief 2-dimensional polygon
 */
class Polygon2d {
 public:
  //! Default constructor
  Polygon2d() = default;

  //! Constructor from Box2d
  explicit Polygon2d(const Box2d& box);

  //! Constructor from points
  explicit Polygon2d(std::vector<Vec2d> points);

  //! Getter for points
  const std::vector<Vec2d>& points() const { return points_; }

  //! Getter for line segments
  const std::vector<LineSegment2d>& line_segments() const {
    return line_segments_;
  }

  //! Getter for number of points
  int num_points() const { return num_points_; }

  //! Checks if the polygon is convex
  bool is_convex() const { return is_convex_; }

  //! Getter for area
  double area() const { return area_; }

  //! Getter for minimum x
  double min_x() const { return min_x_; }

  //! Getter for maximum x
  double max_x() const { return max_x_; }

  //! Getter for minimum y
  double min_y() const { return min_y_; }

  //! Getter for maximum y
  double max_y() const { return max_y_; }

  //! Computes the distance to a point
  double DistanceTo(const Vec2d& point) const;

  //! Computes the squared distance to a point
  double DistanceSquareTo(const Vec2d& point) const;

  //! Computes the distance to a line segment
  double DistanceTo(const LineSegment2d& line_segment) const;

  //! Computes the distance to a box
  double DistanceTo(const Box2d& box) const;

  //! Computes the distance to another polygon
  double DistanceTo(const Polygon2d& polygon) const;

  //! Computes the distance to the boundary
  double DistanceToBoundary(const Vec2d& point) const;

  //! Checks if a point is on the boundary
  bool IsPointOnBoundary(const Vec2d& point) const;

  //! Checks if a point is inside the polygon
  bool IsPointIn(const Vec2d& point) const;

  //! Checks if this polygon overlaps with another polygon
  bool HasOverlap(const Polygon2d& polygon) const;

  //! Checks if this polygon overlaps with a line segment
  bool HasOverlap(const LineSegment2d& line_segment) const;

  //! Gets the overlap line segments with another line segment
  bool GetOverlap(const LineSegment2d& line_segment,
                  std::vector<LineSegment2d>* const overlap_segments) const;

  //! Gets all overlap line segments with another line segment
  std::vector<LineSegment2d> GetAllOverlaps(const LineSegment2d& line_segment) const;

  //! Checks if this polygon contains a line segment
  bool Contains(const LineSegment2d& line_segment) const;

  //! Checks if this polygon contains another polygon
  bool Contains(const Polygon2d& polygon) const;

  //! Gets the axis-aligned bounding box
  AABox2d GetAABox() const;

  //! Gets the next point index
  int Next(int at) const;

  //! Gets the previous point index
  int Prev(int at) const;

  //! Builds the polygon from points
  void BuildFromPoints();

  //! Computes the convex hull
  static bool ComputeConvexHull(const std::vector<Vec2d>& points,
                                Polygon2d* const polygon);

  //! Checks if a polygon is convex
  static bool IsConvexPolygon(const std::vector<Vec2d>& points);

  //! Clips a convex hull with a line segment
  static bool ClipConvexHull(const LineSegment2d& line_segment,
                             std::vector<Vec2d>* const points);

  //! Computes the overlap between two polygons
  bool ComputeOverlap(const Polygon2d& other_polygon,
                      Polygon2d* const overlap_polygon) const;

  //! Computes the intersection over union (IoU)
  double ComputeIoU(const Polygon2d& other_polygon) const;

  //! Expands the polygon by a given distance
  Polygon2d ExpandByDistance(double distance) const;

  //! Returns a human-readable string for debugging
  std::string DebugString() const;

 protected:
  std::vector<Vec2d> points_;
  std::vector<LineSegment2d> line_segments_;
  int num_points_ = 0;
  bool is_convex_ = false;
  bool is_valid_ = true;
  double area_ = 0.0;
  double min_x_ = 0.0;
  double max_x_ = 0.0;
  double min_y_ = 0.0;
  double max_y_ = 0.0;
};

//! Removes duplicate points from a vector
void RemoveDuplicates(std::vector<Vec2d>* points);

//! Converts a protobuf Polygon to Polygon2d
// TODO: Temporarily commented out due to proto Polygon dependency
// Polygon2d ConvertToPolygon2d(const common::Polygon& polygon);

}  // namespace math
}  // namespace common
}  // namespace TL
