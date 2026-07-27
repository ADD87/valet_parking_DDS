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
 * @file geometry_types.h
 * @brief Simplified geometry types to replace proto dependencies
 */

#pragma once

// 使用 proto_convert/types_convert.h 中的统一定义
#include "proto_convert/types_convert.h"

namespace TL {
namespace common {

// 这些类型现在通过 types_convert.h 提供
// PointENU, Polygon 等类型已在 types_convert.h 中定义为 struct

}  // namespace common
}  // namespace TL
