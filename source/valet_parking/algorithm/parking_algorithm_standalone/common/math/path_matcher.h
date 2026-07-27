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
 * @brief Defines path matching functions.
 */

#pragma once

#include <utility>
#include <vector>

// 独立编译改造：先include pnc_point_convert.h 确保使用 struct 版本的 PathPoint
#include "proto_convert/pnc_point_convert.h"
#include "common/pnc_types.h"

namespace TL {
namespace common {
namespace math {

/**
 * @class PathMatcher
 * @brief Matches points to a path
 */
class PathMatcher {
 public:
  //! Matches a point to the closest point on a path
  static common::PathPoint MatchToPath(
      const std::vector<common::PathPoint>& reference_line, const double x,
      const double y);

  //! Finds the projected point on a path
  static std::pair<double, double> GetPathFrenetCoordinate(
      const std::vector<common::PathPoint>& reference_line, const double x,
      const double y);
};

}  // namespace math
}  // namespace common
}  // namespace TL
