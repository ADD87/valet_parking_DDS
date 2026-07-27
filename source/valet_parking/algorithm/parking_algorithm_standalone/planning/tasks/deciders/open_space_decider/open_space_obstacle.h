/*
 * Copyright (c) TL Technologies Co., Ltd. 2022. All rights reserved.
 * Description:  open_space_obstacle.h
 *
 * 独立编译改造（P1改造，doc 159）：
 *   1. 移除 TaskConfig / DependencyInjector 依赖
 *   2. 移除 ThreadSafeIndexedObstacles / FreeSpaceOutArray 依赖
 *   3. 移除 AVPStatus::ParkingType / planning_internal proto 依赖
 *   4. 引入 ObstacleConfig 替代 TaskConfig
 *   5. 引入 ObstacleInput 替代传感器数据对象
 *   6. 保留全部几何逻辑（AddVirtualObs, FilterObs 等）
 *   7. FreeSpace相关逻辑：无数据时使用默认值
 */

#pragma once

#include <algorithm>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "absl/strings/str_cat.h"
#include "common/math/box2d.h"
#include "common/math/line_segment2d.h"
#include "common/math/polygon2d.h"
#include "common/math/vec2d.h"
#include "common/status/status.h"
#include "proto_convert/pnc_point_convert.h"
#include "proto_convert/parking_lot_convert.h"
#include "proto_convert/vehicle_state_convert.h"
#include "planning/open_space/vehicle_param.h"

namespace TL {
namespace planning {

using Vec2d = common::math::Vec2d;

// ================================================================================
// 常量定义（保留自原始 open_space_obstacle.h）
// ================================================================================
static constexpr int kParkingLotVertexNum = 4;
static constexpr double kMinVirtualObsLength = 0.1;
static constexpr double kBtmLowFsBuffer = 0.05;
static constexpr double kADCBoxEps = 0.001;
static constexpr double kNarrowObsLength = 3.5;
static constexpr double kEps = 1e-5;

using ParkingLotVertexType = std::array<common::math::Vec2d, 4>;

// ================================================================================
// ParkLotStatus 枚举
// ================================================================================
enum ParkLotStatus {
  NORMAL = 0,
  INCOMPLETE = 1,
  POSITION_ERROR = 2,
  SMALL = 3,
  UNFREE = 4,
  NONCONVEX = 5,
};

std::string ParkLotStatusToString(const ParkLotStatus& status);

// ================================================================================
// ParkLotInfo 结构体
// 独立编译改造：使用 proto_convert 定义的 ParkType/SenType
// ================================================================================
struct ParkLotInfo {
  TL::perception::ParkingLotOut::ParkType park_type =
      TL::perception::ParkingLotOut::NONE;
  ParkingLotVertexType vertices;
  std::tuple<bool, Vec2d, Vec2d> wheel_mask = {false, {0, 0}, {0, 0}};
  ParkLotStatus status = NORMAL;
  bool is_parking_lot_update = false;
  bool is_high_quality_triggered = false;
  bool is_right_side = false;
  bool is_narrow_spot = false;
  TL::perception::ParkingLotOut::SenType sensor_type =
      TL::perception::ParkingLotOut::CAMERA;
};

// ================================================================================
// 障碍物过滤类型枚举
// ================================================================================
enum OpenSpaceFilterObsType {
  COMMON_OBS = 0,
  USS_OBS = 1,
  BOX_OBS = 2,
  FREE_SPACE_OBS = 3,
  LOW_FREE_SPACE_OBS = 4,
  WHEEL_MASK_OBS = 5,
  HIGH_CURB_FREE_SPACE_OBS = 6,
};

// ================================================================================
// ObsFilter 结构体
// ================================================================================
struct ObsFilter {
  bool use_obstacle = false;
  std::vector<std::pair<common::math::Polygon2d, bool>> filter_areas;
  std::vector<std::pair<common::math::LineSegment2d, bool>> filter_planes;

  std::string ShortDebugString() const;
};

using ObsFilterMap = std::unordered_map<OpenSpaceFilterObsType, ObsFilter>;

// ================================================================================
// ObstacleConfig 独立配置结构体
// 独立编译改造：替代 TaskConfig 中 open_space_roi_decider_config() 相关字段
// ================================================================================
struct ObstacleConfig {
  // 垂直泊入虚拟障碍物
  double vertical_park_in_virtual_obstacle_lon_dis = 3.5;
  // 注意：lat_dis 须 > veh_safety_buffer（0.1m），否则虚拟走廊与安全缓冲之后净间隙=0
  // lat_dis=0.1(原始) → 净间隙=0 → Hybrid A*无法找到可行路径
  // lat_dis=0.0 → 完全禁用虚拟侧墙（用于调试）
  // lat_dis=0.3 → 净间隙=0.2m（1个格），可通过（推荐生产配置）
  double vertical_park_in_virtual_obstacle_lat_dis = 0.0;
  // 感知开关
  bool enable_perception_obstacles = true;
  bool enable_consider_obstacle_box = false;
  bool enable_uss = false;
  // USS墙宽
  double uss_wall_width = 0.5;
  // 转向侧过滤长度
  double turn_side_filter_length = 5.0;
  // 泊出底部虚拟障碍物距离
  double vertical_park_out_virtual_obstacle_bottom_dis = 0.2;
  // 侧方泊入轮廓遮罩到轴距距离
  double wheel_mask_to_wheel_base_distance = 0.0;
};

// ================================================================================
// ObstacleInput 独立输入结构体
// 独立编译改造：替代 ThreadSafeIndexedObstacles* + FreeSpaceOutArray
// ================================================================================
struct ObstacleInput {
  // 预计算障碍物线段（来自 test_data_generator::GenerateObstacles）
  std::vector<std::pair<common::math::LineSegment2d, double>> obs_segments;
  // FS点云（无数据时留空，虚拟障碍物长度使用默认值）
  std::vector<Vec2d> fs_points;
};

// ================================================================================
// OpenSpaceObstacle 类（独立编译版）
// 原始: OpenSpaceObstacle(TaskConfig, DependencyInjector)
// ================================================================================
class OpenSpaceObstacle {
 public:
  explicit OpenSpaceObstacle(const VehicleParam& vehicle_params,
                             const ObstacleConfig& config);
  ~OpenSpaceObstacle() = default;

  /**
   * @brief 从 ObstacleInput 初始化（清空内部状态）
   * 独立编译改造：原始 Init 解析传感器数据，这里直接取预计算线段
   */
  TL::common::Status Init(const ObstacleInput& obstacle_input,
                          const common::PathPoint& cur_adc_pose);

  /**
   * @brief 加载障碍物（应用 ObsFilterMap 过滤规则）
   * 独立编译改造：移除 ThreadSafeIndexedObstacles/AvpSpeedPlanCollisionInfo
   */
  TL::common::Status LoadObs(
      const ParkingLotVertexType& parking_spot_enu,
      const ObsFilterMap& obs_filter_map,
      std::vector<std::pair<common::math::LineSegment2d, double>>* obs_ptr,
      std::vector<std::pair<common::math::LineSegment2d, double>>* linked_obs_ptr,
      std::vector<std::pair<common::math::LineSegment2d, double>>* high_curb_fs_obs_ptr,
      std::vector<std::pair<common::math::LineSegment2d, double>>* low_fs_obs_ptr);

  /**
   * @brief 添加虚拟障碍物（纯几何计算，无传感器依赖）
   * 独立编译改造：移除 FreeSpaceOutArray/ThreadSafeIndexedObstacles/
   *               OpenSpacePathInfo/planning_internal 依赖
   */
  void AddVirtualObs(
      const TL::perception::ParkingLotOut::ParkType& park_lot_type,
      const ParkLotInfo& park_lot_info,
      bool is_parking_in,
      bool is_parking_inwards,
      const common::PathPoint& end_pose_enu,
      const std::vector<common::math::LineSegment2d>& roi_boundary,
      const common::PathPoint& init_adc_pose,
      const common::PathPoint& cur_adc_pose,
      bool is_consider_wheel_mask,
      const common::math::Box2d& wheel_mask_box,
      bool is_narrow_spot,
      std::vector<std::pair<common::math::LineSegment2d, double>>* obs_ptr);

  /**
   * @brief 场景难度评估（简化版，基于车位尺寸）
   * 独立编译改造：移除 FreeSpaceOutArray 依赖
   */
  uint32_t ScenarioDifficultyDecision(bool is_parking_in,
                                      const ParkLotInfo& park_lot_info);

  const std::vector<std::pair<common::math::LineSegment2d, double>>&
  virtual_obs() const { return virtual_obs_; }

 private:
  void GetVerticalSpotParkInVirtualObsLength(
      bool is_right_slot, bool is_parking_inwards, bool is_high_quality_triggered,
      const ParkingLotVertexType& parking_spot_enu,
      const common::PathPoint& end_pose_enu, bool is_narrow_spot_scenario,
      double* left_len, double* right_len);

  double GetMinVirtualObsLengthBasedOnFS(
      bool is_parking_inwards, const common::PathPoint& end_pose_enu,
      const Vec2d& fs_point);

  static bool BuildVerticalSpotParkInVirtualObsFSFilterArea(
      const ParkingLotVertexType& parking_spot_enu,
      common::math::Polygon2d* left_corridor_filter,
      common::math::Polygon2d* right_corridor_filter);

  void AddVerticalSpotParkInVirtualObs(
      bool is_right_slot, bool is_parking_inwards,
      double lateral_nearest_dist_to_boundary,
      const common::math::Polygon2d& adc_polygon,
      const common::PathPoint& end_pose_enu,
      const ParkingLotVertexType& parking_spot_enu,
      bool is_narrow_spot, bool is_high_quality_triggered,
      std::vector<common::math::LineSegment2d>* virtual_obstacles_ptr);

  void AddLateralSpotParkInVirtualObs(
      const common::PathPoint& cur_adc_pose,
      const common::PathPoint& end_pose_enu,
      bool is_consider_wheel_mask,
      const common::math::Box2d& wheel_mask_box,
      std::vector<common::math::LineSegment2d>* virtual_obstacles_ptr);

  void AddVerticalSpotParkOutVirtualObs(
      const common::PathPoint& init_adc_pose, bool is_narrow_spot,
      std::vector<common::math::LineSegment2d>* virtual_obstacles_ptr);

  void FilterObs(
      const ObsFilterMap& obs_filter_map,
      std::vector<std::pair<common::math::LineSegment2d, double>>* obs_ptr);

  static bool FilterSegtBaseArea(
      const std::vector<std::pair<common::math::Polygon2d, bool>>& filter_areas,
      const common::math::LineSegment2d& seg);

  static bool FilterSegBasePlane(
      const std::vector<std::pair<common::math::LineSegment2d, bool>>& filter_planes,
      const common::math::LineSegment2d& seg);

  void AdjustObsBuffer(
      const ParkingLotVertexType& parking_spot_enu,
      std::vector<std::pair<common::math::LineSegment2d, double>>* obs_ptr);

  // 独立编译改造：替代 config_(TaskConfig&)
  ObstacleConfig config_;
  // 独立编译改造：替代 VehicleConfigHelper::GetConfig().vehicle_param()
  VehicleParam vehicle_params_;
  // 来自外部输入的预计算障碍物线段
  std::vector<std::pair<common::math::LineSegment2d, double>> input_obs_;
  // 虚拟障碍物（由 AddVirtualObs 计算）
  std::vector<std::pair<common::math::LineSegment2d, double>> virtual_obs_;
  // FS点云（来自 ObstacleInput::fs_points，用于调整虚拟障碍物长度）
  std::vector<Vec2d> fs_points_;
};

}  // namespace planning
}  // namespace TL
