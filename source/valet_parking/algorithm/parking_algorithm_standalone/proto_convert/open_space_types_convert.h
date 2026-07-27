/**
 * @file open_space_types_convert.h
 * @brief 独立编译改造：开放空间规划相关类型定义
 * @description 简化版的 open_space_info.h 中的核心结构，避免重依赖
 *              包含 OpenSpacePathInput/Output 等 path_generation 所需类型
 */

#pragma once

#include <vector>
#include <sstream>
#include <string>
#include <tuple>
#include <limits>
#include <cmath>
#include "proto_convert/pnc_point_convert.h"
#include "proto_convert/planning_internal_convert.h"  // 独立编译改造：PathUpdateStatus
#include "proto_convert/chassis_convert.h"            // 独立编译改造：GearPosition
#include "common/math/vec2d.h"
#include "common/math/polygon2d.h"
#include "common/math/line_segment2d.h"
#include "planning/common/path/discretized_path.h"    // 独立编译改造：TraceAdjustSearchStrategy需要值类型
#include "planning/common/open_space_info_lite.h"       // 独立编译改造：ParkingScenarioType, ScenarioDiffcultyType, OpenSpaceEnvStructuredInfo等
#include "planning/open_space/coarse_path_generator/path_generator_result.h"  // PathGeneratorResult（独立头文件）

namespace TL {
namespace planning {

// ========== 枚举定义 ==========

enum SpaceStructure {
  DEFAULT = 0,
  VER_PARK_LOT = 1,
  LAT_PARK_LOT = 2,
};

enum UseGeometry {
  NOT_USE = 0,
  ONLY_USE = 1,
  USE_FIRST = 2,
  USE_LAST = 3,
  USE_FIRST_LAST = 4,
  USE_BOTH = 5,
};

// 独立编译改造：从 open_space_info.h 补充的枚举
enum GeometryConnectionType {
  DEFAULT_GEOMETRY_TYPE = 0,
  CYCLE_STRAIGHT = 1,
  STRAIGHT_CYCLE = 2,
  BOTH_TYPE = 3,
};

enum UseGeometryPurpose {
  DEFAULTPURPOSE = 0,
  PRECISEPOSE = 1,
  PRECISEANGLE = 2,
};

enum ParkDirection {
  NODIRECTION = 0,
  PARKIN = 1,
  LEFTPARKOUT = 2,
  RIGHTPARKOUT = 3,
  FORWARDPARKOUT = 4,
};

// ScenarioDiffcultyType 已移至 open_space_info_lite.h，避免 ODR 冲突

// ========== ReferencePoint（简化版，用于坐标变换） ==========

/**
 * @brief 参考点（简化版，支持 getter/setter 接口）
 * 独立编译改造：替代 map/hdmap/path.h 中的 ReferencePoint
 */
struct ReferencePoint {
  double x_ = 0.0;
  double y_ = 0.0;
  double heading_ = 0.0;
  double kappa_ = 0.0;
  double dkappa_ = 0.0;
  double s_ = 0.0;

  // Getter methods (兼容原代码的 getter 风格)
  double x() const { return x_; }
  double y() const { return y_; }
  double heading() const { return heading_; }
  double theta() const { return heading_; }
  double kappa() const { return kappa_; }
  double s() const { return s_; }

  // Setter methods (兼容原代码的 setter 风格)
  void set_x(double v) { x_ = v; }
  void set_y(double v) { y_ = v; }
  void set_heading(double v) { heading_ = v; }
  void set_theta(double v) { heading_ = v; }

  ReferencePoint() = default;
};

// ========== ReferenceLine（简化版） ==========

/**
 * @brief 参考线（简化版，支持 getter/setter 接口）
 * 独立编译改造：替代 map/hdmap/path.h 中的 ReferenceLine
 */
struct ReferenceLine {
 private:
  std::vector<ReferencePoint> points_;
  double interval_ = 0.1;

 public:
  // 兼容 generator.cc 中的 reference_line.reference_points() 调用
  const std::vector<ReferencePoint>& reference_points() const { return points_; }

  // 兼容 generator.cc 中的 SetReferencePoints / GetInterval 调用
  void SetReferencePoints(std::vector<ReferencePoint>&& points, double interval) {
    points_ = std::move(points);
    interval_ = interval;
  }
  double GetInterval() const { return interval_; }

  /**
   * @brief Convert XY coordinates to SL (Frenet) coordinates
   */
  template<typename Vec2dType>
  bool XYToSL(const Vec2dType& xy, common::SLPoint* sl_point) const {
    if (points_.empty() || !sl_point) return false;

    double x = xy.x();
    double y = xy.y();

    double min_dist = std::numeric_limits<double>::max();
    size_t closest_idx = 0;

    for (size_t i = 0; i < points_.size(); ++i) {
      double dx = points_[i].x() - x;
      double dy = points_[i].y() - y;
      double dist = std::sqrt(dx * dx + dy * dy);
      if (dist < min_dist) {
        min_dist = dist;
        closest_idx = i;
      }
    }

    sl_point->s = points_[closest_idx].s();

    if (closest_idx > 0 && closest_idx < points_.size() - 1) {
      double dx_path = points_[closest_idx + 1].x() - points_[closest_idx - 1].x();
      double dy_path = points_[closest_idx + 1].y() - points_[closest_idx - 1].y();
      double path_len = std::sqrt(dx_path * dx_path + dy_path * dy_path);

      if (path_len > 1e-6) {
        double dx_to_point = x - points_[closest_idx].x();
        double dy_to_point = y - points_[closest_idx].y();
        sl_point->l = (dx_to_point * dy_path - dy_to_point * dx_path) / path_len;
      } else {
        sl_point->l = min_dist;
      }
    } else {
      sl_point->l = min_dist;
    }

    return true;
  }

  ReferenceLine() = default;
};

// ========== PathGearPair / DestRegionWithAng ==========
// 注意：这些 using 也在 path_generator.h 中定义，两处完全一致，不违反 ODR

// PathGearPair / DestRegionWithAng 已在 open_space_info_lite.h 中定义，此处不再重复定义

// ========== 策略结构 ==========

/**
 * @brief 几何策略
 * 独立编译改造：增加 consider_kappa_diff 字段，与 open_space_info.h 完整版对齐
 */
struct GeometryStrategy {
  UseGeometry use_geometry = UseGeometry::NOT_USE;
  std::vector<std::pair<double, double>> longitudal_bound;
  // 独立编译改造：使用 int 向量以避免对 GeometryConnectionType/UseGeometryPurpose 枚举的强依赖
  // 在 GenerateCoarsePath 的 SeparateGeometryStrategy 中按 int 值比较
  std::vector<int> geometry_path_type;
  std::vector<int> use_purpose;
  bool consider_kappa_diff = false;  // 独立编译改造：新增，对齐完整版
};

/**
 * @brief 无碰撞搜索策略
 * 独立编译改造：增加 collision_path_point 字段
 */
struct CollisionFreeSearchStrategy {
  bool replan_due_to_collision = false;
  double collision_free_dist = 0.0;
  common::PathPoint collision_path_point;  // 独立编译改造：新增，用于坐标变换
};

/**
 * @brief 轨迹调整搜索策略
 * 独立编译改造：trace_path 从 DiscretizedPath* 改为 DiscretizedPath 值类型
 */
struct TraceAdjustSearchStrategy {
  bool is_trace_adjust = false;
  DiscretizedPath trace_path;  // 独立编译改造：值类型（原为指针，与 open_space_info.h 对齐）
  std::vector<double> xy_bounds;
  double target_s = 0.0;
  double finish_l_threshold = 0.0;
  double finish_theta_threshold = 0.0;
};

/**
 * @brief 路径搜索策略
 * 独立编译改造：包含 Hybrid A* 和 GenerateCoarsePath 所需的全部策略字段
 */
struct PathSearchStrategy {
  int init_path_direction = 0;
  bool limit_init_steer_margin = false;
  bool is_plan_from_start = true;
  int cut_off_strategy = 0;
  SpaceStructure space_structure = SpaceStructure::DEFAULT;
  ParkDirection park_direction = ParkDirection::NODIRECTION;
  bool use_larger_curvature = false;
  bool enable_init_kappa_cost = false;
  GeometryStrategy use_geometry_strategy;
  CollisionFreeSearchStrategy collision_free_search_strategy;
  bool is_dead_end_scenario = false;
  bool is_narrow_passage_scenario = false;
  TraceAdjustSearchStrategy trace_adjust_search_strategy;
  bool is_nns_adjust_senario = false;
  ReferenceLine reference_line;

  std::string DebugString() const {
    std::ostringstream oss;
    oss << "PathSearchStrategy{";
    oss << "init_path_direction=" << init_path_direction << ", ";
    oss << "limit_init_steer=" << (limit_init_steer_margin ? "true" : "false") << ", ";
    oss << "plan_from_start=" << (is_plan_from_start ? "true" : "false") << ", ";
    oss << "cut_off_strategy=" << cut_off_strategy << ", ";
    oss << "space_structure=" << static_cast<int>(space_structure) << ", ";
    oss << "park_direction=" << static_cast<int>(park_direction) << ", ";
    oss << "use_larger_curvature=" << (use_larger_curvature ? "true" : "false") << ", ";
    oss << "is_dead_end=" << (is_dead_end_scenario ? "true" : "false") << ", ";
    oss << "is_narrow_passage=" << (is_narrow_passage_scenario ? "true" : "false") << ", ";
    oss << "is_nns_adjust=" << (is_nns_adjust_senario ? "true" : "false");
    oss << "}";
    return oss.str();
  }

  void Reset() {
    init_path_direction = 0;
    limit_init_steer_margin = false;
    is_plan_from_start = true;
    cut_off_strategy = 0;
    space_structure = SpaceStructure::DEFAULT;
    use_larger_curvature = false;
    use_geometry_strategy.use_geometry = UseGeometry::NOT_USE;
    use_geometry_strategy.longitudal_bound.clear();
    use_geometry_strategy.geometry_path_type.clear();
    use_geometry_strategy.use_purpose.clear();
    collision_free_search_strategy.replan_due_to_collision = false;
    collision_free_search_strategy.collision_free_dist = 0.0;
    trace_adjust_search_strategy.is_trace_adjust = false;
    is_nns_adjust_senario = false;
    reference_line = ReferenceLine();
  }

  PathSearchStrategy() = default;
};

// ========== PathStrategy ==========
/**
 * @brief 路径策略（包含粗路径搜索策略 + 平滑约束）
 * 独立编译改造：从 open_space_info.h 提取
 */
struct PathStrategy {
  PathSearchStrategy path_search_strategy;
  int init_moving_direction{0};  // 1: forward; 0: stop; -1: backward
  bool disable_search{false};
};

// ========== OpenSpaceEnvStructuredInfo ==========
// ParkingScenarioType, OpenSpaceEnvStructuredInfo 已移至 open_space_info_lite.h，避免 ODR 冲突

// ========== OpenSpacePathInput / OpenSpacePathOutput ==========
/**
 * @brief 开放空间路径生成输入
 * 独立编译改造：从 open_space_info.h 提取，用于 OpenSpacePathGenerator::Plan()
 */
struct OpenSpacePathInput {
  int path_id{-1};
  uint32_t replan_status = 0;
  double rotate_angle{0.0};
  common::math::Vec2d translate_origin;
  common::PathPoint start_point;
  common::PathPoint end_pose;
  std::vector<double> xy_bounds;
  bool xy_bounds_is_local{false};  // true: xy_bounds已在局部坐标系(ROI Decider), 跳过二次变换
  std::vector<std::pair<common::math::LineSegment2d, double>> obstacles_segments_vec;
  DiscretizedPath warm_start_path;  // MVP-A: 默认空路径
  DestRegionWithAng dest_region_with_angle;
  PathStrategy path_strategy;
};

/**
 * @brief 开放空间路径生成输出
 * 独立编译改造：从 open_space_info.h 提取
 */
struct OpenSpacePathOutput {
  uint32_t replan_status = 0;
  bool need_collision_free_smooth{true};
  bool has_smoothed{false};
  std::string error_msg;
  std::vector<PathGearPair> partitioned_path;
  int path_type = 0;  // 独立编译改造：使用 int 代替 PathUpdateStatus::PathType
  PathGeneratorResult coarse_path_result;  // 独立编译改造：保存 Hybrid A* 原始结果（用于可视化，全局坐标）

  // ========== 局部坐标系数据（用于可视化调试）==========
  // TransInputToLocalFrame 之后的坐标系参数及各要素局部坐标
  double local_rotate_angle{0.0};                       // 坐标变换旋转角（= 车位朝向角）
  common::math::Vec2d local_translate_origin;           // 坐标变换平移原点（= 车位终点后轴中心）
  common::PathPoint start_point_local;                  // 局部坐标系内起点
  common::PathPoint end_point_local;                    // 局部坐标系内终点
  std::vector<double> xy_bounds_local;                  // 局部坐标系内 ROI 边界
  PathGeneratorResult coarse_path_local;                // Hybrid A* 原始路径（局部坐标，运行过程中）
  std::vector<std::pair<common::math::LineSegment2d, double>> obstacles_local;  // 局部坐标系内障碍物

  void Reset() {
    need_collision_free_smooth = true;
    has_smoothed = false;
    error_msg.clear();
    partitioned_path.clear();
    path_type = 0;
    coarse_path_result.reset();
    local_rotate_angle = 0.0;
    local_translate_origin = common::math::Vec2d(0.0, 0.0);
    start_point_local = common::PathPoint{};
    end_point_local = common::PathPoint{};
    xy_bounds_local.clear();
    coarse_path_local.reset();
    obstacles_local.clear();
  }
};

}  // namespace planning
}  // namespace TL
