#pragma once

/*
 * Copyright (c) TL Technologies Co., Ltd. 2022. All rights reserved.
 * Description:  open_space_fine_tuning.h
 *
 * 独立编译改造（P2，doc 159）：
 *   1. 移除 TaskConfig / DependencyInjector 依赖
 *   2. 引入 FineTuningConfig 替代 TaskConfig
 *   3. 移除 AVPStatus::ParkingType → bool is_parking_in 参数
 *   4. 移除 OpenSpacePathInfo → 拆解为独立参数（obs_segments/origin/rotate_angle/is_parking_inwards）
 *   5. IsBlockByInnerFs → 独立版始终返回 false（无 frame_history）
 *   6. FineTuneTargetBasedOnEndReplanPose → 简化（无重规划上下文，has_end_replaned_恒为false）
 */

#include <complex>
#include <memory>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "absl/strings/str_cat.h"
#include "common/math/math_utils.h"
#include "common/math/vec2d.h"
#include "common/status/status.h"
#include "planning/open_space/vehicle_param.h"
#include "planning/tasks/deciders/open_space_decider/open_space_obstacle.h"
#include "proto_convert/pnc_point_convert.h"

namespace TL {
namespace planning {

// ============================================================
// OpenSpaceFineTuningGrid: BFS搜索网格（无框架依赖，保留原始）
// ============================================================
class OpenSpaceFineTuningGrid {
 public:
  explicit OpenSpaceFineTuningGrid(double x, double y, double x_grid_resolution,
                                   double y_grid_resolution,
                                   const std::vector<double>& xy_bounds)
      : x_(x),
        y_(y),
        x_grid_resolution_(x_grid_resolution),
        y_grid_resolution_(y_grid_resolution),
        xy_bounds_(xy_bounds) {
    if (xy_bounds_.size() != 4U) {
      // 独立编译改造：用简单断言替代 CHECK_EQ（避免 glog CHECK 语法问题）
      LOG(ERROR) << "xy_bounds size is not 4, but" << xy_bounds_.size();
    }
    x = common::math::Clamp(x, xy_bounds[0], xy_bounds[1]);
    y = common::math::Clamp(y, xy_bounds[2], xy_bounds[3]);
    int x_grid = static_cast<int>(round(x / x_grid_resolution));
    int y_grid = static_cast<int>(round(y / y_grid_resolution));
    index_ = absl::StrCat(x_grid, "_", y_grid);
  }

  ~OpenSpaceFineTuningGrid() = default;

  std::vector<std::shared_ptr<OpenSpaceFineTuningGrid>> next() {
    std::vector<std::shared_ptr<OpenSpaceFineTuningGrid>> next_grids;
    const std::vector<double> x_offsets = {-x_grid_resolution_,
                                           0.0,
                                           x_grid_resolution_,
                                           -x_grid_resolution_,
                                           x_grid_resolution_,
                                           -x_grid_resolution_,
                                           0.0,
                                           x_grid_resolution_};
    const std::vector<double> y_offsets = {
        -y_grid_resolution_, -y_grid_resolution_, -y_grid_resolution_, 0.0, 0.0,
        y_grid_resolution_,  y_grid_resolution_,  y_grid_resolution_};
    const auto offsets_num = x_offsets.size();
    for (size_t i = 0; i < offsets_num; ++i) {
      if (x_ + x_offsets.at(i) < xy_bounds_[0] ||
          x_ + x_offsets.at(i) > xy_bounds_[1]) {
        continue;
      }
      if (y_ + y_offsets.at(i) < xy_bounds_[2] ||
          y_ + y_offsets.at(i) > xy_bounds_[3]) {
        continue;
      }
      next_grids.emplace_back(std::make_shared<OpenSpaceFineTuningGrid>(
          x_ + x_offsets.at(i), y_ + y_offsets.at(i), x_grid_resolution_,
          y_grid_resolution_, xy_bounds_));
    }
    next_grids.insert(next_grids.end(), extra_next_grids_.begin(),
                      extra_next_grids_.end());
    return next_grids;
  }

  void add_next_grid(
      const std::shared_ptr<OpenSpaceFineTuningGrid>& extra_next_grid) {
    extra_next_grids_.emplace_back(extra_next_grid);
  }

  std::string index() const { return index_; }
  double x() const { return x_; }
  double y() const { return y_; }

 private:
  double x_, y_, x_grid_resolution_, y_grid_resolution_;
  std::vector<double> xy_bounds_;
  std::string index_;
  std::vector<std::shared_ptr<OpenSpaceFineTuningGrid>> extra_next_grids_;
};

// ============================================================
// OpenSpaceFineTuningObs: 障碍物容器（无框架依赖）
// ============================================================
struct OpenSpaceFineTuningObs {
  std::vector<std::pair<common::math::LineSegment2d, double>> obs_segments_pair;
  std::vector<double> original_buffer;
};

// ============================================================
// OpenSpaceFineTuningConfig: BFS搜索网格参数
// 独立编译改造：直接从原始 header 迁移，无框架依赖
// ============================================================
struct OpenSpaceFineTuningConfig {
  double x_grid_resolution = 0.01;
  double y_grid_resolution = 0.01;
  std::vector<double> xy_bounds = {0.0, 0.0, 0.0, 0.0};
  double w_x = 1.0;
  double w_y = 1.0;
};

// ============================================================
// FineTuningConfig: 独立编译改造配置（替代 TaskConfig 中的微调参数）
// 独立编译改造（P2，doc 159）：替代 config_.open_space_roi_decider_config().xxx()
// ============================================================
struct FineTuningConfig {
  // 网格搜索精度
  double park_in_lat_adjust_resolution = 0.01;     // 横向步长(m)
  double park_in_lon_adjust_resolution = 0.01;     // 纵向步长(m)
  // 垂直泊入调整上限
  double park_in_vertical_target_lat_adjust_threshold = 0.3;  // 横向最大调整量(m)
  double park_in_vertical_target_lon_adjust_threshold = 0.2;  // 纵向最大调整量(m)
  // 侧方泊入调整上限
  double park_in_lateral_target_lat_adjust_threshold = 0.3;
  double park_in_lateral_target_lon_adjust_threshold = 0.2;
  // 底部FS缓冲量
  double lat_spot_bottom_low_fs_adjust_buffer = 0.05;
  double lat_spot_bottom_adjust_buffer = 0.1;
  // 垂直底部碰撞缓冲（替代 open_space_speed_optimizer_config）
  double left_lateral_buffer_low_fs = 0.1;
  double left_lateral_buffer_bigger = 0.2;
  // 泊出目标高度
  double vertical_park_out_height = 0.5;
  double lateral_park_out_height = 0.5;

  static FineTuningConfig GetDefault() { return FineTuningConfig{}; }
};

// ============================================================
// BFS优先队列比较器
// ============================================================
struct cmp {
  bool operator()(const std::pair<std::string, double>& left,
                  const std::pair<std::string, double>& right) const {
    return left.second >= right.second;
  }
};

// ============================================================
// OpenSpaceFineTuning: 终点微调主类
// 独立编译改造（P2，doc 159）：
//   - 构造器从 (TaskConfig, DependencyInjector) 改为 (VehicleParam, FineTuningConfig)
//   - Process() 从 (AVPStatus::ParkingType, OpenSpacePathInfo, ...) 改为
//     (bool is_parking_in, obs_segments, origin, rotate_angle, is_parking_inwards, ...)
// ============================================================
class OpenSpaceFineTuning {
 public:
  // 独立编译改造：替换 (TaskConfig, DependencyInjector) → (VehicleParam, FineTuningConfig)
  explicit OpenSpaceFineTuning(const VehicleParam& vehicle_params,
                                const FineTuningConfig& config);
  ~OpenSpaceFineTuning() = default;

  // 独立编译改造：替换 AVPStatus::ParkingType → bool is_parking_in
  //               替换 OpenSpacePathInfo → (obs_segments, origin, rotate_angle, is_parking_inwards)
  TL::common::Status Process(
      bool is_entered_lateral_slot_domain,
      bool is_consider_wheel_mask,
      bool is_parking_in,  // 独立改造：替代 AVPStatus::ParkingType
      const planning::ParkLotInfo& park_lot,
      const std::vector<std::pair<common::math::LineSegment2d, double>>&
          obstacles_segments_vec,                 // 独立改造：从 OpenSpacePathInfo 中拆出
      const common::math::Vec2d& origin,          // 独立改造：从 OpenSpacePathInfo 中拆出
      double rotate_angle,                        // 独立改造：从 OpenSpacePathInfo 中拆出
      bool is_parking_inwards,                    // 独立改造：从 OpenSpacePathInfo 中拆出
      const std::vector<std::pair<common::math::LineSegment2d, double>>&
          linked_obstacles_segments_vec,
      const std::vector<std::pair<common::math::LineSegment2d, double>>&
          high_curb_obstacles_segments_vec,
      const common::PathPoint& veh_point,
      const common::math::LineSegment2d& reference_curb,
      common::PathPoint* end_pose_enu_ptr);

  TL::common::Status Reset();

  double lon_fine_tune_dis() const { return lon_fine_tune_dis_; }
  double lat_fine_tune_dis() const { return lat_fine_tune_dis_; }
  double yaw_fine_tune_rad() const { return yaw_fine_tune_rad_; }
  bool is_fine_tune_update() const { return is_fine_tune_update_; }

 private:
  void InitConfig(bool is_lat_slot, bool is_enable_lat_fine_tune,
                  bool is_enable_lon_fine_tune);

  void FineTuneParkInTargetBasedOnObs(
      const common::PathPoint& veh_point,
      const planning::ParkLotInfo& park_lot_info,
      const std::vector<std::pair<common::math::LineSegment2d, double>>&
          original_obs_segments_pair,
      const std::vector<std::pair<common::math::LineSegment2d, double>>&
          linked_obs_segments_pair,
      const std::vector<std::pair<common::math::LineSegment2d, double>>&
          high_curb_obs_segments_pair,
      const common::math::Vec2d& origin, double rotate_angle);

  // 独立编译改造：替换 AVPStatus::ParkingType → bool is_parking_in
  bool FineTuneParkOutTargetBasedOnObs(
      bool is_parking_in,  // 独立改造：替代 AVPStatus::ParkingType
      const TL::perception::ParkingLotOut::SenType& sensor_type,
      const TL::perception::ParkingLotOut::ParkType& park_lot_type,
      const ParkingLotVertexType& parking_spot_enu,
      const common::PathPoint& veh_point,
      const common::math::LineSegment2d& reference_curb);

  // 独立编译改造：移除 injector_ 依赖，独立版始终返回 false
  bool IsBlockByInnerFs(bool is_lat_slot, const common::math::Vec2d& origin,
                        double rotate_angle);

  bool IsTargetCollisionWithLinkedObs(
      bool is_lat_slot, bool is_parking_lot_update,
      const std::vector<std::pair<common::math::LineSegment2d, double>>&
          linked_obs_segments_pair);

  bool IsTargetCollisionWithHighCurbObs(
      bool is_lat_slot, bool is_parking_lot_update,
      const std::vector<std::pair<common::math::LineSegment2d, double>>&
          high_curb_obs_segments_pair);

  void UpdateAdjustBuffer(
      bool is_lat_slot, double lat_fine_tune_dis, double lon_fine_tune_dis,
      int idx_obs,
      const std::shared_ptr<OpenSpaceFineTuningObs>& fine_tune_obs);

  void LonFineTuneTargetBasedOnObs(
      bool is_lat_slot,
      const std::vector<std::pair<common::math::LineSegment2d, double>>&
          obs_segments_pair,
      double* lon_fine_tune_dis_ptr);

  void LatFineTuneTargetBasedOnObs(
      bool is_lat_slot,
      const std::vector<std::pair<common::math::LineSegment2d, double>>&
          obs_segments_pair,
      double* lat_fine_tune_dis_ptr);

  static double LonDistanceWithLatBoundary(
      const common::math::LineSegment2d& obs_seg, double left_boundary,
      double right_boundary, double lon_dis_threshold);

  // 独立编译改造：简化（has_end_replaned_恒为false，函数体早退）
  void FineTuneTargetBasedOnEndReplanPose(
      const common::PathPoint& veh_point, bool is_right_side,
      const TL::perception::ParkingLotOut::ParkType& park_lot_type,
      const common::math::Vec2d& origin_point, double target_heading,
      const std::pair<double, double>& lat_space, bool is_consider_wheel_mask,
      bool is_parking_inwards);

  static void LatFineTuneTargetBasedOnEndReplanPose(
      bool is_lat_slot, bool is_right_side,
      const common::math::Vec2d& real_end_replan_point,
      const common::math::Vec2d& end_point, double* lat_fine_tune_dis_ptr);

  void LonFineTuneTargetBasedOnEndReplanPose(
      bool is_lat_slot, bool is_right_side, bool is_consider_wheel_mask,
      bool is_parking_inwards, const std::pair<double, double>& lat_space,
      const common::math::Vec2d& real_end_replan_point,
      const common::math::Vec2d& end_point, double* lon_fine_tune_dis_ptr);

  void FineTuneTargetBasedOnNarrowLatSpot(
      bool is_lat_slot,
      const std::vector<std::pair<common::math::LineSegment2d, double>>&
          obstacles_segments_vec,
      const std::pair<double, double>& lat_space);

  void AddCutOffSearchGrids(
      const std::shared_ptr<OpenSpaceFineTuningGrid>& grid, bool is_lat_slot,
      const std::vector<std::pair<common::math::LineSegment2d, double>>&
          obstacles_segments_vec,
      std::unordered_set<std::string>* visited_grids);

  bool ValidityCheck(
      bool is_lat_slot, bool is_right_side, double lat_fine_tune_dis,
      double lon_fine_tune_dis,
      const std::shared_ptr<OpenSpaceFineTuningObs>& fine_tune_obs);

  static std::pair<double, double> LatDistanceWithLonBoundary(
      const common::math::LineSegment2d& obs_seg, double bottom_boundary,
      double up_boundary, double left_lat_dis_threshold,
      double right_lat_dis_threshold);

  std::pair<double, double> GetSpotLatSpace(
      bool is_lat_slot, double lon_fine_tune_dis,
      const std::vector<std::pair<common::math::LineSegment2d, double>>&
          obstacles_segments_vec,
      bool is_consider_buffer = true, bool is_consider_whole_spot = true);

  double GetSpotLonSpace(
      bool is_lat_slot, double lat_fine_tune_dis,
      const std::vector<std::pair<common::math::LineSegment2d, double>>&
          obstacles_segments_vec);

  std::pair<double, double> GetTopHalfLatSpace(
      bool is_lat_slot,
      const std::vector<std::pair<common::math::LineSegment2d, double>>&
          obstacles_segments_vec);

  common::math::Vec2d GetFineTuneTarget();
  common::math::Vec2d GetFineTuneTarget(double lat_fine_tune_dis,
                                        double lon_fine_tune_dis);

  // 独立编译改造：移除 injector_，添加 VehicleParam 成员
  FineTuningConfig config_;
  OpenSpaceFineTuningConfig search_config_;
  VehicleParam vehicle_params_;  // 独立改造：通过构造器传入

  bool is_entered_lateral_slot_domain_ = false;
  double lon_fine_tune_dis_ = 0.0;
  double lat_fine_tune_dis_ = 0.0;
  double yaw_fine_tune_rad_ = 0.0;
  double target_adc_coord_heading_ = 0.0;
  common::PathPoint raw_end_pose_enu_;
  bool need_lat_fine_tune_ = false;
  common::PathPoint real_end_replan_point_;
  bool has_end_replaned_ = false;  // 独立版恒为false（无 frame_history）
  bool has_narrow_lon_tune_ = false;
  double pre_narrow_lon_fine_tune_dis_ = 0.0;
  bool is_fine_tune_update_ = false;
  std::vector<std::pair<common::math::LineSegment2d, double>> linked_fs_obs_;
  bool is_init_park_in_ = true;
};

}  // namespace planning
}  // namespace TL
