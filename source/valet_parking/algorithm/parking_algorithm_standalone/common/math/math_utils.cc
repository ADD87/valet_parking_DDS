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

#include "common/math/math_utils.h"
#include <sys/types.h>

#include <cmath>
#include <Eigen/Dense>
#include <limits>
#include <utility>

#include "common/math/box2d.h"
#include "common/math/double_type.h"
#include "common/math/polygon2d.h"
#include "common/pnc_types.h"
#include "common/file/log.h"
#include "common/configs/vehicle_config_helper.h"

namespace TL {
namespace common {
namespace math {
// 注意：kAmplificationRatio 已迁移到 HybridAStarConfig::amplification_ratio
// 此处保留是为了向后兼容，建议使用配置参数
// 参考: planning/open_space/hybrid_a_star_config.h
static constexpr double kAmplificationRatio = 1.2;

double Sqr(const double x) {
  return x * x;
}

double InnerProd(const Vec2d& start_point, const Vec2d& end_point_1,
                 const Vec2d& end_point_2) {
  return (end_point_1 - start_point).InnerProd(end_point_2 - start_point);
}

double CrossProd(const double x0, const double y0, const double x1,
                 const double y1) {
  return x0 * y1 - x1 * y0;
}

double InnerProd(const double x0, const double y0, const double x1,
                 const double y1) {
  return x0 * x1 + y0 * y1;
}

double WrapAngle(const double angle) {
  const double new_angle = std::fmod(angle, M_PI * 2.0);
  return new_angle < 0 ? new_angle + M_PI * 2.0 : new_angle;
}

double NormalizeAngle(const double angle) {
  double a = std::fmod(angle + M_PI, 2.0 * M_PI);
  if (a < 0.0) {
    a += (2.0 * M_PI);
  }
  return a - M_PI;
}

double NormalizeAngle2(const double angle) {
  double a = std::fmod(angle, 2.0 * M_PI);
  if (a < 0.0) {
    a += (2 * M_PI);
  }
  return a;
}

double AngleDiff(const double from, const double to) {
  return NormalizeAngle(to - from);
}

bool AngleInRange(double angle, double lower, double upper) {
  return common::math::WrapAngle(angle - lower) <
         common::math::WrapAngle(upper - lower);
};

int RandomInt(const int s, const int t, unsigned int rand_seed) {
  if (s >= t) {
    return s;
  }
  return s + rand_r(&rand_seed) % (t - s + 1);
}

double RandomDouble(const double s, const double t, unsigned int rand_seed) {
  static constexpr u_int32_t kBASE = 16383;
  return s + (t - s) / 16383.0 *
                 (static_cast<u_int32_t>(rand_r(&rand_seed)) & kBASE);
}

// Gaussian
double Gaussian(const double u, const double std, const double x) {
  return (1.0 / std::sqrt(2 * M_PI * std * std)) *
         std::exp(-(x - u) * (x - u) / (2 * std * std));
}

Eigen::Vector2d RotateVector2d(const Eigen::Vector2d& v_in,
                               const double theta) {
  const double cos_theta = std::cos(theta);
  const double sin_theta = std::sin(theta);

  auto x = cos_theta * v_in.x() - sin_theta * v_in.y();
  auto y = sin_theta * v_in.x() + cos_theta * v_in.y();

  return {x, y};
}

std::pair<double, double> Cartesian2Polar(double x, double y) {
  double r = std::sqrt(x * x + y * y);
  double theta = std::atan2(y, x);
  return std::make_pair(r, theta);
}

/**
 * @brief 计算车辆多边形与障碍物线段之间的最小距离，并结合多种过滤条件提前排除不可能发生碰撞的障碍物，提高效率
 *
 * @param polygon2d                车辆的多边形轮廓
 * @param polygon_center           车辆多边形的中心点
 * @param polygon_heading_unit     车辆朝向的单位向量
 * @param obstacle_segment         障碍物的线段
 * @param euclidean_filter_distance 欧氏距离过滤阈值
 * @param longitudinal_filter_distance 纵向过滤阈值
 * @param lateral_filter_distance  横向过滤阈值
 * @return double                  返回车辆轮廓到障碍物线段的最小距离，若提前过滤则返回无穷大
 */
static double GetDistance2ObstacleSegment(  // NOLINT
    const common::math::Polygon2d& polygon2d,
    const common::math::Vec2d& polygon_center,
    const common::math::Vec2d& polygon_heading_unit,
    const common::math::LineSegment2d& obstacle_segment,
    const double euclidean_filter_distance,
    const double longitudinal_filter_distance,
    const double lateral_filter_distance) {
  static constexpr double kMaxDistance =
      std::numeric_limits<double>::infinity();
  // 步骤1：欧氏距离初筛，障碍物线段到车辆中心距离大于阈值直接返回无穷大
  if (obstacle_segment.DistanceTo(polygon_center) > euclidean_filter_distance) {
    return kMaxDistance;
  }
  // 步骤2：计算中心到障碍物线段两端的向量
  auto center_to_start = obstacle_segment.start() - polygon_center;
  auto center_to_end = obstacle_segment.end() - polygon_center;
  // 步骤3：横向过滤，判断障碍物线段是否完全在车辆一侧且横向距离都大于阈值
  double product1 = polygon_heading_unit.CrossProd(center_to_start);
  double product2 = polygon_heading_unit.CrossProd(center_to_end);
  double project1 = polygon_heading_unit.InnerProd(center_to_start);
  double project2 = polygon_heading_unit.InnerProd(center_to_end);
  if (double_type::DefinitelyGreater(product1 * product2, 0.0) &&
      double_type::DefinitelyGreater(std::fabs(product1),
                                     lateral_filter_distance) &&
      double_type::DefinitelyGreater(std::fabs(product2),
                                     lateral_filter_distance)) {
    return kMaxDistance;
  }
  // 步骤4：纵向过滤，判断障碍物线段是否完全在车辆前/后方且纵向距离都大于阈值
  if (double_type::DefinitelyGreater(project1 * project2, 0.0) &&
      double_type::DefinitelyGreater(std::fabs(project1),
                                     longitudinal_filter_distance) &&
      double_type::DefinitelyGreater(std::fabs(project2),
                                     longitudinal_filter_distance)) {
    return kMaxDistance;
  }

  // 步骤5：精确计算车辆多边形到障碍物线段的最小距离
  return polygon2d.DistanceTo(obstacle_segment);
}

// Single-point version: Get minimum distance from vehicle to obstacles at specific pose
double GetMinDistance2ObstaclesSegments(
    const double vehicle_x, const double vehicle_y, const double vehicle_theta,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        obstacles_segments_vec,
    const double obstacle_filter_distance) {
  const auto& vehicle_param =
      common::VehicleConfigHelper::GetConfig().vehicle_param();
  // 使用原始车辆尺寸（不加缓冲区），返回实际车辆轮廓到障碍物的距离
  auto polygon2d = common::VehicleConfigHelper::GetPolygon2dWithBuffer(
      vehicle_x, vehicle_y, vehicle_theta);
  double diff = (vehicle_param.front_edge_to_center -
                 vehicle_param.back_edge_to_center) /
                2.0;
  common::math::Vec2d true_center(vehicle_x + diff * std::cos(vehicle_theta),
                                  vehicle_y + diff * std::sin(vehicle_theta));
  const auto unit_vec2d = common::math::Vec2d::CreateUnitVec2d(vehicle_theta);
  double euclidean_filter_distance =
      std::hypot(vehicle_param.length, vehicle_param.width) / 2.0;
  double lateral_filter_distance = vehicle_param.width / 2.0;
  double longitudinal_filter_distance = vehicle_param.length / 2.0;
  double distance_to_obstalce = std::numeric_limits<double>::infinity();
  for (const auto& obstacle_segment : obstacles_segments_vec) {
    auto extra_filter_distance =
        fmax(obstacle_filter_distance,
             kAmplificationRatio * obstacle_segment.second);
    auto real_distance = GetDistance2ObstacleSegment(
        polygon2d, true_center, unit_vec2d, obstacle_segment.first,
        euclidean_filter_distance + extra_filter_distance,
        longitudinal_filter_distance + extra_filter_distance,
        lateral_filter_distance + extra_filter_distance);
    real_distance =
        double_type::DefinitelyLess(real_distance, obstacle_segment.second)
            ? 0.0
            : real_distance;
    distance_to_obstalce = fmin(distance_to_obstalce, real_distance);

    if (TL::common::math::double_type::IsZero(distance_to_obstalce)) {
      return 0;
    }
  }
  return distance_to_obstalce;
}

// Path-based version: Simplified implementation for path-based collision checking
double GetMinDistance2ObstaclesSegments(
    const std::vector<common::PathPoint>& path,
    const std::vector<Vec2d>& obstacle_segments) {
  if (path.empty() || obstacle_segments.empty()) {
    return std::numeric_limits<double>::infinity();
  }

  double min_distance = std::numeric_limits<double>::infinity();

  // Check distance from each path point to all obstacle points
  for (const auto& path_point : path) {
    Vec2d path_pos(path_point.x, path_point.y);
    for (const auto& obs_point : obstacle_segments) {
      double dist = path_pos.DistanceTo(obs_point);
      min_distance = std::fmin(min_distance, dist);
    }
  }

  return min_distance;
}

// Single-point version (6 parameters): Full version with all options
bool CheckCollisionWithVehiclePolygon2d(
    const double vehicle_x, const double vehicle_y, const double vehicle_theta,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        obstacles_segments_vec,
    size_t* const collision_index_ptr, const bool is_consider_virtual_obs) {
  const auto& vehicle_param =
      common::VehicleConfigHelper::GetConfig().vehicle_param();
  // 使用原始车辆尺寸（不加缓冲区）进行碰撞检测
  auto polygon2d = common::VehicleConfigHelper::GetPolygon2dWithBuffer(
      vehicle_x, vehicle_y, vehicle_theta);
  double diff = (vehicle_param.front_edge_to_center -
                 vehicle_param.back_edge_to_center) /
                2.0;
  common::math::Vec2d true_center(vehicle_x + diff * std::cos(vehicle_theta),
                                  vehicle_y + diff * std::sin(vehicle_theta));
  const auto unit_vec2d = common::math::Vec2d::CreateUnitVec2d(vehicle_theta);
  double euclidean_filter_distance =
      std::hypot(vehicle_param.length, vehicle_param.width) / 2.0;
  double lateral_filter_distance = vehicle_param.width / 2.0;
  double longitudinal_filter_distance = vehicle_param.length / 2.0;
  static constexpr double kEpsilon = 1.e-3;
  for (size_t i = 0; i < obstacles_segments_vec.size(); i++) {
    const auto& obstacle_segment = obstacles_segments_vec[i];
    if (!is_consider_virtual_obs && obstacle_segment.second < kEpsilon) {
      ADEBUG << " ignore virtual obs once";
      continue;
    }
    auto extra_filter_distance = kAmplificationRatio * obstacle_segment.second;
    auto real_distance = GetDistance2ObstacleSegment(
        polygon2d, true_center, unit_vec2d, obstacle_segment.first,
        euclidean_filter_distance + extra_filter_distance,
        longitudinal_filter_distance + extra_filter_distance,
        lateral_filter_distance + extra_filter_distance);
    if (double_type::DefinitelyLess(real_distance, obstacle_segment.second) ||
        double_type::IsZero(real_distance)) {
      if (collision_index_ptr != nullptr) {
        *collision_index_ptr = i;
      }
      return true;
    }
  }
  return false;
}

// Single-point version (4 parameters): Simplified overload for ValidityCheck
bool CheckCollisionWithVehiclePolygon2d(
    const double vehicle_x, const double vehicle_y, const double vehicle_theta,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        obstacles_segments_vec) {
  return CheckCollisionWithVehiclePolygon2d(
      vehicle_x, vehicle_y, vehicle_theta, obstacles_segments_vec,
      nullptr, true);
}

// Single-point version (5 parameters): With collision index output
bool CheckCollisionWithVehiclePolygon2d(
    const double vehicle_x, const double vehicle_y, const double vehicle_theta,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        obstacles_segments_vec,
    size_t* collision_index_ptr) {
  return CheckCollisionWithVehiclePolygon2d(
      vehicle_x, vehicle_y, vehicle_theta, obstacles_segments_vec,
      collision_index_ptr, true);
}

/*
bool CheckCollisionWithRoughVehiclePolygon2d(
    const double vehicle_x, const double vehicle_y, const double vehicle_theta,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        obstacles_segments_vec,
    size_t* const collision_index_ptr, const bool is_consider_virtual_obs) {
  common::VehicleParam vehicle_param =
      common::VehicleConfigHelper::GetConfig().vehicle_param();
  PathPoint veh_point;
  veh_point.set_x(vehicle_x);
  veh_point.set_y(vehicle_y);
  veh_point.set_theta(vehicle_theta);
  auto polygon2d = common::math::Polygon2d(
      common::VehicleConfigHelper::GetBoundingBox(veh_point));
  double diff = (vehicle_param.front_edge_to_center -
                 vehicle_param.back_edge_to_center) /
                2.0;
  common::math::Vec2d true_center(vehicle_x + diff * std::cos(vehicle_theta),
                                  vehicle_y + diff * std::sin(vehicle_theta));
  const auto unit_vec2d = common::math::Vec2d::CreateUnitVec2d(vehicle_theta);
  double euclidean_filter_distance =
      std::hypot(vehicle_param.length, vehicle_param.width) / 2.0;
  double lateral_filter_distance = vehicle_param.width / 2.0;
  double longitudinal_filter_distance = vehicle_param.length / 2.0;
  static constexpr double kEpsilon = 1.e-3;
  for (size_t i = 0; i < obstacles_segments_vec.size(); i++) {
    const auto& obstacle_segment = obstacles_segments_vec[i];
    if (!is_consider_virtual_obs && obstacle_segment.second < kEpsilon) {
      VLOG(4) << " ignore virtual obs once";
      continue;
    }
    auto extra_filter_distance = kAmplificationRatio * obstacle_segment.second;
    auto real_distance = GetDistance2ObstacleSegment(
        polygon2d, true_center, unit_vec2d, obstacle_segment.first,
        euclidean_filter_distance + extra_filter_distance,
        longitudinal_filter_distance + extra_filter_distance,
        lateral_filter_distance + extra_filter_distance);
    if (double_type::DefinitelyLess(real_distance, obstacle_segment.second) ||
        double_type::IsZero(real_distance)) {
      if (collision_index_ptr != nullptr) {
        *collision_index_ptr = i;
      }
      return true;
    }
  }
  return false;
}
*/

bool CheckCollisionWithVehiclePolygon2d(
    const common::math::Polygon2d& polygon, const common::math::Vec2d& center,
    const common::math::Vec2d& unit_vec2d,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        obstacles_segments_vec,
    const double euclidean_filter_distance,
    const double longitudinal_filter_distance,
    const double lateral_filter_distance, size_t* const collision_index_ptr) {
  for (size_t i = 0; i < obstacles_segments_vec.size(); i++) {
    const auto& obstacle_segment = obstacles_segments_vec.at(i);
    auto extra_filter_distance = kAmplificationRatio * obstacle_segment.second;
    auto real_distance = GetDistance2ObstacleSegment(
        polygon, center, unit_vec2d, obstacle_segment.first,
        euclidean_filter_distance + extra_filter_distance,
        longitudinal_filter_distance + extra_filter_distance,
        lateral_filter_distance + extra_filter_distance);
    if (double_type::DefinitelyLess(real_distance, obstacle_segment.second) ||
        double_type::IsZero(real_distance)) {
      if (collision_index_ptr != nullptr) {
        *collision_index_ptr = i;
      }
      return true;
    }
  }
  return false;
}

bool CheckCollisionWithCircle(
    const std::pair<common::math::Vec2d, double>& circle,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        obstacles_segments_vec,
    size_t* collision_index_ptr) {
  if (nullptr == collision_index_ptr) {
    AERROR << "no collision_index input";
    return false;
  }
  static constexpr double kEpsilon = 1.e-3;
  for (size_t i = 0; i < obstacles_segments_vec.size(); i++) {
    const auto& obstacle_segment = obstacles_segments_vec.at(i);
    if (obstacle_segment.first.DistanceTo(circle.first) < circle.second) {
      *collision_index_ptr = i;
      return true;
    }
  }
  return false;
}

double ConvertDisplaySpdToReal(const double spd_ms_display) {
  double constant = 0.3;
  const double spd_km = spd_ms_display * 3.6;
  if (spd_km < 71.0) {
    constant = 0.4;
  } else if (spd_km < 81.0) {
    constant = 0.45;
  } else if (spd_km < 91.0) {
    constant = 0.50;
  } else if (spd_km < 101.0) {
    constant = 0.70;
  } else if (spd_km < 111.0) {
    constant = 0.95;
  } else if (spd_km < 121.0) {
    constant = 1.05;
  } else if (spd_km < 126.0) {
    constant = 1.20;
  } else {
    constant = 1.25;
  }
  return spd_ms_display / 1.05 + constant;
}

// 独立编译改造：取消注释并修改为struct直接访问风格
bool GetGearFromPath(const common::PathPoint& from_point,
                     const common::PathPoint& to_point,
                     soc::GearPosition* const gear) {
  if (gear == nullptr) {
    AERROR << "GetGearFromPath input check fails";
    return false;
  }
  double heading_angle = from_point.theta;
  const Vec2d tracking_vector(to_point.x - from_point.x,
                              to_point.y - from_point.y);
  static constexpr double kSmallestLength = 1e-6;
  if (tracking_vector.Length() < kSmallestLength) {
    AERROR << "from point and to point dist is: " << tracking_vector.Length();
    return false;
  }
  double tracking_angle = tracking_vector.Angle();
  *gear =
      std::fabs(common::math::NormalizeAngle(tracking_angle - heading_angle)) <
              (M_PI_2)
          ? soc::GearPosition::GEAR_DRIVE
          : soc::GearPosition::GEAR_REVERSE;
  return true;
}

uint64_t FactorialFunc(uint64_t n) {
  if (n == 0 || n == 1) {
    return 1;
  }

  uint64_t ret = 1;
  for (uint64_t i = 2; i < n; ++i) {
    ret *= i;
  }
  return ret;
}

uint64_t Nnchoosek(int n, int k) {
  return FactorialFunc(n) / FactorialFunc(k) / FactorialFunc(n - k);
}

}  // namespace math
}  // namespace common
}  // namespace TL
