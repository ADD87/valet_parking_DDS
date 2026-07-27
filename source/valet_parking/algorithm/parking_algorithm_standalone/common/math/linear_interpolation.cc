/*
 * Copyright (c) TL Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description:  common math interpolation
 * Author: ROC
 * 独立编译改造：
 *   - SLPoint/PathPoint/TrajectoryPoint 使用 struct 直接成员访问
 *   - 移除 .set_xxx() / .xxx() proto 风格接口
 *   - has_path_point() → 始终为 true（struct 成员始终存在）
 */

#include "common/math/linear_interpolation.h"

#include "common/geometry_types.h"
#include "common/file/log.h"
#include "common/math/double_type.h"
#include "common/math/math_utils.h"

namespace TL {
namespace common {
namespace math {

using common::PointENU;
using common::math::double_type::Compare;

// Linear interpolation helper function
double lerp(const double v0, const double t0, const double v1, const double t1,
            const double t) {
  if (std::abs(t1 - t0) <= kMathEpsilon) {
    return v0;
  }
  const double r = (t - t0) / (t1 - t0);
  return v0 + r * (v1 - v0);
}

double slerp(const double a0, const double t0, const double a1, const double t1,
             const double t) {
  if (std::abs(t1 - t0) <= kMathEpsilon) {
    VLOG(4) << "input time difference is too small";
    return NormalizeAngle(a0);
  }
  const double a0_n = NormalizeAngle(a0);
  const double a1_n = NormalizeAngle(a1);
  double d = a1_n - a0_n;
  if (d > M_PI) {
    d = d - 2 * M_PI;
  } else if (d < -M_PI) {
    d = d + 2 * M_PI;
  }

  const double r = (t - t0) / (t1 - t0);
  const double a = a0_n + d * r;
  return NormalizeAngle(a);
}

SLPoint InterpolateUsingLinearApproximation(const SLPoint& p0,
                                            const SLPoint& p1, const double w) {
  if (w < 0.0) {
    LOG(ERROR) << "weight must be >= 0.0, got: " << w;
  }

  SLPoint p;
  p.s = (1 - w) * p0.s + w * p1.s;
  p.l = (1 - w) * p0.l + w * p1.l;
  return p;
}

PathPoint InterpolateUsingLinearApproximation(const PathPoint& p0,
                                              const PathPoint& p1,
                                              const double s) {
  double s0 = p0.s;
  double s1 = p1.s;

  PathPoint path_point;
  double weight = (s - s0) / (s1 - s0);
  path_point.x = (1 - weight) * p0.x + weight * p1.x;
  path_point.y = (1 - weight) * p0.y + weight * p1.y;
  path_point.z = (1 - weight) * p0.z + weight * p1.z;
  path_point.theta = slerp(p0.theta, p0.s, p1.theta, p1.s, s);
  path_point.kappa = (1 - weight) * p0.kappa + weight * p1.kappa;
  path_point.dkappa = (1 - weight) * p0.dkappa + weight * p1.dkappa;
  path_point.ddkappa = (1 - weight) * p0.ddkappa + weight * p1.ddkappa;
  path_point.s = s;
  return path_point;
}

TrajectoryPoint InterpolateUsingLinearApproximation(const TrajectoryPoint& tp0,
                                                    const TrajectoryPoint& tp1,
                                                    const double t) {
  // struct TrajectoryPoint 始终包含 path_point，无需 has_path_point() 检查
  const auto& pp0 = tp0.path_point;
  const auto& pp1 = tp1.path_point;
  double t0 = tp0.relative_time;
  double t1 = tp1.relative_time;

  TrajectoryPoint tp;
  tp.v = lerp(tp0.v, t0, tp1.v, t1, t);
  tp.a = lerp(tp0.a, t0, tp1.a, t1, t);
  tp.da = lerp(tp0.da, t0, tp1.da, t1, t);
  tp.relative_time = t;
  tp.steer = slerp(tp0.steer, t0, tp1.steer, t1, t);

  tp.path_point.x = lerp(pp0.x, t0, pp1.x, t1, t);
  tp.path_point.y = lerp(pp0.y, t0, pp1.y, t1, t);
  tp.path_point.z = lerp(pp0.z, t0, pp1.z, t1, t);
  tp.path_point.theta = slerp(pp0.theta, t0, pp1.theta, t1, t);
  tp.path_point.kappa = lerp(pp0.kappa, t0, pp1.kappa, t1, t);
  tp.path_point.dkappa = lerp(pp0.dkappa, t0, pp1.dkappa, t1, t);
  tp.path_point.ddkappa = lerp(pp0.ddkappa, t0, pp1.ddkappa, t1, t);
  tp.path_point.s = lerp(pp0.s, t0, pp1.s, t1, t);

  return tp;
}

double InterpolationOne(const double& input_x,
                        const std::vector<double>& input_v,
                        const std::vector<double>& output_v) {
  uint v_size = input_v.size();
  if (input_x >= input_v[v_size - 1]) {
    return output_v[v_size - 1];
  }
  if (input_x <= input_v[0]) {
    return output_v[0];
  }
  for (size_t a = 1; a < input_v.size(); ++a) {
    if (input_x >= input_v[a - 1] && input_x < input_v[a]) {
      return output_v[a - 1] + (output_v[a] - output_v[a - 1]) *
                                   (input_x - input_v[a - 1]) /
                                   (input_v[a] - input_v[a - 1]);
    }
  }
  VLOG(4) << "InterpolationOne error!";
  return 1.0;
}

PointENU InterpolatePoint(const PointENU& start, const PointENU& end,
                          double length, double s) {
  PointENU new_point;
  double weight = s / length;
  double x_val = (1 - weight) * start.x + weight * end.x;
  double y_val = (1 - weight) * start.y + weight * end.y;
  double z_val = (1 - weight) * start.z + weight * end.z;
  new_point.x = x_val;
  new_point.y = y_val;
  new_point.z = z_val;
  return new_point;
}

Vec2d InterpolatePoint(const Vec2d& start, const Vec2d& end, double length,
                       double s) {
  Vec2d new_point;
  double weight = s / length;
  double x = (1 - weight) * start.x() + weight * end.x();
  double y = (1 - weight) * start.y() + weight * end.y();
  new_point.set_x(x);
  new_point.set_y(y);
  return new_point;
}

std::vector<Vec2d> InterpolateVec2dPoints(const std::vector<Vec2d>& raw_points,
                                          double delta_s) {
  if (raw_points.size() <= 2) {
    return raw_points;
  }
  std::vector<Vec2d> new_points;
  new_points.push_back(raw_points.front());
  double sum_length = 0;
  for (size_t i = 0; i < raw_points.size() - 1; i++) {
    sum_length += raw_points.at(i).DistanceTo(raw_points.at(i + 1));
  }
  if (sum_length <= delta_s) {
    new_points.push_back(raw_points.back());
    return new_points;
  }
  Vec2d now_point;
  Vec2d end_point;
  double total_length = 0;
  for (size_t i = 0; i < raw_points.size() - 1; i++) {
    now_point = raw_points.at(i);
    end_point = raw_points.at(i + 1);
    const double point_distance = now_point.DistanceTo(end_point);
    total_length += point_distance;
    if (Compare(total_length, delta_s) <= 0) {
      continue;
    }
    Vec2d interpolate_point;
    double s_val = delta_s;
    while (s_val < total_length) {
      interpolate_point = InterpolatePoint(now_point, end_point, point_distance,
                                           point_distance - (total_length - s_val));
      new_points.push_back(interpolate_point);
      s_val += delta_s;
    }
    total_length = interpolate_point.DistanceTo(end_point);
  }
  return new_points;
}

}  // namespace math
}  // namespace common
}  // namespace TL
