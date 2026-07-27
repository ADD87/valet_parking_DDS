/**
 * @file planning_internal_convert.h
 * @brief 独立编译改造：将 planning_internal.proto 转换为 C++ 结构体
 * @description 用于替代 proto/planning/planning_internal.pb.h
 *              包含规划内部调试、路径更新状态等核心结构
 */

#pragma once

#include <string>
#include <vector>
#include "proto_convert/pnc_point_convert.h"

namespace TL {
namespace planning_internal {

// ========== PathUpdateStatus 相关枚举和结构 ==========

/**
 * @brief 路径更新状态枚举
 * 原 Proto: message PathUpdateStatus.enum UpdateStatus
 */
enum class UpdateStatus : int32_t {
  NONE = 0,
  SUCCESS = 1,
  PATH_SEGMENT_OVER_LIMIT = 2,
  SEARCH_FAILED = 4,
  OPTIMIZE_FAILED = 5,
  START_POINT_MISMATCH = 6,
  WAIT_RESULT = 7,
  OVER_TIME = 100
};

/**
 * @brief 路径类型枚举
 * 原 Proto: message PathUpdateStatus.enum PathType
 */
enum class PathType : int32_t {
  DEFAULT = 0,
  SEARCH_PATH = 1,              // Hybrid A* 搜索路径
  TRACE_PATH = 10,              // 跟踪路径
  GEOMETRY = 20,                // 几何路径
  GEOMETRY_ADJUST = 30,         // 几何调整路径
  CRUISE_PATH = 40,             // 巡航路径
  SCS_GEOMETRY = 50,            // SCS 几何路径
  ILQR_PATH = 60,               // iLQR 路径
  SEARCH_EXTENSION_PATH = 70    // 搜索扩展路径
};

/**
 * @brief 路径更新状态信息
 * 原 Proto: message PathUpdateStatus
 */
struct PathUpdateStatus {
  UpdateStatus update_status = UpdateStatus::NONE;
  uint32_t path_gear_shift_time = 0;
  PathType path_type = PathType::DEFAULT;

  PathUpdateStatus() = default;
};

// ========== ExpansionInfo 结构（用于 Hybrid A* 扩展信息） ==========

/**
 * @brief 节点扩展信息（用于路径搜索调试）
 * 独立编译改造：新增结构，用于记录节点扩展过程
 */
struct ExpansionInfo {
  TL::common::PathPoint point;
  double cost = 0.0;
  int parent_index = -1;

  ExpansionInfo() = default;
  explicit ExpansionInfo(const TL::common::PathPoint& p) : point(p) {}
};

// ========== RefPoint 和 RefLine（参考线相关） ==========

/**
 * @brief 参考点
 * 原 Proto: message RefPoint
 */
struct RefPoint {
  double x = 0.0;
  double y = 0.0;
  double theta = 0.0;
  double kappa = 0.0;
  double dkappa = 0.0;

  RefPoint() = default;
};

/**
 * @brief 参考线
 * 原 Proto: message RefLine
 */
struct RefLine {
  std::vector<RefPoint> ref_points;
  double interval = 0.0;

  RefLine() = default;
};

// ========== 工具函数 ==========

/**
 * @brief UpdateStatus 枚举转字符串
 */
inline std::string UpdateStatusToString(UpdateStatus status) {
  switch (status) {
    case UpdateStatus::NONE: return "NONE";
    case UpdateStatus::SUCCESS: return "SUCCESS";
    case UpdateStatus::PATH_SEGMENT_OVER_LIMIT: return "PATH_SEGMENT_OVER_LIMIT";
    case UpdateStatus::SEARCH_FAILED: return "SEARCH_FAILED";
    case UpdateStatus::OPTIMIZE_FAILED: return "OPTIMIZE_FAILED";
    case UpdateStatus::START_POINT_MISMATCH: return "START_POINT_MISMATCH";
    case UpdateStatus::WAIT_RESULT: return "WAIT_RESULT";
    case UpdateStatus::OVER_TIME: return "OVER_TIME";
    default: return "UNKNOWN";
  }
}

/**
 * @brief PathType 枚举转字符串
 */
inline std::string PathTypeToString(PathType type) {
  switch (type) {
    case PathType::DEFAULT: return "DEFAULT";
    case PathType::SEARCH_PATH: return "SEARCH_PATH";
    case PathType::TRACE_PATH: return "TRACE_PATH";
    case PathType::GEOMETRY: return "GEOMETRY";
    case PathType::GEOMETRY_ADJUST: return "GEOMETRY_ADJUST";
    case PathType::CRUISE_PATH: return "CRUISE_PATH";
    case PathType::SCS_GEOMETRY: return "SCS_GEOMETRY";
    case PathType::ILQR_PATH: return "ILQR_PATH";
    case PathType::SEARCH_EXTENSION_PATH: return "SEARCH_EXTENSION_PATH";
    default: return "UNKNOWN";
  }
}

// ========== OpenSpaceDebug 及 FinishStatus 枚举 ==========

/**
 * @brief 开放空间调试信息（仅包含 PathPartition 所需的 FinishStatus）
 * 原 Proto: message OpenSpaceDebug
 * 独立编译改造: 仅保留 FinishStatus 枚举，其余调试字段 MVP 阶段不需要
 */
struct OpenSpaceDebug {
  enum FinishStatus {
    UNKNOWN = 0,
    REACH_TARGET = 1,
    COLLISION_FINISH = 2,
    PREFINISH_BRAKING = 3,
    REACH_WHEEL_MASK = 4,
    BLOCK_BY_CURB_IN_SPOT = 5,
    BLOCK_BY_CAR_IN_SPOT = 6,
    OVER_TIME = 7,
    LARGE_ANGLE = 8,
    FAR_AWAY = 9,
    VEHICEL_MOVING = 10,
    OUT_OF_PARK_LOT = 11,
  };

  FinishStatus finish_status = UNKNOWN;
};

// ========== VehicleFollowError 结构体 ==========

/**
 * @brief 车辆跟踪误差
 * 原 Proto: message VehicleFollowError
 */
struct VehicleFollowError {
  double angle_offset = 0.0;
  double lateral_offset = 0.0;
  double longitudinal_offset = 0.0;

  // 兼容 proto setter 接口
  void set_angle_offset(double v) { angle_offset = v; }
  void set_lateral_offset(double v) { lateral_offset = v; }
  void set_longitudinal_offset(double v) { longitudinal_offset = v; }

  VehicleFollowError() = default;
};

// ========== AvpSpeedPlanCollisionInfo 结构体 ==========

/**
 * @brief 速度规划碰撞信息（PathPartition 中为 READ-ONLY）
 * 原 Proto: message AvpSpeedPlanCollisionInfo
 */
struct AvpSpeedPlanCollisionInfo {
  // 碰撞类型枚举
  // 独立编译改造：从 planning_internal.proto 提取
  enum CollisionType {
    NO_COLLISION = 0,
    STATIC_OBSTACLE_COLLISION = 1,
    MOVING_OBSTACLE_COLLISION = 2,
    FREESPACE_POINT_COLLISION = 3,
    FUSION_COLLISION = 4,
  };

  // 速度任务交互阶段枚举
  enum SpeedTaskInteractiveStage {
    INIT = 0,
    RUNNING = 1,
    WAITOBSTACLE = 2,
    WAITREPLAN = 3,
  };

  SpeedTaskInteractiveStage stage = INIT;
  CollisionType collision_type = NO_COLLISION;
  double collision_distance = 1e9;
  bool is_use_middle_buffer = false;
  bool is_stop_near_wheel_mask = false;
  bool is_wheel_mask_valid = false;
  int speed_task_inter_stage = 0;
  int static_obstacle_id = 0;
  int moving_obstacle_id = 0;
  int bigger_buffer_safe_count = 0;
  int path_collision_risk_count = 0;

  // Proto兼容setter接口
  void set_stage(SpeedTaskInteractiveStage s) { stage = s; }
  void set_collision_type(CollisionType t) { collision_type = t; }
  void set_collision_distance(double d) { collision_distance = d; }
  void set_is_use_middle_buffer(bool v) { is_use_middle_buffer = v; }
  void set_is_stop_near_wheel_mask(bool v) { is_stop_near_wheel_mask = v; }

  AvpSpeedPlanCollisionInfo() = default;
};

/**
 * @brief SpeedTaskInteractiveStage 名称查询
 * 原 Proto: AvpSpeedPlanCollisionInfo::SpeedTaskInteractiveStage_Name
 */
inline std::string SpeedTaskInteractiveStage_Name(
    AvpSpeedPlanCollisionInfo::SpeedTaskInteractiveStage stage) {
  switch (stage) {
    case AvpSpeedPlanCollisionInfo::INIT: return "INIT";
    case AvpSpeedPlanCollisionInfo::RUNNING: return "RUNNING";
    case AvpSpeedPlanCollisionInfo::WAITOBSTACLE: return "WAITOBSTACLE";
    case AvpSpeedPlanCollisionInfo::WAITREPLAN: return "WAITREPLAN";
    default: return "UNKNOWN";
  }
}

// ========== SpeedLimitPoint 结构体 ==========

/**
 * @brief 速度限制点（用于STSampleDebug）
 * 独立编译改造：从 planning_internal.proto 提取
 */
struct SpeedLimitPoint {
  double s = 0.0;
  double limit_v = 0.0;

  SpeedLimitPoint() = default;
  SpeedLimitPoint(double _s, double _v) : s(_s), limit_v(_v) {}

  // Proto兼容setter接口
  void set_s(double v) { s = v; }
  void set_limit_v(double v) { limit_v = v; }
};

// ========== STSampleDebug 结构体 ==========

/**
 * @brief ST采样调试信息
 * 原 Proto: message STSampleDebug
 * 独立编译改造：从 planning_internal.proto 提取
 */
struct STSampleDebug {
  double start_v = 0.0;
  double start_acc = 0.0;
  double total_s = 0.0;
  double end_s = 0.0;
  double actual_v = 0.0;
  double actual_acc = 0.0;
  std::vector<SpeedLimitPoint> speed_limit_points;
  std::string best_curve;
  std::string last_curve;
  std::string message;

  // Proto兼容setter接口
  void set_start_v(double v) { start_v = v; }
  void set_start_acc(double v) { start_acc = v; }
  void set_total_s(double v) { total_s = v; }
  void set_end_s(double v) { end_s = v; }
  void set_actual_v(double v) { actual_v = v; }
  void set_actual_acc(double v) { actual_acc = v; }
  void set_best_curve(const std::string& s) { best_curve = s; }
  void set_last_curve(const std::string& s) { last_curve = s; }
  void set_message(const std::string& s) { message = s; }

  std::vector<SpeedLimitPoint>* mutable_speed_limit_points() {
    return &speed_limit_points;
  }

  SpeedLimitPoint* add_speed_limit_points() {
    speed_limit_points.emplace_back();
    return &speed_limit_points.back();
  }

  void Clear() {
    start_v = 0.0;
    start_acc = 0.0;
    total_s = 0.0;
    end_s = 0.0;
    actual_v = 0.0;
    actual_acc = 0.0;
    speed_limit_points.clear();
    best_curve.clear();
    last_curve.clear();
    message.clear();
  }

  STSampleDebug() = default;
};

// ========== PathUpdateStatus 兼容接口 ==========
// 注: PathUpdateStatus 已在上方定义，此处为 PartitionedPath 中使用的类型别名
// 注: PathType 作为嵌套类型也可通过 PathUpdateStatus::PathType 访问
// 为兼容原代码中的 planning_internal::PathUpdateStatus::PathType 用法：
// 已在上方 PathType enum 中定义

}  // namespace planning_internal
}  // namespace TL
