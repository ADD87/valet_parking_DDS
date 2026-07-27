/*
 * Copyright (c) TL Technologies Co., Ltd. 2022. All rights reserved.
 * Description:  open_space_obstacle.cc
 *
 * 独立编译改造（P1改造，doc 159）：
 *   1. 移除 TaskConfig / DependencyInjector 初始化
 *   2. 移除 ThreadSafeIndexedObstacles / FreeSpaceOutArray 解析
 *   3. Init() 从 ObstacleInput 初始化（外部预计算线段）
 *   4. LoadObs() 从 input_obs_ 取线段，应用 ObsFilterMap 过滤
 *   5. AddVirtualObs() 保留核心几何逻辑，跳过 FS/SpeedPlan 依赖部分
 *   6. ScenarioDifficultyDecision() 简化为基于 is_narrow_spot 标志判断
 *
 * 原始: open_space_decider_Original/open_space_obstacle.cc (1969行)
 * 改造后: 保留几何逻辑，移除传感器数据解析
 */

#include "planning/tasks/deciders/open_space_decider/open_space_obstacle.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <tuple>
#include <utility>
#include <vector>

#include "common/configs/vehicle_config_helper.h"
#include "common/math/box2d.h"
#include "common/math/line_segment2d.h"
#include "common/math/polygon2d.h"
#include "common/math/vec2d.h"
#include "common/status/status.h"

namespace TL {
namespace planning {

using TL::common::ErrorCode;
using TL::common::Status;
using common::math::LineSegment2d;
using common::math::Polygon2d;
using common::math::Vec2d;

namespace {
constexpr double kDefaultDistToBoundary = 100.0;
// 注：kEps / kMinVirtualObsLength / kNarrowObsLength 定义于 open_space_obstacle.h
}  // namespace

// ============================================================
// ParkLotStatusToString
// ============================================================
std::string ParkLotStatusToString(const ParkLotStatus& status) {
  switch (status) {
    case NORMAL:         return "NORMAL";
    case INCOMPLETE:     return "INCOMPLETE";
    case POSITION_ERROR: return "POSITION_ERROR";
    case SMALL:          return "SMALL";
    case UNFREE:         return "UNFREE";
    case NONCONVEX:      return "NONCONVEX";
    default:             return "UNDEFINED";
  }
}

// ============================================================
// ObsFilter::ShortDebugString
// ============================================================
std::string ObsFilter::ShortDebugString() const {
  return absl::StrCat(
      "ObsFilter: use_obstacle = ", use_obstacle ? "True" : "False",
      " filter_areas size = ", filter_areas.size(),
      " filter_planes = ", filter_planes.size());
}

// ============================================================
// 构造函数
// 独立编译改造：替代 OpenSpaceObstacle(TaskConfig, DependencyInjector)
// ============================================================
OpenSpaceObstacle::OpenSpaceObstacle(const VehicleParam& vehicle_params,
                                     const ObstacleConfig& config)
    : config_(config), vehicle_params_(vehicle_params) {}

// ============================================================
// Init：从 ObstacleInput 初始化内部状态
// 独立编译改造：原始 Init 解析 FreeSpaceArray，这里直接存储外部输入
// 原始: L89-124
// ============================================================
TL::common::Status OpenSpaceObstacle::Init(
    const ObstacleInput& obstacle_input,
    const common::PathPoint& cur_adc_pose) {
  (void)cur_adc_pose;  // 独立编译改造：初始化时不需要车辆位姿
  // 清空内部状态
  input_obs_.clear();
  virtual_obs_.clear();
  fs_points_.clear();

  // 独立编译改造：从外部输入复制障碍物线段
  input_obs_ = obstacle_input.obs_segments;

  // 独立编译改造：存储FS点云（用于调整虚拟障碍物长度）
  fs_points_ = obstacle_input.fs_points;

  LOG(INFO) << "[OBS_FILTER] Init: input_obs_=" << input_obs_.size()
            << " fs_points=" << fs_points_.size();
  return Status::OK();
}

// ============================================================
// LoadObs：应用 ObsFilterMap 过滤规则，输出障碍物线段
// 独立编译改造：原始解析 USS/BBox/FS 传感器数据，
//               这里从 input_obs_ 取预计算线段，应用 ROI 过滤
// 原始: L126-165
// ============================================================
TL::common::Status OpenSpaceObstacle::LoadObs(
    const ParkingLotVertexType& parking_spot_enu,
    const ObsFilterMap& obs_filter_map,
    std::vector<std::pair<LineSegment2d, double>>* obs_ptr,
    std::vector<std::pair<LineSegment2d, double>>* linked_obs_ptr,
    std::vector<std::pair<LineSegment2d, double>>* high_curb_fs_obs_ptr,
    std::vector<std::pair<LineSegment2d, double>>* low_fs_obs_ptr) {
  if (nullptr == obs_ptr || nullptr == linked_obs_ptr ||
      nullptr == high_curb_fs_obs_ptr || nullptr == low_fs_obs_ptr) {
    LOG(ERROR) << "[OBS_FILTER] LoadObs: null output pointer";
    return Status(ErrorCode::PLANNER_PARKING_ROIDECIDER_ERROR,
                  "LoadObs: null output pointer");
  }

  // 独立编译改造：直接复制预计算障碍物线段
  *obs_ptr = input_obs_;

  // 独立编译改造：linked_obs 和 high_curb_obs 从 input_obs_ 中
  //               不区分（无 FS 分类标记），保留为空
  // 留空 linked_obs_ptr 和 high_curb_fs_obs_ptr

  // 独立编译改造：low_fs_obs 从 input_obs_ 不区分，留空

  // 应用 COMMON_OBS 过滤规则（ROI边界过滤）
  FilterObs(obs_filter_map, obs_ptr);

  // 简化版 AdjustObsBuffer（移除 AvpSpeedPlanCollisionInfo 依赖）
  AdjustObsBuffer(parking_spot_enu, obs_ptr);

  LOG(INFO) << "[OBS_FILTER] LoadObs: obs=" << obs_ptr->size()
            << " after filter/adjust";
  return Status::OK();
}

// ============================================================
// AddVirtualObs：添加虚拟障碍物
// 独立编译改造：移除 FreeSpaceOutArray/OpenSpacePathInfo/
//               ThreadSafeIndexedObstacles/SpeedPlan 依赖
// 保留：垂直泊入/侧方泊入/垂直泊出 虚拟障碍物几何计算
// 跳过：AddTurnSideVirtualObs（需要 FS 实时数据，无输入时为空）
// 原始: L585-672
// ============================================================
void OpenSpaceObstacle::AddVirtualObs(
    const TL::perception::ParkingLotOut::ParkType& park_lot_type,
    const ParkLotInfo& park_lot_info,
    bool is_parking_in,
    bool is_parking_inwards,
    const common::PathPoint& end_pose_enu,
    const std::vector<LineSegment2d>& roi_boundary,
    const common::PathPoint& init_adc_pose,
    const common::PathPoint& cur_adc_pose,
    bool is_consider_wheel_mask,
    const common::math::Box2d& wheel_mask_box,
    bool is_narrow_spot,
    std::vector<std::pair<LineSegment2d, double>>* obs_ptr) {
  if (nullptr == obs_ptr) {
    LOG(ERROR) << "[OBS_FILTER] AddVirtualObs: null obs_ptr";
    return;
  }
  virtual_obs_.clear();

  const auto& parking_spot_enu = park_lot_info.vertices;
  const auto& is_right_slot = park_lot_info.is_right_side;
  const auto& is_high_quality_triggered = park_lot_info.is_high_quality_triggered;

  // ADC 车辆多边形（用于调整虚拟障碍物避开当前车辆）
  // 独立编译改造：原始用 GetBoundingBox(pose, eps, eps)，这里用基础版
  const auto adc_polygon = Polygon2d(
      common::VehicleConfigHelper::GetBoundingBox(cur_adc_pose));

  // 从 ROI 边界开始构建虚拟障碍物列表
  std::vector<LineSegment2d> virtual_obstacles(roi_boundary);

  // 独立编译改造：跳过 AddTurnSideVirtualObs（需要实时 FS 数据）
  // 注释：原始在这里调用 AddTurnSideVirtualObs(..., free_space_array_ptr)
  // 但 standalone 没有 FreeSpace 点云时，该函数会直接 return
  // 简化：当 fs_points_ 非空时，直接使用 fs_points_ 调整虚拟障碍物长度
  double lateral_nearest_dist_to_boundary = kDefaultDistToBoundary;

  if (is_parking_in) {
    // 泊入场景
    if (park_lot_type == TL::perception::ParkingLotOut::VERTICAL ||
        park_lot_type == TL::perception::ParkingLotOut::OBLIQUE) {
      // 垂直/斜向泊入：在车位两侧添加纵向虚拟墙
      AddVerticalSpotParkInVirtualObs(
          is_right_slot, is_parking_inwards,
          lateral_nearest_dist_to_boundary,
          adc_polygon, end_pose_enu, parking_spot_enu,
          is_narrow_spot, is_high_quality_triggered,
          &virtual_obstacles);
    } else {
      // 侧方泊入：在轮廓遮罩处添加底部虚拟墙
      AddLateralSpotParkInVirtualObs(
          cur_adc_pose, end_pose_enu,
          is_consider_wheel_mask, wheel_mask_box,
          &virtual_obstacles);
    }
  } else {
    // 泊出场景
    if (park_lot_type == TL::perception::ParkingLotOut::VERTICAL ||
        park_lot_type == TL::perception::ParkingLotOut::OBLIQUE) {
      AddVerticalSpotParkOutVirtualObs(
          init_adc_pose, is_narrow_spot, &virtual_obstacles);
    }
  }

  // 过滤长度不足或与当前车辆重叠的虚拟障碍物
  for (const auto& obs : virtual_obstacles) {
    if (obs.length() > kMinVirtualObsLength && !adc_polygon.HasOverlap(obs)) {
      virtual_obs_.emplace_back(obs, std::numeric_limits<double>::epsilon());
    } else {
      LOG(INFO) << "[OBS_FILTER] VirtualObs filtered: len="
                << obs.length() << ", overlap=" << adc_polygon.HasOverlap(obs)
                << " | (" << obs.start().x() << "," << obs.start().y()
                << ")->(" << obs.end().x() << "," << obs.end().y() << ")";
    }
  }

  // 追加到输出列表
  obs_ptr->insert(obs_ptr->end(), virtual_obs_.begin(), virtual_obs_.end());
  LOG(INFO) << "[OBS_FILTER] AddVirtualObs: " << virtual_obs_.size()
            << " 虚拟障碍物已添加";
}

// ============================================================
// GetVerticalSpotParkInVirtualObsLength
// 独立编译改造：无 FreeSpaceOutArray 时使用配置默认值
//               有 fs_points_ 时使用几何计算调整
// 原始: L294-359
// ============================================================
void OpenSpaceObstacle::GetVerticalSpotParkInVirtualObsLength(
    bool is_right_slot, bool is_parking_inwards,
    bool is_high_quality_triggered,
    const ParkingLotVertexType& parking_spot_enu,
    const common::PathPoint& end_pose_enu,
    bool is_narrow_spot_scenario,
    double* left_len, double* right_len) {
  if (nullptr == left_len || nullptr == right_len) { return; }

  // 窄车位：固定使用 kNarrowObsLength
  if (is_narrow_spot_scenario) {
    *left_len = kNarrowObsLength;
    *right_len = kNarrowObsLength;
    return;
  }

  // 默认使用配置值
  *left_len = config_.vertical_park_in_virtual_obstacle_lon_dis;
  *right_len = config_.vertical_park_in_virtual_obstacle_lon_dis;

  if (is_high_quality_triggered) { return; }

  // 独立编译改造：有 fs_points_ 时，尝试用几何计算调整
  if (fs_points_.empty()) { return; }

  const bool is_right_turn_side =
      is_parking_inwards ? !is_right_slot : is_right_slot;
  Polygon2d left_corridor_filter;
  Polygon2d right_corridor_filter;
  if (!BuildVerticalSpotParkInVirtualObsFSFilterArea(
          parking_spot_enu, &left_corridor_filter, &right_corridor_filter)) {
    VLOG(2) << "[OBS_FILTER] BuildVerticalSpotPark FSFilter fail";
    return;
  }
  const auto& corridor_filter =
      is_right_turn_side ? right_corridor_filter : left_corridor_filter;
  double virtual_obs_length = config_.vertical_park_in_virtual_obstacle_lon_dis;
  for (const auto& fs_point : fs_points_) {
    if (!corridor_filter.IsPointIn(fs_point)) { continue; }
    virtual_obs_length = std::max(
        virtual_obs_length,
        GetMinVirtualObsLengthBasedOnFS(is_parking_inwards, end_pose_enu, fs_point));
  }
  if (is_right_turn_side) {
    *right_len = virtual_obs_length;
  } else {
    *left_len = virtual_obs_length;
  }
}

// ============================================================
// GetMinVirtualObsLengthBasedOnFS
// 独立编译改造：替换 vehicle_params_.xxx() 为 vehicle_params_.xxx
// 原始: L361-395
// ============================================================
double OpenSpaceObstacle::GetMinVirtualObsLengthBasedOnFS(
    bool is_parking_inwards, const common::PathPoint& end_pose_enu,
    const Vec2d& fs_point) {
  constexpr double kTurnSideFSBuffer = 0.3;
  constexpr double kRadiusMargin = 0.2;
  // 独立编译改造：VehicleParam 无 min_turn_radius 字段，改用 GetMinTurningRadius()
  const double min_radius = vehicle_params_.GetMinTurningRadius() + kRadiusMargin;
  constexpr double kDefaultMinVirtualObsLength = 3.5;
  const auto base_line =
      is_parking_inwards
          ? Vec2d::CreateUnitVec2d(end_pose_enu.theta + M_PI)
          : Vec2d::CreateUnitVec2d(end_pose_enu.theta);
  const auto end_point =
      is_parking_inwards
          // 独立编译改造：wheel_base() → wheel_base
          ? Vec2d(end_pose_enu.x, end_pose_enu.y) -
                vehicle_params_.wheel_base * base_line
          : Vec2d(end_pose_enu.x, end_pose_enu.y);
  const double fs_height = (fs_point - end_point).InnerProd(base_line);
  const double fs_width =
      std::fabs((fs_point - end_point).CrossProd(base_line));
  // 独立编译改造：width() → width
  const double r = min_radius - 0.5 * vehicle_params_.width - kTurnSideFSBuffer;
  const double d = min_radius - fs_width;
  double min_virtual_obs_length =
      r > d ? fs_height - std::sqrt(r * r - d * d) : fs_height;
  // 独立编译改造：front_edge_to_center() → front_edge_to_center
  //               back_edge_to_center() → back_edge_to_center
  min_virtual_obs_length -=
      is_parking_inwards
          ? (vehicle_params_.front_edge_to_center - vehicle_params_.wheel_base)
          : vehicle_params_.back_edge_to_center;
  min_virtual_obs_length =
      std::min(min_virtual_obs_length, kDefaultMinVirtualObsLength);
  return min_virtual_obs_length;
}

// ============================================================
// BuildVerticalSpotParkInVirtualObsFSFilterArea
// 独立编译改造：无变化（纯几何，无依赖）
// 原始: L1927-1969
// ============================================================
bool OpenSpaceObstacle::BuildVerticalSpotParkInVirtualObsFSFilterArea(
    const ParkingLotVertexType& parking_spot_enu,
    Polygon2d* left_corridor_filter,
    Polygon2d* right_corridor_filter) {
  if (nullptr == left_corridor_filter || nullptr == right_corridor_filter) {
    LOG(ERROR) << "BuildVerticalSpotParkInVirtualObsFSFilterArea: null ptr";
    return false;
  }
  const auto top_edge_mid =
      (parking_spot_enu.at(0) + parking_spot_enu.at(3)) * 0.5;
  auto down_top = parking_spot_enu.at(0) - parking_spot_enu.at(1);
  down_top.Normalize();
  auto left_right = parking_spot_enu.at(3) - parking_spot_enu.at(0);
  left_right.Normalize();
  const double filter_width = 3.0;
  const double filter_upper_length = 0.5;
  const double filter_lower_length =
      0.5 * (parking_spot_enu.at(0) - parking_spot_enu.at(1)).Length();

  std::vector<Vec2d> left_filter_points(4);
  left_filter_points.at(3) = top_edge_mid + filter_upper_length * down_top;
  left_filter_points.at(2) = top_edge_mid - filter_lower_length * down_top;
  left_filter_points.at(1) = left_filter_points.at(2) - filter_width * left_right;
  left_filter_points.at(0) = left_filter_points.at(3) - filter_width * left_right;
  *left_corridor_filter = Polygon2d(left_filter_points);

  std::vector<Vec2d> right_filter_points(4);
  right_filter_points.at(0) = top_edge_mid + filter_upper_length * down_top;
  right_filter_points.at(1) = top_edge_mid - filter_lower_length * down_top;
  right_filter_points.at(2) = right_filter_points.at(1) + filter_width * left_right;
  right_filter_points.at(3) = right_filter_points.at(0) + filter_width * left_right;
  *right_corridor_filter = Polygon2d(right_filter_points);

  // 独立编译改造：Polygon2d 无 GetValid()，改用顶点数检查
  return left_corridor_filter->num_points() >= 3 &&
         right_corridor_filter->num_points() >= 3;
}

// ============================================================
// AddVerticalSpotParkInVirtualObs
// 独立编译改造：替换 vehicle_params_.xxx() → .xxx
//               替换 config_.open_space_roi_decider_config().xxx() → config_.xxx
//               替换 double_type::ComparedToZero → direct compare
//               替换 VehicleConfigHelper::GetBoundingBox(pose,eps,eps)
//                   → 不使用，两端已在 AddVirtualObs 处计算
// 原始: L397-489
// ============================================================
void OpenSpaceObstacle::AddVerticalSpotParkInVirtualObs(
    bool is_right_slot, bool is_parking_inwards,
    double lateral_nearest_dist_to_boundary,
    const Polygon2d& adc_polygon,
    const common::PathPoint& end_pose_enu,
    const ParkingLotVertexType& parking_spot_enu,
    bool is_narrow_spot_scenario, bool is_high_quality_triggered,
    std::vector<LineSegment2d>* virtual_obstacles_ptr) {
  if (nullptr == virtual_obstacles_ptr) { return; }

  double left_virtual_obstacle_lon_dis = 0.0;
  double right_virtual_obstacle_lon_dis = 0.0;
  GetVerticalSpotParkInVirtualObsLength(
      is_right_slot, is_parking_inwards, is_high_quality_triggered,
      parking_spot_enu, end_pose_enu, is_narrow_spot_scenario,
      &left_virtual_obstacle_lon_dis, &right_virtual_obstacle_lon_dis);

  VLOG(2) << "[OBS_FILTER] left_lon=" << left_virtual_obstacle_lon_dis
          << " right_lon=" << right_virtual_obstacle_lon_dis;

  // 独立编译改造：config_.open_space_roi_decider_config().vertical_park_in_virtual_obstacle_lat_dis()
  //               → config_.vertical_park_in_virtual_obstacle_lat_dis
  const double virtual_obstacle_lat_dis =
      config_.vertical_park_in_virtual_obstacle_lat_dis;
  if (virtual_obstacle_lat_dis <= 0.0) {
    VLOG(2) << "[OBS_FILTER] virtual obstacle lat_dis not positive, skip";
    return;
  }

  LineSegment2d left_virtual_obs;
  LineSegment2d right_virtual_obs;
  Vec2d end_pose_v = Vec2d(end_pose_enu.x, end_pose_enu.y);
  // 独立编译改造：end_pose_enu.theta() → .theta
  if (is_parking_inwards) {
    // 独立编译改造：vehicle_params_.wheel_base() → .wheel_base
    Vec2d front_axis_center =
        end_pose_v + vehicle_params_.wheel_base *
                         Vec2d::CreateUnitVec2d(end_pose_enu.theta);
    left_virtual_obs = LineSegment2d(
        front_axis_center,
        front_axis_center + left_virtual_obstacle_lon_dis *
                                Vec2d::CreateUnitVec2d(end_pose_enu.theta + M_PI));
    right_virtual_obs = LineSegment2d(
        front_axis_center,
        front_axis_center + right_virtual_obstacle_lon_dis *
                                Vec2d::CreateUnitVec2d(end_pose_enu.theta + M_PI));
  } else {
    left_virtual_obs = LineSegment2d(
        end_pose_v,
        end_pose_v + left_virtual_obstacle_lon_dis *
                         Vec2d::CreateUnitVec2d(end_pose_enu.theta));
    right_virtual_obs = LineSegment2d(
        end_pose_v,
        end_pose_v + right_virtual_obstacle_lon_dis *
                         Vec2d::CreateUnitVec2d(end_pose_enu.theta));
  }

  // Lambda：根据当前车辆位置截断虚拟障碍物
  auto adjust_virtual_obs_length =
      [](const Polygon2d& adc_poly,
         LineSegment2d* const vobs_ptr) {
        double cutoff_dis = 0.0;
        for (const auto& point : adc_poly.points()) {
          double lon_dis = vobs_ptr->ProjectOntoUnit(point);
          if (lon_dis < 0) {
            cutoff_dis = vobs_ptr->length();
            break;
          }
          cutoff_dis = std::max(vobs_ptr->length() - lon_dis, cutoff_dis);
        }
        if (cutoff_dis > kEps) {
          vobs_ptr->Extend(-1 * cutoff_dis);
        }
      };

  // 独立编译改造：left_edge_to_center() → .left_edge_to_center
  //               right_edge_to_center() → .right_edge_to_center
  left_virtual_obs.Translate(
      vehicle_params_.left_edge_to_center + virtual_obstacle_lat_dis,
      left_virtual_obs.heading() + M_PI_2);
  right_virtual_obs.Translate(
      vehicle_params_.right_edge_to_center + virtual_obstacle_lat_dis,
      left_virtual_obs.heading() - M_PI_2);
  adjust_virtual_obs_length(adc_polygon, &left_virtual_obs);
  adjust_virtual_obs_length(adc_polygon, &right_virtual_obs);

  if (left_virtual_obs.length() > kMinVirtualObsLength &&
      left_virtual_obstacle_lon_dis > 0.0) {
    virtual_obstacles_ptr->emplace_back(left_virtual_obs);
  }
  if (right_virtual_obs.length() > kMinVirtualObsLength &&
      right_virtual_obstacle_lon_dis > 0.0) {
    virtual_obstacles_ptr->emplace_back(right_virtual_obs);
  }
}

// ============================================================
// AddLateralSpotParkInVirtualObs
// 独立编译改造：
//   替换 vehicle_params_.xxx() → .xxx
//   替换 config_.open_space_speed_optimizer_config().wheel_mask_to_wheel_base_distance()
//       → config_.wheel_mask_to_wheel_base_distance
//   替换 FLAGS_avp_ego_inflated_buffer_for_checking_collision → 0.1
//   替换 GetBoundingBox(pose, extra, extra) → GetBoundingBox(pose)
// 原始: L491-535
// ============================================================
void OpenSpaceObstacle::AddLateralSpotParkInVirtualObs(
    const common::PathPoint& cur_adc_pose,
    const common::PathPoint& end_pose_enu,
    bool is_consider_wheel_mask,
    const common::math::Box2d& wheel_mask_box,
    std::vector<LineSegment2d>* virtual_obstacles_ptr) {
  if (nullptr == virtual_obstacles_ptr || !is_consider_wheel_mask) { return; }
  const auto end_point = Vec2d(end_pose_enu.x, end_pose_enu.y);
  const auto wheel_mask_seg = LineSegment2d(
      wheel_mask_box.center() +
          wheel_mask_box.half_length() *
              Vec2d::CreateUnitVec2d(wheel_mask_box.heading() + M_PI),
      wheel_mask_box.center() +
          wheel_mask_box.half_length() *
              Vec2d::CreateUnitVec2d(wheel_mask_box.heading()));
  // 独立编译改造：wheel_base() → .wheel_base
  if (wheel_mask_seg.DistanceTo(end_point) > 0.5 * vehicle_params_.wheel_base) {
    LOG(ERROR) << "[OBS_FILTER] wheel mask is opposite to end pose";
    return;
  }
  // 独立编译改造：back_edge_to_center() → .back_edge_to_center
  //               config_.open_space_speed_optimizer_config().wheel_mask_to_wheel_base_distance()
  //               → config_.wheel_mask_to_wheel_base_distance
  const double dist_virtual_obs_to_end_point =
      vehicle_params_.back_edge_to_center + kEps +
      std::max(0.0, wheel_mask_seg.ProductOntoUnit(end_point) -
                        config_.wheel_mask_to_wheel_base_distance);
  const auto end_point_bottom =
      end_point + dist_virtual_obs_to_end_point *
                      Vec2d::CreateUnitVec2d(end_pose_enu.theta + M_PI);
  const auto bottom_virtual_obs = LineSegment2d(
      end_point_bottom + wheel_mask_box.half_length() *
                             Vec2d::CreateUnitVec2d(end_pose_enu.theta + M_PI_2),
      end_point_bottom + wheel_mask_box.half_length() *
                             Vec2d::CreateUnitVec2d(end_pose_enu.theta - M_PI_2));
  // 独立编译改造：GetBoundingBox(cur_adc_pose, extra, extra) → GetBoundingBox(cur_adc_pose)
  const auto cur_adc_polygon =
      Polygon2d(common::VehicleConfigHelper::GetBoundingBox(cur_adc_pose));
  if (!cur_adc_polygon.HasOverlap(bottom_virtual_obs)) {
    virtual_obstacles_ptr->emplace_back(bottom_virtual_obs);
  }
}

// ============================================================
// AddVerticalSpotParkOutVirtualObs
// 独立编译改造：
//   替换 vehicle_params_.xxx() → .xxx
//   替换 config_.open_space_roi_decider_config().vertical_park_out_virtual_obstacle_bottom_dis()
//       → config_.vertical_park_out_virtual_obstacle_bottom_dis
//   替换 config_.open_space_roi_decider_config().vertical_park_in_virtual_obstacle_lat_dis()
//       → config_.vertical_park_in_virtual_obstacle_lat_dis
// 原始: L537-583
// ============================================================
void OpenSpaceObstacle::AddVerticalSpotParkOutVirtualObs(
    const common::PathPoint& init_adc_pose,
    bool is_narrow_spot_scenario,
    std::vector<LineSegment2d>* virtual_obstacles_ptr) {
  if (nullptr == virtual_obstacles_ptr) { return; }

  Vec2d init_adc_pose_vec = Vec2d(init_adc_pose.x, init_adc_pose.y);
  // 独立编译改造：width() → .width
  auto adc_bottom_virtual_obs = LineSegment2d(
      init_adc_pose_vec,
      init_adc_pose_vec + vehicle_params_.width *
                              Vec2d::CreateUnitVec2d(init_adc_pose.theta + M_PI_2));
  // 独立编译改造：left_edge_to_center() → .left_edge_to_center
  adc_bottom_virtual_obs.Translate(
      vehicle_params_.left_edge_to_center, init_adc_pose.theta - M_PI_2);
  // 独立编译改造：back_edge_to_center() → .back_edge_to_center
  adc_bottom_virtual_obs.Translate(
      vehicle_params_.back_edge_to_center +
          config_.vertical_park_out_virtual_obstacle_bottom_dis,
      init_adc_pose.theta + M_PI);
  if (adc_bottom_virtual_obs.length() > kMinVirtualObsLength) {
    virtual_obstacles_ptr->emplace_back(adc_bottom_virtual_obs);
  }

  if (!is_narrow_spot_scenario) { return; }

  auto left_virtual_obs = LineSegment2d(
      init_adc_pose_vec,
      init_adc_pose_vec + kNarrowObsLength *
                              Vec2d::CreateUnitVec2d(init_adc_pose.theta));
  auto right_virtual_obs = left_virtual_obs;
  const double virtual_obstacle_lat_dis =
      config_.vertical_park_in_virtual_obstacle_lat_dis;
  left_virtual_obs.Translate(
      vehicle_params_.left_edge_to_center + virtual_obstacle_lat_dis,
      left_virtual_obs.heading() + M_PI_2);
  right_virtual_obs.Translate(
      vehicle_params_.right_edge_to_center + virtual_obstacle_lat_dis,
      left_virtual_obs.heading() - M_PI_2);
  if (left_virtual_obs.length() > kMinVirtualObsLength) {
    virtual_obstacles_ptr->emplace_back(left_virtual_obs);
  }
  if (right_virtual_obs.length() > kMinVirtualObsLength) {
    virtual_obstacles_ptr->emplace_back(right_virtual_obs);
  }
}

// ============================================================
// FilterObs：ROI内/外过滤（基于 COMMON_OBS 的 filter_areas/filter_planes）
// 独立编译改造：无变化（纯逻辑，无框架依赖）
// 原始: L677-704
// ============================================================
void OpenSpaceObstacle::FilterObs(
    const ObsFilterMap& obs_filter_map,
    std::vector<std::pair<LineSegment2d, double>>* obs_ptr) {
  if (nullptr == obs_ptr || obs_ptr->empty()) { return; }
  if (obs_filter_map.find(COMMON_OBS) == obs_filter_map.end()) { return; }
  const auto& common_obs_filter = obs_filter_map.at(COMMON_OBS);
  VLOG(2) << "[OBS_FILTER] " << common_obs_filter.ShortDebugString();
  if (!common_obs_filter.use_obstacle ||
      (common_obs_filter.filter_areas.empty() &&
       common_obs_filter.filter_planes.empty())) {
    return;
  }
  auto iter = obs_ptr->begin();
  while (iter != obs_ptr->end()) {
    if (FilterSegtBaseArea(common_obs_filter.filter_areas, iter->first) ||
        FilterSegBasePlane(common_obs_filter.filter_planes, iter->first)) {
      iter = obs_ptr->erase(iter);
    } else {
      ++iter;
    }
  }
  LOG(INFO) << "[OBS_FILTER] FilterObs: " << obs_ptr->size() << " 条剩余";
}

// ============================================================
// FilterSegtBaseArea：基于多边形区域过滤线段
// 独立编译改造：static 方法，无变化
// 原始: L1432-1443
// ============================================================
bool OpenSpaceObstacle::FilterSegtBaseArea(
    const std::vector<std::pair<Polygon2d, bool>>& filter_areas,
    const LineSegment2d& obs_seg) {
  return std::any_of(
      filter_areas.begin(), filter_areas.end(),
      [&](const std::pair<Polygon2d, bool>& filter) {
        return (filter.second && filter.first.HasOverlap(obs_seg)) ||
               (!filter.second && !filter.first.HasOverlap(obs_seg));
      });
}

// ============================================================
// FilterSegBasePlane：基于平面方向过滤线段
// 独立编译改造：static 方法，无变化
// 原始: L1443-1460
// ============================================================
bool OpenSpaceObstacle::FilterSegBasePlane(
    const std::vector<std::pair<LineSegment2d, bool>>& filter_planes,
    const LineSegment2d& obs_seg) {
  return std::any_of(
      filter_planes.begin(), filter_planes.end(),
      [&](const std::pair<LineSegment2d, bool>& filter) {
        double cross1 = filter.first.ProductOntoUnit(obs_seg.start());
        double cross2 = filter.first.ProductOntoUnit(obs_seg.end());
        return (filter.second && (cross1 > 0.0 || cross2 > 0.0)) ||
               (!filter.second && (cross1 < 0.0 || cross2 < 0.0));
      });
}

// ============================================================
// AdjustObsBuffer：简化版障碍物缓冲调整
// 独立编译改造：移除 AvpSpeedPlanCollisionInfo 依赖
//               仅保留：距车位上方 kHighDis 的障碍物额外缓冲
// 原始: L705-739（简化）
// ============================================================
void OpenSpaceObstacle::AdjustObsBuffer(
    const ParkingLotVertexType& parking_spot_enu,
    std::vector<std::pair<LineSegment2d, double>>* obs_ptr) {
  if (nullptr == obs_ptr || obs_ptr->empty()) { return; }
  static constexpr double kHighDis = 4.0;
  static constexpr double kExtraBuffer = 0.2;
  // 以车位顶边为基准，向上平移 kHighDis
  LineSegment2d plane{parking_spot_enu[0], parking_spot_enu[3]};
  plane.Translate(kHighDis, plane.heading() + M_PI_2);
  auto is_above_plane = [&](const LineSegment2d& obs_seg) {
    return plane.ProductOntoUnit(obs_seg.start()) > 0.0 &&
           plane.ProductOntoUnit(obs_seg.end()) > 0.0;
  };
  for (auto& obs : *obs_ptr) {
    if (obs.second < kEps) { continue; }
    if (is_above_plane(obs.first)) { obs.second += kExtraBuffer; }
  }
}

// ============================================================
// ScenarioDifficultyDecision：场景难度评估（简化版）
// 独立编译改造：移除 FreeSpaceOutArray 依赖
//               仅基于 is_narrow_spot 标志判断
// 原始: L1554-1573（含 IsDeadendScenario/IsNarrowPassageScenario，需 FS）
// ============================================================
uint32_t OpenSpaceObstacle::ScenarioDifficultyDecision(
    bool is_parking_in, const ParkLotInfo& park_lot_info) {
  (void)is_parking_in;
  // 独立编译改造：简化为仅检查 is_narrow_spot 标志
  // 原始还有 IsDeadendScenario / IsNarrowPassageScenario（需 FS 数据）
  // 场景难度位掩码（与原始 ScenarioDiffcultyType 枚举对应）
  constexpr uint32_t NORMAL_SCENARIO = 0;
  constexpr uint32_t NARROW_SPOT_SCENARIO = 1 << 1;
  uint32_t ret = NORMAL_SCENARIO;
  if (park_lot_info.is_narrow_spot &&
      (park_lot_info.park_type == TL::perception::ParkingLotOut::VERTICAL ||
       park_lot_info.park_type == TL::perception::ParkingLotOut::OBLIQUE)) {
    ret |= NARROW_SPOT_SCENARIO;  // 位掩码运算用 |=（非 +=）
  }
  LOG(INFO) << "[OBS_FILTER] ScenarioDifficulty: " << ret;
  return ret;
}

}  // namespace planning
}  // namespace TL
