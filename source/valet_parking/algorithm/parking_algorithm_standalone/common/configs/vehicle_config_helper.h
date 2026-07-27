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
 * @file vehicle_config_helper.h
 * @brief Simplified VehicleConfigHelper for standalone version
 */

#pragma once

#include <string>
#include <vector>
#include "common/math/box2d.h"
#include "common/math/polygon2d.h"
#include "common/math/vec2d.h"
#include "common/pnc_types.h"
#include "planning/open_space/vehicle_param.h"  // 使用统一的VehicleParam定义

namespace TL {
namespace common {

// 使用planning命名空间的VehicleParam，避免重复定义
using VehicleParam = TL::planning::VehicleParam;

/**
 * @brief Simplified VehicleConfig structure
 */
struct VehicleConfig {
  VehicleParam vehicle_param_;

  // Accessor function to match original code style
  const VehicleParam& vehicle_param() const { return vehicle_param_; }
  VehicleParam& vehicle_param() { return vehicle_param_; }
};

/**
 * @brief VehicleConfigHelper for managing vehicle configuration
 *
 * Simplified version for standalone build - uses hardcoded EP30 parameters
 */
class VehicleConfigHelper {
 public:
  /**
   * @brief Initialize with default configuration
   */
  static void Init();

  /**
   * @brief Initialize with config file
   * @param config_file Path to config file (not used in standalone)
   */
  static void Init(const std::string& config_file);

  /**
   * @brief Initialize with vehicle config
   * @param vehicle_params Vehicle parameters
   */
  static void Init(const VehicleConfig& vehicle_params);

  /**
   * @brief Get vehicle configuration
   * @return Reference to vehicle config
   */
  static const VehicleConfig& GetConfig();

  /**
   * @brief Calculate minimum safe turn radius
   * @return Minimum turn radius in meters
   */
  static double MinSafeTurnRadius();

  /**
   * @brief Get vehicle bounding box at specific pose
   * @param path_point Vehicle position and heading
   * @return Bounding box
   */
  static common::math::Box2d GetBoundingBox(
      const common::PathPoint& path_point);

  /**
   * @brief Get vehicle bounding box at specific pose
   * @param x X coordinate
   * @param y Y coordinate
   * @param heading Vehicle heading in radians
   * @return Bounding box
   */
  static common::math::Box2d GetBoundingBox(
      const double x, const double y, const double heading);

  /**
   * @brief Get vehicle polygon with safety buffer at specific pose
   * @param x X coordinate (rear axle center)
   * @param y Y coordinate (rear axle center)
   * @param heading Vehicle heading in radians
   * @return Vehicle polygon with buffer
   */
  static common::math::Polygon2d GetPolygon2dWithBuffer(
      const double x, const double y, const double heading);

  /**
   * @brief Get vehicle polygon with extra buffer
   * @param x X coordinate
   * @param y Y coordinate
   * @param heading Vehicle heading
   * @param extra_length Extra length buffer
   * @param extra_width Extra width buffer
   * @return Vehicle polygon with extra buffer
   */
  static common::math::Polygon2d GetPolygon2dWithBuffer(
      double x, double y, double theta, double extra_length, double extra_width);

  /**
   * @brief Get mirror circles for collision detection
   * @param x X coordinate
   * @param y Y coordinate
   * @param heading Vehicle heading
   * @return Vector of circles (center, radius)
   */
  static std::vector<std::pair<common::math::Vec2d, double>>
  GetMirrorCirclesWithBuffer(double x, double y, double heading);

  /**
   * @brief Get all radar positions
   * @param x X coordinate
   * @param y Y coordinate
   * @param heading Vehicle heading
   * @return Vector of radar positions
   */
  static std::vector<common::math::Vec2d> GetAllRadarPos(
      const double x, const double y, const double heading);

  /**
   * @brief 后轴中心到车辆几何中心距离
   * 独立编译改造：原函数丢失，从几何关系推导
   * @return (front_edge_to_center - back_edge_to_center) / 2.0
   */
  static double GetRearToCenter();

  /**
   * @brief 车辆包围盒对角线长度
   * 独立编译改造：原函数丢失，从几何关系推导
   * @return sqrt(length^2 + width^2)
   */
  static double GetDiagonal();

  /**
   * @brief 带非对称缓冲的车辆多边形
   * 独立编译改造：7参数版本（前/后/左/右独立缓冲）
   * @param x 后轴中心X
   * @param y 后轴中心Y
   * @param theta 朝向角
   * @param front_buffer 前方缓冲
   * @param rear_buffer 后方缓冲
   * @param left_buffer 左侧缓冲
   * @param right_buffer 右侧缓冲
   * @return 带缓冲的车辆多边形
   */
  static common::math::Polygon2d GetPolygon2dWithBuffer(
      double x, double y, double theta,
      double front_buffer, double rear_buffer,
      double left_buffer, double right_buffer);

 private:
  static VehicleConfig vehicle_config_;
  static bool is_init_;
};

}  // namespace common
}  // namespace TL
