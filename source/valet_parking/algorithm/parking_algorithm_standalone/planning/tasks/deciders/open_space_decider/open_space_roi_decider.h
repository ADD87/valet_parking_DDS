/**
 * @file open_space_roi_decider.h
 * @brief ROI决策器 - 独立编译改造版
 *
 * 独立编译改造：
 * - 移除 Decider 基类继承
 * - 移除 Frame* / DependencyInjector 依赖
 * - 移除 TaskConfig，使用 RoiDeciderConfig 结构体
 * - 移除 FLAGS_xxx，使用 config_ 成员
 * - proto setter/getter → 直接成员赋值
 * - 仅泊入场景(PARKING_IN)核心逻辑保留
 * - 原始代码保存在 open_space_decider_Original/ 目录
 *
 * 改造规律 (doc 130):
 *   1. Proto→convert    2. getter→member    3. setter→assign
 *   4. 注入→直接        5. 去重依赖          6. absl→std
 *   7. 日志适配         8. 本地实现
 *
 * Copyright 2019 The TL Authors. All Rights Reserved.
 */

#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include "common/math/box2d.h"
#include "common/math/line_segment2d.h"
#include "common/math/math_utils.h"
#include "common/math/polygon2d.h"
#include "common/math/vec2d.h"
#include "planning/common/open_space_info_lite.h"  // ParkingScenarioType, DestRegionWithAng
#include "planning/open_space/vehicle_param.h"
#include "proto_convert/parking_lot_convert.h"
#include "proto_convert/pnc_point_convert.h"
#include "proto_convert/vehicle_state_convert.h"
// P1改造：障碍物过滤+虚拟障碍物模块（doc 159）
#include "planning/tasks/deciders/open_space_decider/open_space_obstacle.h"
// P2改造：终点微调模块（doc 159）
#include "planning/tasks/deciders/open_space_decider/open_space_fine_tuning.h"

// 独立编译改造：不再包含 Apollo 框架头文件
// - planning/tasks/deciders/decider.h
// - planning/common/planning_context.h
// - planning/common/planning_gflags.h
// - planning/proto/*.pb.h (非必要)

namespace TL {
namespace planning {

// ============================================================
// 类型别名（保持与原始代码兼容）
// ============================================================
using ParkingLotVertexType = std::array<common::math::Vec2d, 4>;
using Vec2d = common::math::Vec2d;

// ParkLotStatus 枚举已在 open_space_obstacle.h 中定义，无需重复
// ============================================================
// 配置参数结构体
// 独立编译改造：替代 config_.open_space_roi_decider_config().xxx()
// ============================================================
struct RoiDeciderConfig {
  // -- 车位校验参数 --
  // 对齐 planning_config.pb.txt: vertical_slot_longitudinal_buffer: 0.0
  double vertical_min_parklot_lateral_buffer = 0.1;
  double vertical_slot_longitudinal_buffer = 0.0;   // pb.txt: 0.0
  double lateral_min_parklot_lateral_buffer = 0.1;
  double lateral_slot_longitudinal_buffer = 0.5;    // pb.txt: 0.5

  // -- 终点计算参数 --
  double vertical_parking_depth_buffer = 0.2;
  double oblique_parking_depth_buffer = 0.2;

  // -- ROI边界参数 --
  // 对齐 planning_config.pb.txt: roi_longitudinal_range_start/end: 15
  double roi_longitudinal_range_start = 8.0;       // pb.txt: 15
  double roi_longitudinal_range_end = 8.0;         // pb.txt: 15
  double vertical_expand_buffer = 1.02;             // pb.txt: 1.02
  double park_out_vertical_expand_buffer = 0.5;
  double lateral_expand_buffer = 7.0;               // pb.txt: 7.0
  double lateral_expand_bottom_buffer = 0.5;

  // -- ROI松弛参数 --
  // 对齐 planning_config.pb.txt: roi_slack_distance: 1.0, roi_edge_buffer: 1.0
  double roi_slack_distance = 1.0;                  // pb.txt: 1.0
  double roi_edge_buffer = 1.0;                     // pb.txt: 1.0

  // -- 目标区域松弛参数 (垂直泊入) --
  // 对齐 planning_config.pb.txt park_in_vertical_end_pose_relax: h=0.1 v=0.2 a=0.01
  double park_in_vertical_relax_vertical = 0.2;     // pb.txt: vertical: 0.2
  double park_in_vertical_relax_horizontal = 0.1;   // pb.txt: horizontal: 0.1
  double park_in_vertical_relax_angle = 0.01;       // pb.txt: angle: 0.01
  // -- 目标区域松弛参数 (侧方泊入) --
  // 对齐 planning_config.pb.txt park_in_lateral_end_pose_relax: h=0.8 v=0.3 a=0.01
  double park_in_lateral_relax_vertical = 0.3;      // pb.txt: vertical: 0.3
  double park_in_lateral_relax_horizontal = 0.8;    // pb.txt: horizontal: 0.8
  double park_in_lateral_relax_angle = 0.01;        // pb.txt: angle: 0.01

  // -- 通道宽度 --
  double open_space_lane_width = 6.0;

  // -- 是否车头入库 --
  // 独立编译改造：替代 FLAGS_avp_enable_parking_inwards
  bool avp_enable_parking_inwards = false;

  // -- LIDAR ROI 适配 --
  bool enable_adapt_lidar_sensor_roi = false;

  // -- P4: slot inner FS 判断半径 --
  // 独立编译改造：替代 config_.open_space_roi_decider_config().use_slot_inner_fs_radius()
  // 车辆前中心（泊入）或后中心（倒车）到车位入口距离小于此值时，is_slot_inner_fs_valid=true
  double use_slot_inner_fs_radius = 1.5;  // 默认 1.5m（原始配置为 1.5m）

  static RoiDeciderConfig GetDefault() { return RoiDeciderConfig(); }
};

// ============================================================
// ROI决策器输出
// ============================================================
struct RoiDeciderOutput {
  std::vector<double> xy_bounds;                // AABB [xmin, xmax, ymin, ymax] (局部坐标)
  common::PathPoint end_pose;                   // 终点位姿 (ENU坐标)
  DestRegionWithAng dest_region;                // 目标区域
  common::math::Vec2d origin_point;             // 坐标系原点
  double origin_heading = 0.0;                  // 坐标旋转角
  ParkingScenarioType scenario_type = ParkingScenarioType::DEFAULT_TYPE;
  bool is_parking_inwards = false;
  bool is_right_side = false;
  ParkLotStatus lot_status = NORMAL;
  // P1改造：障碍物过滤结果（doc 159）
  std::vector<std::pair<common::math::LineSegment2d, double>> obs_segments;
  std::vector<std::pair<common::math::LineSegment2d, double>> linked_obs_segments;
  std::vector<std::pair<common::math::LineSegment2d, double>> high_curb_obs_segments;
  // P2改造：终点微调结果（doc 159）
  common::PathPoint fine_tuned_end_pose;  // 微调后终点（算法起点优先使用）
  bool has_fine_tuned = false;            // 是否已成功微调
  // P3改造：场景难度 + 泊出终点（doc 159）
  uint32_t scenario_difficulty = 0;       // 场景难度位掩码（0=正常，bit1=窄车位）
  common::PathPoint parking_out_end_pose; // 泊出目标位姿（简化：泊入终点反向+固定偏移）
  // P4改造：车位内FreeSpace有效性 + 上帧终点状态（doc 159）
  // 默认 false：车辆泊车开始时在车位外，只有驶入车位入口 < radius 时才为 true
  bool is_slot_inner_fs_valid = false;    // 车辆是否已进入车位内域（几何判断）

  // ── 可视化中间状态（可视化专用，不参与规划逻辑）──
  // vertices_roi_order: ExtractVertices后的ROI内部顺序
  //   [0]=left_top/TOP_LEFT, [1]=left_down/BOTTOM_LEFT,
  //   [2]=right_down/BOTTOM_RIGHT, [3]=right_top/TOP_RIGHT
  std::vector<std::pair<double,double>> vertices_roi_order;
  double left_top_vrf_y  = 0.0;   // TOP_LEFT在VRF坐标系的Y值（IsParkLotInRightSide）
  double right_top_vrf_y = 0.0;   // TOP_RIGHT在VRF坐标系的Y值
  // InnerRoi 各阶段顶点（ENU）
  std::vector<std::pair<double,double>> inner_roi_pre_slack_enu;   // CalculateInnerRoi后（松弛前）
  std::vector<std::pair<double,double>> inner_roi_post_slack_enu;  // RoiBoundarySlack后（ENU）
  std::vector<std::pair<double,double>> outer_roi_enu;             // CalculateOuterRoi后（ENU，0.5m外扩）
  std::vector<std::pair<double,double>> fsp_aabb_enu;              // FSP感知AABB四角点（ENU）
  std::vector<std::pair<double,double>> inner_roi_local;           // TransRoiAndSetBoundary后（局部坐标）
  // inner_roi 边界线段（ENU 坐标，N-1 条边，通道入口边不闭合）
  // 用于 AddVirtualObs 传入，作为虚拟障碍物参与 ValidityCheck 碰撞检测
  std::vector<common::math::LineSegment2d> inner_roi_boundary_enu;
  // ROI 外境约束线段（ENU，inner_roi 0.5m 外扩 outer_roi 的边界）
  // 车身距 outer_roi 线段 >= 0.5m （kMinSafeDistance）等价车身必须在 inner_roi 内部
  // 已排除通道入口边（TransRoiAndSetBoundary 不构建最后一条闭合边）
  std::vector<std::pair<common::math::LineSegment2d, double>> roi_wall_segments;
  std::vector<std::pair<common::math::LineSegment2d, double>> virtual_obs_segments;
};  // struct RoiDeciderOutput

// ============================================================
// OpenSpaceRoiDecider 类
// 独立编译改造：移除 Decider 基类
// ============================================================
class OpenSpaceRoiDecider {
 public:
  /**
   * @brief 独立编译构造函数
   * @param vehicle_params 车辆参数
   * @param config ROI决策配置
   */
  OpenSpaceRoiDecider(const VehicleParam& vehicle_params,
                      const RoiDeciderConfig& config);

  ~OpenSpaceRoiDecider() = default;

  /**
   * @brief 独立编译入口函数（替代原始 Process(Frame*)）
   * @param parking_lot 目标车位
   * @param vehicle_state 车辆状态
   * @param output 输出结果
   * @return 0=成功, <0=失败
   */
  int Process(const TL::perception::ParkingLotOut& parking_lot,
              const TL::common::VehicleState& vehicle_state,
              RoiDeciderOutput* output);

  /**
   * @brief P1改造：带障碍物输入的 Process 重载（doc 159）
   * @param obstacle_input 外部预计算的障碍物线段
   */
  int Process(const TL::perception::ParkingLotOut& parking_lot,
              const TL::common::VehicleState& vehicle_state,
              const ObstacleInput& obstacle_input,
              RoiDeciderOutput* output);

  /**
   * @brief P4改造：外部设置上一帧终点位姿（替代 GetPreEndPose via frame_history）
   * 调用方在每次规划成功后调用此方法更新历史终点
   * @param pose 上一次规划成功的终点位姿（ENU坐标）
   */
  void SetLastEndPose(const common::PathPoint& pose) {
    last_end_pose_ = pose;
    has_last_end_pose_ = true;
  }

  /**
   * @brief 获取上一帧终点位姿（P4 GetPreEndPose 独立版）
   * @return 上一帧终点，如无历史则返回空 PathPoint
   */
  common::PathPoint GetLastEndPose() const {
    return has_last_end_pose_ ? last_end_pose_ : common::PathPoint{};
  }

  // ============================================================
  // 核心计算方法（保持原始签名，仅修改Framework依赖）
  // ============================================================

  /**
   * @brief 车位形状校验
   * 原始: L99-157 open_space_roi_decider.cc
   */
  void CheckReceiveParkinglot(
      const perception::ParkingLotOut::ParkType& park_type,
      const ParkingLotVertexType& parking_spot_enu,
      ParkLotStatus* park_lot_status_ptr);

  /**
   * @brief 判断车位在车辆右侧
   * 原始: L1420-1429
   * 独立编译改造：使用 vehicle_state_ 替代 init_adc_point_
   */
  bool IsParkLotInRightSide(const Vec2d& left_top_enu,
                            const Vec2d& right_top_enu);

  /**
   * @brief 设置坐标原点
   * 原始: L161-170 (.h inline)
   */
  static void SetOrigin(const ParkingLotVertexType& vertices,
                        common::math::Vec2d* origin_point,
                        double* const origin_heading) {
    if (nullptr == origin_point || nullptr == origin_heading) {
      return;
    }
    *origin_point = vertices[0];
    *origin_heading = (vertices[3] - vertices[0]).Angle();
  }

  /**
   * @brief 垂直/斜向泊入终点计算
   * 原始: L1617-1700
   * 独立编译改造：FLAGS_avp_enable_parking_inwards → config_
   *              set_x/set_y/set_theta → 直接赋值
   */
  void SetNonLateralSlotEndPose(
      const perception::ParkingLotOut::ParkType& park_type,
      const Vec2d& left_top, const Vec2d& left_down,
      const Vec2d& right_top, const Vec2d& right_down,
      const Vec2d& stop_left, const Vec2d& stop_right,
      bool is_right_side, common::PathPoint* end_pose_ptr,
      bool* is_parking_inwards_ptr);

  /**
   * @brief 泊入端点调度
   * 原始: L1577-1615
   */
  void SetParkingSpotEndPose(
      const perception::ParkingLotOut::ParkType& park_type,
      const ParkingLotVertexType& vertices, bool is_right_side,
      const std::tuple<bool, Vec2d, Vec2d>& wheel_mask,
      common::PathPoint* end_pose_ptr, bool* is_parking_inwards);

  /**
   * @brief 侧方泊入终点计算
   * 原始: L1702-1766
   */
  void SetLateralSlotEndPose(bool is_right_side, const Vec2d& left_top,
                             const Vec2d& left_down, const Vec2d& right_top,
                             const Vec2d& right_down, const Vec2d& stop_left,
                             const Vec2d& stop_right,
                             common::PathPoint* end_pose_ptr);

  /**
   * @brief 内部ROI多边形计算
   * 原始: L1831-1946
   * 独立编译改造：vehicle_state_.x() → vehicle_state_.x
   *              config_.open_space_roi_decider_config() → config_
   */
  void CalculateInnerRoi(const perception::ParkingLotOut::ParkType& park_type,
                         const ParkingLotVertexType& spot_vertices,
                         bool is_use_larger_roi,
                         std::vector<Vec2d>* inner_roi);

  /**
   * @brief 外部ROI (0.5m外扩)
   * 原始: L1991-2021 (static)
   */
  static void CalculateOuterRoi(const std::vector<Vec2d>& inner_roi,
                                std::vector<Vec2d>* outer_roi);

  /**
   * @brief 坐标变换 + AABB边界
   * 原始: L2194-2218 (static)
   */
  static void TransRoiAndSetBoundary(
      const Vec2d& origin_point, double origin_heading,
      std::vector<Vec2d>* inner_roi_vertex,
      std::vector<Vec2d>* outer_roi_vertex,
      std::vector<common::math::LineSegment2d>* inner_roi_boundary,
      std::vector<common::math::LineSegment2d>* outer_roi_boundary,
      std::vector<double>* roi_xy_boundary);

  /**
   * @brief ROI边界编排函数
   * 原始: L2173-2193
   * 独立编译改造：移除 Frame* 参数
   */
  void GetParkingBoundary(
      const Vec2d& origin_point, double origin_heading,
      const perception::ParkingLotOut::ParkType& park_type,
      const ParkingLotVertexType& vertices,
      const common::PathPoint& end_pose_enu,
      std::vector<common::math::LineSegment2d>* inner_roi_boundary,
      std::vector<common::math::LineSegment2d>* outer_roi_boundary,
      std::vector<double>* roi_xy_boundary,
      RoiDeciderOutput* vis_output = nullptr);

  /**
   * @brief 车辆是否在ROI内
   * 原始: L2024-2041
   */
  bool IsVehicleInRoi(const Vec2d& origin_point, double origin_heading,
                      const std::vector<double>& roi_xy_boundary);

  /**
   * @brief 泊入目标区域计算
   * 原始: L2573-2593
   */
  void CalculateParkingInRegion(
      const perception::ParkingLotOut::ParkType& park_type,
      const common::PathPoint& end_pose_enu,
      DestRegionWithAng* dest_region_with_angle);

  /**
   * @brief 目标区域调度
   * 原始: L2486-2570
   * 独立编译改造：仅保留PARKING_IN分支
   */
  void CaculateDestRegion(const common::PathPoint& end_pose_enu,
                          const perception::ParkingLotOut::ParkType& park_type,
                          const ParkingLotVertexType& vertices,
                          DestRegionWithAng* dest_region_with_angle);

  /**
   * @brief 场景类型决策
   * 原始: L2665-2793
   * 独立编译改造：仅保留PARKING_IN分支
   */
  void ParkingScenarioTypeDecision(
      bool is_right_side,
      const perception::ParkingLotOut::ParkType& park_type,
      ParkingScenarioType* parking_scenario_type_ptr);

  /**
   * @brief ROI边界松弛
   * 原始: L2220-2297
   * 独立编译改造：简化 VehicleConfigHelper::GetBoundingBox 调用
   */
  void RoiBoundarySlack(const common::PathPoint& end_pose_enu,
                        bool is_use_larger_roi,
                        std::vector<Vec2d>* inner_roi);

  // ============================================================
  // 辅助静态方法
  // ============================================================

  static double CalculateAdcHeight(
      const common::PathPoint& adc_point,
      const common::math::LineSegment2d& left_to_right_bottom);

  static double CalculateAdcDis2ParkBottom(
      const common::PathPoint& adc_point,
      const common::math::LineSegment2d& left_to_right_bottom);

  bool IsUseLargerRoi(const ParkingLotVertexType& spot_vertices);

  /**
   * @brief P4改造：车位内FreeSpace几何检测（独立版，替代 IsSlotInnerFsValid + FreeSpaceOutArray）
   * 原始：依赖 has_valid_history_path_ + is_high_quality_triggered + FreeSpace
   * 独立版：仅基于车辆到车位入口的距离检测
   * @param vertices 车位4角点
   * @param is_parking_inwards 是否车头入库
   * @return true = 车辆已进入车位内域（距车位入口 < use_slot_inner_fs_radius）
   */
  bool IsSlotInnerFsGeometric(const ParkingLotVertexType& vertices,
                               bool is_parking_inwards) const;

  // ============================================================
  // 从 ParkingLotOut 提取4角点
  // ============================================================
  static ParkingLotVertexType ExtractVertices(
      const TL::perception::ParkingLotOut& parking_lot,
      const Vec2d& vehicle_pos);

 private:
  VehicleParam vehicle_params_;
  TL::common::VehicleState vehicle_state_;
  RoiDeciderConfig config_;

  // 运行时状态
  common::PathPoint init_adc_point_;
  bool is_use_larger_roi_ = false;
  double road_width_ = 6.0;
  std::vector<double> last_slack_dist_vec_;

  // 独立编译改造：简化为泊入场景(PARKING_IN)
  // 原始代码中的 parking_type_ = AVPStatus::ParkingType 枚举
  // 此处固定为 PARKING_IN
  int parking_type_mode_ = 0;  // 0=PARKING_IN (默认)

  // P1改造：障碍物处理模块（doc 159）
  std::shared_ptr<OpenSpaceObstacle> open_space_obstacle_;
  ObstacleConfig obstacle_config_;

  // P2改造：终点微调模块（doc 159）
  std::shared_ptr<OpenSpaceFineTuning> open_space_fine_tuning_;
  FineTuningConfig fine_tuning_config_;  // 默认参数，可通过构造器覆盖

  // P4改造：历史终点状态（doc 159，替代 GetPreEndPose via frame_history）
  common::PathPoint last_end_pose_;     // 上一帧规划终点（调用方通过 SetLastEndPose 更新）
  bool has_last_end_pose_ = false;      // 是否有历史终点

  // 独立编译改造：移除以下原始成员（保留注释用于追溯）
  // - std::shared_ptr<DependencyInjector> injector_;
  // - SensorConfigState sensor_config_state_;
  // - AVPStatus::ParkingType parking_type_;
  // - std::unordered_map<uint32_t, ParkLotInfo> park_lot_map_;
  // - 其余 Framework 相关状态变量
};

}  // namespace planning
}  // namespace TL
