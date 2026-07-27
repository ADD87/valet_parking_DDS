/**
 * @file speed_optimizer_types.h
 * @brief 独立编译改造：速度优化器输入/输出数据结构
 * @description 替代原 Frame, OpenSpaceInfo, DependencyInjector 指针依赖
 *              参考 open_space_path_partition 的 PartitionInput/PartitionOutput 模式
 * @date 2025-07-11
 */
#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "common/math/box2d.h"
#include "common/math/line_segment2d.h"
#include "planning/common/open_space_info_lite.h"
#include "planning/common/path/discretized_path.h"
#include "planning/common/speed/speed_data.h"
#include "planning/common/trajectory/discretized_trajectory.h"
#include "proto_convert/open_space_speed_optimizer_config_convert.h"
#include "proto_convert/planning_internal_convert.h"
#include "proto_convert/pnc_point_convert.h"
#include "proto_convert/vehicle_state_convert.h"

namespace TL {
namespace planning {

// 独立编译改造：TrajGearPair 类型定义
// 原定义位于 open_space_info.h，移至此处避免引入 Frame 依赖
using TrajGearPair = std::pair<DiscretizedTrajectory, soc::GearPosition>;

// ============================================================================
// 独立编译改造：PerceptionObstacle_Type 枚举
// 原 proto/perception/perception_obstacle.pb.h 中的枚举
// 在独立版本中直接定义，用于 CollisionInfo 记录碰撞障碍物类型
// ============================================================================
enum PerceptionObstacle_Type {
  PerceptionObstacle_Type_UNKNOWN = 0,
  PerceptionObstacle_Type_UNKNOWN_MOVABLE = 1,
  PerceptionObstacle_Type_UNKNOWN_UNMOVABLE = 2,
  PerceptionObstacle_Type_PEDESTRIAN = 3,
  PerceptionObstacle_Type_BICYCLE = 4,
  PerceptionObstacle_Type_VEHICLE = 5,
};

/**
 * @brief 简化静态障碍物 - 替代原 Obstacle 类
 * 独立编译改造：无感知输入，使用手动/测试定义的障碍物
 * 用于速度优化器的碰撞检测和减速决策
 */
struct SimpleStaticObstacle {
  int id = 0;                                  ///< 障碍物ID
  PerceptionObstacle_Type type = PerceptionObstacle_Type_UNKNOWN_UNMOVABLE;
  common::math::Box2d bounding_box{
      common::math::Vec2d(0, 0), 0, 0, 0};    ///< 包围盒

  SimpleStaticObstacle() = default;
  SimpleStaticObstacle(int id_, PerceptionObstacle_Type type_,
                       double cx, double cy, double heading,
                       double length, double width)
      : id(id_), type(type_),
        bounding_box(common::math::Vec2d(cx, cy), heading, length, width) {}
};

      /**
       * @brief 简化动态障碍物 - standalone 最小 moving obstacle 输入
       * 当前仅用于在速度优化器中区分 MOVING_OBSTACLE_COLLISION 分支。
       */
      struct SimpleMovingObstacle {
        int id = 0;
        PerceptionObstacle_Type type = PerceptionObstacle_Type_VEHICLE;
        common::math::Box2d bounding_box{
            common::math::Vec2d(0, 0), 0, 0, 0};
        double velocity_x = 0.0;
        double velocity_y = 0.0;

        SimpleMovingObstacle() = default;
        SimpleMovingObstacle(int id_, PerceptionObstacle_Type type_,
                 double cx, double cy, double heading,
                             double length, double width,
                             double velocity_x_ = 0.0,
                             double velocity_y_ = 0.0)
            : id(id_), type(type_),
              bounding_box(common::math::Vec2d(cx, cy), heading, length, width),
              velocity_x(velocity_x_), velocity_y(velocity_y_) {}
      };

/**
 * @brief 速度优化器输入 - 替代 Frame, OpenSpaceInfo, DependencyInjector 指针
 */
struct SpeedOptimizerInput {
  // ---- 从 Task3 (PartitionOutput) 获取 ----
  DiscretizedPath discretized_path;           // 当前路径段
  PartitionedPath partitioned_paths;          // 完整分段路径信息（含path_type, path_idx等）

  // ---- 当前段属性（由管线从 PartitionedPath 提取） ----
  soc::GearPosition gear = soc::GearPosition::GEAR_DRIVE;  // 当前段档位
  bool is_gear_changed = false;               // 是否发生档位切换
  bool is_stop_path = false;                  // 是否为停车路径段
  common::TrajectoryPoint start_point;        // 当前段规划起点

  // ---- 车辆状态 ----
  common::VehicleState vehicle_state;
  bool is_vehicle_stand_still = false;

  // ---- 场景环境信息（原自 open_space_info） ----
  OpenSpaceEnvStructuredInfo env_structured_info;
  bool is_consider_wheel_mask = false;
  common::math::Box2d wheel_mask_box{
      common::math::Vec2d(0, 0), 0, 0, 0};   // 默认空box

  // ---- FreeSpace索引（原自 mutable_open_space_info 读取） ----
  std::vector<size_t> under_spot_low_fs_idxs;
  std::vector<size_t> high_curb_fs_idxs;
  std::vector<std::pair<size_t, std::vector<size_t>>> ignore_fs_idxs;
  std::vector<common::PathPoint> spd_limit_points;

  // ---- 模式/场景标志 ----
  bool is_forward = true;
  bool is_mirror_fold = false;
  bool is_rpa_direct_mode = false;

  // ---- 静态障碍物（独立编译改造：替代 Obstacle* 指针） ----
  std::vector<SimpleStaticObstacle> static_obstacles;
  std::vector<SimpleMovingObstacle> moving_obstacles;

  // ---- 速度配置 ----
  OpenSpaceSpeedOptimizerConfig config;

  // ---- 帧历史（用于CalDiffTimeFromLast） ----
  bool has_last_frame = false;
  double last_frame_timestamp = 0.0;
  double last_planning_start_relative_time = 0.0;
};

/**
 * @brief 速度优化器输出 - 替代 OpenSpaceInfo* 写入
 */
struct SpeedOptimizerOutput {
  // ---- 核心输出：速度数据 ----
  SpeedData speed_data;                       // 速度profile
  DiscretizedPath candidate_path;             // 碰撞截断后的候选路径
  TrajGearPair trajectory_gear;               // 最终轨迹+档位对

  // ---- 碰撞/交互状态 ----
  planning_internal::AvpSpeedPlanCollisionInfo speed_plan_collision_info;
  planning_internal::AvpSpeedPlanCollisionInfo::SpeedTaskInteractiveStage interactive_stage =
      planning_internal::AvpSpeedPlanCollisionInfo::INIT;
  bool replan_triggered_by_speed_plan = false;
  bool current_path_has_collision_risk = false;
  common::PathPoint future_collision_point;

  // ---- 调试信息 ----
  planning_internal::STSampleDebug st_debug_info;

  // ---- 执行状态 ----
  bool success = false;
  std::string message;
};

}  // namespace planning
}  // namespace TL
