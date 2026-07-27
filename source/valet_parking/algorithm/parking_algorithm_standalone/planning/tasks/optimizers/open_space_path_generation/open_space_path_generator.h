/*
 * Copyright (c) TL Technologies Co., Ltd. 2023. All rights reserved.
 * Description:  open_space_path_generator.h
 * 独立编译改造：移除 proto 依赖，MVP-A 仅使用 HybridAStar
 */

#pragma once

#include <atomic>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "common/math/vec2d.h"
#include "common/math/line_segment2d.h"
#include "common/status/status.h"
#include "planning/common/path/discretized_path.h"
#include "planning/open_space/coarse_path_generator/path_generator.h"
#include "proto_convert/open_space_types_convert.h"          // 独立编译改造：简化类型
#include "planning/open_space/hybrid_a_star_config.h"       // 独立编译改造：HybridAStarConfig
#include "planning/open_space/vehicle_param.h"              // 独立编译改造：VehicleParam

// 独立编译改造：MVP-A 仅使用 HybridAStar，ILQR/GeometricPath 暂不集成
// namespace TL { namespace planning {
// class GeometricPath;
// class ILQR;
// class GeometryPathGenerator;
// }}

namespace TL {
namespace planning {

class OpenSpacePathGenerator {
 public:
  // 独立编译改造：使用 HybridAStarConfig + VehicleParam（替代 WarmStartConfig）
  explicit OpenSpacePathGenerator(const HybridAStarConfig& config,
                                  const VehicleParam& vehicle_param = VehicleParam());

  ~OpenSpacePathGenerator() = default;

  /**
   * @brief Plan - 路径规划主入口
   */
  void Plan(const std::atomic<bool>& atomic_early_stop_flag,
            const OpenSpacePathInput& input, OpenSpacePathOutput* output);

 private:
  /**
   * @brief TransInputToLocalFrame - 将输入转换到局部坐标系
   */
  static void TransInputToLocalFrame(
      double origin_heading, const common::math::Vec2d& origin_point,
      common::PathPoint* start_point, common::PathPoint* end_pose,
      DestRegionWithAng* dest_region_with_angle,
      std::vector<std::pair<common::math::LineSegment2d, double>>*
          obstacles_segments_vec_ptr);

  /**
   * @brief RemoveCollisionVirtualObs - 移除与起点碰撞的虚拟障碍物
   */
  static void RemoveCollisionVirtualObs(
      const common::PathPoint& start_point,
      std::vector<std::pair<common::math::LineSegment2d, double>>*
          obstacles_segments_vec_ptr);

  /**
   * @brief GenerateCoarsePath - 生成粗路径
   */
  TL::common::Status GenerateCoarsePath(
      const std::atomic<bool>& atomic_early_stop_flag,
      const common::PathPoint& start_point_local,
      const common::PathPoint& end_pose_local,
      const std::vector<double>& xy_bounds,
      const std::vector<std::pair<common::math::LineSegment2d, double>>&
          obstacles_segments_vec_local,
      const DestRegionWithAng& dest_region_with_angle_local,
      const PathStrategy& path_strategy, PathGeneratorResult* coarse_path_ptr,
      bool* need_collision_free_smooth);

  /**
   * @brief PathPointNormalizing - 将路径点从全局坐标转到局部坐标
   */
  static void PathPointNormalizing(double rotate_angle,
                                   const common::math::Vec2d& translate_origin,
                                   common::PathPoint* path_point_ptr);

  /**
   * @brief ReferencePointNormalizing - 将参考点从全局坐标转到局部坐标
   */
  static void ReferencePointNormalizing(double rotate_angle,
                                        const common::math::Vec2d& translate_origin,
                                        ReferencePoint* reference_point_ptr);

  /**
   * @brief SeparateGeometryStrategy - 分离几何策略
   */
  static bool SeparateGeometryStrategy(
      const PathSearchStrategy& path_strategy,
      GeometryStrategy* precise_pose_geometry_strategy,
      GeometryStrategy* precise_angle_geometry_strategy);

  /**
   * @brief PathDeNormal - 将路径从局部坐标转回全局坐标
   */
  static void PathDeNormal(const common::math::Vec2d& origin_point,
                           double origin_heading,
                           std::vector<PathGearPair>* partition_paths);

  /**
   * @brief CombineTraceAdjustPath - 合并轨迹调整路径
   */
  void CombineTraceAdjustPath(const DiscretizedPath& trace_path,
                              const PathGeneratorResult& search_path,
                              PathGeneratorResult* path_result);

  const HybridAStarConfig config_;
  std::unique_ptr<PathGenerator> hybrid_a_star_planner_;
  // 独立编译改造 MVP-A：ILQR/GeometricPath 暂不集成，以下成员已移除
  // std::unique_ptr<PathGenerator> geometry_planner_;
  // std::unique_ptr<GeometricPath> geometric_path_planner_;
  // std::unique_ptr<ILQR> ilqr_planner_;
  PathGeneratorResult default_warm_start_path_result_;
};
}  // namespace planning
}  // namespace TL
