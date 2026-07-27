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

/*
 * @file
 */

#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "planning/common/path/discretized_path.h"
#include "planning/open_space/coarse_path_generator/node3d.h"
#include "planning/open_space/coarse_path_generator/path_generator.h"
#include "planning/open_space/coarse_path_generator/shape/reeds_shepp_path.h"

// 使用轻量级依赖替换重量级Apollo依赖
#include "planning/open_space/hybrid_a_star_config.h"
#include "planning/open_space/vehicle_param.h"
#include "planning/open_space/parking_space_info.h"
// #include "planning/common/open_space_info.h"  // REMOVED: brings heavy dependencies (hdmap)
#include "proto_convert/pnc_point_convert.h"
#include "proto_convert/open_space_types_convert.h"  // 独立编译改造：使用简化的类型定义

constexpr double kEpsilon = 1.0e-3;

namespace TL {
namespace planning {

class HybridAStar : public PathGenerator {
 public:
  explicit HybridAStar(const HybridAStarConfig& config, const VehicleParam& vehicle_param);
  ~HybridAStar() override = default;

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
  bool Plan(const std::atomic<bool>& atomic_early_stop_flag,
            const common::PathPoint& start_point,
            const common::PathPoint& end_point,
            const std::vector<double>& xy_bounds,
            const std::vector<std::pair<common::math::LineSegment2d, double>>&
                obstacles_segments_vec,
            const DestRegionWithAng& dest_region_with_angle,
            const PathSearchStrategy& path_search_strategy,
            PathGeneratorResult* result) override;

  /**
   * @brief
   *
   * @param reeds_shepp_path
   * @return double
   */
  double CaculateRsPathSteerMargion(
      const std::shared_ptr<ReedSheppPath>& reeds_shepp_path);

  /**
   * @brief
   *
   * @param reeds_shepp_path
   * @param node_ptr
   * @return double
   */
  double EvaluateRsPathCost(
      const std::shared_ptr<ReedSheppPath>& reeds_shepp_path,
      const std::shared_ptr<Node3d>& node_ptr);

 private:
  void InitParam();

  /**
   * @brief Analytic expansion can reach target or not
   *
   * @param current_node current expand node
   * @param obstacles_segments_vec obstacles which described as line segment
   * @return true reach the target
   * @return false failed in reaching target
   */
  bool AnalyticExpansion(
      const std::shared_ptr<Node3d>& current_node,
      const std::vector<std::pair<common::math::LineSegment2d, double>>&
          obstacles_segments_vec);
  /**
   * @brief check Reeds Shepp path collision and validity
   *
   * @param current_node
   * @param reeds_shepp_to_end reeds shepp path to end
   * @param obstacles_segments_vec obstacles which described as line segment
   * @return true collision free and valid reeds sheep path
   * @return false
   */
  bool RSPCheck(
      const std::shared_ptr<Node3d>& current_node,
      const std::shared_ptr<ReedSheppPath>& reeds_shepp_to_end,
      const std::vector<std::pair<common::math::LineSegment2d, double>>&
          obstacles_segments_vec);

  /**
   * @brief check Reeds Shepp shift counts
   *
   * @param current_node
   * @param reeds_shepp_to_end reeds shepp path to end
   * @param rs_shift_times
   * @param rs_seg_length
   */
  static void RSShiftTimes(
      const std::shared_ptr<Node3d>& current_node,
      const std::shared_ptr<ReedSheppPath>& reeds_shepp_to_end,
      int* rs_shift_times, std::vector<double>* rs_seg_length = nullptr);

  /**
   * @brief load the whole RSP as nodes and add to the close set
   *
   * @param reeds_shepp_to_end reeds shepp path to end
   * @param current_node current expand node
   * @return std::shared_ptr<Node3d>
   */
  std::shared_ptr<Node3d> LoadRSPinCS(
      const std::shared_ptr<ReedSheppPath>& reeds_shepp_to_end,
      const std::shared_ptr<Node3d>& current_node);

  /**
 * @brief
 *
 * @param current_node
 * @param next_node_index
 * @param is_collison_free_exploration
 * @return std::shared_ptr<Node3d>
 */
  std::shared_ptr<Node3d> Next_node_generator(
      const std::shared_ptr<Node3d>& current_node, size_t next_node_index,
      bool is_collison_free_exploration = false);

  /**
 * @brief
 *
 * @param current_node
 * @param next_node
 * @param distance_to_obstalce
 * @param is_collison_free_exploration
 */
  void CalculateNodeCost(const std::shared_ptr<Node3d>& current_node,
                         const std::shared_ptr<Node3d>& next_node,
                         double distance_to_obstalce,
                         bool is_collison_free_exploration = false);

  /**
   * @brief caculate path cost from start to next node
   *
   * @param current_node parrent node of next node
   * @param next_node child node of current node
   * @param double description path cost
   */
  double PathCost(const std::shared_ptr<Node3d>& current_node,
                  const std::shared_ptr<Node3d>& next_node,
                  double distance_to_obstalce) const;

  /**
  * @brief Caculate path length cost
  *
  * @param path_length
  * @return double
  */
  double CalculatePathLengthCost(const double path_length) const {
    constexpr double kA = -15;
    constexpr double kB = 20;
    return warm_start_config_.path_length_penalty /
           (1 + std::exp(kA + kB * path_length));
  }

  /**
   * @brief caculate cost to reference line
   *
   * @param x
   * @param y
   * @param step_size
   * @param reference_line current reference line
   * @return current cost to reference line
   */
  double GetReferenceLineCost(double x, double y, double step_size,
                              const ReferenceLine& reference_line) const;

  /**
  * @brief Get the Result object
  *
  * @param result
  * @param is_collison_free_exploration_path
  * @return true
  * @return false
  */
  bool GetResult(PathGeneratorResult* result,
                 bool is_collison_free_exploration_path = false);
  bool CalculateParkingPrefinishCondition(
      const std::shared_ptr<Node3d>& current_node);
  /**
 * @brief
 *
 * @param start_point
 * @param end_point
 */
  void NodeGenerationParameterDecision(const common::PathPoint& start_point,
                                       const common::PathPoint& end_point);

  /**
 * @brief cut off strategy at forward direction
 *
 * @param node extend node
 * @param steer steer angle
 * @return true need cut off
 * @return false
 */
  inline bool ForwardSteerActionCutoff(const std::shared_ptr<Node3d>& node,
                                       const double steer) const {
    return node->GetY() < max_y_cut_off_ &&
           steer * path_search_strategy_.cut_off_strategy > kEpsilon;
  }

  /**
  * @brief cut off strategy at backward direction
  *
  * @param node extend node
  * @param steer steer angle
  * @return true need cut off
  * @return false
  */
  inline bool BackwardSteerActionCutoff(const std::shared_ptr<Node3d>& node,
                                        const double steer) const {
    return node->GetY() < max_y_cut_off_ &&
           steer * path_search_strategy_.cut_off_strategy < -1.0 * kEpsilon;
  }

  /**
  * @brief plan direction from start or end decision
  *
  * @param start_point
  * @param dest_region_with_angle
  */
  void PlanDirectionDecision(const common::PathPoint& start_point,
                             const DestRegionWithAng& dest_region_with_angle);

  /**
   * @brief  reverse path generator result
   *
   * @param result
   */
  static void ReversePathGeneratorResult(PathGeneratorResult* const result) {
    if (result == nullptr) {
      std::cerr << "ERROR: ReverseHybridAStarResult input check fails" << std::endl;
      return;
    }
    std::reverse(result->x.begin(), result->x.end());
    std::reverse(result->y.begin(), result->y.end());
    std::reverse(result->phi.begin(), result->phi.end());
  }

  /**
 * @brief
 *
 * @param start_point
 * @param obstacles_segments_vec
 * @param forced_path_direction
 * @param result
 * @param intermediate_point_ptr
 * @param has_search_extension_path
 * @return true
 * @return false
 */
  bool GenerateLocalExtensionPath(
      const common::PathPoint& start_point,
      const std::vector<std::pair<common::math::LineSegment2d, double>>&
          obstacles_segments_vec,
      int* forced_path_direction, PathGeneratorResult* result,
      common::PathPoint* intermediate_point_ptr,
      bool* has_search_extension_path);

  size_t next_node_num_ = 0;
  double step_size_ = 0.0;
  double rs_step_size_ = 0.0;
  double max_y_cut_off_ = 0.0;

  std::shared_ptr<Node3d> start_node_;
  std::shared_ptr<Node3d> end_node_;
  std::vector<double> target_region_;
  std::shared_ptr<Node3d> final_node_;
  DestRegionWithAng dest_region_with_angle_;
  std::vector<double> steer_seq_;
  bool is_valid_end_pose_ = true;

  struct cmp {
    bool operator()(const std::pair<std::string, double>& left,
                    const std::pair<std::string, double>& right) const {
      return left.second >= right.second;
    }
  };

  struct node_cmp {
    bool operator()(const std::shared_ptr<Node3d>& left,
                    const std::shared_ptr<Node3d>& right) const {
      return left->GetPathCost() >= right->GetPathCost();
    }
  };

  std::priority_queue<std::shared_ptr<Node3d>,
                      std::vector<std::shared_ptr<Node3d>>, node_cmp>
      end_node_pq_;
  double cur_steer_angle_ = 0.0;

  std::priority_queue<std::pair<std::string, double>,
                      std::vector<std::pair<std::string, double>>, cmp>
      open_pq_;
  std::unordered_map<std::string, std::shared_ptr<Node3d>> open_set_;
  std::unordered_map<std::string, std::shared_ptr<Node3d>> close_set_;
  std::unique_ptr<ReedShepp> reed_shepp_generator_;
  double lateral_rs_steer_angle_margin_ = M_PI_2;
  int explored_failure_times_ = 0;
  int explored_overtime_times_ = 0;

  // 节点生成pruning计数器
  size_t nextgen_forward_length_cutoff_ = 0;
  size_t nextgen_forward_init_cutoff_ = 0;
  size_t nextgen_forward_action_cutoff_ = 0;
  size_t nextgen_backward_length_cutoff_ = 0;
  size_t nextgen_backward_init_cutoff_ = 0;
  size_t nextgen_backward_action_cutoff_ = 0;
  size_t nextgen_backward_nns_cutoff_ = 0;

  // 边界越界剪枝细分计数器
  size_t nextgen_bounds_x_upper_cutoff_ = 0;  // x超出上界
  size_t nextgen_bounds_x_lower_cutoff_ = 0;  // x超出下界
  size_t nextgen_bounds_y_upper_cutoff_ = 0;  // y超出上界
  size_t nextgen_bounds_y_lower_cutoff_ = 0;  // y超出下界
  double max_x_upper_exceed_ = 0.0;  // x上界最大超出距离
  double max_x_lower_exceed_ = 0.0;  // x下界最大超出距离
  double max_y_upper_exceed_ = 0.0;  // y上界最大超出距离
  double max_y_lower_exceed_ = 0.0;  // y下界最大超出距离

  // 终点条件检查计数器
  size_t prefinish_check_count_ = 0;
  size_t prefinish_angle_in_range_count_ = 0;
  size_t prefinish_point_in_polygon_count_ = 0;
  size_t prefinish_angle_fail_count_ = 0;      // 角度不满足次数
  size_t prefinish_position_fail_count_ = 0;   // 位置不满足次数
  double min_dist_to_target_ = 0.0;
  double min_angle_diff_ = 0.0;

  // 碰撞检测细节统计
  size_t collision_detail_print_count_ = 0;    // 碰撞详情打印计数

  // 节点扩展方向统计
  size_t forward_expansion_attempt_ = 0;       // 前进方向扩展尝试
  size_t forward_expansion_success_ = 0;       // 前进方向扩展成功
  size_t backward_expansion_attempt_ = 0;      // 后退方向扩展尝试
  size_t backward_expansion_success_ = 0;      // 后退方向扩展成功

  // 碰撞失败原因统计结构
  struct CollisionFailureStats {
    // 总体统计
    size_t total_collision_checks = 0;          // 总碰撞检测次数
    size_t total_collision_failures = 0;        // 总碰撞失败次数

    // 按失败原因分类
    size_t safety_distance_violation = 0;       // 安全距离不足
    size_t steer_angle_violation = 0;           // 转向角超限
    size_t curvature_violation = 0;             // 曲率超限（转弯半径过小）
    size_t boundary_violation = 0;              // 边界越界
    size_t vehicle_collision = 0;               // 车身直接碰撞

    // 按方向统计
    size_t forward_collision = 0;               // 前进方向碰撞
    size_t backward_collision = 0;              // 后退方向碰撞

    // 按迭代阶段统计（每10000次为一个阶段）
    std::vector<size_t> stage_collisions;       // 各阶段碰撞次数

    // 障碍物距离统计
    double min_obstacle_distance = 1e9;         // 最小障碍物距离
    double sum_obstacle_distance = 0.0;         // 累计距离（用于计算平均值）
    size_t distance_sample_count = 0;           // 距离采样次数
    std::vector<double> recent_distances;       // 最近的障碍物距离（采样）

    // 最常碰撞的障碍物
    std::unordered_map<size_t, size_t> obstacle_hit_count;  // 障碍物ID -> 碰撞次数

    // 采样打印计数
    size_t sample_print_count = 0;              // 已打印的采样数

    CollisionFailureStats() {
      stage_collisions.resize(100, 0);  // 预分配100个阶段（100万次迭代）
      recent_distances.reserve(1000);    // 预留1000个距离采样
    }
  };

  CollisionFailureStats collision_stats_;       // 碰撞失败统计数据

  PathSearchStrategy path_search_strategy_;
  int forced_path_direction_ = 0;
};

}  // namespace planning
}  // namespace TL
