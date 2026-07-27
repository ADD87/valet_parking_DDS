/*
 * Copyright (c) TL Technologies Co., Ltd. 2023. All rights reserved.
 * Description:  open_space_path_generator.CC
 * 独立编译改造：
 *  - 移除 open_space_info.h 依赖（避免ODR冲突）
 *  - PathPoint getter/setter → 直接成员访问
 *  - common::Clock → 本地 chrono 实现
 *  - debug proto → #ifdef ENABLE_OPENSPACE_DEBUG 保护
 *  - MVP-A: 仅使用 HybridAStar，其余子规划器 nullptr 保护
 */

#include "planning/tasks/optimizers/open_space_path_generation/open_space_path_generator.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <memory>
#include <string>
#include <vector>
#include "common/math/line_segment2d.h"
#include "common/math/math_utils.h"
#include "common/math/polygon2d.h"
#include "common/math/vec2d.h"
#include "common/configs/vehicle_config_helper.h"
#include "common/status/status.h"
#include "planning/common/path/discretized_path.h"
#include "planning/open_space/coarse_path_generator/hybrid_a_star.h"
#include "planning/open_space/hybrid_a_star_config.h"
#include "planning/open_space/vehicle_param.h"
#include "proto_convert/pnc_point_convert.h"
#include "proto_convert/planning_internal_convert.h"

// 独立编译改造 MVP-A：暂不引入子规划器头文件（避免 open_space_info.h 的 ODR 冲突）
// #include "planning/open_space/coarse_path_generator/geometric_path.h"
// #include "planning/open_space/coarse_path_generator/ilqr_path.h"
// #include "planning/open_space/coarse_path_generator/geometry_path_generator.h"

namespace TL {
namespace planning {

using TL::common::ErrorCode;
using TL::common::Status;
using TL::common::math::Vec2d;

// 独立编译改造：本地 Clock 实现（替代 common::Clock::NowInSeconds()）
namespace {
inline double NowInSeconds() {
  return std::chrono::duration<double>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}
constexpr double kEpsilon = 1.0e-3;
}  // namespace

OpenSpacePathGenerator::OpenSpacePathGenerator(const HybridAStarConfig& config,
                                                 const VehicleParam& vehicle_param)
    : config_(config),
      hybrid_a_star_planner_(std::make_unique<HybridAStar>(config_, vehicle_param))
      // 独立编译改造 MVP-A：ILQR/GeometricPath/GeometryPathGenerator 暂不集成
      {}

void OpenSpacePathGenerator::Plan(
    const std::atomic<bool>& atomic_early_stop_flag,
    const OpenSpacePathInput& input, OpenSpacePathOutput* const output) {
  // step 1. check validity of input data
  if (output == nullptr) {
    return;
  }
  output->Reset();
  if (input.xy_bounds.empty()) {
    output->error_msg = "empty xy_bounds";
    output->partitioned_path.clear();
    return;
  }
  output->replan_status = input.replan_status;
  const auto start_timestamp = NowInSeconds();
  auto start_point_local = input.start_point;
  auto end_pose_local = input.end_pose;
  auto dest_region_with_angle_local = input.dest_region_with_angle;
  auto obstacles_segments_vec_local = input.obstacles_segments_vec;
  TransInputToLocalFrame(input.rotate_angle, input.translate_origin,
                         &start_point_local, &end_pose_local,
                         &dest_region_with_angle_local,
                         &obstacles_segments_vec_local);

  // 独立编译改造：将 xy_bounds 从全局坐标变换到局部坐标系
  // xy_bounds = [x_min, x_max, y_min, y_max]（全局VRF）
  // 当 rotate_angle != 0 时，需要对边界矩形四个角点做坐标变换后取包围盒
  // 否则 Hybrid A* 用局部坐标对比全局边界 → ROI失效
  std::vector<double> xy_bounds_local = input.xy_bounds;
  if (input.xy_bounds_is_local) {
    AINFO << "[PARKING] xy_bounds 已是局部坐标系，跳过二次变换: "
          << "x[" << xy_bounds_local[0] << ", " << xy_bounds_local[1] << "] "
          << "y[" << xy_bounds_local[2] << ", " << xy_bounds_local[3] << "]";
  } else if (std::fabs(input.rotate_angle) > kEpsilon ||
      std::fabs(input.translate_origin.x()) > kEpsilon ||
      std::fabs(input.translate_origin.y()) > kEpsilon) {
    const double alpha = input.rotate_angle;
    const double tx = input.translate_origin.x();
    const double ty = input.translate_origin.y();
    const double cos_neg = std::cos(-alpha);
    const double sin_neg = std::sin(-alpha);
    // 四个角点：(x_min/x_max) × (y_min/y_max)
    const double gx[4] = {input.xy_bounds[0], input.xy_bounds[1],
                          input.xy_bounds[0], input.xy_bounds[1]};
    const double gy[4] = {input.xy_bounds[2], input.xy_bounds[2],
                          input.xy_bounds[3], input.xy_bounds[3]};
    double lx_min = 1e9, lx_max = -1e9, ly_min = 1e9, ly_max = -1e9;
    for (int i = 0; i < 4; ++i) {
      const double dx = gx[i] - tx;
      const double dy = gy[i] - ty;
      const double lx = dx * cos_neg - dy * sin_neg;
      const double ly = dx * sin_neg + dy * cos_neg;
      lx_min = std::min(lx_min, lx);
      lx_max = std::max(lx_max, lx);
      ly_min = std::min(ly_min, ly);
      ly_max = std::max(ly_max, ly);
    }
    xy_bounds_local = {lx_min, lx_max, ly_min, ly_max};
    AINFO << "[PARKING] xy_bounds 已转换到局部坐标系: "
          << "x[" << lx_min << ", " << lx_max << "] "
          << "y[" << ly_min << ", " << ly_max << "]";
  }  // end else if
  // 独立编译改造：保存局部坐标系各要素（供可视化调试使用）
  output->local_rotate_angle = input.rotate_angle;
  output->local_translate_origin = input.translate_origin;
  output->start_point_local = start_point_local;
  output->end_point_local = end_pose_local;
  output->obstacles_local = obstacles_segments_vec_local;
  output->xy_bounds_local = xy_bounds_local;
  default_warm_start_path_result_.reset();
  if (!input.warm_start_path.empty()) {
    auto warm_start_path_local = input.warm_start_path;
    for (auto& point : warm_start_path_local) {
      PathPointNormalizing(input.rotate_angle, input.translate_origin, &point);
      // 独立编译改造：PathPoint 直接成员访问
      default_warm_start_path_result_.x.emplace_back(point.x);
      default_warm_start_path_result_.y.emplace_back(point.y);
      default_warm_start_path_result_.phi.emplace_back(point.theta);
    }
    default_warm_start_path_result_.path_type =
        static_cast<int>(planning_internal::PathType::TRACE_PATH);
  }
  output->partitioned_path.clear();
  PathStrategy path_strategy_loc = input.path_strategy;
  if (path_strategy_loc.path_search_strategy.collision_free_search_strategy
          .replan_due_to_collision) {
    common::PathPoint collsion_point_loc =
        path_strategy_loc.path_search_strategy.collision_free_search_strategy
            .collision_path_point;
    PathPointNormalizing(input.rotate_angle, input.translate_origin,
                         &collsion_point_loc);
    path_strategy_loc.path_search_strategy.collision_free_search_strategy
        .collision_path_point = collsion_point_loc;
  }
  if (path_strategy_loc.path_search_strategy.trace_adjust_search_strategy
          .is_trace_adjust) {
    auto& trace_path_local = path_strategy_loc.path_search_strategy
                                 .trace_adjust_search_strategy.trace_path;
    for (auto& point : trace_path_local) {
      PathPointNormalizing(input.rotate_angle, input.translate_origin, &point);
    }
  }
  // trans referenceline to local frame
  auto& reference_line = path_strategy_loc.path_search_strategy.reference_line;
  if (path_strategy_loc.path_search_strategy.is_nns_adjust_senario &&
      !reference_line.reference_points().empty()) {
    std::vector<ReferencePoint> temp_reference_line;
    ReferencePoint temp_path_point;
    for (const auto& reference_point : reference_line.reference_points()) {
      temp_path_point = reference_point;
      ReferencePointNormalizing(input.rotate_angle, input.translate_origin,
                                &temp_path_point);
      temp_reference_line.push_back(temp_path_point);
    }
    reference_line.SetReferencePoints(std::move(temp_reference_line),
                                      reference_line.GetInterval());
  }
  RemoveCollisionVirtualObs(start_point_local, &obstacles_segments_vec_local);
  // step 2.Generate coarse path by hybrid a star or geometry planner
  ADEBUG << "start generate coarse path";
  PathGeneratorResult result;
  output->need_collision_free_smooth = true;
  const double coarse_start_time = NowInSeconds();
  Status status = GenerateCoarsePath(
      atomic_early_stop_flag, start_point_local, end_pose_local,
      xy_bounds_local, obstacles_segments_vec_local,
      dest_region_with_angle_local, path_strategy_loc, &result,
      &(output->need_collision_free_smooth));
  if (status != Status::OK()) {
    output->error_msg = status.error_message();
    return;
  }
  ADEBUG << "result.path_type " << result.path_type;
  output->path_type = result.path_type;
  output->coarse_path_result = result;  // 独立编译改造：保存原始 Hybrid A* 结果（用于可视化）
  output->coarse_path_local = result;    // 独立编译改造：同时保存局部坐标版本（PathDeNormal之前）

  // step 3.Path partition
  if (!PathGenerator::PathPartition(result, &(output->partitioned_path))) {
    output->error_msg = "Hybrid Astar partition failed";
    return;
  }
  PathDeNormal(input.translate_origin, input.rotate_angle,
               &(output->partitioned_path));

  // 独立编译改造：将 coarse_path_result 从局部坐标逆变换回全局坐标
  // （PathDeNormal 只处理 partitioned_path，coarse_path_result 需单独转换）
  {
    const double alpha = input.rotate_angle;
    const double tx = input.translate_origin.x();
    const double ty = input.translate_origin.y();
    auto& cpr = output->coarse_path_result;
    for (size_t i = 0; i < cpr.x.size(); ++i) {
      const double lx = cpr.x[i];
      const double ly = cpr.y[i];
      cpr.x[i] = lx * std::cos(alpha) - ly * std::sin(alpha) + tx;
      cpr.y[i] = lx * std::sin(alpha) + ly * std::cos(alpha) + ty;
      cpr.phi[i] = common::math::NormalizeAngle(cpr.phi[i] + alpha);
    }
  }

  const double coarse_total_time = NowInSeconds() - coarse_start_time;
  AINFO << "[PARKING] open space coarse path total time: "
        << NowInSeconds() - start_timestamp << "s";
}

void OpenSpacePathGenerator::TransInputToLocalFrame(
    const double origin_heading, const common::math::Vec2d& origin_point,
    common::PathPoint* const start_point_ptr,
    common::PathPoint* const end_pose_ptr,
    DestRegionWithAng* const dest_region_with_angle_ptr,
    std::vector<std::pair<common::math::LineSegment2d, double>>* const
        obstacles_segments_vec_ptr) {
  PathPointNormalizing(origin_heading, origin_point, start_point_ptr);
  PathPointNormalizing(origin_heading, origin_point, end_pose_ptr);
  const auto& polygon_enu = std::get<0>(*dest_region_with_angle_ptr);
  const double from_angle = std::get<1>(*dest_region_with_angle_ptr);
  const double to_angle = std::get<2>(*dest_region_with_angle_ptr);
  if (polygon_enu.points().size() > 2) {
    std::vector<Vec2d> polygon_points(polygon_enu.points());
    for (auto& point : polygon_points) {
      point -= origin_point;
      point.SelfRotate(-origin_heading);
    }
    *dest_region_with_angle_ptr = {common::math::Polygon2d(polygon_points),
                                   from_angle - origin_heading,
                                   to_angle - origin_heading};
  }
  for (auto& seg_pair : *obstacles_segments_vec_ptr) {
    seg_pair.first.Transform(origin_point, origin_heading);
  }
}

void OpenSpacePathGenerator::RemoveCollisionVirtualObs(
    const common::PathPoint& start_point,
    std::vector<std::pair<common::math::LineSegment2d, double>>* const
        obstacles_segments_vec_ptr) {
  if (nullptr == obstacles_segments_vec_ptr ||
      obstacles_segments_vec_ptr->empty()) {
    return;
  }
  static constexpr double kEps = 1e-5;
  const auto start_polygon = common::math::Polygon2d(
      common::VehicleConfigHelper::GetBoundingBox(start_point));
  int cnt = 0;
  auto iter = obstacles_segments_vec_ptr->begin();
  while (iter != obstacles_segments_vec_ptr->end()) {
    if (iter->second < kEps && start_polygon.HasOverlap(iter->first)) {
      // virtual obs has overlaps with start_point
      iter = obstacles_segments_vec_ptr->erase(iter);
      ++cnt;
    } else {
      ++iter;
    }
  }
  ADEBUG << "RemoveCollisionVirtualObs cnt " << cnt;
}

TL::common::Status OpenSpacePathGenerator::GenerateCoarsePath(
    const std::atomic<bool>& atomic_early_stop_flag,
    const common::PathPoint& start_point_local,
    const common::PathPoint& end_pose_local,
    const std::vector<double>& xy_bounds,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        obstacles_segments_vec_local,
    const DestRegionWithAng& dest_region_with_angle_local,
    const PathStrategy& path_strategy,
    PathGeneratorResult* const coarse_path_ptr,
    bool* const need_collision_free_smooth) {
  ADEBUG << "start generate coarse path";
  if (nullptr == coarse_path_ptr) {
    std::string msg = "coarse path ptr is null";
    AERROR << msg;
    return Status(ErrorCode::PLANNER_PARKING_PATHGENERATOR_ERROR, msg);
  }
  if (path_strategy.disable_search) {
    ADEBUG << "disable search path, use default path directly";
    *coarse_path_ptr = default_warm_start_path_result_;
    default_warm_start_path_result_.reset();
    return Status::OK();
  }
  // 独立编译改造 MVP-A：ILQR 子规划器暂未集成
  // if (path_strategy.path_search_strategy.park_direction == PARKIN &&
  //     ilqr_planner_ && ilqr_planner_->Plan(...)) {
  //   ADEBUG << "ilqr suc, return directly";
  //   return Status::OK();
  // }
  PathSearchStrategy path_search_strategy = path_strategy.path_search_strategy;
  PathSearchStrategy precise_pose_strategy = path_search_strategy;
  PathSearchStrategy precise_angle_strategy = path_search_strategy;
  if (!SeparateGeometryStrategy(
          path_search_strategy, &(precise_pose_strategy.use_geometry_strategy),
          &(precise_angle_strategy.use_geometry_strategy))) {
    std::string msg = "load geometry strategy fail";
    AERROR << msg;
    return Status(ErrorCode::PLANNER_PARKING_PATHGENERATOR_ERROR, msg);
  }
  const auto& use_geometry =
      path_search_strategy.use_geometry_strategy.use_geometry;
  if (nullptr != need_collision_free_smooth) {
    *need_collision_free_smooth = true;
  }
  std::string warm_start_msg = "init to generate coarse path";
  switch (use_geometry) {
    case ONLY_USE: {
      // 独立编译改造 MVP-A：geometric_path_planner_ 暂未集成，降级到 HybridAStar
      AINFO << "[PARKING] MVP-A: GeometricPath not available, fallback to HybridAStar";
      goto hybrid_astar_default;
    }
    case USE_FIRST: {
      // 独立编译改造 MVP-A：geometric_path_planner_ 暂未集成，直接用 HybridAStar
      if (!hybrid_a_star_planner_->Plan(
              atomic_early_stop_flag, start_point_local, end_pose_local,
              xy_bounds, obstacles_segments_vec_local,
              dest_region_with_angle_local, path_search_strategy,
              coarse_path_ptr)) {
        warm_start_msg = "use geometry first,  generate coarse path  fail";
        AERROR << warm_start_msg;
      }
      break;
    }
    case USE_FIRST_LAST:
    case USE_BOTH: {
      // 独立编译改造 MVP-A：geometry_planner_ 暂未集成，fallthrough 到 USE_LAST
    }
    [[fallthrough]];
    case USE_LAST: {
      PathGeneratorResult result_explore;
      PathGeneratorResult result_geometry;
      const bool explore_ret =
          (use_geometry != USE_BOTH) &&
          hybrid_a_star_planner_->Plan(atomic_early_stop_flag,
                                       start_point_local, end_pose_local,
                                       xy_bounds, obstacles_segments_vec_local,
                                       dest_region_with_angle_local,
                                       path_search_strategy, &result_explore);
      // 独立编译改造 MVP-A：geometry_planner_ 暂未集成
      const bool geometry_ret = false;
      if (!explore_ret && !geometry_ret) {
        ADEBUG << "both explore and  geomery fail";
        if (path_search_strategy.space_structure != LAT_PARK_LOT ||
            !path_search_strategy.collision_free_search_strategy
                 .replan_due_to_collision) {
          warm_start_msg =
              "both precise angle geomery and search fail, use history path";
          AERROR << warm_start_msg;
          break;
        }
        if (result_geometry.x.empty() ||
            result_geometry.x.size() != result_geometry.y.size() ||
            result_geometry.x.size() != result_geometry.phi.size()) {
          warm_start_msg = "precise angle geomery fallback validity checkfails";
          AERROR << warm_start_msg;
          break;
        }
        ADEBUG << "precise angle purpose in lat slot park, use geomery result "
                  "fallback";
        *coarse_path_ptr = result_geometry;
        if (nullptr != need_collision_free_smooth) {
          *need_collision_free_smooth = false;
        }
      }
      if (!explore_ret) {
        *coarse_path_ptr = result_geometry;
      } else if (!geometry_ret) {
        *coarse_path_ptr = result_explore;
      } else {
        if (result_explore.y.empty() || result_geometry.y.empty()) {
          warm_start_msg = "geometry or explore result is invalid";
          AERROR << warm_start_msg;
          break;
        }
        *coarse_path_ptr = result_geometry;
        if (path_search_strategy.space_structure == LAT_PARK_LOT &&
            result_geometry.y.back() > result_explore.y.back()) {
          ADEBUG << " explore ret is better";
          *coarse_path_ptr = result_explore;
        }
      }
      break;
    }
    default:
    hybrid_astar_default: {
      if (!hybrid_a_star_planner_->Plan(
              atomic_early_stop_flag, start_point_local, end_pose_local,
              xy_bounds, obstacles_segments_vec_local,
              dest_region_with_angle_local, path_search_strategy,
              coarse_path_ptr)) {
        warm_start_msg = "use hybrid only, generate coarse path  fail";
        AERROR << warm_start_msg;
      }
      break;
    }
  }
  if (path_search_strategy.trace_adjust_search_strategy.is_trace_adjust) {
    const auto search_path_result = *coarse_path_ptr;
    CombineTraceAdjustPath(
        path_search_strategy.trace_adjust_search_strategy.trace_path,
        search_path_result, coarse_path_ptr);
  }
  if (coarse_path_ptr->x.empty()) {
    // failed generate coarse path, use defalut path
    if (default_warm_start_path_result_.x.empty()) {
      return Status(ErrorCode::PLANNER_PARKING_PATHGENERATOR_ERROR,
                    warm_start_msg);
    }
    *coarse_path_ptr = default_warm_start_path_result_;
    default_warm_start_path_result_.reset();
  }
  return Status::OK();
}

void OpenSpacePathGenerator::PathPointNormalizing(
    double rotate_angle, const common::math::Vec2d& translate_origin,
    common::PathPoint* const path_point_ptr) {
  if (path_point_ptr != nullptr) {
    // 独立编译改造：PathPoint 直接成员访问
    double x = path_point_ptr->x - translate_origin.x();
    double y = path_point_ptr->y - translate_origin.y();
    path_point_ptr->x = x * std::cos(-rotate_angle) -
                        y * std::sin(-rotate_angle);
    path_point_ptr->y = x * std::sin(-rotate_angle) +
                        y * std::cos(-rotate_angle);
    path_point_ptr->theta =
        common::math::NormalizeAngle(path_point_ptr->theta - rotate_angle);
  }
}

void OpenSpacePathGenerator::ReferencePointNormalizing(
    const double rotate_angle, const common::math::Vec2d& translate_origin,
    ReferencePoint* const reference_point_ptr) {
  if (reference_point_ptr != nullptr) {
    // ReferencePoint 保留 getter/setter 接口
    double x = reference_point_ptr->x() - translate_origin.x();
    double y = reference_point_ptr->y() - translate_origin.y();
    reference_point_ptr->set_x(x * std::cos(-rotate_angle) -
                               y * std::sin(-rotate_angle));
    reference_point_ptr->set_y(x * std::sin(-rotate_angle) +
                               y * std::cos(-rotate_angle));
    reference_point_ptr->set_heading(common::math::NormalizeAngle(
        reference_point_ptr->heading() - rotate_angle));
  }
}

bool OpenSpacePathGenerator::SeparateGeometryStrategy(
    const PathSearchStrategy& path_strategy,
    GeometryStrategy* const precise_pose_geometry_strategy,
    GeometryStrategy* const precise_angle_geometry_strategy) {
  if (nullptr == precise_pose_geometry_strategy ||
      nullptr == precise_angle_geometry_strategy ||
      (path_strategy.use_geometry_strategy.use_geometry != NOT_USE &&
       (path_strategy.use_geometry_strategy.use_purpose.empty() ||
        path_strategy.use_geometry_strategy.use_purpose.size() !=
            path_strategy.use_geometry_strategy.geometry_path_type.size() ||
        path_strategy.use_geometry_strategy.use_purpose.size() !=
            path_strategy.use_geometry_strategy.longitudal_bound.size()))) {
    AERROR << "SeparateGeometryStrategy input check fails";
    return false;
  }
  if (path_strategy.use_geometry_strategy.use_geometry == NOT_USE) {
    ADEBUG << " geometry planner is not used";
    return true;
  }
  precise_pose_geometry_strategy->geometry_path_type.clear();
  precise_pose_geometry_strategy->longitudal_bound.clear();
  precise_pose_geometry_strategy->use_purpose.clear();
  precise_angle_geometry_strategy->geometry_path_type.clear();
  precise_angle_geometry_strategy->longitudal_bound.clear();
  precise_angle_geometry_strategy->use_purpose.clear();
  for (size_t i = 0; i < path_strategy.use_geometry_strategy.use_purpose.size();
       i++) {
    if (path_strategy.use_geometry_strategy.use_purpose.at(i) == PRECISEPOSE) {
      precise_pose_geometry_strategy->use_purpose.emplace_back(PRECISEPOSE);
      precise_pose_geometry_strategy->geometry_path_type.emplace_back(
          path_strategy.use_geometry_strategy.geometry_path_type.at(i));
      precise_pose_geometry_strategy->longitudal_bound.emplace_back(
          path_strategy.use_geometry_strategy.longitudal_bound.at(i));
    } else {
      precise_angle_geometry_strategy->use_purpose.emplace_back(PRECISEANGLE);
      precise_angle_geometry_strategy->geometry_path_type.emplace_back(
          path_strategy.use_geometry_strategy.geometry_path_type.at(i));
      precise_angle_geometry_strategy->longitudal_bound.emplace_back(
          path_strategy.use_geometry_strategy.longitudal_bound.at(i));
    }
  }
  return true;
}

void OpenSpacePathGenerator::PathDeNormal(
    const common::math::Vec2d& origin_point, const double origin_heading,
    std::vector<PathGearPair>* const partition_paths) {
  if (partition_paths == nullptr) {
    AERROR << "PathDeNormal input check fails";
    return;
  }
  for (auto& path_pair : *partition_paths) {
    for (auto& point : path_pair.first) {
      // 独立编译改造：PathPoint 直接成员访问
      double tmp_x = point.x;
      double tmp_y = point.y;
      point.x = tmp_x * std::cos(origin_heading) -
                tmp_y * std::sin(origin_heading) + origin_point.x();
      point.y = tmp_x * std::sin(origin_heading) +
                tmp_y * std::cos(origin_heading) + origin_point.y();
      point.theta =
          common::math::NormalizeAngle(point.theta + origin_heading);
    }
  }
}

void OpenSpacePathGenerator::CombineTraceAdjustPath(
    const DiscretizedPath& trace_path,
    const PathGeneratorResult& search_path_result,
    PathGeneratorResult* const path_result) {
  if (nullptr == path_result || search_path_result.x.empty()) {
    return;
  }
  *path_result = default_warm_start_path_result_;
  common::SLPoint search_end_sl;
  if (!trace_path.XYToSL(search_path_result.x.back(),
                         search_path_result.y.back(), &search_end_sl)) {
    return;
  }
  *path_result = search_path_result;
  const double step_s = 0.1;
  // 独立编译改造：SLPoint 直接成员访问
  double acc_s = search_end_sl.s + step_s;
  common::PathPoint path_point;
  while (acc_s < trace_path.Length()) {
    path_point = trace_path.Evaluate(acc_s);
    // 独立编译改造：PathPoint 直接成员访问
    path_result->x.push_back(path_point.x);
    path_result->y.push_back(path_point.y);
    path_result->phi.push_back(path_point.theta);
    acc_s += step_s;
  }
  path_result->path_type = default_warm_start_path_result_.path_type;
}

}  // namespace planning
}  // namespace TL
