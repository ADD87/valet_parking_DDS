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
 * @brief Defines search algorithms.
 */

#pragma once

#include <functional>

namespace TL {
namespace common {
namespace math {

//! Golden section search for finding minimum of a unimodal function
double GoldenSectionSearch(const std::function<double(double)>& func,
                           const double lower_bound, const double upper_bound,
                           const double tol = 1e-6);

}  // namespace math
}  // namespace common
}  // namespace TL
