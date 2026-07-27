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
 * @brief Defines common mathematical utility functions.
 */

#pragma once

#include <vector>
#include "common/math/vec2d.h"
#include "common/math/line_segment2d.h"
#include "proto_convert/pnc_point_convert.h"  // 独立编译改造：必须在 pnc_types.h 之前，确保 struct PathPoint
#include "common/pnc_types.h"
#include "proto_convert/chassis_convert.h"  // 独立编译改造：for soc::GearPosition
#include <utility>

namespace TL {
namespace common {
namespace math {
class Polygon2d;  // Forward declaration
}  // namespace math
}  // namespace common
namespace common {
namespace math {

//! Square function
double Sqr(const double x);

//! Cross product
double CrossProd(const double x0, const double y0, const double x1,
                 const double y1);

//! Inner product
double InnerProd(const double x0, const double y0, const double x1,
                 const double y1);

//! Inner product for Vec2d
double InnerProd(const Vec2d& start_point, const Vec2d& end_point_1,
                 const Vec2d& end_point_2);

//! Wraps angle to [0, 2*PI)
double WrapAngle(const double angle);

//! Normalizes angle to [-PI, PI)
double NormalizeAngle(const double angle);

//! Normalizes angle to [0, 2*PI)
double NormalizeAngle2(const double angle);

//! Computes angle difference
double AngleDiff(const double from, const double to);

//! Checks if angle is in range
bool AngleInRange(double angle, double lower, double upper);

//! Random integer in range [s, t]
int RandomInt(const int s, const int t, unsigned int rand_seed = 1);

//! Random double in range [s, t]
double RandomDouble(const double s, const double t, unsigned int rand_seed = 1);

//! Gaussian function
double Gaussian(const double u, const double std, const double x);

//! Template clamp function
template <typename T>
T Clamp(const T value, T bound1, T bound2) {
  if (bound1 > bound2) {
    std::swap(bound1, bound2);
  }

  if (value < bound1) {
    return bound1;
  } else if (value > bound2) {
    return bound2;
  }
  return value;
}

// Path-based utility functions
double GetMinDistance2ObstaclesSegments(
    const std::vector<common::PathPoint>& path,
    const std::vector<Vec2d>& obstacle_segments);

bool CheckCollisionWithVehiclePolygon2d(
    const std::vector<common::PathPoint>& path,
    const std::vector<std::vector<Vec2d>>& obstacles_segments_vec);

bool CheckCollisionWithCircle(
    const std::vector<common::PathPoint>& path,
    const std::vector<Vec2d>& circle_centers, const double radius);

// Circle vs freespace-segment collision (single circle)
bool CheckCollisionWithCircle(
    const std::pair<common::math::Vec2d, double>& circle,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        obstacles_segments_vec,
    size_t* collision_index_ptr);

// Single-point collision detection functions (for Hybrid A* ValidityCheck)
double GetMinDistance2ObstaclesSegments(
    const double vehicle_x, const double vehicle_y, const double vehicle_theta,
    const std::vector<std::pair<common::math::LineSegment2d, double>>& obstacles_segments_vec,
    const double obstacle_filter_distance);

bool CheckCollisionWithVehiclePolygon2d(
    const double vehicle_x, const double vehicle_y, const double vehicle_theta,
    const std::vector<std::pair<common::math::LineSegment2d, double>>& obstacles_segments_vec);

// Single-point collision with collision index output
bool CheckCollisionWithVehiclePolygon2d(
    const double vehicle_x, const double vehicle_y, const double vehicle_theta,
    const std::vector<std::pair<common::math::LineSegment2d, double>>& obstacles_segments_vec,
    size_t* collision_index_ptr);

// Detailed polygon-based collision detection with filter distances
bool CheckCollisionWithVehiclePolygon2d(
    const common::math::Polygon2d& polygon, const common::math::Vec2d& center,
    const common::math::Vec2d& unit_vec2d,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        obstacles_segments_vec,
    const double euclidean_filter_distance,
    const double longitudinal_filter_distance,
    const double lateral_filter_distance, size_t* const collision_index_ptr);

double ConvertDisplaySpdToReal(const double spd_ms_display);

/**
 * @brief ENU坐标转FLU坐标
 * 独立编译改造：原函数声明丢失，从使用处推断签名
 * @param enu_x ENU X
 * @param enu_y ENU Y
 * @param ref_x 参考点X
 * @param ref_y 参考点Y
 * @param ref_theta 参考朝向
 * @return FLU (forward, left) 坐标对
 */
inline std::pair<double, double> ENUToFLU(double enu_x, double enu_y,
                                          double ref_x, double ref_y,
                                          double ref_theta) {
  double dx = enu_x - ref_x;
  double dy = enu_y - ref_y;
  double flu_x = dx * std::cos(ref_theta) + dy * std::sin(ref_theta);
  double flu_y = -dx * std::sin(ref_theta) + dy * std::cos(ref_theta);
  return {flu_x, flu_y};
}

// 独立编译改造：使用 soc::Chassis::GearPosition (from chassis_convert.h)
bool GetGearFromPath(const common::PathPoint& from_point,
                     const common::PathPoint& to_point, soc::GearPosition* gear);

}  // namespace math
}  // namespace common
}  // namespace TL
