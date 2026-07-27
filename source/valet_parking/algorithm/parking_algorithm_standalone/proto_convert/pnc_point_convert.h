/******************************************************************************
 * PnC Point Proto to C++ Conversion Header
 *
 * Description: Convert common/pnc_point.proto to C++ structures
 * Original Proto: proto/common/pnc_point.proto
 * Priority: P1 (Core trajectory data structures)
 *
 * Date: 2025-12-15
 * Note: This file replaces Protobuf dependencies for standalone compilation
 *****************************************************************************/

#pragma once

#include "types_convert.h"  // 独立编译改造：使用 types_convert.h 中的 Point3D
#include <vector>
#include <cstdint>
#include <cmath>

// 定义此宏，告诉 common/pnc_types.h 不要重复定义类型
// 避免 SLPoint/PathPoint/TrajectoryPoint 重复定义错误
#define PNC_POINT_CONVERT_H_LIGHTWEIGHT_TYPES_DEFINED

namespace TL {
namespace common {

// 独立编译改造：Point3D 已在 types_convert.h 中定义，此处不再重复

/**
 * @brief Path Point - basic geometric point on a path
 */
struct PathPoint {
  double x = 0.0;             ///< X coordinate (meters)
  double y = 0.0;             ///< Y coordinate (meters)
  double z = 0.0;             ///< Z coordinate (meters)
  double theta = 0.0;         ///< Heading angle (radians)
  double kappa = 0.0;         ///< Curvature (1/meters)
  double s = 0.0;             ///< Accumulated distance from path start (meters)
  double dkappa = 0.0;        ///< Curvature derivative (1/m^2)
  double ddkappa = 0.0;       ///< Second derivative of curvature (1/m^3)
  double l = 0.0;             ///< Lateral offset from reference line (meters)
  double x_derivative = 0.0;  ///< Derivative of x w.r.t s
  double y_derivative = 0.0;  ///< Derivative of y w.r.t s
};

/**
 * @brief Trajectory Point - path point with velocity and acceleration
 */
struct TrajectoryPoint {
  PathPoint path_point;       ///< Geometric path point
  double v = 0.0;             ///< Velocity (m/s)
  double a = 0.0;             ///< Acceleration (m/s^2)
  double relative_time = 0.0; ///< Relative time from trajectory start (seconds)
  double da = 0.0;            ///< Jerk (m/s^3)
  double steer = 0.0;         ///< Steering angle (radians)
  double steer_rate = 0.0;    ///< Steering rate (rad/s)

  // Proto compatibility: Clear() method for RepeatedPtrField
  void Clear() {
    path_point = PathPoint();
    v = 0.0;
    a = 0.0;
    relative_time = 0.0;
    da = 0.0;
    steer = 0.0;
    steer_rate = 0.0;
  }

  // Proto compatibility: MergeFrom/CopyFrom for RepeatedPtrField<TrajectoryPoint>
  // protobuf 的 RepeatedPtrField<T>::operator= 内部调用 CopyFrom -> MergeFrom
  // 独立编译改造：struct 版本的 TrajectoryPoint 需要提供这些方法
  void MergeFrom(const TrajectoryPoint& other) {
    path_point = other.path_point;
    v = other.v;
    a = other.a;
    relative_time = other.relative_time;
    da = other.da;
    steer = other.steer;
    steer_rate = other.steer_rate;
  }
  void CopyFrom(const TrajectoryPoint& other) { MergeFrom(other); }
};

/**
 * @brief Speed Point - speed profile point
 */
struct SpeedPoint {
  double s = 0.0;             ///< Position along path (meters)
  double t = 0.0;             ///< Time (seconds)
  double v = 0.0;             ///< Velocity (m/s)
  double a = 0.0;             ///< Acceleration (m/s^2)
  double da = 0.0;            ///< Jerk (m/s^3)
};

/**
 * @brief Gaussian Distribution Information (for uncertainty)
 */
struct GaussianInfo {
  double sigma_x = 0.0;         ///< Standard deviation in x (meters)
  double sigma_y = 0.0;         ///< Standard deviation in y (meters)
  double correlation = 0.0;     ///< Correlation coefficient [-1, 1]
  double ellipse_a = 0.0;       ///< Ellipse semi-major axis (meters)
  double ellipse_b = 0.0;       ///< Ellipse semi-minor axis (meters)
  double ellipse_theta = 0.0;   ///< Ellipse orientation angle (radians)
};

/**
 * @brief Path - sequence of path points
 */
struct Path {
  std::string name = "";                  ///< Path name/identifier
  std::vector<PathPoint> path_point;      ///< Array of path points
  std::vector<GaussianInfo> uncertainty;  ///< Uncertainty info for each point
};

/**
 * @brief SL Point - Frenet coordinate (station, lateral)
 * Defined before ReferenceLine because template methods need complete type
 */
struct SLPoint {
  double s = 0.0;  ///< Station coordinate (along reference line, meters)
  double l = 0.0;  ///< Lateral coordinate (meters)
};

/**
 * @brief Reference Line - path with lane information
 */
struct ReferenceLine {
  std::string id = "";                    ///< Reference line ID
  std::vector<PathPoint> path_point;      ///< Array of path points
  double speed_limit = 0.0;               ///< Speed limit (m/s)

  /**
   * @brief Convert XY coordinates to SL (Frenet) coordinates
   * @param xy XY coordinate point (Vec2d with x() and y() methods)
   * @param sl_point Output SL point
   * @return true if conversion successful
   */
  template<typename Vec2dType>
  bool XYToSL(const Vec2dType& xy, SLPoint* sl_point) const {
    if (path_point.empty() || !sl_point) return false;

    double x = xy.x();
    double y = xy.y();

    // Find closest point on reference line
    double min_dist = std::numeric_limits<double>::max();
    size_t closest_idx = 0;

    for (size_t i = 0; i < path_point.size(); ++i) {
      double dx = path_point[i].x - x;
      double dy = path_point[i].y - y;
      double dist = std::sqrt(dx * dx + dy * dy);
      if (dist < min_dist) {
        min_dist = dist;
        closest_idx = i;
      }
    }

    sl_point->s = path_point[closest_idx].s;

    // Calculate lateral offset (simplified - doesn't consider direction)
    // For accurate lateral offset, we need to calculate perpendicular distance
    if (closest_idx > 0 && closest_idx < path_point.size() - 1) {
      // Use direction from previous to next point
      double dx_path = path_point[closest_idx + 1].x - path_point[closest_idx - 1].x;
      double dy_path = path_point[closest_idx + 1].y - path_point[closest_idx - 1].y;
      double path_len = std::sqrt(dx_path * dx_path + dy_path * dy_path);

      if (path_len > 1e-6) {
        double dx_to_point = x - path_point[closest_idx].x;
        double dy_to_point = y - path_point[closest_idx].y;

        // Cross product to get signed lateral distance
        sl_point->l = (dx_to_point * dy_path - dy_to_point * dx_path) / path_len;
      } else {
        sl_point->l = min_dist;
      }
    } else {
      sl_point->l = min_dist;
    }

    return true;
  }

  /**
   * @brief Evaluate path point at given s coordinate
   * @param s Station coordinate
   * @return PathPoint at s (interpolated if necessary)
   */
  PathPoint Evaluate(double s) const {
    if (path_point.empty()) return PathPoint();

    // If s is before first point
    if (s <= path_point.front().s) return path_point.front();

    // If s is after last point
    if (s >= path_point.back().s) return path_point.back();

    // Find segment containing s
    for (size_t i = 1; i < path_point.size(); ++i) {
      if (path_point[i].s >= s) {
        const auto& p0 = path_point[i - 1];
        const auto& p1 = path_point[i];

        double ratio = (s - p0.s) / (p1.s - p0.s + 1e-6);

        PathPoint result;
        result.x = p0.x + ratio * (p1.x - p0.x);
        result.y = p0.y + ratio * (p1.y - p0.y);
        result.theta = p0.theta + ratio * (p1.theta - p0.theta);
        result.kappa = p0.kappa + ratio * (p1.kappa - p0.kappa);
        result.s = s;
        result.dkappa = p0.dkappa + ratio * (p1.dkappa - p0.dkappa);
        result.ddkappa = p0.ddkappa + ratio * (p1.ddkappa - p0.ddkappa);

        return result;
      }
    }

    return path_point.back();
  }

  /**
   * @brief Get total length of reference line
   * @return Length in meters
   */
  double Length() const {
    if (path_point.empty()) return 0.0;
    return path_point.back().s;
  }
};

// SLPoint is already defined above, before ReferenceLine

/**
 * @brief Frenet Frame Point
 */
struct FrenetFramePoint {
  double s = 0.0;      ///< Station coordinate (meters)
  double l = 0.0;      ///< Lateral coordinate (meters)
  double dl = 0.0;     ///< Lateral velocity (m/s)
  double ddl = 0.0;    ///< Lateral acceleration (m/s^2)
};

/**
 * @brief Path Boundary - lateral bounds along path
 */
struct PathBoundary {
  std::string name = "";                      ///< Boundary name
  double start_s = 0.0;                       ///< Start station (meters)
  double delta_s = 0.0;                       ///< Station interval (meters)
  std::vector<std::pair<double, double>> boundary;  ///< (left_bound, right_bound) pairs
};

/**
 * @brief Engaged State
 */
enum EngageAdvice {
  UNKNOWN = 0,
  DISALLOW_ENGAGE = 1,
  READY_TO_ENGAGE = 2
};

}  // namespace common
}  // namespace TL
