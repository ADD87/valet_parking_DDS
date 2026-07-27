/*
 * Copyright (c) TL Technologies Co., Ltd. 2022. All rights reserved.
 * Description: path_generator.h
 */

#pragma once

#include <algorithm>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "planning/open_space/coarse_path_generator/node3d.h"
#include "planning/open_space/hybrid_a_star_config.h"
#include "planning/open_space/vehicle_param.h"
#include "proto_convert/pnc_point_convert.h"
#include "proto_convert/open_space_types_convert.h"  // 独立编译改造：使用简化的类型定义
#include "proto_convert/chassis_convert.h"            // For GearPosition
#include "planning/common/path/discretized_path.h"   // For DiscretizedPath
#include "planning/open_space/coarse_path_generator/path_generator_result.h"  // PathGeneratorResult（独立头文件，避免循环依赖）
#include "common/math/polygon2d.h"  // For Polygon2d (used in DestRegionWithAng)
#include <tuple>  // For std::tuple

namespace TL {
namespace planning {

// PathGearPair: path with gear information
using PathGearPair = std::pair<DiscretizedPath, soc::GearPosition>;

// DestRegionWithAng: destination region with angle constraints
using DestRegionWithAng = std::tuple<common::math::Polygon2d, double, double>;

// PathGeneratorResult 已移至 path_generator_result.h（避免循环依赖）

class PathGenerator {
 public:
  explicit PathGenerator(const HybridAStarConfig& hybrid_a_star_config,
                         const VehicleParam& vehicle_param);

  virtual ~PathGenerator() = default;

  /**
 * @brief Use hybird a star algorithm to plan a feasible path with kinemic
 * constrain
 *
 * @param atomic_early_stop_flag
 * @param start_point
 * @param end_point
 * @param xy_bounds
 * @param obstacles_segments_vec
 * @param dest_region_with_angle
 * @param path_search_strategy
 * @param result
 * @return true
 * @return false
 */
  virtual bool Plan(
      const std::atomic<bool>& atomic_early_stop_flag,
      const common::PathPoint& start_point, const common::PathPoint& end_point,
      const std::vector<double>& xy_bounds,
      const std::vector<std::pair<common::math::LineSegment2d, double>>&
          obstacles_segments_vec,
      const DestRegionWithAng& dest_region_with_angle,
      const PathSearchStrategy& path_search_strategy,
      PathGeneratorResult* result) = 0;

  /**
   * @brief 路径分段方法 - 将连续路径按前进/后退分段
   * @param result Hybrid A*输出的连续路径
   * @param partition_paths 输出：分段后的路径
   * @return true 分段成功
   * @return false 分段失败
   */
  static bool PathPartition(const PathGeneratorResult& result,
                            std::vector<PathGearPair>* partition_paths);

 protected:
  /**
   * @brief check node is valid or not and return distance value
   *
   * @param node
   * @param obstacles_segments_vec obstacles which described as line segment
   * @param distance_to_obstalce minimal distance to obstacle
   * @return true has collision
   * @return false collision free
   */
  bool ValidityCheck(
      const std::shared_ptr<Node3d>& node,
      const std::vector<std::pair<common::math::LineSegment2d, double>>&
          obstacles_segments_vec,
      double* distance_to_obstalce = nullptr);
  /**
   * @brief insert value into vec with binary order
   *
   * @param par number to be sort
   * @param vec save the new value
   */
  void InsertWithBinaryOrder(const std::pair<size_t, size_t>& par,
                             std::vector<size_t>* vec) const;

  /**
   * @brief
   *
   * @param kappa
   * @return double
   */
  inline double TransKappaToSteering(double kappa) {
    return atan(kappa * vehicle_param_.wheel_base) *
           vehicle_param_.steer_ratio;
  }

  const HybridAStarConfig warm_start_config_;
  const VehicleParam vehicle_param_;

  double obstacle_filter_distance_ = 0.5;
  std::vector<double> xy_bounds_;
  double xy_grid_resolution_ = 0.1;
  double phi_grid_resolution_ = 0.1;
};

}  // namespace planning
}  // namespace TL
