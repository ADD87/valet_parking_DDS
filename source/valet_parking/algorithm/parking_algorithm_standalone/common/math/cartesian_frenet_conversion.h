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
 * @brief Defines Cartesian-Frenet coordinate conversion.
 */

#pragma once

#include <array>

namespace TL {
namespace common {
namespace math {

/**
 * @class CartesianFrenetConverter
 * @brief Converts between Cartesian and Frenet coordinates
 */
class CartesianFrenetConverter {
 public:
  //! Converts from Cartesian to Frenet coordinates
  static void cartesian_to_frenet(
      const double rs, const double rx, const double ry, const double rtheta,
      const double rkappa, const double rdkappa, const double x, const double y,
      const double v, const double a, const double theta, const double kappa,
      std::array<double, 3>* const ptr_s_condition,
      std::array<double, 3>* const ptr_d_condition);

  //! Converts from Frenet to Cartesian coordinates
  static void frenet_to_cartesian(
      const double rs, const double rx, const double ry, const double rtheta,
      const double rkappa, const double rdkappa,
      const std::array<double, 3>& s_condition,
      const std::array<double, 3>& d_condition, double* const ptr_x,
      double* const ptr_y, double* const ptr_theta, double* const ptr_kappa,
      double* const ptr_v, double* const ptr_a);
};

}  // namespace math
}  // namespace common
}  // namespace TL
