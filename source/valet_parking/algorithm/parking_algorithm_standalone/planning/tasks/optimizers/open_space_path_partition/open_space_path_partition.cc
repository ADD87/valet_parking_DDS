/******************************************************************************
 * Copyright 2019 The Apollo Authors. All Rights Reserved.
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
 * @file open_space_path_partition.cc
 * @brief 独立编译改造版：路径分段决策模块实现
 *
 * 改造要点：
 * - 去掉 frame_/injector_ 依赖 → PartitionInput/PartitionOutput
 * - VehicleConfigHelper → 本地 GetBoundingBox 辅助函数
 * - Clock::NowInSeconds() → NowInSeconds() (chrono)
 * - FLAGS_xxx → config 成员或常量
 * - ADEBUG/AERROR → std::cout/std::cerr
 * - 多个 MVP 简化函数
 **/

#include "planning/tasks/optimizers/open_space_path_partition/open_space_path_partition.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "common/math/box2d.h"
#include "common/math/double_type.h"
#include "common/math/line_segment2d.h"
#include "common/math/linear_interpolation.h"
#include "common/math/math_utils.h"
#include "common/math/polygon2d.h"
#include "common/math/vec2d.h"
#include "common/status/status.h"

namespace TL {
namespace planning {

using common::math::double_type::Compare;  // NOLINT
using common::math::double_type::DefinitelyGreaterEqual;
using common::math::double_type::DefinitelyLess;
using TL::common::PathPoint;
using TL::common::Status;
using TL::common::math::Box2d;
using TL::common::math::NormalizeAngle;
using TL::common::math::Polygon2d;
using TL::common::math::Vec2d;

// ========== 本地辅助：替代 VehicleConfigHelper ==========
namespace {

// 全局车辆参数缓存（在构造函数中设置）
static const VehicleParam* g_vehicle_param = nullptr;

/**
 * @brief 根据 PathPoint 的位置和朝向，构造车辆包围盒
 * 替代 common::VehicleConfigHelper::GetBoundingBox(path_point)
 */
Box2d GetBoundingBox(const PathPoint& point) {
  if (g_vehicle_param == nullptr) {
    // fallback
    return Box2d({point.x, point.y}, point.theta, 4.5, 1.86);
  }
  const auto& vp = *g_vehicle_param;
  // 后轴中心到几何中心的偏移
  double center_shift = (vp.front_edge_to_center - vp.back_edge_to_center) / 2.0;
  double center_x = point.x + center_shift * std::cos(point.theta);
  double center_y = point.y + center_shift * std::sin(point.theta);
  return Box2d({center_x, center_y}, point.theta, vp.length, vp.width);
}

/**
 * @brief 根据 x,y,theta 构造带缓冲区的车辆多边形
 * 替代 common::VehicleConfigHelper::GetPolygon2dWithBuffer(x, y, theta)
 */
Polygon2d GetPolygon2dWithBuffer(double x, double y, double theta,
                                 double buffer = 0.0) {
  PathPoint pp;
  pp.x = x;
  pp.y = y;
  pp.theta = theta;
  Box2d box = GetBoundingBox(pp);
  if (buffer > 0.0) {
    box = Box2d(box.center(), box.heading(),
                box.length() + 2 * buffer, box.width() + 2 * buffer);
  }
  return Polygon2d(box);
}

// MVP: 替代 FLAGS_apa_gear_shift_limit
constexpr uint32_t kApaGearShiftLimit = 10;

// MVP: 替代 FLAGS_park_out_early_stop_angle_limit
constexpr double kParkOutEarlyStopAngleLimit = 0.175;  // ~10 degrees

// MVP: 替代 FLAGS_pause_brake_acceleration
constexpr double kPauseBrakeAcceleration = -2.5;

}  // namespace

// ========== 构造/重置 ==========

OpenSpacePathPartition::OpenSpacePathPartition(
    const OpenSpacePathPartitionConfig& config,
    const VehicleParam& vehicle_param)
    : open_space_path_partition_config_(config),
      vehicle_param_(vehicle_param) {
  g_vehicle_param = &vehicle_param_;
  std::cout << "[INFO] OpenSpacePathPartition Init (standalone)" << std::endl;
}

Status OpenSpacePathPartition::Reset() {
  history_path_ = PartitionedPath();
  is_warm_start_ = false;
  frozen_near_end_time_ = NowInSeconds();
  frozen_time_ = NowInSeconds();
  yaw_track_abnormal_start_time_ = NowInSeconds();
  is_mirror_fold_ = false;
  is_veh_reach_destination_ = false;
  is_veh_prefinish_brake_saftisfied_ = false;
  is_clipped_ = false;
  task_finish_status_ = planning_internal::OpenSpaceDebug::UNKNOWN;
  gear_shift_ = {soc::GearPosition::GEAR_PARKING, 0};
  std::cout << "[INFO] Path Partition is reset" << std::endl;
  return Status::OK();
}

// ========== 主入口 ==========

Status OpenSpacePathPartition::Execute(const PartitionInput& input,
                                       PartitionOutput* output) {
  if (output == nullptr) {
    return Status(TL::common::ErrorCode::PLANNER_PARKING_PATHPARTITION_ERROR,
                  "output is nullptr");
  }
  // 缓存当前输入输出引用
  current_input_ = &input;
  current_output_ = output;

  UpdateParam(input);
  path_decision_debug_.clear();

  OpenSpacePathDecision openspace_path_decision = OpenSpacePathDecision::UNKOWN;
  output->partitioned_paths = PartitionedPath();  // 初始化

  UpdatePathDecision(input, &openspace_path_decision,
                     &output->partitioned_paths);

  if (task_finish_status_ == planning_internal::OpenSpaceDebug::OVER_TIME) {
    const std::string msg = "task finish status is over time";
    std::cerr << "[ERROR] " << msg << std::endl;
    output->path_decision = openspace_path_decision;
    output->finish_status = task_finish_status_;
    return Status(TL::common::ErrorCode::PLANNER_PARKING_PATHPARTITION_ERROR,
                  msg);
  }

  output->chosen_path_idx = std::make_pair(
      output->partitioned_paths.path_idx,
      output->partitioned_paths.point_idx);

  switch (openspace_path_decision) {
    case OpenSpacePathDecision::TASK_FINISH: {
      output->is_stop_path = true;
      path_decision_debug_ += " stop for task finished ";
      break;
    }
    case OpenSpacePathDecision::PREPARE_FINISH: {
      path_decision_debug_ += " stop for prepare finish";
      break;
    }
    case OpenSpacePathDecision::TRACK_ABNORMAL: {
      OpenSpaceStatus::UpdateReplanStatus(
          OpenSpaceStatus::TRACK_ABNORMAL,
          &output->open_space_status);
      output->is_stop_path = true;
      path_decision_debug_ += " track abnormal ";
      break;
    }
    case OpenSpacePathDecision::NO_VALID_PATH: {
      OpenSpaceStatus::UpdateReplanStatus(
          OpenSpaceStatus::NO_VALID_PATH,
          &output->open_space_status);
      output->is_stop_path = true;
      path_decision_debug_ += " stop for no valid path ";
      break;
    }
    case OpenSpacePathDecision::CHOOSE_HISTORY_PATH: {
      path_decision_debug_ += " choose history path ";
      UpdateHistoryPath(output->partitioned_paths);
      UpdateParkDisplay(&output->partitioned_paths);
      break;
    }
    case OpenSpacePathDecision::CHOOSE_NEW_PATH: {
      path_decision_debug_ += " choose new path ";
      UpdateHistoryPath(output->partitioned_paths);
      UpdateParkDisplay(&output->partitioned_paths);
      break;
    }
    default:
      return Status(TL::common::ErrorCode::PLANNER_PARKING_PATHPARTITION_ERROR,
                    "error partition");
  }

  AdjustRelativeS(
      output->partitioned_paths.path_set, output->partitioned_paths.path_idx,
      output->partitioned_paths.point_idx, &output->chosen_partitioned_path);

  UpdateInfoForPreFinishCondition(openspace_path_decision,
                                  &output->chosen_partitioned_path);
  UpdateStatusBasedPartitionResult();

  output->is_mirror_fold = is_mirror_fold_;
  output->path_decision = openspace_path_decision;
  output->finish_status = task_finish_status_;
  output->destination_reached = is_veh_reach_destination_;
  output->path_decision_debug = path_decision_debug_;

  // yaw_track_abnormal 时间更新
  if (!output->open_space_status.has_replan() ||
      (output->open_space_status.replan &
       static_cast<uint32_t>(OpenSpaceStatus::YAW_TRACK_ABNORMAL)) == 0U) {
    yaw_track_abnormal_start_time_ = NowInSeconds();
  }

  std::cout << "[DEBUG] path_decision: " << path_decision_debug_ << std::endl;

  current_input_ = nullptr;
  current_output_ = nullptr;
  return Status::OK();
}

// ========== UpdateParam ==========

void OpenSpacePathPartition::UpdateParam(const PartitionInput& input) {
  ego_x_ = input.vehicle_state.x;
  ego_y_ = input.vehicle_state.y;
  ego_theta_ = input.vehicle_state.heading;
  start_point_ = input.planning_start_point;

  // 简化版：根据输入更新 warm_start
  if (input.pub_gear != input.vehicle_state.gear) {
    is_warm_start_ = false;
    frozen_near_end_time_ = NowInSeconds();
  }
  is_warm_start_ = is_warm_start_ || !input.is_vehicle_stand_still;

  // MVP: 简化 stop_by_plan 逻辑
  const bool stop_by_plan =
      (input.speed_plan_collision_info.stage !=
       planning_internal::AvpSpeedPlanCollisionInfo::INIT);
  if (stop_by_plan || !input.is_vehicle_stand_still) {
    frozen_near_end_time_ = NowInSeconds();
  }
}

// ========== UpdateStatusBasedPartitionResult (MVP 简化) ==========

void OpenSpacePathPartition::UpdateStatusBasedPartitionResult() {
  if (current_output_ == nullptr || current_input_ == nullptr) return;

  auto* partitioned_paths_ptr = &current_output_->partitioned_paths;
  auto* chosen_partitioned_path_ptr = &current_output_->chosen_partitioned_path;

  if (partitioned_paths_ptr->path_set.size() >
      partitioned_paths_ptr->path_idx) {
    current_output_->open_space_status.set_current_part_path_length(
        partitioned_paths_ptr->path_set[partitioned_paths_ptr->path_idx]
            .first.Length());
  }

  if (chosen_partitioned_path_ptr == nullptr ||
      chosen_partitioned_path_ptr->first.empty() ||
      chosen_partitioned_path_ptr->first.back().s >
          open_space_path_partition_config_
              .rough_longitudinal_offset_to_midpoint) {
    frozen_near_end_time_ = NowInSeconds();
  }

  // MVP: 跳过 dynamic replan 触发逻辑（需要 frame_ 中的场景信息）
  // TODO: 实现 dynamic replan 触发
}

// ========== UpdatePathDecision ==========

void OpenSpacePathPartition::UpdatePathDecision(
    const PartitionInput& input,
    OpenSpacePathDecision* const openspace_path_decision_ptr,
    PartitionedPath* const choose_path_ptr) {
  auto pub_gear = input.pub_gear;

  // MVP: 不折叠后视镜
  // is_mirror_fold_ = is_mirror_fold_ || IsMirrorFold(...);

  AlternativePath alternative_path;
  OpenSpacePathDeciderStatus open_space_path_decider_status;
  PartitionedPath path_result = input.path_result;

  // 检查历史路径
  PartitionedPath history_path_partition = history_path_;
  uint32_t executable_status = 0;
  bool is_history_path_match_suc =
      PathMatch(&history_path_partition, &executable_status);

  // MVP: 跳过 GetLastCyclePubPath (需要 previous_frame)

  if (is_history_path_match_suc) {
    if (input.replan_triggered_by_speed_plan || input.guard_triggered) {
      executable_status += COLLISION_RISK;
    }
    open_space_path_decider_status.executable_status = executable_status;
    open_space_path_decider_status.gear_shift_num =
        gear_shift_.second + history_path_partition.path_set.size() -
        history_path_partition.path_idx - 1;
    UpdateCollisionDistance(history_path_partition,
                            &open_space_path_decider_status.collision_distance);
    alternative_path.emplace_back(history_path_partition,
                                  open_space_path_decider_status);
  }

  executable_status = 0;
  const bool is_path_result_match_suc =
      PathMatch(&path_result, &executable_status);
  if (is_path_result_match_suc) {
    open_space_path_decider_status.Reset();
    UpdatePathExcutableStatus(path_result, &executable_status);
    open_space_path_decider_status.executable_status = executable_status;
    const auto& path_set = path_result.path_set;
    open_space_path_decider_status.gear_shift_num =
        gear_shift_.first == path_set.front().second
            ? gear_shift_.second + path_set.size() - 1
            : gear_shift_.second + path_set.size();
    UpdateCollisionDistance(path_result,
                            &open_space_path_decider_status.collision_distance);
    alternative_path.emplace_back(path_result, open_space_path_decider_status);
  }
  open_space_path_decider_status.Reset();

  int chosen_path_set_idx = PathDecider(alternative_path);
  PartitionedPath reserved_partitioned_paths;
  if (chosen_path_set_idx < 0 ||
      chosen_path_set_idx >= static_cast<int>(alternative_path.size())) {
    *openspace_path_decision_ptr = OpenSpacePathDecision::NO_VALID_PATH;
    if (!alternative_path.empty()) {
      open_space_path_decider_status = alternative_path.back().second;
      reserved_partitioned_paths = alternative_path.front().first;
    }
    SetStopPath(pub_gear, reserved_partitioned_paths, choose_path_ptr);
  } else {
    *choose_path_ptr = alternative_path[chosen_path_set_idx].first;
    open_space_path_decider_status =
        alternative_path[chosen_path_set_idx].second;
    reserved_partitioned_paths = *choose_path_ptr;
    if (is_history_path_match_suc && 0 == chosen_path_set_idx) {
      *openspace_path_decision_ptr = OpenSpacePathDecision::CHOOSE_HISTORY_PATH;
    } else {
      *openspace_path_decision_ptr = OpenSpacePathDecision::CHOOSE_NEW_PATH;
    }
  }

  if (!GetEndPointSLInCurrentPath(choose_path_ptr->path_set,
                                  choose_path_ptr->path_idx, &end_point_sl_)) {
    end_point_sl_.l = INFINITY;
    std::cout << "[DEBUG] get end pose SLpoint failed" << std::endl;
  }

  if (IsTaskFinish(*choose_path_ptr)) {
    pub_gear = soc::GearPosition::GEAR_PARKING;
    SetStopPath(pub_gear, reserved_partitioned_paths, choose_path_ptr);
    *openspace_path_decision_ptr = OpenSpacePathDecision::TASK_FINISH;
    return;
  }

  if (IsTrackAbnormal()) {
    SetStopPath(pub_gear, reserved_partitioned_paths, choose_path_ptr);
    *openspace_path_decision_ptr = OpenSpacePathDecision::TRACK_ABNORMAL;
    return;
  }

  if (task_finish_status_ ==
      planning_internal::OpenSpaceDebug::PREFINISH_BRAKING) {
    std::cout << "[DEBUG] prefinish condition satisfied" << std::endl;
    *openspace_path_decision_ptr = OpenSpacePathDecision::PREPARE_FINISH;
    return;
  }

  // 更新 replan 状态
  uint32_t replan_status = 0;
  GetReplanStatusBasedExcutableStatus(open_space_path_decider_status,
                                      &replan_status);
  if (replan_status > 0 && current_output_ != nullptr) {
    current_output_->open_space_status.replan |= replan_status;
  }
}

// ========== SetStopPath ==========

void OpenSpacePathPartition::SetStopPath(
    const soc::GearPosition& pub_gear,
    const PartitionedPath& reserved_partitioned_paths,
    PartitionedPath* const partitioned_paths_ptr) {
  std::vector<PathGearPair> rest_path_set;
  if (!reserved_partitioned_paths.path_set.empty()) {
    const auto path_idx = reserved_partitioned_paths.path_idx;
    rest_path_set.assign(reserved_partitioned_paths.path_set.begin() +
                             static_cast<int>(path_idx + 1),
                         reserved_partitioned_paths.path_set.end());
  }
  const auto last_gear = pub_gear == soc::GearPosition::GEAR_NEUTRAL
                             ? soc::GearPosition::GEAR_PARKING
                             : pub_gear;
  ResetPartitionPath(partitioned_paths_ptr);
  partitioned_paths_ptr->path_set.emplace_back();
  partitioned_paths_ptr->path_set.back().first.GenerateStopPath(
      start_point_.x, start_point_.y, start_point_.theta,
      start_point_.kappa);
  partitioned_paths_ptr->path_set.back().second = last_gear;
  partitioned_paths_ptr->path_set.insert(partitioned_paths_ptr->path_set.end(),
                                         rest_path_set.begin(),
                                         rest_path_set.end());
}

// ========== PathMatch ==========

bool OpenSpacePathPartition::PathMatch(
    PartitionedPath* const partition_path_ptr,
    uint32_t* const executable_status_ptr) {
  if (nullptr == partition_path_ptr || nullptr == executable_status_ptr ||
      partition_path_ptr->path_set.empty()) {
    return false;
  }
  size_t path_idx = partition_path_ptr->path_idx;
  size_t path_point_idx = partition_path_ptr->point_idx;
  if (GetPathMatchIdx(*partition_path_ptr, path_idx, &path_point_idx)) {
    UpdatePathMatchIdx(path_idx, path_point_idx, partition_path_ptr,
                       executable_status_ptr);
    const auto& match_path =
        partition_path_ptr->path_set[partition_path_ptr->path_idx].first;
    const auto& match_point = match_path[partition_path_ptr->point_idx];
    const auto& end_point = match_path.back();

    if (current_output_ != nullptr) {
      CalculatePoseError(ego_x_, ego_y_, ego_theta_, match_point.x,
                         match_point.y, match_point.theta,
                         &current_output_->vehicle_follow_error);
      CalculatePoseError(
          ego_x_, ego_y_, ego_theta_, end_point.x, end_point.y,
          end_point.theta,
          &current_output_->vehicle_to_current_end_error);
    }
    return true;
  }
  return false;
}

// ========== GetPathMatchIdx ==========

bool OpenSpacePathPartition::GetPathMatchIdx(
    const PartitionedPath& partition_path, const size_t path_idx,
    size_t* path_point_idx_ptr) {
  bool match_suc = false;
  if (nullptr == path_point_idx_ptr) {
    std::cerr << "[ERROR] GetPathMatchIdx input check error" << std::endl;
    return match_suc;
  }
  if (path_idx < partition_path.path_set.size()) {
    const auto& path_pair = partition_path.path_set[path_idx];
    const auto& path = path_pair.first;
    const auto& path_gear = path_pair.second;
    const size_t search_start = std::min(*path_point_idx_ptr, path.size() - 1);
    double best_dist_sqr = std::numeric_limits<double>::infinity();
    size_t best_match_idx = search_start;
    for (size_t i = search_start; i < path.size(); ++i) {
      if (PointMatch(path[i], path_gear)) {
        const double dx = path[i].x - start_point_.x;
        const double dy = path[i].y - start_point_.y;
        const double dist_sqr = dx * dx + dy * dy;
        if (dist_sqr < best_dist_sqr) {
          best_dist_sqr = dist_sqr;
          best_match_idx = i;
          match_suc = true;
        }
      }
    }
    if (match_suc) {
      *path_point_idx_ptr = best_match_idx;
    }
    // Box match fallback
    if (!match_suc) {
      const auto start_point_polygon =
          common::math::Polygon2d(GetBoundingBox(start_point_));
      std::pair<double, int> largest_iou_point(0.0, static_cast<int>(search_start));
      for (size_t i = search_start; i < path.size(); ++i) {
        double iou = start_point_polygon.ComputeIoU(
            Polygon2d(GetBoundingBox(path[i])));
        if (iou > largest_iou_point.first) {
          largest_iou_point = {iou, static_cast<int>(i)};
        }
      }
      if (largest_iou_point.first >
          open_space_path_partition_config_.point_match_iou_threshold) {
        *path_point_idx_ptr = largest_iou_point.second;
        match_suc = true;
      }
    }
  }
  return match_suc;
}

// ========== PointMatch ==========

bool OpenSpacePathPartition::PointMatch(
    const common::PathPoint& path_point,
    const soc::GearPosition& gear) {
  Vec2d tracking_vector(path_point.x - start_point_.x,
                        path_point.y - start_point_.y);
  const double distance = tracking_vector.Length();
  const double path_point_heading =
      gear == soc::GearPosition::GEAR_DRIVE
          ? path_point.theta
          : NormalizeAngle(path_point.theta + M_PI);
  const double head_track_difference =
      std::fabs(NormalizeAngle(tracking_vector.Angle() - path_point_heading));
  return distance < open_space_path_partition_config_.distance_search_range &&
         head_track_difference <
             open_space_path_partition_config_.heading_track_range;
}

// ========== UpdatePathMatchIdx ==========

void OpenSpacePathPartition::UpdatePathMatchIdx(
    const size_t match_path_idx, const size_t match_point_idx,
    PartitionedPath* const partition_path_ptr,
    uint32_t* const executable_status_ptr) {
  partition_path_ptr->path_idx = match_path_idx;
  partition_path_ptr->point_idx = match_point_idx;
  partition_path_ptr->path_shift = false;

  const bool has_next_path =
      match_path_idx + 1 < partition_path_ptr->path_set.size();
  if (!has_next_path || (current_input_ != nullptr &&
                         !current_input_->is_vehicle_stand_still)) {
    return;
  }
  const bool at_current_path_tail =
      match_point_idx + 1 >=
      partition_path_ptr->path_set[match_path_idx].first.size();
  if (current_input_ != nullptr &&
      partition_path_ptr->path_set[match_path_idx + 1].second ==
          current_input_->vehicle_state.gear &&
      !at_current_path_tail) {
    return;
  }

  const auto& partitioned_path_set = partition_path_ptr->path_set;
  PathPoint project_path_point;
  ProjectOnPath(
      partition_path_ptr->path_set[partition_path_ptr->path_idx].first,
      partition_path_ptr->point_idx, start_point_, &project_path_point);
  const double lon_dis =
      fabs(partitioned_path_set[match_path_idx].first.back().s -
           project_path_point.s);

  size_t next_path_idx = partition_path_ptr->path_idx + 1;
  size_t next_point_idx = 0;
  if (AbleToGearShift(*partition_path_ptr, lon_dis, &next_path_idx,
                      &next_point_idx, executable_status_ptr)) {
    partition_path_ptr->path_idx = next_path_idx;
    partition_path_ptr->point_idx = next_point_idx;
    partition_path_ptr->path_shift = next_path_idx > match_path_idx;
  }
}

// ========== AbleToGearShift ==========

bool OpenSpacePathPartition::AbleToGearShift(
    const PartitionedPath& partition_path,
    const double lon_dis_to_gear_shift_point, size_t* const next_path_idx_ptr,
    size_t* const next_path_point_idx_ptr,
    uint32_t* const executable_status_ptr) {
  if (lon_dis_to_gear_shift_point >
      open_space_path_partition_config_
          .rough_longitudinal_offset_to_midpoint) {
    return false;
  }
  if (nullptr == next_path_idx_ptr || nullptr == next_path_point_idx_ptr ||
      nullptr == executable_status_ptr) {
    std::cerr << "[ERROR] AbleToGearShift input has null" << std::endl;
    return false;
  }
  auto gear_shift_longitudinal_threshold = SetGearShiftDis();
  const bool allow_standstill_shift =
      current_input_ != nullptr && current_input_->is_vehicle_stand_still;
    const bool can_shift_now = is_warm_start_ || allow_standstill_shift;
    if (can_shift_now &&
      lon_dis_to_gear_shift_point < gear_shift_longitudinal_threshold) {
    if (!GetPathMatchIdx(partition_path, *next_path_idx_ptr,
                         next_path_point_idx_ptr)) {
      *next_path_idx_ptr = partition_path.path_idx + 1;
      *next_path_point_idx_ptr = 0;
    }
    const auto& next_match_path_pair =
        partition_path.path_set[*next_path_idx_ptr];
    PathPoint project_path_point(start_point_);
    ProjectOnPath(next_match_path_pair.first, *next_path_point_idx_ptr,
                  start_point_, &project_path_point);
    const double yaw_track_abnormal_period =
        NowInSeconds() - yaw_track_abnormal_start_time_;
    const bool is_yaw_track_abnormal =
        yaw_track_abnormal_period < open_space_path_partition_config_
                                        .yaw_error_replan_time_threshold &&
        IsYawTrackAbnormal(project_path_point, next_match_path_pair.second);
    if (is_yaw_track_abnormal) {
      *executable_status_ptr += LARGE_YAW_ERROR_IN_GEAR_SHIFT;
      return false;
    }
  } else {
    double frozen_duration = NowInSeconds() - frozen_near_end_time_;
    const double time_threshold =
        is_warm_start_
            ? open_space_path_partition_config_
                  .warm_start_response_time_threshold
            : open_space_path_partition_config_
                  .cold_start_response_time_threshold;
    if (frozen_duration > time_threshold) {
      *executable_status_ptr += TOO_SHORT_TO_LAUNCH;
    }
    return false;
  }
  return can_shift_now;
}

// ========== ProjectOnPath ==========

void OpenSpacePathPartition::ProjectOnPath(const DiscretizedPath& path,
                                           size_t next_path_point_idx,
                                           const common::PathPoint& curr_p,
                                           common::PathPoint* project_point) {
  if (nullptr == project_point) {
    return;
  }
  if (path.empty()) {
    *project_point = curr_p;
    return;
  }
  if (next_path_point_idx + 1 >= path.size()) {
    *project_point = path.back();
    return;
  }
  if (next_path_point_idx > 0) {
    static constexpr double kEpison = 1e-10;
    const auto& pre_p = path[next_path_point_idx - 1];
    const auto& next_p = path[next_path_point_idx];

    Vec2d pre_curr(curr_p.x - pre_p.x, curr_p.y - pre_p.y);
    Vec2d pre_next(next_p.x - pre_p.x, next_p.y - pre_p.y);

    auto ratio = pre_curr.InnerProd(pre_next) /
                 (pre_next.Length() * pre_next.Length() + kEpison);
    *project_point = TL::common::math::InterpolateUsingLinearApproximation(
        pre_p, next_p, pre_p.s + ratio * pre_next.Length());
    return;
  }
  *project_point = path.at(next_path_point_idx);
}

// ========== GetCompletePathSize ==========

size_t OpenSpacePathPartition::GetCompletePathSize(
    const std::vector<PathGearPair>& partitioned_paths) {
  size_t num = 0;
  for (const auto& path_pair : partitioned_paths) {
    num += path_pair.first.size();
  }
  num = partitioned_paths.size() < 2 ? num : num + 1 - partitioned_paths.size();
  return num;
}

// ========== CalculatePoseError ==========

void OpenSpacePathPartition::CalculatePoseError(
    const double cur_x, const double cur_y, const double cur_theta,
    const double target_x, const double target_y, const double target_theta,
    planning_internal::VehicleFollowError* const pose_error) {
  const Vec2d point_to_ego(cur_x - target_x, cur_y - target_y);
  const double diff_angle =
      std::fabs(NormalizeAngle(cur_theta - point_to_ego.Angle()));
  const double diff_distance = std::hypot(target_x - cur_x, target_y - cur_y);
  const double lateral_offset = std::fabs(diff_distance * std::sin(diff_angle));
  const double longitudinal_offset =
      std::fabs(diff_distance * std::cos(diff_angle));
  pose_error->set_angle_offset(
      std::fabs(NormalizeAngle(cur_theta - target_theta)));
  pose_error->set_lateral_offset(lateral_offset);
  pose_error->set_longitudinal_offset(longitudinal_offset);
}

// ========== IsAddStartPointToPath ==========

bool OpenSpacePathPartition::IsAddStartPointToPath(
    const common::PathPoint& pre_p, const common::PathPoint& curr_p,
    const common::PathPoint& next_p, common::PathPoint* const proj_p) {
  static constexpr double kEpison = 1e-10;
  Vec2d pre_curr(curr_p.x - pre_p.x, curr_p.y - pre_p.y);
  Vec2d pre_next(next_p.x - pre_p.x, next_p.y - pre_p.y);

  auto ratio = pre_curr.InnerProd(pre_next) /
               (pre_next.Length() * pre_next.Length() + kEpison);
  *proj_p = TL::common::math::InterpolateUsingLinearApproximation(
      pre_p, next_p, pre_p.s + ratio * pre_next.Length());

  return DefinitelyLess(ratio, 1.0) && DefinitelyGreaterEqual(ratio, 0.0);
}

// ========== AdjustRelativeS ==========

void OpenSpacePathPartition::AdjustRelativeS(
    const std::vector<PathGearPair>& partitioned_paths,
    const size_t current_path_index, const size_t closest_path_point_index,
    PathGearPair* const chosen_partitioned_path) {
  if (chosen_partitioned_path == nullptr) {
    std::cerr << "[ERROR] AdjustRelativeS input nullptr" << std::endl;
    return;
  }

  if (partitioned_paths.empty() ||
      partitioned_paths.size() <= current_path_index ||
      partitioned_paths.at(current_path_index).first.size() <=
          closest_path_point_index) {
    // 使用当前输入的档位
    chosen_partitioned_path->second =
        (current_input_ != nullptr) ? current_input_->vehicle_state.gear
                                    : soc::GearPosition::GEAR_PARKING;
    chosen_partitioned_path->first.GenerateStopPath(
        start_point_.x, start_point_.y, start_point_.theta,
        start_point_.kappa);
    std::cerr << "[ERROR] invalid partitioned path" << std::endl;
    return;
  }

  chosen_partitioned_path->second =
      partitioned_paths.at(current_path_index).second;
  auto& des_path = chosen_partitioned_path->first;
  const auto& curr_gear = chosen_partitioned_path->second;
  const auto& src_path = partitioned_paths[current_path_index].first;

  // MVP: 简化 gear change 检测
  bool is_gear_changed = false;
  if (current_input_ != nullptr &&
      current_input_->pub_gear != curr_gear) {
    is_gear_changed = true;
  }

  bool is_add_start_point = false;
  PathPoint project_path_point;
  if (closest_path_point_index > 0) {
    is_add_start_point = IsAddStartPointToPath(
        src_path[closest_path_point_index - 1], start_point_,
        src_path[closest_path_point_index], &project_path_point);
  }

  if (current_output_ != nullptr) {
    current_output_->is_gear_changed = is_gear_changed;
  }

  if (closest_path_point_index > 0 && is_add_start_point) {
    size_t partition_size = src_path.size() - closest_path_point_index;
    des_path.resize(partition_size + 1);

    Vec2d pre(project_path_point.x, project_path_point.y);
    Vec2d curr(src_path[closest_path_point_index].x,
               src_path[closest_path_point_index].y);
    double offset_s =
        pre.DistanceTo(curr) - src_path[closest_path_point_index].s;

    des_path[0] = project_path_point;
    des_path[0].s = 0.0;
    for (size_t i = 1; i < partition_size + 1; i++) {
      PathPoint& path_point = des_path[i];
      path_point = src_path[closest_path_point_index + i - 1];
      path_point.s = path_point.s + offset_s;
    }
  } else {
    double offset_s = -src_path[closest_path_point_index].s;
    auto partition_size = src_path.size() - closest_path_point_index;
    des_path.resize(partition_size);

    for (size_t i = 0; i < partition_size; ++i) {
      PathPoint& path_point = des_path[i];
      path_point = src_path[closest_path_point_index + i];
      path_point.s = path_point.s + offset_s;
    }
  }
}

// ========== PathDecider (MVP 简化) ==========

int32_t OpenSpacePathPartition::PathDecider(
    const AlternativePath& alternative_path) {
  // 记录 debug 信息
  for (size_t i = 0; i < alternative_path.size(); ++i) {
    path_decision_debug_ += "path idx " + std::to_string(i) + " : " +
                            alternative_path[i].second.DebugString() + " \n";
  }

  // MVP: 跳过 mirror fold 检查

  if (alternative_path.empty()) {
    return -1;
  }
  if (alternative_path.size() < 2) {
    const auto& executable_status =
        alternative_path.front().second.executable_status;
    return (executable_status == EXCUTABLE ||
            executable_status == TOO_SHORT_TO_LAUNCH ||
            executable_status == COLLISION_RISK)
               ? 0
               : -1;
  }

  const auto& history_path = alternative_path.front();
  const auto& new_path = alternative_path.back();

  // geometry adjust 路径避免频繁换挡
  if (new_path.first.path_type ==
          planning_internal::PathType::GEOMETRY_ADJUST &&
      history_path.first.path_type ==
          planning_internal::PathType::GEOMETRY_ADJUST &&
      !is_warm_start_) {
    return 0;
  }

  const bool is_history_executable =
      history_path.second.executable_status == EXCUTABLE;
  const bool is_new_path_executable =
      new_path.second.executable_status == EXCUTABLE;

  if (is_mirror_fold_) {
    return 0;
  }

  if (!is_history_executable && !is_new_path_executable) {
    if (new_path.second.executable_status == TOO_SHORT_TO_LAUNCH) {
      return 1;
    }
    return -1;
  }

  if (is_history_executable && is_new_path_executable) {
    if (new_path.second.gear_shift_num <= history_path.second.gear_shift_num) {
      return 1;
    }
    if (new_path.second.gear_shift_num > kApaGearShiftLimit) {
      return 0;
    }
    if ((new_path.first.replan_status &
         static_cast<uint32_t>(OpenSpaceStatus::TARGET_UPDATE)) != 0U) {
      return 1;
    }
    // MVP: 简化 replan 逻辑，选择换挡次数少的
    if (new_path.first.replan_status ==
            static_cast<uint32_t>(OpenSpaceStatus::TARGET_UPDATE_SLIGHTLY) ||
        new_path.first.replan_status ==
            static_cast<uint32_t>(OpenSpaceStatus::REPLAN_FOR_SPEED_WARN)) {
      return new_path.second.gear_shift_num <=
                     history_path.second.gear_shift_num
                 ? 1
                 : 0;
    }
    // 返回安全的那个
    return history_path.second.collision_distance >
                   new_path.second.collision_distance + 0.1
               ? 0
               : 1;
  }

  return is_new_path_executable ? 1 : 0;
}

// ========== IsTrackAbnormal ==========

bool OpenSpacePathPartition::IsTrackAbnormal() {
  planning_internal::VehicleFollowError error;
  CalculatePoseError(ego_x_, ego_y_, ego_theta_, start_point_.x,
                     start_point_.y, start_point_.theta, &error);
  const double distance_threshold =
      open_space_path_partition_config_.distance_search_range;
  if (error.longitudinal_offset > distance_threshold ||
      error.lateral_offset > distance_threshold) {
    if (current_output_ != nullptr) {
      current_output_->vehicle_follow_error = error;
    }
    return true;
  }
  return false;
}

// ========== IsYawTrackAbnormal ==========

bool OpenSpacePathPartition::IsYawTrackAbnormal(
    const common::PathPoint& track_point,
    const soc::GearPosition& gear) {
  bool is_yaw_track_abnomal = false;
  double yaw_track_error = track_point.theta - ego_theta_;
  int next_path_direction = gear == soc::GearPosition::GEAR_DRIVE ? 1 : -1;
  const bool is_convergence_trend =
      common::math::double_type::DefinitelyGreater(
          yaw_track_error * track_point.kappa * next_path_direction, 0.0);
  if (is_convergence_trend) {
    is_yaw_track_abnomal =
        fabs(yaw_track_error) > open_space_path_partition_config_
                                    .convergence_trend_yaw_error_threshold;
  } else {
    is_yaw_track_abnomal =
        fabs(yaw_track_error) >
        open_space_path_partition_config_.divergent_trend_yaw_error_threshold;
  }
  return is_yaw_track_abnomal;
}

// ========== IsTaskFinish ==========

bool OpenSpacePathPartition::IsTaskFinish(const PartitionedPath& chosen_path) {
  if (current_input_ == nullptr) return false;

  const auto& veh_gear = current_input_->vehicle_state.gear;
  const bool is_adc_gear_valid =
      (veh_gear == soc::GearPosition::GEAR_DRIVE ||
       veh_gear == soc::GearPosition::GEAR_REVERSE);
  if (is_adc_gear_valid && veh_gear != gear_shift_.first) {
    if (gear_shift_.first != soc::GearPosition::GEAR_PARKING) {
      ++gear_shift_.second;
    }
    gear_shift_.first = veh_gear;
  }

  bool is_task_finish = false;
  std::string msg;

  // MVP: 根据 parking_action_type 决定逻辑
  if (current_input_->parking_action_type == 0) {
    // 泊入
    if (!is_veh_reach_destination_) {
      AdcStatus adc_status;
      GetAdcStatus(current_input_->vehicle_state, chosen_path, &adc_status);
      UpdateFinishStatusBasedOnStatus(adc_status);
      UpdateReplanInfoBasedOnStatus(adc_status);
      if (IsEndReplanTriggered()) {
        task_finish_status_ =
            TL::planning_internal::OpenSpaceDebug::LARGE_ANGLE;
      }
      std::cout << "[DEBUG] " << adc_status.DebugString() << std::endl;
      if (FinishCheck() == SUCCESS) {
        is_veh_reach_destination_ = true;
      }
    }
    if (is_veh_reach_destination_) {
      msg = "Vehicle is near to destination, Parking finished";
      is_task_finish = true;
    }
  } else if (current_input_->parking_action_type == 1) {
    // 泊出
    if (is_veh_reach_destination_ ||
        IsSatisfyParkOutFinishCondition(current_input_->vehicle_state,
                                        current_input_->dest_region_with_angle)) {
      msg = "Vehicle is satisfy park out finish condition, Parking finished";
      is_veh_reach_destination_ = true;
      is_task_finish = true;
    }
  }
  // else: 其他模式不判定结束

  if (current_output_ != nullptr) {
    current_output_->destination_reached = is_veh_reach_destination_;
    current_output_->finish_status = task_finish_status_;
  }

  if (!msg.empty()) {
    std::cout << "[INFO] " << msg << std::endl;
  }

  return is_task_finish;
}

// ========== GetAdcStatus ==========

void OpenSpacePathPartition::GetAdcStatus(
    const common::VehicleState& vehicle_state,
    const PartitionedPath& chosen_path, AdcStatus* const adc_status_ptr) {
  if (nullptr == adc_status_ptr) {
    std::cerr << "[ERROR] adc status input check error." << std::endl;
    return;
  }
  const auto end_pose_enu = TaskTargetPose(chosen_path);

  if (current_input_ != nullptr && current_input_->is_vehicle_stand_still) {
    adc_status_ptr->is_stand_still = true;
  } else {
    frozen_time_ = NowInSeconds();
  }
  constexpr double kAlmostStandStillSpd = 0.3;
  adc_status_ptr->is_almost_stand_still =
      fabs(vehicle_state.linear_velocity) <= kAlmostStandStillSpd;

  adc_status_ptr->is_collision_near_target =
      IsCollisionNearTarget(vehicle_state, end_pose_enu);

  adc_status_ptr->is_reach_wheel_mask = IsReachWheelMask();

  IsBlockByCurbOrCar(end_pose_enu, &adc_status_ptr->is_block_by_curb,
                     &adc_status_ptr->is_block_by_car,
                     &adc_status_ptr->is_block_by_other_fs);

  // MVP: 简化 uss_spot 检测
  adc_status_ptr->is_uss_spot = false;

  GetAdcPosStatus(vehicle_state, end_pose_enu, adc_status_ptr);
  GetAdcHeadingStatus(vehicle_state, end_pose_enu, adc_status_ptr);

  // MVP: 简化场景类型判断
  if (current_input_ != nullptr) {
    adc_status_ptr->is_lateral_park_in =
        current_input_->parking_scenario_type ==
            ParkingScenarioType::LEFT_LATERAL_PARKING_IN ||
        current_input_->parking_scenario_type ==
            ParkingScenarioType::RIGHT_LATERAL_PARKING_IN;
  }

  adc_status_ptr->frozen_duration = NowInSeconds() - frozen_time_;
  static constexpr double plan_cycle = 0.1;
  adc_status_ptr->is_over_time =
      adc_status_ptr->frozen_duration >=
      plan_cycle *
          open_space_path_partition_config_.destination_long_time_count;
  adc_status_ptr->is_blocked_over_time =
      adc_status_ptr->frozen_duration >
      open_space_path_partition_config_.early_blocked_replan_time_threshold;

  adc_status_ptr->is_execute_last_part_path =
      IsAdcExecuteLastPartPath(chosen_path);
}

// ========== GetAdcPosStatus ==========

void OpenSpacePathPartition::GetAdcPosStatus(
    const common::VehicleState& vehicle_state,
    const common::PathPoint& end_pose_enu, AdcStatus* const adc_status_ptr) {
  if (nullptr == adc_status_ptr) {
    return;
  }

  const auto target_to_adc = Vec2d(vehicle_state.x - end_pose_enu.x,
                                   vehicle_state.y - end_pose_enu.y);
  adc_status_ptr->dist_error = target_to_adc.Length();

  static constexpr double kLonReachThresholdSlack = 0.1;
  const double distance_threshold =
      open_space_path_partition_config_
          .is_near_destination_distance_threshold;
  const double lon_distance_threshold =
      (adc_status_ptr->is_block_by_other_fs || adc_status_ptr->is_uss_spot)
          ? distance_threshold + kLonReachThresholdSlack
          : distance_threshold;
  adc_status_ptr->is_distance_reach =
      adc_status_ptr->dist_error < distance_threshold;

  // MVP: 简化版不做车位内的精确SL判断
  // 基于距离判断 lon/lat reach
  adc_status_ptr->is_lon_reach = adc_status_ptr->dist_error < lon_distance_threshold;
  adc_status_ptr->is_lat_reach = adc_status_ptr->dist_error < distance_threshold;
}

// ========== GetAdcHeadingStatus ==========

void OpenSpacePathPartition::GetAdcHeadingStatus(
    const common::VehicleState& vehicle_state,
    const common::PathPoint& end_pose_enu, AdcStatus* const adc_status_ptr) {
  if (nullptr == adc_status_ptr) {
    return;
  }
  adc_status_ptr->heading_error = std::fabs(
      common::math::AngleDiff(vehicle_state.heading, end_pose_enu.theta));
  const double angle_diff_threhold =
      open_space_path_partition_config_.is_near_destination_theta_threshold;
  adc_status_ptr->is_heading_reach =
      adc_status_ptr->heading_error < angle_diff_threhold;
}

// ========== UpdateFinishStatusBasedOnStatus ==========

void OpenSpacePathPartition::UpdateFinishStatusBasedOnStatus(
    const AdcStatus& adc_status) {
  task_finish_status_ = planning_internal::OpenSpaceDebug::UNKNOWN;

  if (adc_status.is_collision_near_target) {
    task_finish_status_ = planning_internal::OpenSpaceDebug::COLLISION_FINISH;
    return;
  }

  if (adc_status.is_reach_wheel_mask) {
    // MVP: 简化，不区分场景
    task_finish_status_ = planning_internal::OpenSpaceDebug::REACH_WHEEL_MASK;
    return;
  }

  if (adc_status.is_block_by_car && adc_status.is_lon_reach &&
      adc_status.is_lat_reach && adc_status.is_execute_last_part_path) {
    task_finish_status_ =
        planning_internal::OpenSpaceDebug::BLOCK_BY_CAR_IN_SPOT;
    return;
  }

  if (!adc_status.is_stand_still) {
    task_finish_status_ = planning_internal::OpenSpaceDebug::VEHICEL_MOVING;
    if (!adc_status.is_lateral_park_in || !adc_status.is_almost_stand_still) {
      return;
    }
    const bool is_heading_reach =
        adc_status.heading_error <=
        open_space_path_partition_config_.is_earily_finish_theta_threshold;
    const bool is_pose_reach = adc_status.is_distance_reach && is_heading_reach;
    const bool is_path_pass_through_target =
        fabs(end_point_sl_.l) < open_space_path_partition_config_
                                    .is_near_destination_distance_threshold;
    if (is_veh_prefinish_brake_saftisfied_ ||
        (is_pose_reach && is_path_pass_through_target)) {
      is_veh_prefinish_brake_saftisfied_ = true;
      task_finish_status_ =
          planning_internal::OpenSpaceDebug::PREFINISH_BRAKING;
    }
    return;
  }

  if (adc_status.is_block_by_curb && adc_status.is_lon_reach &&
      adc_status.is_lat_reach && adc_status.is_execute_last_part_path) {
    task_finish_status_ =
        planning_internal::OpenSpaceDebug::BLOCK_BY_CURB_IN_SPOT;
    return;
  }

  if (is_veh_prefinish_brake_saftisfied_) {
    task_finish_status_ = planning_internal::OpenSpaceDebug::REACH_TARGET;
    return;
  }

  if (!adc_status.is_heading_reach) {
    if (adc_status.is_distance_reach && adc_status.is_execute_last_part_path &&
        adc_status.is_over_time) {
      task_finish_status_ = planning_internal::OpenSpaceDebug::OVER_TIME;
      return;
    }
    task_finish_status_ = planning_internal::OpenSpaceDebug::LARGE_ANGLE;
    return;
  }

  if (!adc_status.is_distance_reach || !adc_status.is_execute_last_part_path ||
      !is_warm_start_) {
    task_finish_status_ = planning_internal::OpenSpaceDebug::FAR_AWAY;
    return;
  }

  task_finish_status_ = planning_internal::OpenSpaceDebug::REACH_TARGET;
}

// ========== UpdateReplanInfoBasedOnStatus ==========

void OpenSpacePathPartition::UpdateReplanInfoBasedOnStatus(
    const AdcStatus& adc_status) {
  if (task_finish_status_ ==
      TL::planning_internal::OpenSpaceDebug::PREFINISH_BRAKING) {
    return;
  }
  if (task_finish_status_ == planning_internal::OpenSpaceDebug::REACH_TARGET &&
      is_veh_prefinish_brake_saftisfied_) {
    is_veh_prefinish_brake_saftisfied_ = false;
    return;
  }
  if (FinishCheck() != SUCCESS) {
    if (!adc_status.is_heading_reach && adc_status.is_distance_reach &&
        adc_status.is_stand_still && is_warm_start_ &&
        adc_status.is_execute_last_part_path && !adc_status.is_over_time) {
      if (current_output_ != nullptr) {
        OpenSpaceStatus::UpdateReplanStatus(
            OpenSpaceStatus::END_ANGLE_UNREACHABLE,
            &current_output_->open_space_status);
      }
    }
    return;
  }

  if (adc_status.is_blocked_over_time) {
    return;
  }

  // MVP: 简化，仅处理泊入场景
  if (current_input_ == nullptr || current_input_->parking_action_type != 0) {
    return;
  }

  if (!adc_status.is_reach_wheel_mask && !adc_status.is_block_by_curb &&
      !adc_status.is_block_by_car) {
    return;
  }
  constexpr double kLatDisThreshold = 0.02;
  const double angle_diff_threshold =
      adc_status.lat_error < kLatDisThreshold
          ? open_space_path_partition_config_.is_earily_finish_theta_threshold
          : open_space_path_partition_config_
                .is_precisely_arrive_theta_threshold;
  const bool is_heading_reach = adc_status.heading_error < angle_diff_threshold;

  if (!is_heading_reach && current_output_ != nullptr) {
    std::cout << "[INFO] Not precisely arrived yet, replan again." << std::endl;
    OpenSpaceStatus::UpdateReplanStatus(
        OpenSpaceStatus::END_ANGLE_UNREACHABLE,
        &current_output_->open_space_status);
  }
}

// ========== IsEndReplanTriggered ==========

bool OpenSpacePathPartition::IsEndReplanTriggered() {
  if (current_output_ == nullptr) return false;
  const auto replan_status = current_output_->open_space_status.replan;
  return (replan_status &
          static_cast<uint32_t>(
              TL::planning::OpenSpaceStatus::END_ANGLE_UNREACHABLE)) != 0U;
}

// ========== IsCollisionNearTarget (MVP 简化) ==========

bool OpenSpacePathPartition::IsCollisionNearTarget(
    const common::VehicleState& vehicle_state,
    const common::PathPoint& end_pose_enu) {
  // MVP: 简化碰撞检测 - 仅检查 IoU
  if (current_input_ == nullptr) return false;

  const auto& park_scenario_type = current_input_->parking_scenario_type;
  const bool need_check =
      (park_scenario_type == ParkingScenarioType::LEFT_OBLIQUE_PARKING_IN ||
       park_scenario_type == ParkingScenarioType::RIGHT_OBLIQUE_PARKING_IN ||
       park_scenario_type == ParkingScenarioType::LEFT_VERTICAL_PARKING_IN ||
       park_scenario_type == ParkingScenarioType::RIGHT_VERTICAL_PARKING_IN);

  if (!need_check) return false;

  // 简化：检查车辆与目标位置的重叠
  static constexpr double valid_speed_limit = 0.05;
  if (vehicle_state.linear_velocity > valid_speed_limit &&
      vehicle_state.gear ==
          soc::GearPosition::GEAR_REVERSE) {
    const auto vehicle_polygon =
        GetPolygon2dWithBuffer(vehicle_state.x, vehicle_state.y,
                               vehicle_state.heading);
    const auto ideal_vehicle_polygon =
        GetPolygon2dWithBuffer(end_pose_enu.x, end_pose_enu.y,
                               end_pose_enu.theta);
    const double iou = vehicle_polygon.ComputeIoU(ideal_vehicle_polygon);
    return iou > open_space_path_partition_config_.reach_ideal_pose_threshold;
  }
  return false;
}

// ========== IsBlockByCurbOrCar (MVP: all false) ==========

void OpenSpacePathPartition::IsBlockByCurbOrCar(
    const common::PathPoint& end_pose_enu, bool* const is_block_by_curb,
    bool* const is_block_by_car, bool* const is_block_by_other_fs) {
  if (nullptr == is_block_by_curb || nullptr == is_block_by_car ||
      nullptr == is_block_by_other_fs) {
    return;
  }
  // MVP: 需要 freespace 感知数据，暂时全部设为 false
  // TODO: 实现基于 freespace 的障碍物检测
  *is_block_by_curb = false;
  *is_block_by_car = false;
  *is_block_by_other_fs = false;
}

// ========== IsSatisfyParkOutFinishCondition ==========

bool OpenSpacePathPartition::IsSatisfyParkOutFinishCondition(
    const common::VehicleState& vehicle_state,
    const DestRegionWithAng& dest_region_with_angle) {
  if (current_input_ == nullptr) return false;

  double base_angle = 0.5 * (std::get<1>(dest_region_with_angle) +
                             std::get<2>(dest_region_with_angle));

  bool isParkingOutFinish = false;
  auto finish_status = planning_internal::OpenSpaceDebug::UNKNOWN;

  if (!current_input_->is_vehicle_stand_still) {
    finish_status = planning_internal::OpenSpaceDebug::VEHICEL_MOVING;
  } else {
    isParkingOutFinish =
        IsVehicleReachDestinationZone(vehicle_state, dest_region_with_angle);
    if (isParkingOutFinish) {
      finish_status = planning_internal::OpenSpaceDebug::REACH_TARGET;
    } else {
      double ang_diff = std::fabs(
          common::math::AngleDiff(vehicle_state.heading, base_angle));
      finish_status = planning_internal::OpenSpaceDebug::FAR_AWAY;
      if (ang_diff > kParkOutEarlyStopAngleLimit) {
        finish_status = planning_internal::OpenSpaceDebug::LARGE_ANGLE;
      }
    }
  }
  task_finish_status_ = finish_status;
  return isParkingOutFinish;
}

// ========== IsVehicleReachDestinationZone ==========

bool OpenSpacePathPartition::IsVehicleReachDestinationZone(
    const common::VehicleState& vehicle_state,
    const DestRegionWithAng& dest_region_with_angle) {
  Vec2d vehicle_position(vehicle_state.x, vehicle_state.y);
  const auto& destregion_polygon = std::get<0>(dest_region_with_angle);
  const double destregion_fromangle = std::get<1>(dest_region_with_angle);
  const double destregion_toangle = std::get<2>(dest_region_with_angle);

  if (destregion_polygon.num_points() < 3) {
    std::cerr << "[ERROR] dest region is not valid" << std::endl;
    return false;
  }
  const bool position_in_destregion =
      destregion_polygon.DistanceTo(vehicle_position) <
      open_space_path_partition_config_
          .is_near_destination_distance_threshold;
  bool heading_in_destregion = false;
  double theta_to_destregion = 0.0;
  if (!common::math::AngleInRange(vehicle_state.heading, destregion_fromangle,
                                  destregion_toangle)) {
    theta_to_destregion =
        std::max(std::fabs(common::math::AngleDiff(vehicle_state.heading,
                                                   destregion_fromangle)),
                 std::fabs(common::math::AngleDiff(vehicle_state.heading,
                                                   destregion_toangle)));
  }
  heading_in_destregion =
      theta_to_destregion <
      open_space_path_partition_config_.is_near_destination_theta_threshold;

  return (current_input_ != nullptr &&
          current_input_->is_vehicle_stand_still &&
          position_in_destregion && heading_in_destregion);
}

// ========== TaskTargetPose ==========

common::PathPoint OpenSpacePathPartition::TaskTargetPose(
    const PartitionedPath& chosen_path) {
  static const double kEps = 1e-3;
  const bool is_apa_path =
      chosen_path.path_type !=
          planning_internal::PathType::TRACE_PATH &&
      chosen_path.path_type != planning_internal::PathType::CRUISE_PATH;
  const bool has_valid_path =
      is_apa_path && !chosen_path.path_set.empty() &&
      !chosen_path.path_set.back().first.empty() &&
      chosen_path.path_set.back().first.back().s > kEps;
  if (has_valid_path) {
    return chosen_path.path_set.back().first.back();
  }
  // fallback to input end_pose
  return (current_input_ != nullptr) ? current_input_->end_pose
                                     : common::PathPoint();
}

// ========== IsAdcExecuteLastPartPath ==========

bool OpenSpacePathPartition::IsAdcExecuteLastPartPath(
    const PartitionedPath& chosen_path) {
  if (chosen_path.path_set.empty() || current_input_ == nullptr) {
    return false;
  }
  const int path_size = static_cast<int>(chosen_path.path_set.size());
  const int path_idx = static_cast<int>(chosen_path.path_idx);
  const auto path_gear = chosen_path.path_set.at(path_idx).second;
  return (path_idx + 1 == path_size) &&
         current_input_->vehicle_state.gear == path_gear;
}

// ========== GetEndPointSLInCurrentPath ==========

bool OpenSpacePathPartition::GetEndPointSLInCurrentPath(
    const std::vector<PathGearPair>& partitioned_path, const size_t path_idx,
    common::SLPoint* const end_point_sl_ptr) {
  if (nullptr == end_point_sl_ptr) {
    return false;
  }
  static constexpr double kEps = 1e-3;
  if (partitioned_path.empty() || partitioned_path.back().first.empty() ||
      partitioned_path.back().first.back().s < kEps) {
    return false;
  }
  const auto& current_partitioned_path = partitioned_path.at(path_idx);
  const auto& end_pose = partitioned_path.back().first.back();
  if (!current_partitioned_path.first.XYToSL(end_pose.x, end_pose.y,
                                             end_point_sl_ptr)) {
    return false;
  }
  return true;
}

// ========== SetGearShiftDis ==========

double OpenSpacePathPartition::SetGearShiftDis() {
  return open_space_path_partition_config_.longitudinal_offset_to_midpoint;
  // MVP: 跳过 speed bump 检测
}

// ========== UpdateParkDisplay (MVP: 仅日志) ==========

void OpenSpacePathPartition::UpdateParkDisplay(PartitionedPath* const path) {
  if (path == nullptr) {
    return;
  }
  // MVP: 计算并打印进度，不需要写入 injector
  auto current_point_index = static_cast<double>(path->point_idx);
  auto current_total_size =
      static_cast<double>(path->path_set.at(path->path_idx).first.size());
  static constexpr double kEpsilon = 1e-6;
  if (current_total_size > kEpsilon) {
    double curr_gear_percent = current_point_index / current_total_size;
    double curr_rest_dist =
        fabs(path->path_set.at(path->path_idx).first.back().s -
             path->path_set.at(path->path_idx).first.at(path->point_idx).s);
    std::cout << "[DEBUG] Park progress: " << std::fixed << std::setprecision(1)
              << (curr_gear_percent * 100.0) << "%, rest: "
              << std::setprecision(2) << curr_rest_dist << "m" << std::endl;
  }
}

// ========== UpdateCollisionDistance ==========

void OpenSpacePathPartition::UpdateCollisionDistance(
    const PartitionedPath& path, double* const collision_distance_ptr) {
  if (nullptr == collision_distance_ptr) {
    return;
  }
  *collision_distance_ptr = INFINITY;
  if (path.path_set.empty() || path.path_idx >= path.path_set.size()) {
    return;
  }
  const auto& match_path = path.path_set[path.path_idx].first;
  if (current_input_ == nullptr) return;

  for (size_t i = path.point_idx; i < match_path.size(); ++i) {
    const auto& path_point = match_path[i];
    if (common::math::CheckCollisionWithVehiclePolygon2d(
            path_point.x, path_point.y, path_point.theta,
            current_input_->obstacles_segments_vec)) {
      *collision_distance_ptr = path_point.s - match_path[path.point_idx].s;
      break;
    }
  }
}

// ========== UpdatePathExcutableStatus ==========

void OpenSpacePathPartition::UpdatePathExcutableStatus(
    const PartitionedPath& path, uint32_t* const executable_status_ptr) {
  if (nullptr == executable_status_ptr) {
    return;
  }
  // MVP: IsPathTooShortToBrake 和 IsPathSteerRateLarge 已简化为 return false
  if (IsPathTooShortToBrake(path)) {
    *executable_status_ptr += TOO_SHORT_TO_BRAKE;
  }
  if (IsPathSteerRateLarge(path)) {
    *executable_status_ptr += LARGE_STEER_RATE;
  }
}

// ========== UpdateInfoForPreFinishCondition (MVP 简化) ==========

void OpenSpacePathPartition::UpdateInfoForPreFinishCondition(
    const OpenSpacePathDecision& openspace_path_decision,
    PathGearPair* const chosen_partitioned_path) {
  if (nullptr == chosen_partitioned_path || current_input_ == nullptr) {
    return;
  }

  const bool is_lateral_park_in =
      current_input_->parking_scenario_type ==
          ParkingScenarioType::LEFT_LATERAL_PARKING_IN ||
      current_input_->parking_scenario_type ==
          ParkingScenarioType::RIGHT_LATERAL_PARKING_IN;
  if (!is_lateral_park_in) {
    return;
  }

  if (openspace_path_decision != OpenSpacePathDecision::PREPARE_FINISH &&
      fabs(end_point_sl_.l) > open_space_path_partition_config_
                                  .is_near_destination_distance_threshold) {
    return;
  }

  // MVP: 简化路径裁剪逻辑
  constexpr double kPreFinishClipLength = 0.1;
  if (openspace_path_decision != OpenSpacePathDecision::PREPARE_FINISH) {
    return;
  }

  if (!is_clipped_) {
    clipped_point_ = chosen_partitioned_path->first.back();
    for (const auto& point : chosen_partitioned_path->first) {
      if (point.s > kPreFinishClipLength) {
        clipped_point_ = point;
        break;
      }
    }
  }

  auto iter = chosen_partitioned_path->first.begin();
  bool is_reach_clipped = false;
  while (iter != chosen_partitioned_path->first.end()) {
    if (is_reach_clipped) {
      iter = chosen_partitioned_path->first.erase(iter);
    } else {
      is_reach_clipped = DiscretizedPath::IsSamePoint(*iter, clipped_point_);
      iter++;
    }
  }
  if (!is_reach_clipped) {
    chosen_partitioned_path->first.GenerateStopPath(
        chosen_partitioned_path->first.begin()->x,
        chosen_partitioned_path->first.begin()->y,
        chosen_partitioned_path->first.begin()->theta,
        chosen_partitioned_path->first.begin()->kappa);
  }
  is_clipped_ = true;
}

}  // namespace planning
}  // namespace TL
