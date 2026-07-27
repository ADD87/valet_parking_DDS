/******************************************************************************
 * Copyright 2019 The TL Authors. All Rights Reserved.
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
// 独立编译改造版：path_handle.cc
// 改造内容：
//   1. Proto include → convert/types include
//   2. Obstacle 类依赖 → 移除（独立版本无感知输入,障碍物碰撞检测桩化）
//   3. OpenSpaceInfo → SpeedOptimizerInput/Output
//   4. config_.xxx() → config_.xxx
//   5. Clock::NowInSeconds() → GetCurrentTimeSeconds()
// 原始文件备份：path_handle_original.cc

#include "planning/tasks/optimizers/open_space_speed_optimizer/path_handle.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <deque>
#include <limits>
#include <memory>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>
#include "common/configs/vehicle_config_helper.h"
#include "common/math/box2d.h"
#include "common/math/double_type.h"
#include "common/math/line_segment2d.h"
#include "common/math/math_utils.h"
#include "common/math/polygon2d.h"
#include "common/math/vec2d.h"
#include "planning/common/open_space_info_lite.h"
#include "planning/common/path/discretized_path.h"
// 独立编译改造：移除 planning/common/util/common.h（会传递引入 obstacle.h → proto/perception）
#include "planning/tasks/optimizers/open_space_speed_optimizer/speed_optimizer_utils.h"
#include "proto_convert/open_space_speed_optimizer_config_convert.h"
#include "proto_convert/planning_internal_convert.h"
#include "proto_convert/pnc_point_convert.h"
#include "proto_convert/vehicle_state_convert.h"

namespace TL {
namespace planning {

using common::math::double_type::DefinitelyGreater;
using common::math::double_type::DefinitelyLess;
using common::math::Vec2d;
using perception::FreeSpaceOut;
using planning_internal::AvpSpeedPlanCollisionInfo;
using planning_internal::PathType;

namespace {
const double kSquareTwo = 1.414;
const double kAmplificationRatio = 1.2;
const double kVehiclLateralBuffer = 0.1;
}  // namespace

// ============================================================================
// Constructor
// ============================================================================
PathHandle::PathHandle(const OpenSpaceSpeedOptimizerConfig& config)
    : config_(config),
      predict_box_size_(static_cast<size_t>(
          config_.prediction_time_period / predict_unit_t_ + 1)),
      speed_limit_unit_s_(config_.speed_limit_unit_s) {
  moving_obstacle_boxes_.assign(
      predict_box_size_, std::vector<common::math::Box2d>());
  wait_replan_start_time_ = GetCurrentTimeSeconds();
  wait_replan_to_init_time_ = GetCurrentTimeSeconds();
  wait_obstacle_start_time_ = GetCurrentTimeSeconds();
}

// ============================================================================
// Init - 场景参数初始化
// ============================================================================
void PathHandle::Init(const planning::OpenSpaceEnvStructuredInfo&
                          open_space_env_structured_info) {
  not_lidar_not_vehicle_fs_segments_.clear();
  not_lidar_vehicle_fs_segments_.clear();
  lidar_not_vehicle_fs_segments_.clear();
  lidar_vehicle_fs_segments_.clear();
  low_height_fs_segments_.clear();
  high_height_curb_fs_segments_.clear();
  not_lidar_not_vehicle_fs_.clear();
  not_lidar_vehicle_fs_.clear();
  lidar_not_vehicle_fs_.clear();
  lidar_vehicle_fs_.clear();
  low_height_fs_.clear();
  high_height_curb_fs_.clear();
  speed_limits_.clear();
  all_freespace_segments_.clear();
  switch (open_space_env_structured_info.parking_scenario_type) {
    case ParkingScenarioType::LEFT_VERTICAL_PARKING_IN:
    case ParkingScenarioType::RIGHT_VERTICAL_PARKING_IN:
    case ParkingScenarioType::LEFT_OBLIQUE_PARKING_IN:
    case ParkingScenarioType::RIGHT_OBLIQUE_PARKING_IN:
      is_vertical_park_in_ = true;
      break;
    case ParkingScenarioType::LEFT_LATERAL_PARKING_IN:
    case ParkingScenarioType::RIGHT_LATERAL_PARKING_IN:
      is_lateral_park_in_ = true;
      break;
    case ParkingScenarioType::LEFT_LATERAL_PARKING_OUT:
    case ParkingScenarioType::RIGHT_LATERAL_PARKING_OUT:
      is_lateral_park_out_ = true;
      break;
    case ParkingScenarioType::FREESPACE_FORWARD_EXPLORATION:
      is_nns_adjust_ = true;
      break;
    default:
      is_lateral_park_in_ = false;
      is_lateral_park_out_ = false;
      is_vertical_park_in_ = false;
      is_nns_adjust_ = false;
      break;
  }
  is_narrow_spot_scenario_ =
      (open_space_env_structured_info.parking_scenario_diffculty_type &
       NARROW_SPOT_SCENARIO) != 0;
}

// ============================================================================
// CutOffPathByWheelMask - 轮挡截断
// ============================================================================
bool PathHandle::CutOffPathByWheelMask(const DiscretizedPath& path,
                                       const bool is_forward,
                                       const bool is_parking_inwards,
                                       const bool is_consider_wheel_mask,
                                       const Box2d& wheel_mask_box,
                                       DiscretizedPath* const new_path) {
  if (path.empty() || nullptr == new_path) {
    return false;
  }
  if ((!is_forward && is_parking_inwards) ||
      (is_forward && !is_parking_inwards) || !is_consider_wheel_mask ||
      !(wheel_mask_box.area() > 0)) {
    pre_wheel_mask_valid_ = false;
    *new_path = path;
    return true;
  }

  size_t wheel_mask_index = path.size();
  const double half_width =
      0.5 * common::VehicleConfigHelper::GetConfig().vehicle_param().width;
  auto wheel_mask_center = wheel_mask_box.center();
  if (is_forward && is_parking_inwards) {
    double slot_heading =
        common::math::NormalizeAngle(path.back().theta + M_PI);
    const double wheel_base =
        common::VehicleConfigHelper::GetConfig().vehicle_param().wheel_base;
    wheel_mask_center =
        wheel_mask_center + wheel_base * Vec2d::CreateUnitVec2d(slot_heading);
  }

  common::SLPoint sl_point;
  if (path.XYToSL(wheel_mask_center.x(), wheel_mask_center.y(), &sl_point)) {
    sl_point.s = (sl_point.s - config_.wheel_mask_to_wheel_base_distance);
    if (DefinitelyLess(sl_point.s, path.back().s) &&
        DefinitelyLess(path.front().s, sl_point.s) &&
        DefinitelyLess(fabs(sl_point.l), half_width)) {
      wheel_mask_index = std::distance(
          path.begin(), std::lower_bound(path.begin(), path.end(), sl_point.s,
                                         [](const common::PathPoint& p,
                                            double s) { return p.s < s; }));
    }
    if (pre_wheel_mask_valid_ && DefinitelyLess(sl_point.s, 0) &&
        DefinitelyLess(fabs(sl_point.l), half_width)) {
      wheel_mask_index = 0;
    }
  }

  pre_wheel_mask_valid_ = wheel_mask_index < path.size();
  *new_path = DiscretizedPath(std::vector<common::PathPoint>(
      path.begin(), path.begin() + static_cast<int>(wheel_mask_index)));
  if (wheel_mask_index > 0 && wheel_mask_index < path.size()) {
    new_path->emplace_back(path.Evaluate(sl_point.s));
  }

  return !new_path->empty();
}

// ============================================================================
// UpdateFreeSpaceInfo - FreeSpace信息更新（保留完整逻辑）
// ============================================================================
void PathHandle::UpdateFreeSpaceInfo(
    const DiscretizedPath& path,
    const std::shared_ptr<const FreeSpaceOutArray>& freespace_out_array,
    const std::vector<size_t>& under_spot_low_fs_idxs,
    const std::vector<size_t>& high_curb_fs_idxs,
    const std::vector<std::pair<size_t, std::vector<size_t>>>& ignore_fs_idxs) {
  if (path.empty() || nullptr == freespace_out_array) {
    return;
  }

  // 独立编译改造：FreeSpaceOutArray 为空桩，此处 segments 将为空
  std::vector<FreeSpaceSegment> freespace_segments;
  speed_optimizer_util::UpdateFreeSpaceSegmentsByPath(
      path, freespace_segments, &freespace_segments);
  ADEBUG << "freespace segments size: " << freespace_segments.size();
  auto is_lidar_fs = [](FreeSpaceOut::SensorType type) {
    return FreeSpaceOut::LIDAR == type ||
           FreeSpaceOut::LIDAR_CAM_FUSION == type ||
           FreeSpaceOut::USS_LIDAR_CAM_FUSION == type ||
           FreeSpaceOut::USS_LIDAR_FUSION == type;
  };

  for (const auto& freespace_segment : freespace_segments) {
    auto index_in_freespace_out = freespace_segment.index_in_freespace_out;
    auto index_in_keypoints = freespace_segment.index_in_keypoints;
    auto ignore_iter =
        std::find_if(ignore_fs_idxs.begin(), ignore_fs_idxs.end(),
                     [&index_in_freespace_out](const auto& p) {
                       return p.first == index_in_freespace_out;
                     });
    if (ignore_iter != ignore_fs_idxs.end() &&
        (std::find(ignore_iter->second.begin(), ignore_iter->second.end(),
                   index_in_keypoints) != ignore_iter->second.end() ||
         std::find(ignore_iter->second.begin(), ignore_iter->second.end(),
                   index_in_keypoints - 1) != ignore_iter->second.end())) {
      continue;
    }
    auto is_low_fs =
        std::find(under_spot_low_fs_idxs.begin(), under_spot_low_fs_idxs.end(),
                  index_in_freespace_out) != under_spot_low_fs_idxs.end();
    auto is_high_curb_fs =
        std::find(high_curb_fs_idxs.begin(), high_curb_fs_idxs.end(),
                  index_in_freespace_out) != high_curb_fs_idxs.end();
    auto is_lidar = is_lidar_fs(freespace_segment.sensor_type);
    auto is_vehicle = FreeSpaceOut::VEHICLE == freespace_segment.cls_type;
    std::pair<common::math::LineSegment2d, double> temp;
    temp.first = freespace_segment.segment;
    temp.second = 0.0;
    all_freespace_segments_.emplace_back(freespace_segment.segment,
                                         config_.path_collision_check_buffer);
    if (is_low_fs) {
      low_height_fs_segments_.emplace_back(freespace_segment);
      low_height_fs_.emplace_back(std::move(temp));
    } else if (!is_lidar && !is_vehicle) {
      not_lidar_not_vehicle_fs_segments_.emplace_back(freespace_segment);
      not_lidar_not_vehicle_fs_.emplace_back(std::move(temp));
    } else if (!is_lidar && is_vehicle) {
      not_lidar_vehicle_fs_segments_.emplace_back(freespace_segment);
      not_lidar_vehicle_fs_.emplace_back(std::move(temp));
    } else if (is_lidar && is_vehicle) {
      lidar_vehicle_fs_segments_.emplace_back(freespace_segment);
      lidar_vehicle_fs_.emplace_back(std::move(temp));
    } else if (is_lidar && !is_vehicle) {
      lidar_not_vehicle_fs_segments_.emplace_back(freespace_segment);
      lidar_not_vehicle_fs_.emplace_back(std::move(temp));
    }
    if (is_high_curb_fs) {
      high_height_curb_fs_segments_.emplace_back(freespace_segment);
      high_height_curb_fs_.emplace_back(std::move(temp));
    }
  }
}

// ============================================================================
// UpdateCollisionBuffer - 碰撞缓冲区参数选择
// ============================================================================
OpenSpaceSpeedOptimizerConfig::CollisionBufferInfo
PathHandle::UpdateCollisionBuffer(const bool is_forward) const {
  OpenSpaceSpeedOptimizerConfig::CollisionBufferInfo collision_buffer =
      is_forward ? config_.vertical_forward_collision_buffer
                 : config_.vertical_backward_collision_buffer;
  if (is_lateral_park_in_ || is_lateral_park_out_) {
    collision_buffer = is_forward
                           ? config_.parallel_forward_collision_buffer
                           : config_.parallel_backward_collision_buffer;
  }
  return collision_buffer;
}

// ============================================================================
// CalLateralBufferByControlDiff - 控制偏差的横向缓冲补偿
// ============================================================================
void PathHandle::CalLateralBufferByControlDiff(
    const common::VehicleState& vehicle_state,
    const common::PathPoint& path_p, double* const left_lateral_buffer,
    double* const right_lateral_buffer) {
  if (nullptr == left_lateral_buffer || nullptr == right_lateral_buffer) {
    return;
  }
  double dl = vehicle_state.y - path_p.y;
  double dtheta = vehicle_state.heading - path_p.theta;
  if (DefinitelyGreater(dl, 0.0) && DefinitelyGreater(dtheta, 0.0)) {
    *left_lateral_buffer = std::fmax(0.0, *left_lateral_buffer - fabs(dl));
  } else if (DefinitelyLess(dl, 0.0) && DefinitelyLess(dtheta, 0.0)) {
    *right_lateral_buffer = std::fmax(0.0, *right_lateral_buffer - fabs(dl));
  }
}

// ============================================================================
// IsCollisionWithStaticObstacle - 简化版静态障碍物碰撞检测
// 独立编译改造：替代原实现（依赖 Obstacle 类）
// 使用 SimpleStaticObstacle 的 Box2d 与车辆 Polygon2d 做 overlap 检测
// 参考 path_handle_original.cc 的碰撞缓冲逻辑
// ============================================================================
bool PathHandle::IsCollisionWithStaticObstacle(
    const DiscretizedPath& path, const bool is_forward,
    CollisionInfo::ObstacleCollisionInfo* const collision_info) {
  if (path.empty() || static_obstacles_.empty() || nullptr == collision_info) {
    return false;
  }
  collision_info->Init(path.size());

  const auto& vehicle_param =
      common::VehicleConfigHelper::GetConfig().vehicle_param();
  // 车辆几何中心偏移（后轴中点到几何中心）
  const double diff = (vehicle_param.front_edge_to_center -
                       vehicle_param.back_edge_to_center) / 2.0;

  for (size_t i = 0; i < path.size(); ++i) {
    const auto& pp = path.at(i);
    for (const auto& obs : static_obstacles_) {
      // 根据障碍物类型选择碰撞缓冲
      double buffer = config_.collision_buffer_for_other;
      if (obs.type == PerceptionObstacle_Type_PEDESTRIAN) {
        buffer = config_.collision_buffer_for_static_pedestrian;
      } else if (obs.type == PerceptionObstacle_Type_VEHICLE) {
        buffer = config_.collision_buffer_for_static_vehicle;
      }

      // 方向感知缓冲：前进时前方加大缓冲，后退时后方加大缓冲
      double front_buf = is_forward ? buffer : 0.0;
      double rear_buf = is_forward ? 0.0 : buffer;

      // 构造带缓冲的车辆 Box2d
      double ego_length = vehicle_param.length + front_buf + rear_buf;
      double ego_width = vehicle_param.width + buffer * 2.0;
      double center_offset = diff + (front_buf - rear_buf) / 2.0;
      common::math::Vec2d ego_center(
          pp.x + center_offset * std::cos(pp.theta),
          pp.y + center_offset * std::sin(pp.theta));
      common::math::Box2d ego_box(ego_center, pp.theta, ego_length, ego_width);

      // Box2d 对 Box2d 碰撞检测
      if (ego_box.HasOverlap(obs.bounding_box)) {
        collision_info->is_collision = true;
        collision_info->collision_index = i;
        collision_info->collision_obstacle_id = obs.id;
        collision_info->collision_obstacle_type = obs.type;
        return true;
      }
    }
  }
  return false;
}

// ============================================================================
// UpdateMovingObstacleBoxes - 动态障碍物预测盒缓存
// standalone 对齐原版 moving_obs_boxs_ 结构：
// 以固定时间分辨率预生成每个 moving obstacle 的未来 box。
// ============================================================================
void PathHandle::UpdateMovingObstacleBoxes() {
  moving_obstacle_boxes_.assign(
      predict_box_size_,
      std::vector<common::math::Box2d>(moving_obstacles_.size()));

  for (size_t step = 0; step < predict_box_size_; ++step) {
    const double predict_t = predict_unit_t_ * static_cast<double>(step);
    for (size_t obstacle_index = 0; obstacle_index < moving_obstacles_.size();
         ++obstacle_index) {
      const auto& obstacle = moving_obstacles_.at(obstacle_index);
      const common::math::Vec2d predicted_center(
          obstacle.bounding_box.center_x() + obstacle.velocity_x * predict_t,
          obstacle.bounding_box.center_y() + obstacle.velocity_y * predict_t);
      moving_obstacle_boxes_[step][obstacle_index] = common::math::Box2d(
          predicted_center, obstacle.bounding_box.heading(),
          obstacle.bounding_box.length(), obstacle.bounding_box.width());
    }
  }
}

// ============================================================================
// IsCollisionWithMovingObstacle - standalone 最小动态障碍物碰撞检测
// 使用预生成的 moving obstacle 预测包围盒与车辆包围盒做 overlap。
// ============================================================================
bool PathHandle::IsCollisionWithMovingObstacle(
    const DiscretizedPath& path, const bool is_forward,
    CollisionInfo::ObstacleCollisionInfo* const collision_info) {
  if (path.empty() || moving_obstacles_.empty() || nullptr == collision_info) {
    return false;
  }
  collision_info->Init(path.size());

  const auto& vehicle_param =
      common::VehicleConfigHelper::GetConfig().vehicle_param();
  const double diff = (vehicle_param.front_edge_to_center -
                       vehicle_param.back_edge_to_center) / 2.0;

  for (size_t i = 0; i < path.size(); ++i) {
    const auto& pp = path.at(i);
    for (size_t obstacle_index = 0; obstacle_index < moving_obstacles_.size();
         ++obstacle_index) {
      const auto& obs = moving_obstacles_.at(obstacle_index);
      const double long_buffer = is_forward
          ? config_.apa_moving_obstacle_front_buffer
          : config_.apa_moving_obstacle_rear_buffer;
      const double lat_buffer = config_.apa_moving_obstacle_left_right_buffer;

      const double ego_length = vehicle_param.length + long_buffer;
      const double ego_width = vehicle_param.width + lat_buffer * 2.0;
      common::math::Vec2d ego_center(
          pp.x + diff * std::cos(pp.theta),
          pp.y + diff * std::sin(pp.theta));
      common::math::Box2d ego_box(ego_center, pp.theta, ego_length, ego_width);

      for (size_t step = 0; step < predict_box_size_; ++step) {
        if (step >= moving_obstacle_boxes_.size()) {
          break;
        }
        if (obstacle_index >= moving_obstacle_boxes_[step].size()) {
          continue;
        }

        if (ego_box.HasOverlap(moving_obstacle_boxes_[step][obstacle_index])) {
          collision_info->is_collision = true;
          collision_info->collision_index = i;
          collision_info->collision_obstacle_id = obs.id;
          collision_info->collision_obstacle_type = obs.type;
          return true;
        }
      }
    }
  }
  return false;
}

// ============================================================================
// IsCollisionWithFreeSpaceSegment - FreeSpace碰撞检测（完整保留）
// ============================================================================
bool PathHandle::IsCollisionWithFreeSpaceSegment(
    const DiscretizedPath& path,
    const OpenSpaceSpeedOptimizerConfig::CollisionBufferInfo& collision_buffer,
    const double left_control_diff, const double right_control_diff,
    const bool is_use_middle_buffer, const bool is_mirror_fold,
    const planning_internal::PathType& path_type,
    const size_t cur_path_idx,
    CollisionInfo::FreeSpaceCollisionInfo* const collision_info) {
  if (nullptr == collision_info || path.empty()) {
    return false;
  }

  std::pair<double, double> lateral_buffer_for_vehicle;
  std::pair<double, double> lateral_buffer_for_not_vehicle;
  std::pair<double, double> lateral_buffer_for_low_fs;
  UpdateLateralBuffer(collision_buffer, is_use_middle_buffer,
                      is_narrow_spot_scenario_, is_lateral_park_in_,
                      is_lateral_park_out_, path_type, cur_path_idx,
                      &lateral_buffer_for_vehicle,
                      &lateral_buffer_for_not_vehicle,
                      &lateral_buffer_for_low_fs);

  auto front_longitudinal_buffer = collision_buffer.front_longitudinal_buffer;
  auto rear_longitudinal_buffer = collision_buffer.rear_longitudinal_buffer;
  auto front_longitudinal_buffer_lidar =
      collision_buffer.front_longitudinal_buffer_lidar;
  auto rear_longitudinal_buffer_lidar =
      collision_buffer.rear_longitudinal_buffer_lidar;
  auto front_longitudinal_buffer_low_fs =
      collision_buffer.front_longitudinal_buffer_low_fs;
  auto rear_longitudinal_buffer_low_fs =
      collision_buffer.rear_longitudinal_buffer_low_fs;

  size_t collision_segment_index = 0;
  for (size_t i = 0; i < path.size(); i++) {
    const auto& path_point = path.at(i);
    auto polygon2d_center =
        Vec2d(path_point.x, path_point.y) +
        common::VehicleConfigHelper::GetRearToCenter() *
            Vec2d::CreateUnitVec2d(path_point.theta);
    auto unit_vec2d = Vec2d::CreateUnitVec2d(path_point.theta);
    const double euclidean_filter_distance =
        common::VehicleConfigHelper::GetDiagonal() * 0.5;
    const double longitudinal_filter_distance =
        common::VehicleConfigHelper::GetConfig()
            .vehicle_param()
            .front_edge_to_center;
    const double lateral_filter_distance =
        common::VehicleConfigHelper::GetConfig()
            .vehicle_param()
            .width *
        0.5;

    // not_lidar not_vehicle freespace check
    auto polygon2d = common::VehicleConfigHelper::GetPolygon2dWithBuffer(
        path_point.x, path_point.y, path_point.theta,
        front_longitudinal_buffer, rear_longitudinal_buffer,
        lateral_buffer_for_not_vehicle.first - left_control_diff,
        lateral_buffer_for_not_vehicle.second - right_control_diff);
    if (common::math::CheckCollisionWithVehiclePolygon2d(
            polygon2d, polygon2d_center, unit_vec2d,
            not_lidar_not_vehicle_fs_,
            euclidean_filter_distance + front_longitudinal_buffer * kSquareTwo *
                                            kAmplificationRatio,
            longitudinal_filter_distance +
                front_longitudinal_buffer * kAmplificationRatio,
            lateral_filter_distance +
                lateral_buffer_for_not_vehicle.first * kAmplificationRatio,
            &collision_segment_index)) {
      collision_info->is_collision = true;
      collision_info->collision_index = i;
      if (collision_segment_index <
          not_lidar_not_vehicle_fs_segments_.size()) {
        collision_info->freespace_segment =
            not_lidar_not_vehicle_fs_segments_[collision_segment_index];
      }
      auto flu_point = common::math::ENUToFLU(
          not_lidar_not_vehicle_fs_[collision_segment_index].first.start().x(),
          not_lidar_not_vehicle_fs_[collision_segment_index].first.start().y(),
          path_point.x, path_point.y, path_point.theta);
      collision_info->freespace_flu_point.x = flu_point.first;
      collision_info->freespace_flu_point.y = flu_point.second;
      return true;
    }

    // not_lidar vehicle freespace check
    polygon2d = common::VehicleConfigHelper::GetPolygon2dWithBuffer(
        path_point.x, path_point.y, path_point.theta,
        front_longitudinal_buffer, rear_longitudinal_buffer,
        lateral_buffer_for_vehicle.first - left_control_diff,
        lateral_buffer_for_vehicle.second - right_control_diff);
    if (common::math::CheckCollisionWithVehiclePolygon2d(
            polygon2d, polygon2d_center, unit_vec2d,
            not_lidar_vehicle_fs_,
            euclidean_filter_distance + front_longitudinal_buffer * kSquareTwo *
                                            kAmplificationRatio,
            longitudinal_filter_distance +
                front_longitudinal_buffer * kAmplificationRatio,
            lateral_filter_distance +
                lateral_buffer_for_vehicle.first * kAmplificationRatio,
            &collision_segment_index)) {
      collision_info->is_collision = true;
      collision_info->collision_index = i;
      if (collision_segment_index < not_lidar_vehicle_fs_segments_.size()) {
        collision_info->freespace_segment =
            not_lidar_vehicle_fs_segments_[collision_segment_index];
      }
      auto flu_point = common::math::ENUToFLU(
          not_lidar_vehicle_fs_[collision_segment_index].first.start().x(),
          not_lidar_vehicle_fs_[collision_segment_index].first.start().y(),
          path_point.x, path_point.y, path_point.theta);
      collision_info->freespace_flu_point.x = flu_point.first;
      collision_info->freespace_flu_point.y = flu_point.second;
      return true;
    }

    // lidar vehicle freespace check
    polygon2d = common::VehicleConfigHelper::GetPolygon2dWithBuffer(
        path_point.x, path_point.y, path_point.theta,
        front_longitudinal_buffer_lidar, rear_longitudinal_buffer_lidar,
        lateral_buffer_for_vehicle.first - left_control_diff,
        lateral_buffer_for_vehicle.second - right_control_diff);
    if (common::math::CheckCollisionWithVehiclePolygon2d(
            polygon2d, polygon2d_center, unit_vec2d,
            lidar_vehicle_fs_,
            euclidean_filter_distance +
                front_longitudinal_buffer_lidar * kSquareTwo *
                    kAmplificationRatio,
            longitudinal_filter_distance +
                front_longitudinal_buffer_lidar * kAmplificationRatio,
            lateral_filter_distance +
                lateral_buffer_for_vehicle.first * kAmplificationRatio,
            &collision_segment_index)) {
      collision_info->is_collision = true;
      collision_info->collision_index = i;
      if (collision_segment_index < lidar_vehicle_fs_segments_.size()) {
        collision_info->freespace_segment =
            lidar_vehicle_fs_segments_[collision_segment_index];
      }
      auto flu_point = common::math::ENUToFLU(
          lidar_vehicle_fs_[collision_segment_index].first.start().x(),
          lidar_vehicle_fs_[collision_segment_index].first.start().y(),
          path_point.x, path_point.y, path_point.theta);
      collision_info->freespace_flu_point.x = flu_point.first;
      collision_info->freespace_flu_point.y = flu_point.second;
      return true;
    }

    // lidar not_vehicle freespace check
    polygon2d = common::VehicleConfigHelper::GetPolygon2dWithBuffer(
        path_point.x, path_point.y, path_point.theta,
        front_longitudinal_buffer_lidar, rear_longitudinal_buffer_lidar,
        lateral_buffer_for_not_vehicle.first - left_control_diff,
        lateral_buffer_for_not_vehicle.second - right_control_diff);
    if (common::math::CheckCollisionWithVehiclePolygon2d(
            polygon2d, polygon2d_center, unit_vec2d,
            lidar_not_vehicle_fs_,
            euclidean_filter_distance +
                front_longitudinal_buffer_lidar * kSquareTwo *
                    kAmplificationRatio,
            longitudinal_filter_distance +
                front_longitudinal_buffer_lidar * kAmplificationRatio,
            lateral_filter_distance +
                lateral_buffer_for_not_vehicle.first * kAmplificationRatio,
            &collision_segment_index)) {
      collision_info->is_collision = true;
      collision_info->collision_index = i;
      if (collision_segment_index < lidar_not_vehicle_fs_segments_.size()) {
        collision_info->freespace_segment =
            lidar_not_vehicle_fs_segments_[collision_segment_index];
      }
      auto flu_point = common::math::ENUToFLU(
          lidar_not_vehicle_fs_[collision_segment_index].first.start().x(),
          lidar_not_vehicle_fs_[collision_segment_index].first.start().y(),
          path_point.x, path_point.y, path_point.theta);
      collision_info->freespace_flu_point.x = flu_point.first;
      collision_info->freespace_flu_point.y = flu_point.second;
      return true;
    }

    // low height freespace check
    polygon2d = common::VehicleConfigHelper::GetPolygon2dWithBuffer(
        path_point.x, path_point.y, path_point.theta,
        front_longitudinal_buffer_low_fs, rear_longitudinal_buffer_low_fs,
        lateral_buffer_for_low_fs.first - left_control_diff,
        lateral_buffer_for_low_fs.second - right_control_diff);
    if (common::math::CheckCollisionWithVehiclePolygon2d(
            polygon2d, polygon2d_center, unit_vec2d,
            low_height_fs_,
            euclidean_filter_distance +
                front_longitudinal_buffer_low_fs * kSquareTwo *
                    kAmplificationRatio,
            longitudinal_filter_distance +
                front_longitudinal_buffer_low_fs * kAmplificationRatio,
            lateral_filter_distance +
                lateral_buffer_for_low_fs.first * kAmplificationRatio,
            &collision_segment_index)) {
      collision_info->is_collision = true;
      collision_info->collision_index = i;
      if (collision_segment_index < low_height_fs_segments_.size()) {
        collision_info->freespace_segment =
            low_height_fs_segments_[collision_segment_index];
      }
      auto flu_point = common::math::ENUToFLU(
          low_height_fs_[collision_segment_index].first.start().x(),
          low_height_fs_[collision_segment_index].first.start().y(),
          path_point.x, path_point.y, path_point.theta);
      collision_info->freespace_flu_point.x = flu_point.first;
      collision_info->freespace_flu_point.y = flu_point.second;
      return true;
    }

    // mirror check (high curb)
    if (!is_mirror_fold) {
      const auto circles =
          common::VehicleConfigHelper::GetMirrorCirclesWithBuffer(
              path_point.x, path_point.y, path_point.theta);
      if (circles.size() == 2) {
        if (common::math::CheckCollisionWithCircle(
                circles.front(), high_height_curb_fs_,
                &collision_segment_index)) {
          collision_info->is_collision = true;
          collision_info->collision_index = i;
          if (collision_segment_index < high_height_curb_fs_segments_.size()) {
            collision_info->freespace_segment =
                high_height_curb_fs_segments_[collision_segment_index];
          }
          return true;
        }
        if (common::math::CheckCollisionWithCircle(
                circles.back(), high_height_curb_fs_,
                &collision_segment_index)) {
          collision_info->is_collision = true;
          collision_info->collision_index = i;
          if (collision_segment_index < high_height_curb_fs_segments_.size()) {
            collision_info->freespace_segment =
                high_height_curb_fs_segments_[collision_segment_index];
          }
          return true;
        }
      }
    }
  }

  return false;
}

// ============================================================================
// CollisionInfoDecision - 综合碰撞判定（完整保留）
// ============================================================================
void PathHandle::CollisionInfoDecision(CollisionInfo* const collision_info) {
  if (nullptr == collision_info) {
    return;
  }
  collision_info->is_collision =
      collision_info->moving_obstacle_collision_info.is_collision ||
      collision_info->static_obstacle_collision_info.is_collision ||
      collision_info->freespace_collision_info.is_collision ||
      collision_info->outside_wheelmask_obstacle_collision_info.is_collision;
  collision_info->first_collision_index =
      std::min(std::initializer_list<size_t>{
          collision_info->moving_obstacle_collision_info.collision_index,
          collision_info->static_obstacle_collision_info.collision_index,
          collision_info->freespace_collision_info.collision_index,
          collision_info->outside_wheelmask_obstacle_collision_info
              .collision_index});
  if (collision_info->moving_obstacle_collision_info.is_collision) {
    collision_info->collision_type =
        AvpSpeedPlanCollisionInfo::MOVING_OBSTACLE_COLLISION;
  }
  if (collision_info->static_obstacle_collision_info.is_collision ||
      collision_info->outside_wheelmask_obstacle_collision_info.is_collision) {
    collision_info->collision_type =
        AvpSpeedPlanCollisionInfo::NO_COLLISION ==
                collision_info->collision_type
            ? AvpSpeedPlanCollisionInfo::STATIC_OBSTACLE_COLLISION
            : AvpSpeedPlanCollisionInfo::FUSION_COLLISION;
  }
  if (collision_info->freespace_collision_info.is_collision) {
    collision_info->collision_type =
        AvpSpeedPlanCollisionInfo::NO_COLLISION ==
                collision_info->collision_type
            ? AvpSpeedPlanCollisionInfo::FREESPACE_POINT_COLLISION
            : AvpSpeedPlanCollisionInfo::FUSION_COLLISION;
  }
  if (collision_info->freespace_collision_info.is_collision &&
      fabs(collision_info->freespace_collision_info.freespace_flu_point.y) >
          0.5 * common::VehicleConfigHelper::GetConfig()
                    .vehicle_param()
                    .width) {
    collision_info->stop_reserve_distance =
        config_.lateral_collision_stop_buffer;
  }
}

// ============================================================================
// UpdateCollisionInfo - 碰撞信息更新（移除Obstacle依赖）
// ============================================================================
bool PathHandle::UpdateCollisionInfo(
    const DiscretizedPath& path,
    const std::shared_ptr<const FreeSpaceOutArray>& freespace_out_array,
    const std::vector<size_t>& under_spot_low_fs_idxs,
    const std::vector<size_t>& high_curb_fs_idxs,
    const std::vector<std::pair<size_t, std::vector<size_t>>>& ignore_fs_idxs,
    const common::VehicleState& vehicle_state, const bool is_forward,
    const bool is_rpa_direct_mode, const bool is_mirror_fold,
    const PartitionedPath& partitioned_paths,
    CollisionInfo* const collision_info) {
  if (nullptr == collision_info || path.empty()) {
    return false;
  }
  collision_info->Init(path.size());

  // 独立编译改造：仅更新 FreeSpace（无 Obstacle 类可用）
  UpdateFreeSpaceInfo(path, freespace_out_array, under_spot_low_fs_idxs,
                      high_curb_fs_idxs, ignore_fs_idxs);

  auto collision_buffer = UpdateCollisionBuffer(is_forward);
  double left_control_diff = 0.0;
  double right_control_diff = 0.0;
  CalLateralBufferByControlDiff(vehicle_state, path.front(), &left_control_diff,
                                &right_control_diff);

  // 独立编译改造：Obstacle碰撞检测恢复
  // 使用 SimpleStaticObstacle 替代原 Obstacle* 指针
  if (!static_obstacles_.empty()) {
    IsCollisionWithStaticObstacle(
        path, is_forward,
        &collision_info->static_obstacle_collision_info);
  }
  if (!moving_obstacles_.empty()) {
    IsCollisionWithMovingObstacle(
        path, is_forward,
        &collision_info->moving_obstacle_collision_info);
  }
  // WheelMaskObstacle → 独立版本无额外感知数据，跳过

  // FreeSpace碰撞检测完整保留
  IsCollisionWithFreeSpaceSegment(
      path, collision_buffer, left_control_diff, right_control_diff,
      is_use_middle_buffer_, is_mirror_fold, partitioned_paths.path_type,
      partitioned_paths.path_idx, &collision_info->freespace_collision_info);
  CollisionInfoDecision(collision_info);
  UpdatePathCollisionRiskCount(path, partitioned_paths.path_type,
                               partitioned_paths.path_idx, is_mirror_fold,
                               is_forward);

  CollisionInfo::FreeSpaceCollisionInfo tmp;
  bool is_bigger_buffer_safe = true;
  if (is_use_middle_buffer_) {
    is_bigger_buffer_safe = !IsCollisionWithFreeSpaceSegment(
        path, collision_buffer, left_control_diff, right_control_diff, false,
        is_mirror_fold, partitioned_paths.path_type, partitioned_paths.path_idx,
        &tmp);
  }
  bigger_buffer_safe_count_ = is_use_middle_buffer_ && is_bigger_buffer_safe
                                  ? bigger_buffer_safe_count_ + 1
                                  : 0;
  is_use_middle_buffer_ =
      is_use_middle_buffer_ &&
      bigger_buffer_safe_count_ <= config_.bigger_buffer_safe_min_count;

  return true;
}

// ============================================================================
// UpdateInteractiveStage - 交互状态机（Clock→chrono）
// ============================================================================
void PathHandle::UpdateInteractiveStage(
    const bool is_vehicle_still, const bool is_rpa_direct_mode,
    const CollisionInfo& collision_info,
    AvpSpeedPlanCollisionInfo::SpeedTaskInteractiveStage* interactive_stage) {
  static constexpr double kMinMoveDisatnce = 0.3;
  if (nullptr == interactive_stage) {
    return;
  }
  auto collision_type = collision_info.collision_type;
  auto is_restore_running = [&]() {
    return AvpSpeedPlanCollisionInfo::NO_COLLISION != collision_type
               ? DefinitelyLess(collision_info.stop_reserve_distance +
                                    config_.restore_dis_buffer,
                                collision_info.curr_collision_distance)
               : true;
  };

  switch (*interactive_stage) {
    case AvpSpeedPlanCollisionInfo::INIT:
      if (is_vehicle_still &&
          AvpSpeedPlanCollisionInfo::NO_COLLISION != collision_type &&
          DefinitelyLess(
              collision_info.curr_collision_distance,
              collision_info.stop_reserve_distance + kMinMoveDisatnce)) {
        if (!config_.enable_wait_for_replan ||
            AvpSpeedPlanCollisionInfo::MOVING_OBSTACLE_COLLISION ==
                collision_type) {
          *interactive_stage = AvpSpeedPlanCollisionInfo::WAITOBSTACLE;
        } else {
          *interactive_stage = AvpSpeedPlanCollisionInfo::WAITREPLAN;
          wait_replan_start_time_ = GetCurrentTimeSeconds();
          wait_replan_to_init_time_ = GetCurrentTimeSeconds();
        }
      }
      break;
    case AvpSpeedPlanCollisionInfo::WAITREPLAN:
      if (!is_restore_running()) {
        wait_replan_to_init_time_ = GetCurrentTimeSeconds();
      }
      if (DefinitelyGreater(
              GetCurrentTimeSeconds() - wait_replan_to_init_time_,
              config_.min_wait_replan_state_time)) {
        *interactive_stage = AvpSpeedPlanCollisionInfo::INIT;
      } else if (AvpSpeedPlanCollisionInfo::MOVING_OBSTACLE_COLLISION ==
                 collision_type) {
        *interactive_stage = AvpSpeedPlanCollisionInfo::WAITOBSTACLE;
        wait_obstacle_start_time_ = GetCurrentTimeSeconds();
      } else {
        double wait_replan_time =
            GetCurrentTimeSeconds() - wait_replan_start_time_;
        double threshold_time =
            is_rpa_direct_mode ? config_.max_wait_time_for_replan_rpa_direct
                               : config_.max_wait_time_for_replan;
        if (DefinitelyGreater(wait_replan_time, threshold_time)) {
          *interactive_stage = AvpSpeedPlanCollisionInfo::WAITOBSTACLE;
          wait_obstacle_start_time_ = GetCurrentTimeSeconds();
        }
      }
      break;
    case AvpSpeedPlanCollisionInfo::WAITOBSTACLE:
      if (!is_restore_running()) {
        wait_obstacle_start_time_ = GetCurrentTimeSeconds();
      }
      if (DefinitelyGreater(
              GetCurrentTimeSeconds() - wait_obstacle_start_time_,
              config_.min_wait_obstacle_state_time)) {
        *interactive_stage = AvpSpeedPlanCollisionInfo::RUNNING;
      }
      break;
    case AvpSpeedPlanCollisionInfo::RUNNING:
      *interactive_stage = AvpSpeedPlanCollisionInfo::RUNNING;
      break;
    default:
      *interactive_stage = AvpSpeedPlanCollisionInfo::INIT;
      break;
  }
}

// ============================================================================
// UpdateIsUseMiddleBuffer
// ============================================================================
void PathHandle::UpdateIsUseMiddleBuffer(
    const AvpSpeedPlanCollisionInfo::SpeedTaskInteractiveStage&
        interactive_stage,
    const bool curr_is_forward,
    const planning_internal::PathType& path_type) {
  if (path_type == planning_internal::PathType::SEARCH_EXTENSION_PATH &&
      !is_lateral_park_out_ &&
      (!last_is_forward_.first || last_is_forward_.second != curr_is_forward)) {
    is_use_middle_buffer_ = true;
  }
  wait_obstacle_count_ =
      AvpSpeedPlanCollisionInfo::WAITOBSTACLE == interactive_stage
          ? wait_obstacle_count_ + 1
          : 0;
  is_use_middle_buffer_ =
      is_use_middle_buffer_ ||
      wait_obstacle_count_ > config_.wait_obstacle_min_count;
  last_is_forward_.second = curr_is_forward;
  last_is_forward_.first = true;
}

// ============================================================================
// CutOffPathByCollisionInfo - 碰撞截断（完整保留）
// ============================================================================
bool PathHandle::CutOffPathByCollisionInfo(
    const DiscretizedPath& path, const CollisionInfo& collision_info,
    DiscretizedPath* const candidate_path) {
  if (nullptr == candidate_path || path.empty()) {
    return false;
  }
  candidate_path->clear();
  if (collision_info.is_collision &&
      collision_info.first_collision_index < path.size()) {
    double stop_distance =
        std::fmax(path.at(collision_info.first_collision_index).s -
                      collision_info.stop_reserve_distance,
                  0.0);
    const auto index = std::distance(
        path.begin(), std::lower_bound(path.begin(), path.end(), stop_distance,
                                       [](const common::PathPoint& p,
                                          double s) { return p.s < s; }));
    const auto stop_index = index > 0 ? index - 1 : index;
    *candidate_path = DiscretizedPath(std::vector<common::PathPoint>(
        path.begin(), path.begin() + stop_index + 1));
    if (index > 0) {
      auto proj_p = path.Evaluate(stop_distance);
      candidate_path->emplace_back(proj_p);
    }
  } else {
    *candidate_path = path;
  }
  return !candidate_path->empty();
}

// ============================================================================
// CalLimitSpeedByS - S距离限速（完整保留）
// ============================================================================
double PathHandle::CalLimitSpeedByS(
    const double s,
    const OpenSpaceSpeedOptimizerConfig::SpeedBoundInfo& speed_bound_info) {
  static constexpr double kA = 6.2;
  static constexpr double kB = -23.5;
  static constexpr double kMinRation = 0.1;
  double ratio = std::max(
      speed_bound_info.max_sample_speed - speed_bound_info.min_sample_speed,
      kMinRation);
  return ratio * (1.0 / (1.0 + exp(kA + kB * s))) +
         speed_bound_info.min_sample_speed;
}

// ============================================================================
// UpdateSpeedLimits - 速度限制生成（移除 Obstacle 距离计算，保留 FreeSpace 距离）
// ============================================================================
void PathHandle::UpdateSpeedLimits(
    const DiscretizedPath& path,
    const OpenSpaceSpeedOptimizerConfig::SpeedBoundInfo& speed_bound_info,
    const bool is_forward,
    const std::vector<common::PathPoint>& limit_speed_path_points) {
  if (path.empty()) {
    speed_limits_.clear();
    return;
  }

  const auto speed_limits_size =
      static_cast<size_t>(path.Length() / speed_limit_unit_s_ + 2);
  speed_limits_.resize(speed_limits_size);
  common::math::Box2d ego_box;
  common::PathPoint last_point;
  const auto& vehicle_param =
      common::VehicleConfigHelper::GetConfig().vehicle_param();
  const auto rear_fillter_distance =
      -vehicle_param.back_edge_to_center - config_.rear_fillter_buffer;
  const auto front_fillter_distance =
      vehicle_param.front_edge_to_center + config_.front_fillter_buffer;

  for (size_t i = 0; i < speed_limits_size; i++) {
    auto point = path.Evaluate(static_cast<double>(i) * speed_limit_unit_s_);
    ego_box = common::VehicleConfigHelper::GetBoundingBox(point);
    double nearest_obs_dist = std::numeric_limits<double>::max();

    // 独立编译改造：添加静态障碍物距离计算
    for (const auto& obs : static_obstacles_) {
      double curr_dis = ego_box.DistanceTo(obs.bounding_box);
      if (DefinitelyLess(curr_dis, nearest_obs_dist)) {
        nearest_obs_dist = curr_dis;
      }
    }

    // 保留：limit_speed_path_points 距离计算
    for (const auto& limit_p : limit_speed_path_points) {
      double curr_dis = ego_box.DistanceTo(Vec2d(limit_p.x, limit_p.y));
      if (DefinitelyLess(curr_dis, nearest_obs_dist)) {
        nearest_obs_dist = curr_dis;
      }
    }

    // 保留：FreeSpace 段距离计算
    auto update_neareset_dist =
        [&](const std::pair<common::math::LineSegment2d, double>& segment) {
          const auto flu_point_start = common::math::ENUToFLU(
              segment.first.start().x(), segment.first.start().y(), point.x,
              point.y, point.theta);
          const auto flu_point_end = common::math::ENUToFLU(
              segment.first.end().x(), segment.first.end().y(), point.x,
              point.y, point.theta);
          if ((is_forward && flu_point_start.first < rear_fillter_distance &&
               flu_point_end.first < rear_fillter_distance) ||
              (!is_forward && flu_point_start.first > front_fillter_distance &&
               flu_point_end.first > front_fillter_distance)) {
            return;
          }
          double curr_dis = ego_box.DistanceTo(segment.first);
          if (DefinitelyLess(curr_dis, nearest_obs_dist)) {
            nearest_obs_dist = curr_dis;
          }
        };
    std::for_each(not_lidar_not_vehicle_fs_.begin(),
                  not_lidar_not_vehicle_fs_.end(), update_neareset_dist);
    std::for_each(not_lidar_vehicle_fs_.begin(), not_lidar_vehicle_fs_.end(),
                  update_neareset_dist);
    std::for_each(lidar_vehicle_fs_.begin(), lidar_vehicle_fs_.end(),
                  update_neareset_dist);
    std::for_each(lidar_not_vehicle_fs_.begin(), lidar_not_vehicle_fs_.end(),
                  update_neareset_dist);

    speed_limits_[i] = CalLimitSpeedByS(nearest_obs_dist, speed_bound_info);

    if (i > 0) {
      // 独立编译改造：使用 speed_optimizer_util 版本
      speed_limits_[i] = std::min(
          speed_limits_[i],
          speed_optimizer_util::GetDkappaSpeedLimit(
              last_point, point, config_.speed_upper_bound));
    }
    last_point = point;
  }
  SmoothSpeedLimits();
}

// ============================================================================
// SmoothSpeedLimits - 速度限制平滑（完整保留）
// ============================================================================
void PathHandle::SmoothSpeedLimits() {
  if (speed_limits_.empty()) {
    return;
  }
  int window_size = static_cast<int>(config_.speed_limit_smooth_window_size);
  const auto unsmoothed = speed_limits_;
  std::deque<int> ascending_indexs;
  const auto smooth_end_index =
      static_cast<int>(unsmoothed.size()) + window_size;
  for (int i = 0; i < smooth_end_index; ++i) {
    if (static_cast<size_t>(i) < unsmoothed.size()) {
      while (!ascending_indexs.empty() &&
             unsmoothed[i] <= unsmoothed[ascending_indexs.back()]) {
        ascending_indexs.pop_back();
      }
      ascending_indexs.push_back(i);
    }
    if (ascending_indexs.front() < i - 2 * window_size) {
      ascending_indexs.pop_front();
    }
    const auto start_index = i - window_size;
    if (0 <= start_index &&
        start_index < static_cast<int>(speed_limits_.size())) {
      speed_limits_[start_index] = unsmoothed[ascending_indexs.front()];
    }
  }
}

// ============================================================================
// UpdateDebugInfo - 写入 SpeedOptimizerOutput（替代 OpenSpaceInfo*）
// ============================================================================
void PathHandle::UpdateDebugInfo(
    const CollisionInfo& collision_info,
    const AvpSpeedPlanCollisionInfo::SpeedTaskInteractiveStage&
        interactive_stage,
    const common::PathPoint& future_collision_point,
    const bool is_vehicle_still, const double wheel_mask_distance,
    SpeedOptimizerOutput* const output) {
  if (nullptr == output) {
    return;
  }

  // 独立编译改造：写入 SpeedOptimizerOutput 而非 OpenSpaceInfo*
  if (AvpSpeedPlanCollisionInfo::WAITREPLAN == interactive_stage &&
      collision_info.collision_type !=
          AvpSpeedPlanCollisionInfo::NO_COLLISION) {
    output->replan_triggered_by_speed_plan = true;
  } else {
    output->replan_triggered_by_speed_plan = false;
  }

  auto& speed_plan_collision_info = output->speed_plan_collision_info;
  if (current_path_has_collision_count_ >
      config_.path_collision_risk_max_count) {
    output->current_path_has_collision_risk = true;
  } else {
    output->current_path_has_collision_risk = false;
  }

  speed_plan_collision_info.is_wheel_mask_valid = pre_wheel_mask_valid_;
  speed_plan_collision_info.is_stop_near_wheel_mask = false;
  if (pre_wheel_mask_valid_ && is_vehicle_still &&
      DefinitelyLess(wheel_mask_distance, config_.wheel_mask_stop_accuracy)) {
    speed_plan_collision_info.is_stop_near_wheel_mask = true;
  }
  speed_plan_collision_info.speed_task_inter_stage =
      static_cast<int>(interactive_stage);
  speed_plan_collision_info.collision_type = collision_info.collision_type;
  if (collision_info.is_collision) {
    speed_plan_collision_info.collision_distance =
        collision_info.curr_collision_distance;
    output->future_collision_point = future_collision_point;
  }

  if (collision_info.static_obstacle_collision_info.is_collision) {
    speed_plan_collision_info.static_obstacle_id =
        collision_info.static_obstacle_collision_info.collision_obstacle_id;
  }
  if (collision_info.moving_obstacle_collision_info.is_collision) {
    speed_plan_collision_info.moving_obstacle_id =
        collision_info.moving_obstacle_collision_info.collision_obstacle_id;
  }
  speed_plan_collision_info.is_use_middle_buffer = is_use_middle_buffer_;
  speed_plan_collision_info.bigger_buffer_safe_count =
      bigger_buffer_safe_count_;
  speed_plan_collision_info.path_collision_risk_count =
      current_path_has_collision_count_;
}

// ============================================================================
// Process - 主处理函数（改造为 SpeedOptimizerInput/Output 签名）
// ============================================================================
std::string PathHandle::Process(const SpeedOptimizerInput& input,
                                SpeedOptimizerOutput* output) {
  std::string msg;
  if (nullptr == output) {
    msg = "path handle process output is nullptr";
    return msg;
  }

  Init(input.env_structured_info);

  // 独立编译改造：缓存输入的静态障碍物
  static_obstacles_ = input.static_obstacles;
  moving_obstacles_ = input.moving_obstacles;
  UpdateMovingObstacleBoxes();

  DiscretizedPath collision_check_path;
  if (!CutOffPathByWheelMask(
          input.discretized_path, input.is_forward,
          input.env_structured_info.is_parking_inwards,
          input.is_consider_wheel_mask,
          input.wheel_mask_box, &collision_check_path)) {
    output->speed_plan_collision_info.is_stop_near_wheel_mask = true;
    msg = "cut off path by wheel mask failed";
    return msg;
  }

  double wheel_mask_distance = collision_check_path.Length();
  CollisionInfo collision_info;

  // 独立编译改造：使用 SpeedOptimizerInput 中的 freespace 索引
  auto freespace_out_array = std::make_shared<FreeSpaceOutArray>();
  if (!UpdateCollisionInfo(
          collision_check_path, freespace_out_array,
          input.under_spot_low_fs_idxs, input.high_curb_fs_idxs,
          input.ignore_fs_idxs, input.vehicle_state, input.is_forward,
          input.is_rpa_direct_mode, input.is_mirror_fold,
          input.partitioned_paths, &collision_info)) {
    msg = "update collision info failed";
    return msg;
  }

  if (!CutOffPathByCollisionInfo(collision_check_path, collision_info,
                                 &output->candidate_path)) {
    msg = "cut off path by collision info failed";
    return msg;
  }

  collision_info.curr_collision_distance =
      collision_check_path.at(collision_info.first_collision_index).s;
  UpdateInteractiveStage(input.is_vehicle_stand_still, input.is_rpa_direct_mode,
                         collision_info, &output->interactive_stage);
  UpdateIsUseMiddleBuffer(output->interactive_stage, input.is_forward,
                          input.partitioned_paths.path_type);

  // 速度限制计算 - 使用输入配置中的 speed_bound_info
  auto speed_bound_info = input.is_forward
      ? input.config.apa_speed_bound_info.forward_info
      : input.config.apa_speed_bound_info.reverse_info;
  UpdateSpeedLimits(output->candidate_path, speed_bound_info, input.is_forward,
                    input.spd_limit_points);
  UpdateDebugInfo(collision_info, output->interactive_stage,
                  collision_check_path.at(collision_info.first_collision_index),
                  input.is_vehicle_stand_still, wheel_mask_distance, output);

  return msg;
}

// ============================================================================
// UpdatePathCollisionRiskCount - 碰撞风险计数（完整保留）
// ============================================================================
void PathHandle::UpdatePathCollisionRiskCount(
    const DiscretizedPath& path,
    const planning_internal::PathType& path_type,
    const size_t cur_path_idx, const bool is_mirror_fold,
    const bool is_forward) {
  if ((last_is_forward_.first && last_is_forward_.second != is_forward) ||
      path.empty()) {
    current_path_has_collision_count_ = 0;
  }
  if (path_type == planning_internal::PathType::SEARCH_EXTENSION_PATH &&
      cur_path_idx == 0) {
    current_path_has_collision_count_ = 0;
    return;
  }

  double path_collision_risk_max_distance = 0.0;
  if (is_vertical_park_in_ && is_narrow_spot_scenario_) {
    path_collision_risk_max_distance =
        config_.path_collision_risk_max_distance_for_narrow_spot;
  } else if (is_nns_adjust_) {
    path_collision_risk_max_distance =
        config_.path_collision_risk_max_distance_for_nns_adjust;
  } else {
    path_collision_risk_max_distance =
        config_.path_collision_risk_max_distance;
  }
  for (const auto& point : path) {
    if (point.s >= path_collision_risk_max_distance) {
      break;
    }
    if (common::math::CheckCollisionWithVehiclePolygon2d(
            point.x, point.y, point.theta, all_freespace_segments_,
            &current_path_collision_index_)) {
      current_path_has_collision_count_++;
      return;
    }
    if (is_mirror_fold) {
      continue;
    }
    const auto circles =
        common::VehicleConfigHelper::GetMirrorCirclesWithBuffer(
            point.x, point.y, point.theta);
    if (circles.size() != 2) {
      AERROR << "used circle size is not 2";
      return;
    }
    if (common::math::CheckCollisionWithCircle(
            circles.front(), high_height_curb_fs_,
            &current_path_collision_index_) ||
        common::math::CheckCollisionWithCircle(
            circles.back(), high_height_curb_fs_,
            &current_path_collision_index_)) {
      current_path_has_collision_count_++;
      return;
    }
  }
  current_path_has_collision_count_ = 0;
}

// ============================================================================
// UpdateLateralBuffer - 横向缓冲更新（完整保留）
// ============================================================================
void PathHandle::UpdateLateralBuffer(
    const OpenSpaceSpeedOptimizerConfig::CollisionBufferInfo& collision_buffer,
    const bool is_use_middle_buffer, const bool is_narrow_spot_scenario,
    const bool is_lat_park_in, const bool is_lat_park_out,
    const planning_internal::PathType& path_type,
    const size_t cur_path_idx,
    std::pair<double, double>* const lateral_buffer_for_vehicle,
    std::pair<double, double>* const lateral_buffer_for_not_vehicle,
    std::pair<double, double>* const lateral_buffer_for_low_fs) {
  if (nullptr == lateral_buffer_for_vehicle ||
      nullptr == lateral_buffer_for_not_vehicle ||
      nullptr == lateral_buffer_for_low_fs) {
    ADEBUG << "update lateral buffer for speed failed";
    return;
  }
  static constexpr double kDecreaseBufferForSearchExtension = 0.05;
  static constexpr double kDecraeseBufferForParkout = 0.08;
  static constexpr double kDecraeseBufferForParkin = 0.05;
  const bool need_shrink_for_search_extension =
      is_use_middle_buffer &&
      path_type == planning_internal::PathType::SEARCH_EXTENSION_PATH &&
      cur_path_idx == 0;

  if (is_use_middle_buffer && is_narrow_spot_scenario) {
    lateral_buffer_for_vehicle->first = collision_buffer.left_lateral_buffer;
    lateral_buffer_for_vehicle->second = collision_buffer.right_lateral_buffer;
  } else if (is_use_middle_buffer != is_narrow_spot_scenario) {
    lateral_buffer_for_vehicle->first =
        collision_buffer.left_lateral_buffer_middle;
    lateral_buffer_for_vehicle->second =
        collision_buffer.right_lateral_buffer_middle;
  } else if (!is_use_middle_buffer && !is_narrow_spot_scenario) {
    lateral_buffer_for_vehicle->first =
        collision_buffer.left_lateral_buffer_bigger;
    lateral_buffer_for_vehicle->second =
        collision_buffer.right_lateral_buffer_bigger;
  }

  lateral_buffer_for_not_vehicle->first = collision_buffer.left_lateral_buffer;
  lateral_buffer_for_not_vehicle->second = collision_buffer.right_lateral_buffer;
  lateral_buffer_for_low_fs->first = collision_buffer.left_lateral_buffer_low_fs;
  lateral_buffer_for_low_fs->second =
      collision_buffer.right_lateral_buffer_low_fs;

  if (need_shrink_for_search_extension) {
    lateral_buffer_for_vehicle->first -= kDecreaseBufferForSearchExtension;
    lateral_buffer_for_vehicle->second -= kDecreaseBufferForSearchExtension;
    lateral_buffer_for_not_vehicle->first -= kDecreaseBufferForSearchExtension;
    lateral_buffer_for_not_vehicle->second -= kDecreaseBufferForSearchExtension;
  }
  if (is_lat_park_in && is_use_middle_buffer) {
    lateral_buffer_for_low_fs->first -= kDecraeseBufferForParkin;
    lateral_buffer_for_low_fs->second -= kDecraeseBufferForParkin;
  } else if (is_lat_park_out) {
    lateral_buffer_for_low_fs->first -= kDecraeseBufferForParkout;
    lateral_buffer_for_low_fs->second -= kDecraeseBufferForParkout;
  } else if (need_shrink_for_search_extension) {
    lateral_buffer_for_low_fs->first -= kDecreaseBufferForSearchExtension;
    lateral_buffer_for_low_fs->second -= kDecreaseBufferForSearchExtension;
  }
}

}  // namespace planning
}  // namespace TL
