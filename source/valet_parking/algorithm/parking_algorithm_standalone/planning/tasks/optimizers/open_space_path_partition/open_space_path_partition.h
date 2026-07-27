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
 * @file open_space_path_partition.h
 * @brief 独立编译改造版：路径分段决策模块
 *
 * 改造要点：
 * - 去掉 PathOptimizer 继承
 * - 去掉 frame_/injector_ 依赖，改为 PartitionInput/PartitionOutput
 * - VehicleParam 改用 planning::VehicleParam (struct)
 * - Config 改用 OpenSpacePathPartitionConfig (struct)
 * - absl::StrCat → std::string 拼接
 * - Clock::NowInSeconds() → chrono
 * - FLAGS_xxx → config 成员或默认值
 **/

#pragma once

#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <queue>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "common/math/box2d.h"
#include "common/math/polygon2d.h"
#include "common/status/status.h"
#include "planning/common/open_space_info_lite.h"
#include "planning/common/path/discretized_path.h"
#include "planning/open_space/vehicle_param.h"

#include "proto_convert/pnc_point_convert.h"
#include "proto_convert/chassis_convert.h"
#include "proto_convert/vehicle_state_convert.h"
#include "proto_convert/planning_internal_convert.h"
#include "proto_convert/open_space_path_partition_config_convert.h"
#include "proto_convert/open_space_status_convert.h"

namespace TL {
namespace planning {

// ========== 比较器 ==========
struct pair_comp_ {
  bool operator()(
      const std::pair<std::pair<size_t, size_t>, double>& left,
      const std::pair<std::pair<size_t, size_t>, double>& right) const {
    return left.second <= right.second;
  }
};

struct comp_ {
  bool operator()(const std::pair<size_t, double>& left,
                  const std::pair<size_t, double>& right) {
    return left.second <= right.second;
  }
};

using PointsMatchPriorityQueue =
    std::priority_queue<std::pair<size_t, double>,
                        std::vector<std::pair<size_t, double>>, comp_>;

using PointsMatchWithPathPriorityQueue = std::priority_queue<
    std::pair<std::pair<size_t, size_t>, double>,
    std::vector<std::pair<std::pair<size_t, size_t>, double>>, pair_comp_>;

// ========== 枚举 ==========
enum OpenSpacePathDecision {
  UNKOWN = 0,
  TASK_FINISH = 1,
  TRACK_ABNORMAL = 2,
  NO_VALID_PATH = 3,
  CHOOSE_NEW_PATH = 4,
  CHOOSE_HISTORY_PATH = 5,
  PREPARE_FINISH = 6,
};

enum OpenSpacePathExcutableStatus {
  EXCUTABLE = 0,
  TOO_SHORT_TO_LAUNCH = 1,
  TOO_SHORT_TO_BRAKE = 2,
  LARGE_STEER_RATE = 4,
  LARGE_YAW_ERROR_IN_GEAR_SHIFT = 8,
  COLLISION_RISK = 16,
};

// ========== AdcStatus ==========
struct AdcStatus {
  double lat_error = INFINITY;
  double lon_error = INFINITY;
  double dist_error = INFINITY;
  double heading_error = INFINITY;
  double frozen_duration = INFINITY;
  bool is_stand_still = false;
  bool is_almost_stand_still = false;
  bool is_collision_near_target = false;
  bool is_reach_wheel_mask = false;
  bool is_block_by_car = false;
  bool is_block_by_curb = false;
  bool is_block_by_other_fs = false;
  bool is_heading_reach = false;
  bool is_distance_reach = false;
  bool is_lon_reach = false;
  bool is_lat_reach = false;
  bool is_over_time = false;
  bool is_blocked_over_time = false;
  bool is_lateral_park_in = false;
  bool is_uss_spot = false;
  bool is_execute_last_part_path = false;

  std::string DebugString() const {
    std::ostringstream oss;
    oss << "lat_error: " << lat_error
        << "\n lon_error: " << lon_error
        << "\n dist_error: " << dist_error
        << "\n heading_error: " << heading_error
        << "\n frozen_duration: " << frozen_duration
        << "\n is_stand_still: " << is_stand_still
        << "\n is_almost_stand_still: " << is_almost_stand_still
        << "\n is_collision_near_target: " << is_collision_near_target
        << "\n is_reach_wheel_mask: " << is_reach_wheel_mask
        << "\n is_block_by_car: " << is_block_by_car
        << "\n is_block_by_curb: " << is_block_by_curb
        << "\n is_block_by_other_fs: " << is_block_by_other_fs
        << "\n is_heading_reach: " << is_heading_reach
        << "\n is_distance_reach: " << is_distance_reach
        << "\n is_lon_reach: " << is_lon_reach
        << "\n is_lat_reach: " << is_lat_reach
        << "\n is_lateral_park_in: " << is_lateral_park_in
        << "\n is_uss_spot: " << is_uss_spot
        << "\n is_execute_last_part_path: " << is_execute_last_part_path;
    return oss.str();
  }
};

// ========== OpenSpacePathDeciderStatus ==========
struct OpenSpacePathDeciderStatus {
  uint8_t gear_shift_num{0};
  uint32_t executable_status{0};
  double collision_distance = INFINITY;

  void Reset() {
    gear_shift_num = 0;
    executable_status = 0;
    collision_distance = INFINITY;
  }

  std::string DebugString() const {
    return "gear_shift_num: " + std::to_string(gear_shift_num) +
           " executable_status: " + std::to_string(executable_status) +
           " collision_distance: " + std::to_string(collision_distance);
  }
};

using AlternativePath =
    std::vector<std::pair<PartitionedPath, OpenSpacePathDeciderStatus>>;

enum FinishType {
  ONRUN = 0,
  FAIL = 1,
  SUCCESS = 2,
  TBD = 3,
};

// ==========================================================================
// PartitionInput: 替代 frame_ 和 injector_ 的所有外部数据
// ==========================================================================
struct PartitionInput {
  // ---- 车辆状态 ----
  common::VehicleState vehicle_state;
  common::PathPoint planning_start_point;  // frame_->PlanningStartPoint().path_point()
  soc::GearPosition pub_gear = soc::GearPosition::GEAR_PARKING;  // 上一周期发布的档位

  // ---- 路径搜索结果 ----
  PartitionedPath path_result;  // 新搜索出的分段路径

  // ---- 场景信息 ----
  ParkingScenarioType parking_scenario_type = ParkingScenarioType::DEFAULT_TYPE;
  bool is_parking_inwards = false;

  // ---- 终点区域 ----
  common::PathPoint end_pose;
  DestRegionWithAng dest_region_with_angle;

  // ---- 障碍物（用于碰撞检测） ----
  std::vector<std::pair<common::math::LineSegment2d, double>> obstacles_segments_vec;

  // ---- 前一周期状态 (MVP: 简化) ----
  bool is_vehicle_stand_still = true;
  bool replan_triggered_by_speed_plan = false;
  bool guard_triggered = false;

  // ---- 重规划状态（输入） ----
  uint32_t input_replan_status = 0;

  // ---- 速度计划碰撞信息 (MVP: 默认值) ----
  planning_internal::AvpSpeedPlanCollisionInfo speed_plan_collision_info;

  // ---- MVP 简化：泊入/泊出类型 ----
  // 0=PARKING_IN, 1=PARKING_OUT, 2=NNS等
  int parking_action_type = 0;  // 0=泊入
};

// ==========================================================================
// PartitionOutput: 替代 frame_->mutable_open_space_info() 的所有输出
// ==========================================================================
struct PartitionOutput {
  // ---- 分段路径输出 ----
  PartitionedPath partitioned_paths;
  PathGearPair chosen_partitioned_path;
  std::pair<size_t, size_t> chosen_path_idx = {0, 0};

  // ---- 决策结果 ----
  OpenSpacePathDecision path_decision = OpenSpacePathDecision::UNKOWN;
  std::string path_decision_debug;

  // ---- 终止状态 ----
  planning_internal::OpenSpaceDebug::FinishStatus finish_status =
      planning_internal::OpenSpaceDebug::UNKNOWN;
  bool destination_reached = false;
  bool is_stop_path = false;

  // ---- 跟踪误差 ----
  planning_internal::VehicleFollowError vehicle_follow_error;
  planning_internal::VehicleFollowError vehicle_to_current_end_error;

  // ---- 重规划状态（输出） ----
  OpenSpaceStatus open_space_status;

  // ---- 后视镜折叠 ----
  bool is_mirror_fold = false;

  // ---- 换挡信息 ----
  bool is_gear_changed = false;
};

// ==========================================================================
// OpenSpacePathPartition 类
// ==========================================================================
class OpenSpacePathPartition {
 public:
  /**
   * @brief 构造函数（独立编译版）
   * @param config 路径分段配置
   * @param vehicle_param 车辆参数
   */
  OpenSpacePathPartition(const OpenSpacePathPartitionConfig& config,
                         const VehicleParam& vehicle_param);

  ~OpenSpacePathPartition() = default;

  /**
   * @brief 重置内部状态
   */
  common::Status Reset();

  /**
   * @brief 主入口：执行路径分段决策
   * @param input 输入数据（替代 frame_/injector_）
   * @param output 输出结果
   * @return Status
   */
  common::Status Execute(const PartitionInput& input, PartitionOutput* output);

 private:
  // ---- 时钟辅助 ----
  static double NowInSeconds() {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(now.time_since_epoch()).count();
  }

  // ---- 参数更新 ----
  void UpdateParam(const PartitionInput& input);

  // ---- 路径决策 ----
  void UpdatePathDecision(const PartitionInput& input,
                          OpenSpacePathDecision* openspace_path_decision_ptr,
                          PartitionedPath* choose_path_ptr);

  int32_t PathDecider(const AlternativePath& alternative_path);

  // ---- 路径匹配 ----
  bool PathMatch(PartitionedPath* partition_path_ptr,
                 uint32_t* executable_status_ptr);

  bool GetPathMatchIdx(const PartitionedPath& partition_path, size_t path_idx,
                       size_t* path_point_idx_ptr);

  bool PointMatch(const common::PathPoint& path_point,
                  const soc::GearPosition& gear);

  void UpdatePathMatchIdx(size_t match_path_idx, size_t match_point_idx,
                          PartitionedPath* partition_path_ptr,
                          uint32_t* executable_status_ptr);

  void UpdateHistoryPath(const PartitionedPath& path) { history_path_ = path; }

  // ---- 路径工具 ----
  void AdjustRelativeS(const std::vector<PathGearPair>& partitioned_paths,
                       size_t current_path_index,
                       size_t closest_path_point_index,
                       PathGearPair* chosen_partitioned_path);

  static size_t GetCompletePathSize(
      const std::vector<PathGearPair>& partitioned_paths);

  static void CalculatePoseError(
      double cur_x, double cur_y, double cur_theta, double target_x,
      double target_y, double target_theta,
      planning_internal::VehicleFollowError* pose_error);

  static bool IsAddStartPointToPath(const common::PathPoint& pre_p,
                                    const common::PathPoint& curr_p,
                                    const common::PathPoint& next_p,
                                    common::PathPoint* proj_p);

  static void ProjectOnPath(const DiscretizedPath& path,
                            size_t next_path_point_idx,
                            const common::PathPoint& curr_p,
                            common::PathPoint* project_point);

  static bool GetEndPointSLInCurrentPath(
      const std::vector<PathGearPair>& partitioned_path, size_t path_idx,
      common::SLPoint* end_point_sl_ptr);

  void SetStopPath(const soc::GearPosition& pub_gear,
                   const PartitionedPath& reserved_partitioned_paths,
                   PartitionedPath* partitioned_paths_ptr);

  static void ResetPartitionPath(PartitionedPath* partitioned_paths_ptr) {
    if (nullptr != partitioned_paths_ptr) {
      partitioned_paths_ptr->path_idx = 0;
      partitioned_paths_ptr->point_idx = 0;
      partitioned_paths_ptr->path_shift = false;
      partitioned_paths_ptr->path_set.clear();
      partitioned_paths_ptr->path_type =
          planning_internal::PathType::DEFAULT;
    }
  }

  // ---- 终止判断 ----
  bool IsTaskFinish(const PartitionedPath& chosen_path);
  common::PathPoint TaskTargetPose(const PartitionedPath& chosen_path);

  void GetAdcStatus(const common::VehicleState& vehicle_state,
                    const PartitionedPath& chosen_path,
                    AdcStatus* adc_status_ptr);

  void GetAdcPosStatus(const common::VehicleState& vehicle_state,
                       const common::PathPoint& end_pose_enu,
                       AdcStatus* adc_status_ptr);

  void GetAdcHeadingStatus(const common::VehicleState& vehicle_state,
                           const common::PathPoint& end_pose_enu,
                           AdcStatus* adc_status_ptr);

  void UpdateFinishStatusBasedOnStatus(const AdcStatus& adc_status);
  void UpdateReplanInfoBasedOnStatus(const AdcStatus& adc_status);
  bool IsEndReplanTriggered();

  // ---- 碰撞/障碍检测 ----
  bool IsCollisionNearTarget(const common::VehicleState& vehicle_state,
                             const common::PathPoint& end_pose_enu);

  // ---- 跟踪异常 ----
  bool IsTrackAbnormal();
  bool IsYawTrackAbnormal(const common::PathPoint& track_point,
                          const soc::GearPosition& gear);

  // ---- 换挡 ----
  bool AbleToGearShift(const PartitionedPath& partition_path,
                       double lon_dis_to_gear_shift_point,
                       size_t* next_path_idx_ptr,
                       size_t* next_path_point_idx_ptr,
                       uint32_t* executable_status_ptr);
  double SetGearShiftDis();

  bool IsAdcExecuteLastPartPath(const PartitionedPath& chosen_path);

  // ---- 路径可执行性 ----
  void UpdateCollisionDistance(const PartitionedPath& path,
                               double* collision_distance_ptr);

  // ---- 目标区域到达判断 ----
  bool IsSatisfyParkOutFinishCondition(
      const common::VehicleState& vehicle_state,
      const DestRegionWithAng& dest_region_with_angle);

  bool IsVehicleReachDestinationZone(
      const common::VehicleState& vehicle_state,
      const DestRegionWithAng& dest_region_with_angle);

  FinishType FinishCheck() {
    FinishType ret = ONRUN;
    switch (task_finish_status_) {
      case planning_internal::OpenSpaceDebug::REACH_TARGET:
      case planning_internal::OpenSpaceDebug::COLLISION_FINISH:
      case planning_internal::OpenSpaceDebug::REACH_WHEEL_MASK:
      case planning_internal::OpenSpaceDebug::BLOCK_BY_CURB_IN_SPOT:
      case planning_internal::OpenSpaceDebug::BLOCK_BY_CAR_IN_SPOT: {
        ret = SUCCESS;
        break;
      }
      case planning_internal::OpenSpaceDebug::OVER_TIME: {
        ret = FAIL;
        break;
      }
      case planning_internal::OpenSpaceDebug::UNKNOWN: {
        ret = TBD;
        break;
      }
      default: {
        break;
      }
    }
    return ret;
  }

  static bool IsVerticalParkOut(
      const ParkingScenarioType& parking_scenario_type) {
    return (parking_scenario_type == LEFT_VERTICAL_PARKING_OUT ||
            parking_scenario_type == RIGHT_VERTICAL_PARKING_OUT ||
            parking_scenario_type == FORWARD_VERTICAL_PARKING_OUT ||
            parking_scenario_type == LEFT_OBLIQUE_PARKING_OUT ||
            parking_scenario_type == RIGHT_OBLIQUE_PARKING_OUT ||
            parking_scenario_type == FORWARD_OBLIQUE_PARKING_OUT);
  }

  static void GetReplanStatusBasedExcutableStatus(
      const OpenSpacePathDeciderStatus& open_space_path_decider_status,
      uint32_t* replan_status_ptr) {
    if (nullptr == replan_status_ptr) {
      return;
    }
    const uint32_t executable_status =
        open_space_path_decider_status.executable_status;
    if ((executable_status & TOO_SHORT_TO_LAUNCH) != 0) {
      *replan_status_ptr +=
          static_cast<uint32_t>(OpenSpaceStatus::FREEZE_NEAR_END);
    }
    if ((executable_status & LARGE_YAW_ERROR_IN_GEAR_SHIFT) != 0) {
      *replan_status_ptr +=
          static_cast<uint32_t>(OpenSpaceStatus::YAW_TRACK_ABNORMAL);
    }
  }

  // ---- MVP 简化桩函数 ----
  void UpdateParkDisplay(PartitionedPath* path);  // → 仅日志
  void UpdateStatusBasedPartitionResult();         // → 简化
  bool IsVehOnRoad() { return false; }            // TODO: MVP 简化
  bool IsMirrorFold() { return false; }           // TODO: MVP 简化
  void IsBlockByCurbOrCar(const common::PathPoint&, bool*, bool*, bool*);  // → all false
  bool IsPathTooShortToBrake(const PartitionedPath&) { return false; }     // TODO: MVP
  bool IsPathSteerRateLarge(const PartitionedPath&) { return false; }      // TODO: MVP
  bool IsReachWheelMask() { return false; }       // TODO: MVP
  void UpdatePathExcutableStatus(const PartitionedPath& path,
                                 uint32_t* executable_status_ptr);
  void UpdateInfoForPreFinishCondition(
      const OpenSpacePathDecision& openspace_path_decision,
      PathGearPair* chosen_partitioned_path);
  bool IsUseSpeedWarnReplan(uint32_t) { return false; }  // TODO: MVP

  // ---- 成员变量 ----
  OpenSpacePathPartitionConfig open_space_path_partition_config_;
  VehicleParam vehicle_param_;

  std::pair<soc::GearPosition, uint16_t> gear_shift_ = {
      soc::GearPosition::GEAR_PARKING, 0};
  PartitionedPath history_path_;
  planning_internal::OpenSpaceDebug::FinishStatus task_finish_status_ =
      planning_internal::OpenSpaceDebug::UNKNOWN;
  std::string path_decision_debug_;
  double ego_x_ = 0.0;
  double ego_y_ = 0.0;
  double ego_theta_ = 0.0;
  common::PathPoint start_point_;
  bool is_warm_start_ = false;
  double frozen_near_end_time_ = 0.0;
  double frozen_time_ = 0.0;
  bool is_veh_reach_destination_ = false;
  bool is_veh_prefinish_brake_saftisfied_ = false;
  bool is_clipped_ = false;
  double yaw_track_abnormal_start_time_ = 0.0;
  common::SLPoint end_point_sl_;
  common::PathPoint clipped_point_;
  bool is_mirror_fold_ = false;

  // ---- 缓存输入引用 ----
  const PartitionInput* current_input_ = nullptr;
  PartitionOutput* current_output_ = nullptr;
};

}  // namespace planning
}  // namespace TL
