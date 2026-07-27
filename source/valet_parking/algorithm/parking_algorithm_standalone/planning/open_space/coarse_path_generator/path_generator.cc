/*
 * Copyright (c) TL Technologies Co., Ltd. 2022. All rights reserved.
 * Description: path_generator.cc
 */

#include "planning/open_space/coarse_path_generator/path_generator.h"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <limits>
#include <memory>
#include <utility>
#include <iostream>

#include "common/math/double_type.h"
#include "common/math/math_utils.h"
#include "common/math/vec2d.h"
#include <utility>
#include "common/file/log.h"  // 统一日志宏定义

namespace TL {
namespace planning {

using TL::common::math::Vec2d;

PathGenerator::PathGenerator(const HybridAStarConfig& hybrid_a_star_config,
                             const VehicleParam& vehicle_param)
    : warm_start_config_(hybrid_a_star_config),
      vehicle_param_(vehicle_param),
      xy_grid_resolution_(hybrid_a_star_config.xy_grid_resolution),
      phi_grid_resolution_(hybrid_a_star_config.phi_grid_resolution) {}

bool PathGenerator::ValidityCheck(
    const std::shared_ptr<Node3d>& node,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        obstacles_segments_vec,
    double* const distance_to_obstalce) {
  CHECK_NOTNULL(node);
  CHECK_GT(node->GetStepSize(), 0U);
  if (distance_to_obstalce != nullptr) {
    *distance_to_obstalce = std::numeric_limits<double>::infinity();
  }
  if (obstacles_segments_vec.empty()) {
    return true;
  }
  size_t node_step_size = node->GetStepSize();
  const auto& traversed_x = node->GetXs();
  const auto& traversed_y = node->GetYs();
  const auto& traversed_phi = node->GetPhis();

  // 检查所有路径点（不再使用二分采样，确保不遗漏碰撞）
  // 第一个点通常是父节点位置，已检查过，从索引1开始
  size_t check_start_index = (node_step_size > 1) ? 1 : 0;

  // 遍历所有离散点进行碰撞检测
  for (size_t i = check_start_index; i < node_step_size; ++i) {
    // Boundary check
    if (traversed_x[i] > xy_bounds_[1] || traversed_x[i] < xy_bounds_[0] ||
        traversed_y[i] > xy_bounds_[3] || traversed_y[i] < xy_bounds_[2]) {
      return false;
    }

    // Collision detection with obstacles 碰撞检测
    // 计算到障碍物的距离（无论是否需要返回）
    double current_distance = common::math::GetMinDistance2ObstaclesSegments(
        traversed_x[i], traversed_y[i], traversed_phi[i],
        obstacles_segments_vec, obstacle_filter_distance_);

    // 如果调用者需要距离信息，则返回
    if (distance_to_obstalce != nullptr) {
      *distance_to_obstalce = current_distance;
    }

    // 检查是否满足最小安全距离要求
    // 从配置读取，与 hybrid_a_star.cc 保持一致（obstacle_filter_distance）
    // 原值0.5m导致inner_roi_boundary加入obs_segments后全部车位规划失败
    // 原因：廊道宽6m, 两侧各0.5m → 有效5m, 车辆对角线4.86m, 转弯空间不足
    // 当前配置值0.2m: 仅防止物理碰撞, 真实障碍物安全靠per-segment buffer保证
    const double kMinSafeDistance = warm_start_config_.obstacle_filter_distance;
    if (current_distance < kMinSafeDistance) {
      return false;
    }
  }
  return true;
}

void PathGenerator::InsertWithBinaryOrder(
    const std::pair<size_t, size_t>& par,
    std::vector<size_t>* const vec) const {
  if (vec == nullptr || par.second - par.first <= 1) {
    return;
  }
  std::queue<std::pair<size_t, size_t>> q;
  std::pair<size_t, size_t> pt;
  q.push(par);
  while (!q.empty()) {
    pt = q.front();
    q.pop();
    vec->emplace_back(std::floor((pt.second + pt.first) / 2));
    if (static_cast<size_t>(std::floor((pt.second + pt.first) / 2)) >
        pt.first + 1)
      q.push(std::make_pair(pt.first, std::floor((pt.second + pt.first) / 2)));
    if (pt.second >
        static_cast<size_t>(std::floor((pt.second + pt.first) / 2)) + 1)
      q.push(std::make_pair(std::floor((pt.second + pt.first) / 2), pt.second));
  }
}

// ========== PathPartition方法 - 路径分段 ==========
// 【解释】：将Hybrid A*生成的连续路径按前进/后退方向分段
bool PathGenerator::PathPartition(
    const PathGeneratorResult& result,
    std::vector<PathGearPair>* const partition_paths) {
  if (partition_paths == nullptr || result.x.size() < 2) {
    AERROR << "input checck fails";
    return false;
  }
  const auto& x = result.x;
  const auto& y = result.y;
  const auto& phi = result.phi;
  if (x.empty() || x.size() != y.size() || x.size() != phi.size()) {
    AERROR << "states sizes are not equal or empty when do path "
              "partitioning of "
              "Hybrid A Star result";
    return false;
  }
  size_t horizon = x.size();
  partition_paths->clear();
  partition_paths->emplace_back();
  auto* current_path = &(partition_paths->back().first);
  double heading_angle = phi.front();
  const Vec2d init_tracking_vector(x[1] - x[0], y[1] - y[0]);
  double tracking_angle = init_tracking_vector.Angle();
  bool current_gear =
      std::abs(common::math::NormalizeAngle(tracking_angle - heading_angle)) <
      (M_PI_2);
  // 设置初始挡位
  partition_paths->back().second = current_gear ? soc::GearPosition::GEAR_DRIVE
                                                 : soc::GearPosition::GEAR_REVERSE;
  common::PathPoint tmp;
  // to partition the path according to the gears
  for (size_t i = 0; i < horizon - 1; ++i) {
    heading_angle = phi[i];
    const Vec2d tracking_vector(x[i + 1] - x[i], y[i + 1] - y[i]);
    tracking_angle = tracking_vector.Angle();
    bool gear =
        std::abs(common::math::NormalizeAngle(tracking_angle - heading_angle)) <
        (M_PI_2);
    if (gear != current_gear) {
      tmp.x = x[i];
      tmp.y = y[i];
      tmp.theta = phi[i];
      current_path->push_back(tmp);
      partition_paths->emplace_back();
      current_path = &(partition_paths->back().first);
      current_gear = gear;
      // 设置新段的挡位
      partition_paths->back().second = current_gear ? soc::GearPosition::GEAR_DRIVE
                                                     : soc::GearPosition::GEAR_REVERSE;
    }
    tmp.x = x[i];
    tmp.y = y[i];
    tmp.theta = phi[i];
    current_path->push_back(tmp);
  }
  tmp.x = x.back();
  tmp.y = y.back();
  tmp.theta = phi.back();
  current_path->push_back(tmp);

  return true;
}

}  // namespace planning
}  // namespace TL
