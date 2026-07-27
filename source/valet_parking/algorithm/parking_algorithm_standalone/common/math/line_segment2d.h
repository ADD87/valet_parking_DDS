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
 * @brief Defines the LineSegment2d class and the MultiLineSegment class.
 */

#pragma once

#include <string>
#include <vector>

#include "common/file/log.h"  // For LOG macro
#include "common/math/vec2d.h"

namespace TL {
namespace common {
namespace math {

// Helper functions
void GenerateCenterPoint(const std::vector<Vec2d>& left_point,
                         const std::vector<Vec2d>& right_point,
                         std::vector<Vec2d>* cent_point);

void CenterPoint(const std::vector<Vec2d>& project_points,
                 const std::vector<Vec2d>& points,
                 std::vector<Vec2d>* cent_point);

bool CalculateLambda(const Vec2d& p1, const Vec2d& p2, const Vec2d& p3,
                     const Vec2d& p4, double* lambda);

/**
 * @class LineSegment2d
 * @brief 2-dimensional line segment
 */
class LineSegment2d {
 public:
  //! Default constructor
  LineSegment2d();

  //! Constructor with start and end points
  LineSegment2d(const Vec2d& start, const Vec2d& end);

  //! Initialize the line segment with start and end points
  void Init(const Vec2d& lhs, const Vec2d& rhs);

  //! Getter for start point
  const Vec2d& start() const { return start_; }

  //! Getter for end point
  const Vec2d& end() const { return end_; }

  //! Getter for unit direction vector
  const Vec2d& unit_direction() const { return unit_direction_; }

  //! Getter for heading angle
  double heading() const { return heading_; }

  //! Getter for center point
  Vec2d center() const { return (start_ + end_) / 2.0; }

  //! Rotates the line segment
  Vec2d rotate(const double angle) const;

  //! Gets the length of the line segment
  double length() const;

  //! Gets the squared length of the line segment
  double length_sqr() const;

  //! Computes the distance to a point
  double DistanceTo(const Vec2d& point) const;

  //! Computes the distance to a point and returns the nearest point
  double DistanceTo(const Vec2d& point, Vec2d* const nearest_pt) const;

  //! Computes the squared distance to a point
  double DistanceSquareTo(const Vec2d& point,
                          std::string* const str_info = nullptr) const;

  //! Computes the squared distance to a point and returns projection info
  double DistanceSquareTo(const Vec2d& point, Vec2d* const nearest_pt,
                          std::string* const str_info = nullptr) const;

  //! Checks if a point is on the line segment
  bool IsPointIn(const Vec2d& point) const;

  //! Projects a point onto the unit direction vector
  double ProjectOntoUnit(const Vec2d& point) const;

  //! Computes the cross product with a point
  double ProductOntoUnit(const Vec2d& point) const;

  //! Checks if this segment intersects with another segment
  bool HasIntersect(const LineSegment2d& other_segment) const;

  //! Gets the intersection point with another segment
  bool GetIntersect(const LineSegment2d& other_segment,
                    Vec2d* const point) const;

  //! Gets the perpendicular foot from a point to the line
  double GetPerpendicularFoot(const Vec2d& point,
                              Vec2d* const foot_point) const;

  //! Gets the projection of a point onto the line segment
  void GetProjectPoint(const Vec2d& point, Vec2d* const project_point) const;

  //! Translates the line segment
  void Translate(double distance, double direction);

  //! Extends the line segment
  void Extend(double distance);

  //! Transforms the line segment
  void Transform(const Vec2d& origin, double direction);

  //! Returns a human-readable string for debugging
  std::string DebugString() const;

 protected:
  Vec2d start_;
  Vec2d end_;
  Vec2d unit_direction_;
  double heading_ = 0.0;
  double length_ = 0.0;
};

/**
 * @class MultiLineSegment
 * @brief Multiple connected line segments
 */
class MultiLineSegment {
 public:
  //! Default constructor
  MultiLineSegment() = default;

  //! Initialize with a vector of points
  void Init(const std::vector<Vec2d>& points);

  //! Gets the projection of a point onto the path
  bool GetProjection(const Vec2d& point, double* accumulate_s, double* lateral,
                     double* min_distance, int* index_min = nullptr,
                     double radius1d = -1, int index_center = -1,
                     std::string* const str_info = nullptr) const;

  //! Getter for path points
  const std::vector<Vec2d>& path_points() const { return path_points_; }

  //! Getter for accumulated s values
  const std::vector<double>& accumulated_s() const { return accumulated_s_; }

  //! Getter for line segments
  const std::vector<LineSegment2d>& segments() const { return segments_; }

  //! Getter for total length
  double length() const { return length_; }

  //! Getter for number of points
  int num_points() const { return num_points_; }

  //! Getter for number of segments
  int num_segments() const { return num_segments_; }

 protected:
  std::vector<Vec2d> path_points_;
  std::vector<double> accumulated_s_;
  std::vector<LineSegment2d> segments_;
  int num_points_ = 0;
  int num_segments_ = 0;
  double length_ = 0.0;
};

}  // namespace math
}  // namespace common
}  // namespace TL
