/*
 * Copyright (c) TL Technologies Co., Ltd. 2022. All rights reserved.
 * Description:  ilqr_path.cc
 */

#include "planning/open_space/coarse_path_generator/ilqr_path.h"
#include "proto_convert/planning_internal_convert.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iterator>
#include <limits>
#include <memory>

#include "planning/open_space/ilqr_smoother/al_ilqr_interface.h"
#include "planning/open_space/vehicle_param.h"
#include "proto_convert/pnc_point_convert.h"
#include "proto_convert/vehicle_config_convert.h"

namespace TL {
namespace planning {

// Default collision buffer: 0.3m (original FLAGS value)
static constexpr double kDefaultEgoInflatedCollisionBuffer = 0.3;

ILQR::ILQR(const WarmStartConfig& warm_start_config)
    : PathGenerator(TL::planning::HybridAStarConfig(),
                    TL::planning::LoadEP30VehicleParam()),
      extra_collision_buffer_(
          warm_start_config.extra_distance_for_geometry_path) {
  Param param;
  al_ilqr_interface_ = std::make_unique<ALILQR_INTERFACE>(param);
}

bool ILQR::Plan(
    const std::atomic<bool>& /*atomic_early_stop_flag*/,
    const common::PathPoint& start_point, const common::PathPoint& end_point,
    const std::vector<double>& xy_bounds,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        obstacles_segments_vec,
    const DestRegionWithAng& /*dest_region_with_angle*/,
    const PathSearchStrategy& path_search_strategy,
    PathGeneratorResult* const result) {
  xy_bounds_ = xy_bounds;
  double x1 = start_point.x;
  double y1 = start_point.y;
  double theta1 = start_point.theta;
  double x2 = end_point.x;
  double y2 = end_point.y;
  double theta2 = end_point.theta;
  const bool is_lat_spot =
      path_search_strategy.space_structure == SpaceStructure::LAT_PARK_LOT;
  if (!is_lat_spot) {
    // TDDO(jyw): remove force a straight line
    const double kDis =
        common::math::NormalizeAngle(theta2) > 0.0
            ? GetStraightenDist(x2, y2, theta2, obstacles_segments_vec)
            : GetStraightenDist(x2, y2, theta2 + M_PI, obstacles_segments_vec);
    ADEBUG << "kdis " << kDis;
    x2 += kDis * std::cos(theta2);
    y2 += kDis * std::sin(theta2);
  }
  al_ilqr_interface_->Init(x1, y1, theta1, x2, y2, theta2, true, is_lat_spot,
                           path_search_strategy.init_path_direction,
                           obstacles_segments_vec);
  if (!al_ilqr_interface_->GetOptimal(&(result->x), &(result->y),
                                      &(result->phi))) {
    result->reset();
    al_ilqr_interface_->Init(x1, y1, theta1, x2, y2, theta2, false, is_lat_spot,
                             path_search_strategy.init_path_direction,
                             obstacles_segments_vec);
    if (!is_lat_spot ||
        (is_lat_spot && !al_ilqr_interface_->GetOptimal(
                            &(result->x), &(result->y), &(result->phi)))) {
      result->reset();
      return false;
    }
  }
  if (!is_lat_spot) {
    result->x.insert(result->x.end(), end_point.x);
    result->y.insert(result->y.end(), end_point.y);
    result->phi.insert(result->phi.end(), end_point.theta);
  }

  std::vector<std::pair<common::math::LineSegment2d, double>>
      obstacles_without_virtual;
  static constexpr double kEpsilon = 1.0e-3;
  for (auto obstacles_segments : obstacles_segments_vec) {
    if (obstacles_segments.second > kEpsilon) {
      obstacles_segments.second += is_lat_spot ? 0.0 : extra_collision_buffer_;
      obstacles_without_virtual.push_back(obstacles_segments);
    }
  }
  common::math::LineSegment2d line_1(common::math::Vec2d(xy_bounds.at(0), xy_bounds.at(2)),
                                     common::math::Vec2d(xy_bounds.at(0), xy_bounds.at(3)));
  common::math::LineSegment2d line_2(common::math::Vec2d(xy_bounds.at(1), xy_bounds.at(2)),
                                     common::math::Vec2d(xy_bounds.at(1), xy_bounds.at(3)));
  common::math::LineSegment2d line_3(common::math::Vec2d(xy_bounds.at(0), xy_bounds.at(2)),
                                     common::math::Vec2d(xy_bounds.at(1), xy_bounds.at(2)));
  common::math::LineSegment2d line_4(common::math::Vec2d(xy_bounds.at(0), xy_bounds.at(3)),
                                     common::math::Vec2d(xy_bounds.at(1), xy_bounds.at(3)));
  obstacles_without_virtual.emplace_back(line_1, kEpsilon);
  obstacles_without_virtual.emplace_back(line_2, kEpsilon);
  obstacles_without_virtual.emplace_back(line_3, kEpsilon);
  obstacles_without_virtual.emplace_back(line_4, kEpsilon);
  if (!CollisonCheck(*result, obstacles_without_virtual)) {
    result->reset();
    return false;
  }
  result->path_type = static_cast<int>(planning_internal::PathType::ILQR_PATH);
  return true;
}

bool ILQR::CollisonCheck(
    const PathGeneratorResult& ilqr_path,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        obstacles_segments_vec) {
  for (int i = 0; i < ilqr_path.x.size(); ++i) {
    std::shared_ptr<Node3d> path_nodes = std::make_shared<Node3d>(
        ilqr_path.x[i], ilqr_path.y[i], ilqr_path.phi[i], xy_bounds_,
        xy_grid_resolution_, phi_grid_resolution_);
    if (!ValidityCheck(path_nodes, obstacles_segments_vec)) {
      AERROR << " collision check failed " << path_nodes->GetX() << " "
             << path_nodes->GetY() << " " << path_nodes->GetPhi();
      return false;
    }
  }
  return true;
}

double ILQR::GetStraightenDist(
    const double x, const double y, const double theta,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        obstacles_segments_vec) {
  static constexpr double kEpsilon = 1.0e-3;
  const double kMinDis = 0.5;
  const double kMaxDis = 3.5;
  const double step = 0.25;
  const double back_edge_to_center =
      vehicle_param_.back_edge_to_center;
  double straighten_dist = kMinDis;
  while (straighten_dist < kMaxDis) {
    double x_tmp =
        x + (straighten_dist + back_edge_to_center) * std::cos(theta);
    double y_tmp =
        y + (straighten_dist + back_edge_to_center) * std::sin(theta);
    common::math::Box2d ego_box(
        common::math::Vec2d(x_tmp, y_tmp), theta,
        vehicle_param_.length, vehicle_param_.width);
    bool is_collision = false;
    for (const auto& obs_pair : obstacles_segments_vec) {
      if (obs_pair.second < kEpsilon &&
          ego_box.DistanceTo(obs_pair.first) <
              kDefaultEgoInflatedCollisionBuffer) {
        is_collision = true;
        break;
      }
    }
    if (!is_collision) {
      break;
    }
    straighten_dist += step;
  }
  return std::clamp(straighten_dist, kMinDis, kMaxDis);
}

}  // namespace planning
}  // namespace TL
