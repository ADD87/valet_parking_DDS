/**
 * @file speed_optimizer_utils.h
 * @brief 速度优化器工具函数
 * @description 独立编译改造：从 planning/common/util/util.cc 和 common.cc 提取
 *              用于 open_space_speed_optimizer 模块
 * @date 2025-07-11
 */

#pragma once

#include <cmath>
#include <set>
#include <vector>

#include "common/math/polygon2d.h"
#include "common/math/vec2d.h"
#include "common/configs/vehicle_config_helper.h"
#include "planning/common/open_space_info_lite.h"
#include "planning/common/path/discretized_path.h"

namespace TL {
namespace planning {
namespace speed_optimizer_util {

/**
 * @brief 双三角形构造器
 * 独立编译改造：从 common::util::DoubleTriangleConstructor 提取
 * 用于 FreeSpace 过滤的双三角形区域检测
 *
 * @param x 中心点 x
 * @param y 中心点 y
 * @param theta 朝向角
 * @param triangle_height 三角形高度
 * @param bottom_angle 底角角度（弧度）
 * @param up_triangle 上方三角形输出
 * @param down_triangle 下方三角形输出
 * @return true 构造成功
 */
inline bool DoubleTriangleConstructor(
    double x, double y, double theta, double triangle_height,
    double bottom_angle, common::math::Polygon2d* up_triangle,
    common::math::Polygon2d* down_triangle) {
  if (up_triangle == nullptr || down_triangle == nullptr) {
    return false;
  }

  static constexpr double kEpsilon = 1e-6;
  const double edge_length =
      std::fabs(triangle_height / std::fmax(std::sin(bottom_angle), kEpsilon));

  const auto left_up_edge_unit =
      common::math::Vec2d::CreateUnitVec2d(theta + M_PI - bottom_angle);
  const auto left_up_edge_vec = left_up_edge_unit * edge_length;
  const common::math::Vec2d left_up_point = {x + left_up_edge_vec.x(),
                                              y + left_up_edge_vec.y()};

  const auto right_up_edge_unit =
      common::math::Vec2d::CreateUnitVec2d(theta + bottom_angle);
  const auto right_up_edge_vec = right_up_edge_unit * edge_length;
  const common::math::Vec2d right_up_point = {x + right_up_edge_vec.x(),
                                               y + right_up_edge_vec.y()};

  const auto left_down_edge_unit =
      common::math::Vec2d::CreateUnitVec2d(theta - M_PI + bottom_angle);
  const auto left_down_edge_vec = left_down_edge_unit * edge_length;
  const common::math::Vec2d left_down_point = {x + left_down_edge_vec.x(),
                                                y + left_down_edge_vec.y()};

  const auto right_down_edge_unit =
      common::math::Vec2d::CreateUnitVec2d(theta - bottom_angle);
  const auto right_down_edge_vec = right_down_edge_unit * edge_length;
  const common::math::Vec2d right_down_point = {x + right_down_edge_vec.x(),
                                                 y + right_down_edge_vec.y()};

  *up_triangle = common::math::Polygon2d(
      {{x, y}, right_up_point, left_up_point});
  *down_triangle = common::math::Polygon2d(
      {{x, y}, right_down_point, left_down_point});
  return true;
}

/**
 * @brief 根据路径过滤 FreeSpace 边界段
 * 独立编译改造：从 planning/common/util/util.cc 提取
 * 使用双三角形区域 + 起终点距离过滤
 *
 * @param discretized_path 路径
 * @param all_freespace_segments 所有 FreeSpace 段（由 ROI 阶段预处理）
 * @param freespace_segments 输出：过滤后的有效 FreeSpace 段
 */
inline void UpdateFreeSpaceSegmentsByPath(
    const DiscretizedPath& discretized_path,
    const std::vector<FreeSpaceSegment>& all_freespace_segments,
    std::vector<FreeSpaceSegment>* freespace_segments) {
  if (freespace_segments == nullptr) {
    return;
  }
  freespace_segments->clear();

  if (discretized_path.empty() || all_freespace_segments.empty()) {
    return;
  }

  static constexpr double kTriangleHeight = 5.0;
  static constexpr double kTriangleBottomAngle = 80.0 / 180.0 * M_PI;

  common::math::Polygon2d up_triangle;
  common::math::Polygon2d down_triangle;
  std::set<size_t> valid_segment_index;

  for (const auto& path_point : discretized_path) {
    if (!DoubleTriangleConstructor(
            path_point.x, path_point.y, path_point.theta,
            kTriangleHeight, kTriangleBottomAngle,
            &up_triangle, &down_triangle)) {
      continue;
    }
    auto path_vec2d = common::math::Vec2d(path_point.x, path_point.y);

    const common::math::Vec2d triangle_height_unit =
        common::math::Vec2d::CreateUnitVec2d(path_point.theta + M_PI_2);
    const double half_bottom_edge_width =
        std::fabs(triangle_height_unit.CrossProd(
            up_triangle.points().at(1) - path_vec2d));

    for (size_t i = 0; i < all_freespace_segments.size(); ++i) {
      const auto vec_a =
          all_freespace_segments[i].segment.start() - path_vec2d;
      const auto vec_b =
          all_freespace_segments[i].segment.end() - path_vec2d;
      const double cross_prod_a = triangle_height_unit.CrossProd(vec_a);
      const double cross_prod_b = triangle_height_unit.CrossProd(vec_b);
      const bool is_left_or_right =
          std::fabs(cross_prod_a) > half_bottom_edge_width &&
          std::fabs(cross_prod_b) > half_bottom_edge_width &&
          cross_prod_a * cross_prod_b > 0.0;

      const double inner_prod_a = triangle_height_unit.InnerProd(vec_a);
      const double inner_prod_b = triangle_height_unit.InnerProd(vec_b);
      const bool is_up_or_down =
          std::fabs(inner_prod_a) > kTriangleHeight &&
          std::fabs(inner_prod_b) > kTriangleHeight &&
          inner_prod_a * inner_prod_b > 0.0;

      if (is_left_or_right || is_up_or_down) {
        continue;
      }
      valid_segment_index.emplace(i);
    }
  }

  // 起终点距离过滤
  static constexpr double kFilterDistance = 2.0;
  const auto ego_box_front =
      common::VehicleConfigHelper::GetBoundingBox(discretized_path.front());
  const auto ego_box_end =
      common::VehicleConfigHelper::GetBoundingBox(discretized_path.back());
  for (size_t i = 0; i < all_freespace_segments.size(); ++i) {
    if (ego_box_front.DistanceTo(all_freespace_segments[i].segment) <
            kFilterDistance ||
        ego_box_end.DistanceTo(all_freespace_segments[i].segment) <
            kFilterDistance) {
      valid_segment_index.emplace(i);
    }
  }

  for (const auto index : valid_segment_index) {
    freespace_segments->emplace_back(all_freespace_segments[index]);
  }
}

/**
 * @brief 基于曲率变化(dkappa)计算速度限制
 * 独立编译改造：从 planning/common/util/common.cc 提取
 * 基于方向盘转速限制表查询
 *
 * @param pre_point 前一个路径点
 * @param point 当前路径点
 * @param speed_upper_bound 速度上限（替代 FLAGS_speed_upper_bound）
 * @return 速度限制值
 */
inline double GetDkappaSpeedLimit(const common::PathPoint& pre_point,
                                  const common::PathPoint& point,
                                  double speed_upper_bound) {
  static constexpr double kEpsilon = 0.0001;
  const auto& vehicle_param =
      common::VehicleConfigHelper::GetConfig().vehicle_param();

  // 计算相邻路径点之间的方向盘转角变化量（度）
  const auto delta_steer_angle =
      fabs(atan(vehicle_param.wheel_base * point.kappa) -
           atan(vehicle_param.wheel_base * pre_point.kappa)) *
      vehicle_param.steer_ratio * 180.0 / M_PI;
  const auto delta_s = fabs(point.s - pre_point.s);

  // 检查方向盘转速限制分段表是否配置
  if (!vehicle_param.has_steer_wheel_speed_segment()) {
    return speed_upper_bound;
  }

  const auto& speed_segments =
      vehicle_param.steer_wheel_speed_segment.vehicle_speed_segment;
  const auto& steer_speed_segments =
      vehicle_param.steer_wheel_speed_segment.steering_wheel_speed_limit_segment;

  // 从高速到低速反向遍历：找到方向盘转速不超限的最高车速
  for (int i = static_cast<int>(speed_segments.size()) - 1; i >= 0; --i) {
    double t = delta_s / (speed_segments[i] + kEpsilon);  // 以该车速通过delta_s的时间
    double min_steer_speed = delta_steer_angle / (t + kEpsilon);  // 所需最小方向盘转速
    if (steer_speed_segments[i] > min_steer_speed) {
      return speed_segments[i];  // 该速度档位下方向盘转速够用
    }
  }

  return speed_upper_bound;
}

}  // namespace speed_optimizer_util
}  // namespace planning
}  // namespace TL
