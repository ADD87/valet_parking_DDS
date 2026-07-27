/*
 * Copyright (c) TL Technologies Co., Ltd. 2022. All rights reserved.
 * Description:  open_space_fine_tuning.cc
 *
 * 独立编译改造（P2，doc 159）：
 *   1. 移除 TaskConfig / DependencyInjector 依赖
 *   2. 移除 hdmap_avp_state.h (AVPStatus 定义)
 *   3. 构造器改为 (VehicleParam, FineTuningConfig)
 *   4. Process() 参数：bool is_parking_in + 独立 obs 参数
 *   5. IsBlockByInnerFs → 始终返回 false（无 frame_history）
 *   6. FineTuneTargetBasedOnEndReplanPose → has_end_replaned_ 恒 false
 *   7. vehicle_params_.xxx() → .xxx（直接成员）
 *   8. config_.open_space_roi_decider_config().xxx() → config_.xxx
 *   9. AERROR/ADEBUG/AINFO → LOG(ERROR)/VLOG(2)/LOG(INFO)
 *  10. double_type::SeemsEqual → std::fabs(a-b) < kEpsilon
 */
#include <algorithm>
#include <chrono>
#include <cmath>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "common/math/linear_interpolation.h"
#include "common/math/math_utils.h"
#include "common/math/vec2d.h"
#include "common/status/status.h"
#include "planning/tasks/deciders/open_space_decider/open_space_fine_tuning.h"

// 独立编译改造：common::Clock 不可用（clock.h 为二进制文件）
// 使用 std::chrono 替代
namespace {
double NowInSeconds() {
  return std::chrono::duration<double>(
      std::chrono::steady_clock::now().time_since_epoch()).count();
}  // NOLINT
}  // namespace for NowInSeconds

using TL::common::ErrorCode;
using TL::common::Status;
using TL::common::math::Vec2d;

namespace {
constexpr double kEpsilon = 1.0e-3;
}  // namespace

namespace TL {
namespace planning {

// ============================================================
// Constructor
// 独立编译改造：替换 (TaskConfig, DependencyInjector) → (VehicleParam, FineTuningConfig)
// ============================================================
OpenSpaceFineTuning::OpenSpaceFineTuning(const VehicleParam& vehicle_params,
                                          const FineTuningConfig& config)
    : config_(config), vehicle_params_(vehicle_params) {}

// ============================================================
// Process
// 独立编译改造：
//   parking_type → bool is_parking_in
//   OpenSpacePathInfo → (obs_segments, origin, rotate_angle, is_parking_inwards)
// ============================================================
Status OpenSpaceFineTuning::Process(
    const bool is_entered_lateral_slot_domain,
    const bool is_consider_wheel_mask,
    const bool is_parking_in,
    const planning::ParkLotInfo& park_lot,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        obstacles_segments_vec,
    const common::math::Vec2d& origin,
    const double rotate_angle,
    const bool is_parking_inwards,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        linked_obstacles_segments_vec,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        high_curb_obstacles_segments_vec,
    const common::PathPoint& veh_point,
    const common::math::LineSegment2d& reference_curb,
    common::PathPoint* const end_pose_enu_ptr) {
  if (nullptr == end_pose_enu_ptr) {
    const std::string msg = "Fine tuning failed, end_pose_enu_ptr is nullptr";
    LOG(ERROR) << "[FINE_TUNE] " << msg;
    return Status(ErrorCode::PLANNER_PARKING_ROIDECIDER_ERROR, msg);
  }
  is_entered_lateral_slot_domain_ = is_entered_lateral_slot_domain;
  raw_end_pose_enu_ = *end_pose_enu_ptr;
  target_adc_coord_heading_ = raw_end_pose_enu_.theta - M_PI_2;
  const bool is_lat_slot =
      (TL::perception::ParkingLotOut::LATERAL == park_lot.park_type);
  // 独立编译改造：parking_type != PARKING_IN → !is_parking_in
  if (!is_parking_in) {
    target_adc_coord_heading_ =
        (park_lot.vertices.at(3) - park_lot.vertices.at(0)).Angle();
  } else if (is_lat_slot) {
    target_adc_coord_heading_ = park_lot.is_right_side
                                    ? raw_end_pose_enu_.theta
                                    : raw_end_pose_enu_.theta + M_PI;
  }
  const auto lat_space =
      GetTopHalfLatSpace(is_lat_slot, obstacles_segments_vec);
  const double last_lat_fine_tune_dis = lat_fine_tune_dis_;
  const double last_lon_fine_tune_dis = lon_fine_tune_dis_;
  const double last_yaw_fine_tune_rad = yaw_fine_tune_rad_;
  is_fine_tune_update_ = false;
  // 独立编译改造：FineTuneParkOutTargetBasedOnObs 参数去掉 parking_type，改用 is_parking_in
  if (FineTuneParkOutTargetBasedOnObs(is_parking_in, park_lot.sensor_type,
                                      park_lot.park_type, park_lot.vertices,
                                      veh_point, reference_curb)) {
    const auto fine_tune_target = GetFineTuneTarget();
    end_pose_enu_ptr->x = fine_tune_target.x();
    end_pose_enu_ptr->y = fine_tune_target.y();
    end_pose_enu_ptr->theta = raw_end_pose_enu_.theta + yaw_fine_tune_rad_;
    if (std::fabs(last_lat_fine_tune_dis - lat_fine_tune_dis_) > kEpsilon ||
        std::fabs(last_lon_fine_tune_dis - lon_fine_tune_dis_) > kEpsilon ||
        std::fabs(common::math::AngleDiff(yaw_fine_tune_rad_,
                                          last_yaw_fine_tune_rad)) > kEpsilon) {
      is_fine_tune_update_ = true;
    }
    return Status::OK();
  }
  // 独立编译改造：!is_parking_in → return early
  if (!is_parking_in) {
    return Status::OK();
  }
  FineTuneParkInTargetBasedOnObs(veh_point, park_lot, obstacles_segments_vec,
                                 linked_obstacles_segments_vec,
                                 high_curb_obstacles_segments_vec, origin,
                                 rotate_angle);
  FineTuneTargetBasedOnNarrowLatSpot(is_lat_slot, obstacles_segments_vec,
                                     lat_space);
  FineTuneTargetBasedOnEndReplanPose(
      veh_point, park_lot.is_right_side, park_lot.park_type, origin,
      target_adc_coord_heading_, lat_space, is_consider_wheel_mask,
      is_parking_inwards);
  if (std::fabs(last_lat_fine_tune_dis - lat_fine_tune_dis_) > kEpsilon ||
      std::fabs(last_lon_fine_tune_dis - lon_fine_tune_dis_) > kEpsilon ||
      std::fabs(common::math::AngleDiff(yaw_fine_tune_rad_,
                                        last_yaw_fine_tune_rad)) > kEpsilon) {
    is_fine_tune_update_ = true;
  }
  const auto fine_tune_target = GetFineTuneTarget();
  end_pose_enu_ptr->x = fine_tune_target.x();
  end_pose_enu_ptr->y = fine_tune_target.y();
  is_init_park_in_ = false;
  return Status::OK();
}

// ============================================================
// Reset
// ============================================================
Status OpenSpaceFineTuning::Reset() {
  is_entered_lateral_slot_domain_ = false;
  lon_fine_tune_dis_ = 0.0;
  lat_fine_tune_dis_ = 0.0;
  yaw_fine_tune_rad_ = 0.0;
  target_adc_coord_heading_ = 0.0;
  need_lat_fine_tune_ = false;
  has_end_replaned_ = false;
  has_narrow_lon_tune_ = false;
  pre_narrow_lon_fine_tune_dis_ = 0.0;
  is_init_park_in_ = true;
  VLOG(2) << "[FINE_TUNE] fine tuning is reseted";
  return Status::OK();
}

// ============================================================
// GetFineTuneTarget（两个重载）
// ============================================================
common::math::Vec2d OpenSpaceFineTuning::GetFineTuneTarget() {
  Vec2d fine_tune_target{raw_end_pose_enu_.x, raw_end_pose_enu_.y};
  return fine_tune_target +
         lat_fine_tune_dis_ *
             common::math::Vec2d::CreateUnitVec2d(target_adc_coord_heading_) +
         lon_fine_tune_dis_ * common::math::Vec2d::CreateUnitVec2d(
                                  target_adc_coord_heading_ + M_PI_2);
}

common::math::Vec2d OpenSpaceFineTuning::GetFineTuneTarget(
    const double lat_fine_tune_dis, const double lon_fine_tune_dis) {
  Vec2d fine_tune_target{raw_end_pose_enu_.x, raw_end_pose_enu_.y};
  return fine_tune_target +
         lat_fine_tune_dis *
             common::math::Vec2d::CreateUnitVec2d(target_adc_coord_heading_) +
         lon_fine_tune_dis * common::math::Vec2d::CreateUnitVec2d(
                                 target_adc_coord_heading_ + M_PI_2);
}

// ============================================================
// InitConfig
// 独立编译改造：config_.open_space_roi_decider_config().xxx() → config_.xxx
// ============================================================
void OpenSpaceFineTuning::InitConfig(const bool is_lat_slot,
                                      const bool is_enable_lat_fine_tune,
                                      const bool is_enable_lon_fine_tune) {
  search_config_.x_grid_resolution = config_.park_in_lat_adjust_resolution;
  search_config_.y_grid_resolution = config_.park_in_lon_adjust_resolution;
  search_config_.xy_bounds.resize(4);
  if (is_enable_lat_fine_tune) {
    search_config_.xy_bounds[0] =
        is_lat_slot
            ? -1 * config_.park_in_lateral_target_lat_adjust_threshold
            : -1 * config_.park_in_vertical_target_lat_adjust_threshold;
    search_config_.xy_bounds[1] =
        is_lat_slot ? config_.park_in_lateral_target_lat_adjust_threshold
                    : config_.park_in_vertical_target_lat_adjust_threshold;
  } else {
    search_config_.xy_bounds[0] = 0.0;
    search_config_.xy_bounds[1] = 0.0;
  }
  if (is_enable_lon_fine_tune) {
    search_config_.xy_bounds[2] = 0.0;
    search_config_.xy_bounds[3] =
        is_lat_slot ? config_.park_in_lateral_target_lon_adjust_threshold
                    : config_.park_in_vertical_target_lon_adjust_threshold;
  } else {
    search_config_.xy_bounds[2] = 0.0;
    search_config_.xy_bounds[3] = 0.0;
  }
}

// ============================================================
// FineTuneParkInTargetBasedOnObs
// 独立编译改造：vehicle_params_.xxx() → vehicle_params_.xxx
// ============================================================
void OpenSpaceFineTuning::FineTuneParkInTargetBasedOnObs(
    const common::PathPoint& veh_point,
    const planning::ParkLotInfo& park_lot_info,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        original_obs_segments_pair,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        linked_obs_segments_pair,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        high_curb_obs_segments_pair,
    const common::math::Vec2d& origin, const double rotate_angle) {
  if (park_lot_info.sensor_type == TL::perception::ParkingLotOut::USS) {
    return;
  }
  const bool is_lat_slot =
      (TL::perception::ParkingLotOut::LATERAL == park_lot_info.park_type);
  const bool is_parking_lot_update = park_lot_info.is_parking_lot_update;
  const bool is_right_side = park_lot_info.is_right_side;
  const auto curr_need_lat_fine_tune = [&]() {
    if (IsBlockByInnerFs(is_lat_slot, origin, rotate_angle)) {
      return true;
    }
    constexpr double kTriggerFineTuneThetaThreshold = 0.35;
    const double angle_diff =
        common::math::AngleDiff(veh_point.theta, raw_end_pose_enu_.theta);
    if (std::fabs(angle_diff) > kTriggerFineTuneThetaThreshold) {
      return false;
    }
    return IsTargetCollisionWithLinkedObs(is_lat_slot, is_parking_lot_update,
                                          linked_obs_segments_pair) ||
           IsTargetCollisionWithHighCurbObs(is_lat_slot, is_parking_lot_update,
                                            high_curb_obs_segments_pair);
  };
  const bool is_enable_lat_fine_tune =
      curr_need_lat_fine_tune() ||
      (need_lat_fine_tune_ && is_parking_lot_update);
  const bool is_enable_lon_fine_tune =
      is_lat_slot && (is_init_park_in_ ||
                      (park_lot_info.is_high_quality_triggered &&
                       IsBlockByInnerFs(is_lat_slot, origin, rotate_angle)));
  if (!is_enable_lat_fine_tune && !is_enable_lon_fine_tune) {
    return;
  }

  InitConfig(is_lat_slot, is_enable_lat_fine_tune, is_enable_lon_fine_tune);
  // 独立编译改造：vehicle_params_.length() → vehicle_params_.length
  const double shift_distance =
      0.5 * vehicle_params_.length - vehicle_params_.back_edge_to_center;
  Vec2d target_adc_center =
      Vec2d(raw_end_pose_enu_.x, raw_end_pose_enu_.y) +
      shift_distance * Vec2d::CreateUnitVec2d(raw_end_pose_enu_.theta);
  const auto fine_tune_obs = std::make_shared<OpenSpaceFineTuningObs>();
  for (const auto& obs_seg_pair : original_obs_segments_pair) {
    if (obs_seg_pair.second < kEpsilon) {
      continue;
    }
    fine_tune_obs->original_buffer.emplace_back(obs_seg_pair.second);
    fine_tune_obs->obs_segments_pair.emplace_back(obs_seg_pair);
    fine_tune_obs->obs_segments_pair.back().first.Transform(
        target_adc_center, target_adc_coord_heading_);
  }
  for (const auto& obs_seg_pair : high_curb_obs_segments_pair) {
    if (obs_seg_pair.second < kEpsilon) {
      continue;
    }
    fine_tune_obs->original_buffer.emplace_back(obs_seg_pair.second);
    fine_tune_obs->obs_segments_pair.emplace_back(obs_seg_pair);
    fine_tune_obs->obs_segments_pair.back().first.Transform(
        target_adc_center, target_adc_coord_heading_);
  }
  if (fine_tune_obs->obs_segments_pair.size() !=
      fine_tune_obs->original_buffer.size()) {
    LOG(ERROR) << "[FINE_TUNE] obs_segments_pair size is not equal to original_buffer size!";
    return;
  }

  if (ValidityCheck(is_lat_slot, is_right_side, lat_fine_tune_dis_,
                    lon_fine_tune_dis_, fine_tune_obs)) {
    VLOG(2) << "[FINE_TUNE] target pose is collision free using previous lon_fine_tune_dis "
            << lon_fine_tune_dis_ << " and previous lat_fine_tune_dis "
            << lat_fine_tune_dis_;
    return;
  }

  auto start_time = NowInSeconds();
  std::unordered_map<std::string, std::shared_ptr<OpenSpaceFineTuningGrid>>
      search_grids_set;
  std::priority_queue<std::pair<std::string, double>,
                      std::vector<std::pair<std::string, double>>, cmp>
      search_grids_pq;
  std::unordered_set<std::string> visited_grids;
  const auto start_grid = std::make_shared<OpenSpaceFineTuningGrid>(
      0.0, 0.0, search_config_.x_grid_resolution,
      search_config_.y_grid_resolution, search_config_.xy_bounds);
  search_grids_set.emplace(start_grid->index(), start_grid);
  search_grids_pq.emplace(start_grid->index(), 0.0);
  while (!search_grids_pq.empty()) {
    const auto grid_index = search_grids_pq.top().first;
    search_grids_pq.pop();
    const auto& grid = search_grids_set.at(grid_index);
    if (visited_grids.find(grid_index) != visited_grids.end() ||
        grid->x() < search_config_.xy_bounds[0] ||
        grid->x() > search_config_.xy_bounds[1] ||
        grid->y() < search_config_.xy_bounds[2] ||
        grid->y() > search_config_.xy_bounds[3]) {
      continue;
    }
    visited_grids.insert(grid->index());
    if (ValidityCheck(is_lat_slot, is_right_side, grid->x(), grid->y(),
                      fine_tune_obs)) {
      lat_fine_tune_dis_ = grid->x();
      lon_fine_tune_dis_ = grid->y();
      VLOG(2) << "[FINE_TUNE] target pose is collision free after fine tune with both "
                 "lon_fine_tune_dis " << lon_fine_tune_dis_
              << " and lat_fine_tune_dis " << lat_fine_tune_dis_;
      LOG(INFO) << "[FINE_TUNE] fine tune time is "
                << NowInSeconds() - start_time;
      return;
    }
    AddCutOffSearchGrids(grid, is_lat_slot, fine_tune_obs->obs_segments_pair,
                         &visited_grids);
    const auto next_grids = grid->next();
    for (const auto& next_grid : next_grids) {
      if (visited_grids.find(next_grid->index()) != visited_grids.end()) {
        continue;
      }
      const double cost = search_config_.w_x * next_grid->x() * next_grid->x() +
                          search_config_.w_y * next_grid->y() * next_grid->y();
      search_grids_set.emplace(next_grid->index(), next_grid);
      search_grids_pq.emplace(next_grid->index(), cost);
    }
  }
  VLOG(2) << "[FINE_TUNE] search collision-free grid failed";
  LOG(INFO) << "[FINE_TUNE] fine tune time is "
            << NowInSeconds() - start_time;
}

// ============================================================
// IsBlockByInnerFs
// 独立编译改造：移除 injector_ 依赖，始终返回 false
// 原始逻辑依赖 frame_history → planning_context → speed_plan_collision_info
// 独立版无此上下文，故始终返回 false（不触发内部 FS 阻挡逻辑）
// ============================================================
bool OpenSpaceFineTuning::IsBlockByInnerFs(const bool /*is_lat_slot*/,
                                            const common::math::Vec2d& /*origin*/,
                                            const double /*rotate_angle*/) {
  // 独立编译改造：无 frame_history，始终返回 false
  return false;
}

// ============================================================
// IsTargetCollisionWithLinkedObs
// 独立编译改造：vehicle_params_ → PathPoint .theta（直接成员访问）
// ============================================================
bool OpenSpaceFineTuning::IsTargetCollisionWithLinkedObs(
    const bool is_lat_slot, const bool is_parking_lot_update,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        linked_obs_segments_pair) {
  if (!is_lat_slot || !is_parking_lot_update) {
    return false;
  }
  const auto fine_tune_target = GetFineTuneTarget();
  return common::math::CheckCollisionWithVehiclePolygon2d(
      fine_tune_target.x(), fine_tune_target.y(), raw_end_pose_enu_.theta,
      linked_obs_segments_pair);
}

// ============================================================
// IsTargetCollisionWithHighCurbObs
// ============================================================
bool OpenSpaceFineTuning::IsTargetCollisionWithHighCurbObs(
    const bool is_lat_slot, const bool is_parking_lot_update,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        high_curb_obs_segments_pair) {
  if (is_lat_slot || !is_parking_lot_update) {
    return false;
  }
  const auto fine_tune_target = GetFineTuneTarget();
  return common::math::CheckCollisionWithVehiclePolygon2d(
      fine_tune_target.x(), fine_tune_target.y(), raw_end_pose_enu_.theta,
      high_curb_obs_segments_pair);
}

// ============================================================
// UpdateAdjustBuffer
// 独立编译改造：config_.open_space_roi_decider_config().xxx() → config_.xxx
//               vehicle_params_.xxx() → vehicle_params_.xxx
// ============================================================
void OpenSpaceFineTuning::UpdateAdjustBuffer(
    const bool is_lat_slot, const double lat_fine_tune_dis,
    const double lon_fine_tune_dis, const int idx_obs,
    const std::shared_ptr<OpenSpaceFineTuningObs>& fine_tune_obs) {
  if (nullptr == fine_tune_obs) {
    LOG(ERROR) << "[FINE_TUNE] fine_tune_obs is null";
    return;
  }
  if (idx_obs < 0 || idx_obs >= static_cast<int>(fine_tune_obs->obs_segments_pair.size())) {
    LOG(ERROR) << "[FINE_TUNE] index is invalid";
    return;
  }
  const auto& obs_inflat_buffer = fine_tune_obs->original_buffer.at(idx_obs);
  auto& obs_seg_pair = fine_tune_obs->obs_segments_pair.at(idx_obs);
  obs_seg_pair.second = obs_inflat_buffer;
  static constexpr double extra_lat_spot_bottom_adjust_buffer = 0.1;
  static constexpr double extra_non_lat_spot_bottom_adjust_buffer = 0.05;
  if (is_lat_slot) {
    // 独立编译改造：vehicle_params_.length() → vehicle_params_.length
    const double lat_dis_to_center = 0.5 * vehicle_params_.length;
    const double lon_dis_to_center = 0.5 * vehicle_params_.width;
    const double bottom_boundary = -1 * lon_dis_to_center + lon_fine_tune_dis;
    const double left_boundary =
        -1 * lat_dis_to_center - obs_inflat_buffer + lat_fine_tune_dis;
    const double right_boundary =
        lat_dis_to_center + obs_inflat_buffer + lat_fine_tune_dis;
    if (obs_seg_pair.first.start().x() < left_boundary &&
        obs_seg_pair.first.end().x() < left_boundary) {
      return;
    }
    if (obs_seg_pair.first.start().x() > right_boundary &&
        obs_seg_pair.first.end().x() > right_boundary) {
      return;
    }
    if (obs_seg_pair.first.start().y() > bottom_boundary &&
        obs_seg_pair.first.end().y() > bottom_boundary) {
      return;
    }
    // 独立编译改造：config_.xxx 直接访问
    obs_seg_pair.second =
        (std::fabs(obs_seg_pair.second - kBtmLowFsBuffer) <= kEpsilon)
            ? config_.lat_spot_bottom_low_fs_adjust_buffer
            : config_.lat_spot_bottom_adjust_buffer;
    obs_seg_pair.second +=
        (is_entered_lateral_slot_domain_ ? extra_lat_spot_bottom_adjust_buffer
                                         : 0);
  } else {
    // 独立编译改造：vehicle_params_.length() → vehicle_params_.length
    const double lon_dis_to_center = 0.5 * vehicle_params_.length;
    const double bottom_boundary = -1 * lon_dis_to_center +
                                   vehicle_params_.back_edge_to_center -
                                   obs_inflat_buffer + lon_fine_tune_dis;
    const double up_boundary =
        lon_dis_to_center + obs_inflat_buffer + lon_fine_tune_dis;
    if (obs_seg_pair.first.start().y() > up_boundary &&
        obs_seg_pair.first.end().y() > up_boundary) {
      return;
    }
    if (obs_seg_pair.first.start().y() < bottom_boundary &&
        obs_seg_pair.first.end().y() < bottom_boundary) {
      return;
    }
    // 独立编译改造：使用 FineTuningConfig 中的固定缓冲值
    obs_seg_pair.second =
        (std::fabs(obs_seg_pair.second - kBtmLowFsBuffer) <= kEpsilon)
            ? config_.left_lateral_buffer_low_fs
            : config_.left_lateral_buffer_bigger;
    obs_seg_pair.second += extra_non_lat_spot_bottom_adjust_buffer;
  }
}

// ============================================================
// LonFineTuneTargetBasedOnObs
// 独立编译改造：config_.open_space_roi_decider_config().xxx() → config_.xxx
//               vehicle_params_.xxx() → vehicle_params_.xxx
// ============================================================
void OpenSpaceFineTuning::LonFineTuneTargetBasedOnObs(
    const bool is_lat_slot,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        obs_segments_pair,
    double* const lon_fine_tune_dis_ptr) {
  if (nullptr == lon_fine_tune_dis_ptr) {
    LOG(ERROR) << "[FINE_TUNE] LonFineTuneTargetBasedOnObs failed, ptr is nullptr";
    return;
  }
  double max_fine_tune_lon_dis =
      config_.park_in_vertical_target_lon_adjust_threshold;
  double lat_dis_to_center = 0.5 * vehicle_params_.width;
  double lon_dis_to_center = 0.5 * vehicle_params_.length;
  if (is_lat_slot) {
    max_fine_tune_lon_dis = config_.park_in_lateral_target_lon_adjust_threshold;
    lat_dis_to_center = 0.5 * vehicle_params_.length;
    lon_dis_to_center = 0.5 * vehicle_params_.width;
  }
  double obs_min_lon_dis_to_center = -1 * lon_dis_to_center;
  for (const auto& obs_seg_pair : obs_segments_pair) {
    const auto& obs_seg = obs_seg_pair.first;
    const double obs_inflat_buffer = obs_seg_pair.second + kEpsilon;
    const double lon_distance = LonDistanceWithLatBoundary(
        obs_seg, -lat_dis_to_center - obs_inflat_buffer,
        lat_dis_to_center + obs_inflat_buffer,
        -1 * lon_dis_to_center - obs_inflat_buffer);
    obs_min_lon_dis_to_center =
        std::max(obs_min_lon_dis_to_center, lon_distance + obs_inflat_buffer);
  }
  *lon_fine_tune_dis_ptr =
      common::math::Clamp(lon_dis_to_center + obs_min_lon_dis_to_center, 0.0,
                          max_fine_tune_lon_dis);
}

// ============================================================
// LatFineTuneTargetBasedOnObs
// 独立编译改造：vehicle_params_.xxx() → vehicle_params_.xxx
// ============================================================
void OpenSpaceFineTuning::LatFineTuneTargetBasedOnObs(
    const bool is_lat_slot,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        obs_segments_pair,
    double* const lat_fine_tune_dis_ptr) {
  if (nullptr == lat_fine_tune_dis_ptr) {
    LOG(ERROR) << "[FINE_TUNE] LatFineTuneTargetBasedOnObs failed, ptr is nullptr";
    return;
  }
  double max_fine_tune_lat_dis =
      config_.park_in_vertical_target_lat_adjust_threshold;
  double lat_dis_to_center = 0.5 * vehicle_params_.width;
  double lon_dis_to_center = 0.5 * vehicle_params_.length;
  if (is_lat_slot) {
    lat_dis_to_center = 0.5 * vehicle_params_.length;
    lon_dis_to_center = 0.5 * vehicle_params_.width;
    max_fine_tune_lat_dis = config_.park_in_lateral_target_lat_adjust_threshold;
  }
  const double min_width =
      is_lat_slot ? vehicle_params_.length : vehicle_params_.width;

  double obs_min_left_lat_dis_to_center =
      -lat_dis_to_center - max_fine_tune_lat_dis;
  double obs_min_right_lat_dis_to_center =
      lat_dis_to_center + max_fine_tune_lat_dis;
  for (const auto& obs_seg_pair : obs_segments_pair) {
    const auto& obs_seg = obs_seg_pair.first;
    const double obs_inflat_buffer = obs_seg_pair.second + kEpsilon;
    const double bottom_boundary = -1 * lon_dis_to_center - obs_inflat_buffer;
    const double up_boundary = lon_dis_to_center + obs_inflat_buffer;
    const double max_y = std::max(obs_seg.start().y(), obs_seg.end().y());
    const double min_y = std::min(obs_seg.start().y(), obs_seg.end().y());
    if (max_y < bottom_boundary || min_y > up_boundary) {
      continue;
    }
    const double min_x = std::min(obs_seg.start().x(), obs_seg.end().x());
    const double max_x = std::max(obs_seg.start().x(), obs_seg.end().x());
    if (min_x < 0) {
      obs_min_left_lat_dis_to_center =
          std::max(max_x + obs_inflat_buffer, obs_min_left_lat_dis_to_center);
    }
    if (max_x > 0) {
      obs_min_right_lat_dis_to_center =
          std::min(min_x - obs_inflat_buffer, obs_min_right_lat_dis_to_center);
    }
    if (obs_min_right_lat_dis_to_center - obs_min_left_lat_dis_to_center <
        min_width) {
      *lat_fine_tune_dis_ptr = 0.0;
      VLOG(2) << "[FINE_TUNE] freespace points do not allow valid lateral tuning with left "
                 "obs dist " << obs_min_left_lat_dis_to_center
              << " and right obs dist " << obs_min_right_lat_dis_to_center;
      return;
    }
  }
  const double lat_left_fine_tune_dis =
      common::math::Clamp(obs_min_left_lat_dis_to_center + lat_dis_to_center,
                          0.0, max_fine_tune_lat_dis);
  const double lat_right_fine_tune_dis =
      common::math::Clamp(obs_min_right_lat_dis_to_center - lat_dis_to_center,
                          -1 * max_fine_tune_lat_dis, 0.0);
  *lat_fine_tune_dis_ptr = std::fabs(lat_right_fine_tune_dis) < kEpsilon
                               ? lat_left_fine_tune_dis
                               : lat_right_fine_tune_dis;
}

// ============================================================
// LonDistanceWithLatBoundary（静态）
// 独立编译改造：double_type::SeemsEqual → std::fabs(a-b) < kEpsilon
// ============================================================
double OpenSpaceFineTuning::LonDistanceWithLatBoundary(
    const common::math::LineSegment2d& obs_seg, const double left_boundary,
    const double right_boundary, const double lon_dis_threshold) {
  double lon_dis = lon_dis_threshold;
  double min_x = std::min(obs_seg.start().x(), obs_seg.end().x());
  double max_x = std::max(obs_seg.start().x(), obs_seg.end().x());
  double min_y = std::min(obs_seg.start().y(), obs_seg.end().y());
  if (min_x > right_boundary || max_x < left_boundary || min_y > 0.0) {
    return lon_dis;
  }
  lon_dis = std::max(obs_seg.start().y(), obs_seg.end().y());
  // 独立编译改造：double_type::SeemsEqual → std::fabs(a-b) < kEpsilon
  if (std::fabs(min_x - max_x) < kEpsilon) {
    return lon_dis;
  }
  int quadrant = static_cast<int>(
      common::math::NormalizeAngle2(obs_seg.heading()) / M_PI_2);
  switch (quadrant) {
    case 0: {
      if (obs_seg.end().x() > right_boundary) {
        lon_dis = obs_seg.start().y() + (right_boundary - obs_seg.start().x()) *
                                            std::tan(obs_seg.heading());
      }
      break;
    }
    case 1: {
      if (obs_seg.end().x() < left_boundary) {
        lon_dis = obs_seg.start().y() + (obs_seg.start().x() - left_boundary) *
                                            std::tan(M_PI - obs_seg.heading());
      }
      break;
    }
    case 2: {
      if (obs_seg.start().x() > right_boundary) {
        lon_dis = obs_seg.end().y() + (right_boundary - obs_seg.end().x()) *
                                          std::tan(obs_seg.heading() - M_PI);
      }
      break;
    }
    case 3: {
      if (obs_seg.start().x() < left_boundary) {
        lon_dis =
            obs_seg.end().y() + (obs_seg.end().x() - left_boundary) *
                                    std::tan(2 * M_PI - obs_seg.heading());
      }
      break;
    }
    default:
      break;
  }
  return lon_dis;
}

// ============================================================
// FineTuneTargetBasedOnEndReplanPose
// 独立编译改造：移除 injector_->planning_context() 依赖
//   has_end_replaned_ 始终为 false（独立版无重规划上下文），函数早退
// ============================================================
void OpenSpaceFineTuning::FineTuneTargetBasedOnEndReplanPose(
    const common::PathPoint& /*veh_point*/, const bool /*is_right_side*/,
    const TL::perception::ParkingLotOut::ParkType& /*park_lot_type*/,
    const Vec2d& /*origin_point*/, const double /*target_heading*/,
    const std::pair<double, double>& /*lat_space*/,
    const bool /*is_consider_wheel_mask*/, const bool /*is_parking_inwards*/) {
  // 独立编译改造：无重规划上下文，has_end_replaned_ 恒 false，直接返回
  if (!has_end_replaned_) {
    return;
  }
  // 以下代码在独立版中不会执行（保留以便将来接入重规划状态）
}

// ============================================================
// LatFineTuneTargetBasedOnEndReplanPose（静态）
// ============================================================
void OpenSpaceFineTuning::LatFineTuneTargetBasedOnEndReplanPose(
    const bool is_lat_slot, const bool is_right_side,
    const common::math::Vec2d& real_end_replan_point,
    const common::math::Vec2d& end_point, double* const lat_fine_tune_dis_ptr) {
  if (nullptr == lat_fine_tune_dis_ptr || !is_lat_slot) {
    return;
  }
  const double tune_direction = is_right_side ? 1 : -1;
  double lat_fine_tune_dis = real_end_replan_point.x() - end_point.x();
  if (tune_direction * lat_fine_tune_dis < kEpsilon) {
    VLOG(2) << "[FINE_TUNE] target pose is not cut off, no need to lat adjust target";
    return;
  }
  *lat_fine_tune_dis_ptr = lat_fine_tune_dis;
}

// ============================================================
// LonFineTuneTargetBasedOnEndReplanPose
// 独立编译改造：config_.open_space_roi_decider_config().xxx() → config_.xxx
// ============================================================
void OpenSpaceFineTuning::LonFineTuneTargetBasedOnEndReplanPose(
    const bool is_lat_slot, const bool /*is_right_side*/,
    const bool is_consider_wheel_mask, const bool is_parking_inwards,
    const std::pair<double, double>& lat_space,
    const common::math::Vec2d& real_end_replan_point,
    const common::math::Vec2d& end_point, double* const lon_fine_tune_dis_ptr) {
  if (nullptr == lon_fine_tune_dis_ptr) {
    return;
  }
  constexpr double narrow_dis_thres = 1.0;
  // 独立编译改造：vehicle_params_.length() → vehicle_params_.length
  const bool is_right_side_actual = (lat_space.second >= 0);
  const double extra_space_dis =
      (is_right_side_actual ? lat_space.second : lat_space.first) -
      0.5 * vehicle_params_.length;
  if (is_lat_slot &&
      (extra_space_dis > narrow_dis_thres || !is_consider_wheel_mask)) {
    return;
  }
  double lon_fine_tune_dis = real_end_replan_point.y() - end_point.y();
  if ((lon_fine_tune_dis < kEpsilon && !is_parking_inwards) ||
      (lon_fine_tune_dis > -kEpsilon && is_parking_inwards)) {
    LOG(INFO) << "[FINE_TUNE] fine tune dis for end replan is invalid, target pose is not cut off";
    return;
  }
  double max_fine_tune_lon_dis =
      is_lat_slot ? config_.park_in_lateral_target_lon_adjust_threshold
                  : config_.park_in_vertical_target_lon_adjust_threshold;
  *lon_fine_tune_dis_ptr =
      is_parking_inwards ? std::max(lon_fine_tune_dis, -max_fine_tune_lon_dis)
                         : std::min(lon_fine_tune_dis, max_fine_tune_lon_dis);
}

// ============================================================
// FineTuneTargetBasedOnNarrowLatSpot
// 独立编译改造：vehicle_params_.length() → vehicle_params_.length
//               config_.xxx() → config_.xxx
// ============================================================
void OpenSpaceFineTuning::FineTuneTargetBasedOnNarrowLatSpot(
    const bool is_lat_slot,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        obstacles_segments_vec,
    const std::pair<double, double>& lat_space) {
  const auto last_fine_tune_target = GetFineTuneTarget();
  if (common::math::CheckCollisionWithVehiclePolygon2d(
          last_fine_tune_target.x(), last_fine_tune_target.y(),
          raw_end_pose_enu_.theta, obstacles_segments_vec)) {
    VLOG(2) << "[FINE_TUNE] target pose is now in collision , do not "
               "narrow tuning under current condition";
    return;
  }
  const bool is_enable_narrow_lon_fine_tune =
      !has_narrow_lon_tune_ && is_entered_lateral_slot_domain_ && is_lat_slot;
  if (!is_enable_narrow_lon_fine_tune) {
    lon_fine_tune_dis_ =
        std::max(pre_narrow_lon_fine_tune_dis_, lon_fine_tune_dis_);
    return;
  }
  has_narrow_lon_tune_ = true;
  static constexpr double kLatSpotLengthUpperBound = 1.5;
  static constexpr double kLatSpotLengthLowerBound = 0.8;
  // 独立编译改造：vehicle_params_.length() → vehicle_params_.length
  const double lat_spot_length =
      std::max(lat_space.second - lat_space.first, 0.0);
  const double extra_lat_spot_length = lat_spot_length - vehicle_params_.length;
  const std::vector<double> input_v = {kLatSpotLengthLowerBound,
                                       kLatSpotLengthUpperBound};
  const std::vector<double> output_v = {
      config_.park_in_lateral_target_lon_adjust_threshold, 0.0};
  const double lat_fine_tune_dis = lat_fine_tune_dis_;
  const double lon_fine_tune_dis =
      common::math::InterpolationOne(extra_lat_spot_length, input_v, output_v);
  VLOG(2) << "[FINE_TUNE] Lon tune dis due to narrow_lat_spot : " << lon_fine_tune_dis;
  pre_narrow_lon_fine_tune_dis_ = lon_fine_tune_dis;
  if (lon_fine_tune_dis < lon_fine_tune_dis_) {
    return;
  }
  auto fine_tune_target =
      GetFineTuneTarget(lat_fine_tune_dis, lon_fine_tune_dis);
  if (!common::math::CheckCollisionWithVehiclePolygon2d(
          fine_tune_target.x(), fine_tune_target.y(), raw_end_pose_enu_.theta,
          obstacles_segments_vec)) {
    lon_fine_tune_dis_ = lon_fine_tune_dis;
    return;
  }
  VLOG(2) << "[FINE_TUNE] target pose is in collision after narrow lat spot fine tune";
}

// ============================================================
// AddCutOffSearchGrids
// 独立编译改造：vehicle_params_.length() → vehicle_params_.length
// ============================================================
void OpenSpaceFineTuning::AddCutOffSearchGrids(
    const std::shared_ptr<OpenSpaceFineTuningGrid>& grid,
    const bool is_lat_slot,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        obstacles_segments_vec,
    std::unordered_set<std::string>* const visited_grids) {
  if (nullptr == visited_grids) {
    return;
  }
  const double lat_fine_tune_dis = grid->x();
  const double lon_fine_tune_dis = grid->y();
  if (is_lat_slot) {
    const auto lat_space =
        GetSpotLatSpace(is_lat_slot, lon_fine_tune_dis, obstacles_segments_vec);
    // 独立编译改造：vehicle_params_.length() → vehicle_params_.length
    double lat_dis_to_center = 0.5 * vehicle_params_.length;
    const double max_lat_fine_tune_dis = lat_space.second - lat_dis_to_center;
    const double min_lat_fine_tune_dis = lat_space.first + lat_dis_to_center;
    double x = search_config_.xy_bounds[0];
    double y = lon_fine_tune_dis;
    while (x <= search_config_.xy_bounds[1]) {
      if (x < min_lat_fine_tune_dis || x > max_lat_fine_tune_dis) {
        const OpenSpaceFineTuningGrid cut_off_grid(
            x, y, search_config_.x_grid_resolution,
            search_config_.y_grid_resolution, search_config_.xy_bounds);
        visited_grids->insert(cut_off_grid.index());
      }
      x += search_config_.x_grid_resolution;
    }
    if (grid->x() < min_lat_fine_tune_dis) {
      const double next_x =
          std::ceil(min_lat_fine_tune_dis / search_config_.x_grid_resolution) *
          search_config_.x_grid_resolution;
      const auto extra_next_grid = std::make_shared<OpenSpaceFineTuningGrid>(
          next_x, lon_fine_tune_dis, search_config_.x_grid_resolution,
          search_config_.y_grid_resolution, search_config_.xy_bounds);
      grid->add_next_grid(extra_next_grid);
    }
    if (grid->x() > max_lat_fine_tune_dis) {
      const double next_x =
          std::floor(max_lat_fine_tune_dis / search_config_.x_grid_resolution) *
          search_config_.x_grid_resolution;
      const auto extra_next_grid = std::make_shared<OpenSpaceFineTuningGrid>(
          next_x, lon_fine_tune_dis, search_config_.x_grid_resolution,
          search_config_.y_grid_resolution, search_config_.xy_bounds);
      grid->add_next_grid(extra_next_grid);
    }
  } else {
    const auto lon_space =
        GetSpotLonSpace(is_lat_slot, lat_fine_tune_dis, obstacles_segments_vec);
    // 独立编译改造：vehicle_params_.length() → vehicle_params_.length
    double lon_dis_to_center = 0.5 * vehicle_params_.length;
    const double min_lon_fine_tune_dis = lon_space + lon_dis_to_center;
    double x = lat_fine_tune_dis;
    double y = search_config_.xy_bounds[2];
    while (y <= search_config_.xy_bounds[3]) {
      if (y < min_lon_fine_tune_dis) {
        OpenSpaceFineTuningGrid cut_off_grid(
            x, y, search_config_.x_grid_resolution,
            search_config_.y_grid_resolution, search_config_.xy_bounds);
        visited_grids->insert(cut_off_grid.index());
      }
      y += search_config_.y_grid_resolution;
    }
    if (grid->y() < min_lon_fine_tune_dis) {
      const double next_y =
          std::ceil(min_lon_fine_tune_dis / search_config_.y_grid_resolution) *
          search_config_.y_grid_resolution;
      const auto extra_next_grid = std::make_shared<OpenSpaceFineTuningGrid>(
          next_y, lon_fine_tune_dis, search_config_.x_grid_resolution,
          search_config_.y_grid_resolution, search_config_.xy_bounds);
      grid->add_next_grid(extra_next_grid);
    }
  }
}

// ============================================================
// ValidityCheck
// 独立编译改造：vehicle_params_.xxx() → vehicle_params_.xxx
// ============================================================
bool OpenSpaceFineTuning::ValidityCheck(
    const bool is_lat_slot, const bool is_right_side,
    const double lat_fine_tune_dis, const double lon_fine_tune_dis,
    const std::shared_ptr<OpenSpaceFineTuningObs>& fine_tune_obs) {
  for (int i = 0; i < static_cast<int>(fine_tune_obs->obs_segments_pair.size()); ++i) {
    UpdateAdjustBuffer(is_lat_slot, lat_fine_tune_dis, lon_fine_tune_dis, i,
                       fine_tune_obs);
  }
  double adc_heading = M_PI_2;
  if (is_lat_slot) {
    adc_heading = is_right_side ? 0 : M_PI;
  }
  // 独立编译改造：vehicle_params_.xxx() → vehicle_params_.xxx
  const double shift_distance =
      0.5 * vehicle_params_.length - vehicle_params_.back_edge_to_center;
  const auto fine_tune_target =
      Vec2d(lat_fine_tune_dis, lon_fine_tune_dis) +
      shift_distance * Vec2d::CreateUnitVec2d(adc_heading + M_PI);
  return !common::math::CheckCollisionWithVehiclePolygon2d(
      fine_tune_target.x(), fine_tune_target.y(), adc_heading,
      fine_tune_obs->obs_segments_pair);
}

// ============================================================
// LatDistanceWithLonBoundary（静态）
// ============================================================
std::pair<double, double> OpenSpaceFineTuning::LatDistanceWithLonBoundary(
    const common::math::LineSegment2d& obs_seg, const double bottom_boundary,
    const double up_boundary, const double left_lat_dis_threshold,
    const double right_lat_dis_threshold) {
  std::pair<double, double> nearest_obs_to_center = {left_lat_dis_threshold,
                                                     right_lat_dis_threshold};
  const double max_y = std::max(obs_seg.start().y(), obs_seg.end().y());
  const double min_y = std::min(obs_seg.start().y(), obs_seg.end().y());
  if (max_y < bottom_boundary || min_y > up_boundary) {
    return nearest_obs_to_center;
  }
  const double min_x = std::min(obs_seg.start().x(), obs_seg.end().x());
  const double max_x = std::max(obs_seg.start().x(), obs_seg.end().x());
  if (min_x < 0) {
    nearest_obs_to_center.first = max_x;
  }
  if (max_x > 0) {
    nearest_obs_to_center.second = min_x;
  }
  return nearest_obs_to_center;
}

// ============================================================
// GetSpotLatSpace
// 独立编译改造：vehicle_params_.xxx() → vehicle_params_.xxx
// ============================================================
std::pair<double, double> OpenSpaceFineTuning::GetSpotLatSpace(
    const bool is_lat_slot, const double lon_fine_tune_dis,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        transformed_obs_seg,
    const bool is_consider_buffer, const bool is_consider_whole_spot) {
  double lon_dis_to_center = is_lat_slot ? 0.5 * vehicle_params_.width
                                         : 0.5 * vehicle_params_.length;
  double obs_min_left_lat_dis_to_center = -10.0;
  double obs_min_right_lat_dis_to_center = 10.0;
  for (const auto& obs_seg_pair : transformed_obs_seg) {
    const double obs_inflat_buffer =
        is_consider_buffer ? obs_seg_pair.second + kEpsilon : 0.0;
    const double bottom_boundary =
        is_consider_whole_spot
            ? -1 * lon_dis_to_center - obs_inflat_buffer + lon_fine_tune_dis
            : 0.0;
    const double up_boundary =
        lon_dis_to_center + obs_inflat_buffer + lon_fine_tune_dis;
    const auto nearest_obs_to_center = LatDistanceWithLonBoundary(
        obs_seg_pair.first, bottom_boundary, up_boundary, -10.0, 10.0);
    obs_min_left_lat_dis_to_center =
        std::max(nearest_obs_to_center.first + obs_inflat_buffer,
                 obs_min_left_lat_dis_to_center);
    obs_min_right_lat_dis_to_center =
        std::min(nearest_obs_to_center.second - obs_inflat_buffer,
                 obs_min_right_lat_dis_to_center);
  }
  return {obs_min_left_lat_dis_to_center, obs_min_right_lat_dis_to_center};
}

// ============================================================
// GetSpotLonSpace
// 独立编译改造：vehicle_params_.xxx() → vehicle_params_.xxx
// ============================================================
double OpenSpaceFineTuning::GetSpotLonSpace(
    const bool is_lat_slot, const double lat_fine_tune_dis,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        obstacles_segments_vec) {
  double lat_dis_to_center = is_lat_slot ? 0.5 * vehicle_params_.length
                                         : 0.5 * vehicle_params_.width;
  double obs_min_lon_dis_to_center = -10.0;
  for (const auto& obs_seg_pair : obstacles_segments_vec) {
    const auto& obs_seg = obs_seg_pair.first;
    const double obs_inflat_buffer = obs_seg_pair.second + kEpsilon;
    const double lon_distance = LonDistanceWithLatBoundary(
        obs_seg, -lat_dis_to_center - obs_inflat_buffer + lat_fine_tune_dis,
        lat_dis_to_center + obs_inflat_buffer + lat_fine_tune_dis, -10.0);
    obs_min_lon_dis_to_center =
        std::max(obs_min_lon_dis_to_center, lon_distance + obs_inflat_buffer);
  }
  return obs_min_lon_dis_to_center;
}

// ============================================================
// GetTopHalfLatSpace
// 独立编译改造：vehicle_params_.xxx() → vehicle_params_.xxx
// ============================================================
std::pair<double, double> OpenSpaceFineTuning::GetTopHalfLatSpace(
    const bool is_lat_slot,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        obstacles_segments_vec) {
  std::vector<std::pair<common::math::LineSegment2d, double>>
      transformed_obs_seg;
  // 独立编译改造：vehicle_params_.length() → vehicle_params_.length
  const double shift_distance =
      0.5 * vehicle_params_.length - vehicle_params_.back_edge_to_center;
  Vec2d target_adc_center =
      Vec2d(raw_end_pose_enu_.x, raw_end_pose_enu_.y) +
      shift_distance * Vec2d::CreateUnitVec2d(raw_end_pose_enu_.theta);
  for (const auto& obs_seg_pair : obstacles_segments_vec) {
    auto obs = obs_seg_pair;
    obs.first.Transform(target_adc_center, target_adc_coord_heading_);
    transformed_obs_seg.emplace_back(obs);
  }
  const auto lat_space = GetSpotLatSpace(is_lat_slot, lon_fine_tune_dis_,
                                         transformed_obs_seg, false, false);
  VLOG(2) << "[FINE_TUNE] space " << lat_space.first << "," << lat_space.second;
  return lat_space;
}

// ============================================================
// FineTuneParkOutTargetBasedOnObs
// 独立编译改造：
//   bool is_parking_in 替代 AVPStatus::ParkingType
//   !is_parking_in → 认为是泊出（不区分 LEFT/RIGHT，左右由 park_lot_type/sensor 决定）
//   config_.open_space_roi_decider_config().xxx() → config_.xxx
// ============================================================
bool OpenSpaceFineTuning::FineTuneParkOutTargetBasedOnObs(
    const bool is_parking_in,
    const TL::perception::ParkingLotOut::SenType& sensor_type,
    const TL::perception::ParkingLotOut::ParkType& park_lot_type,
    const ParkingLotVertexType& parking_spot_enu,
    const common::PathPoint& veh_point,
    const common::math::LineSegment2d& reference_curb) {
  // 独立编译改造：parking_type is PARKING_IN → is_parking_in，不是泊出直接返回 false
  if (sensor_type == TL::perception::ParkingLotOut::USS ||
      is_parking_in ||  // 独立改造：is_parking_in=true 说明不是泊出
      reference_curb.length() < kEpsilon ||
      std::fabs(yaw_fine_tune_rad_) > kEpsilon) {
    return false;
  }
  const auto lt2rt = common::math::LineSegment2d(parking_spot_enu.at(0),
                                                 parking_spot_enu.at(3));
  constexpr double kCurbHeightThreshold = 1.5;
  if (lt2rt.ProductOntoUnit(reference_curb.start()) < kCurbHeightThreshold &&
      lt2rt.ProductOntoUnit(reference_curb.end()) < kCurbHeightThreshold) {
    return false;
  }
  if (lt2rt.ProductOntoUnit(Vec2d(veh_point.x, veh_point.y)) < kEpsilon) {
    return false;
  }
  // 独立编译改造：PARKING_OUT_RIGHT vs PARKING_OUT_LEFT 判断
  // 简化：对于非 LATERAL 车位，认为是右侧泊出；LATERAL 认为是左侧
  const bool is_park_out_right_side =
      (park_lot_type != TL::perception::ParkingLotOut::LATERAL);
  const double target_heading = is_park_out_right_side
                                    ? reference_curb.heading()
                                    : reference_curb.heading() + M_PI;
  constexpr double kAngleDiffUpperThreshold = M_PI_2;
  constexpr double kAngleDiffLowerThreshold = 0.35;
  const double angle_diff =
      common::math::AngleDiff(raw_end_pose_enu_.theta, target_heading);
  if (std::fabs(angle_diff) > kAngleDiffUpperThreshold ||
      std::fabs(angle_diff) < kAngleDiffLowerThreshold) {
    return false;
  }
  Vec2d raw_target_point = Vec2d(raw_end_pose_enu_.x, raw_end_pose_enu_.y);
  // 独立编译改造：config_.vertical/lateral_park_out_height
  const double park_out_height =
      (park_lot_type == TL::perception::ParkingLotOut::LATERAL)
          ? config_.lateral_park_out_height
          : config_.vertical_park_out_height;
  const auto fine_tune_target =
      raw_target_point +
      (park_out_height - reference_curb.ProductOntoUnit(raw_target_point)) *
          Vec2d::CreateUnitVec2d(reference_curb.heading() + M_PI_2);
  const auto raw_to_target = fine_tune_target - raw_target_point;
  const auto unit_vec = lt2rt.unit_direction();
  lon_fine_tune_dis_ = unit_vec.CrossProd(raw_to_target);
  lat_fine_tune_dis_ = unit_vec.InnerProd(raw_to_target);
  yaw_fine_tune_rad_ = angle_diff;
  return true;
}

}  // namespace planning
}  // namespace TL
