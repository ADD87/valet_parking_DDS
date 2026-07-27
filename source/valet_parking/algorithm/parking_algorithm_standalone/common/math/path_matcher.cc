/******************************************************************************
 * Copyright 2018 The Apollo Authors. All Rights Reserved.
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
 * 独立编译改造：PathPoint proto accessor → struct 直接成员访问
 **/

#include "common/math/path_matcher.h"

#include <algorithm>
#include <cmath>
#include <vector>

#include "glog/logging.h"

#include "common/math/math_utils.h"
#include "common/math/linear_interpolation.h"

namespace TL {
namespace common {
namespace math {

namespace {
// Helper function to find projection point on a line segment
PathPoint FindProjectionPoint(const PathPoint& p0, const PathPoint& p1,
                               double x, double y) {
  const double dx = p1.x - p0.x;
  const double dy = p1.y - p0.y;
  const double px = x - p0.x;
  const double py = y - p0.y;

  const double proj_len = dx * dx + dy * dy;
  if (proj_len <= kMathEpsilon) {
    return p0;
  }

  const double s = (px * dx + py * dy) / proj_len;
  if (s <= 0.0) {
    return p0;
  }
  if (s >= 1.0) {
    return p1;
  }

  PathPoint proj_pt;
  proj_pt.x = p0.x + s * dx;
  proj_pt.y = p0.y + s * dy;
  proj_pt.theta = p0.theta + s * (p1.theta - p0.theta);
  proj_pt.kappa = p0.kappa + s * (p1.kappa - p0.kappa);
  proj_pt.s = p0.s + s * (p1.s - p0.s);
  return proj_pt;
}
}  // namespace

PathPoint PathMatcher::MatchToPath(const std::vector<PathPoint>& reference_line,
                                   const double x, const double y) {
  CHECK_GT(reference_line.size(), 0U);

  auto func_distance_square = [](const PathPoint& point, const double x,
                                 const double y) {
    double dx = point.x - x;
    double dy = point.y - y;
    return dx * dx + dy * dy;
  };

  double distance_min = func_distance_square(reference_line.front(), x, y);
  std::size_t index_min = 0;

  for (std::size_t i = 1; i < reference_line.size(); ++i) {
    double distance_temp = func_distance_square(reference_line[i], x, y);
    if (distance_temp < distance_min) {
      distance_min = distance_temp;
      index_min = i;
    }
  }

  std::size_t index_start = (index_min == 0) ? index_min : index_min - 1;
  std::size_t index_end =
      (index_min + 1 == reference_line.size()) ? index_min : index_min + 1;

  if (index_start == index_end) {
    return reference_line[index_start];
  }

  return FindProjectionPoint(reference_line[index_start],
                             reference_line[index_end], x, y);
}

std::pair<double, double> PathMatcher::GetPathFrenetCoordinate(
    const std::vector<PathPoint>& reference_line, const double x,
    const double y) {
  auto matched_path_point = MatchToPath(reference_line, x, y);
  double rtheta = matched_path_point.theta;
  double rx = matched_path_point.x;
  double ry = matched_path_point.y;
  double delta_x = x - rx;
  double delta_y = y - ry;
  double side = std::cos(rtheta) * delta_y - std::sin(rtheta) * delta_x;
  std::pair<double, double> relative_coordinate;
  relative_coordinate.first = matched_path_point.s;
  relative_coordinate.second =
      std::copysign(std::hypot(delta_x, delta_y), side);
  return relative_coordinate;
}

}  // namespace math
}  // namespace common
}  // namespace TL
