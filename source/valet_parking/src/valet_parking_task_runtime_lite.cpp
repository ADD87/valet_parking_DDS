#include "valet_parking_task_runtime_lite.h"

#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

namespace valet_parking {

namespace {

double NormalizeAngle(double angle) {
  constexpr double kPi = 3.14159265358979323846;
  constexpr double kTwoPi = 2.0 * kPi;
  while (angle >= kPi) {
    angle -= kTwoPi;
  }
  while (angle < -kPi) {
    angle += kTwoPi;
  }
  return angle;
}

void ApplyDirectSpeedBound(
    const DirectSpeedOptimizerProfile& profile,
    TL::planning::OpenSpaceSpeedOptimizerConfig::SpeedBoundInfo* bound) {
  if (bound == nullptr) {
    return;
  }
  bound->min_sample_speed = profile.min_sample_speed;
  bound->max_sample_speed = profile.max_sample_speed;
}

}  // namespace

TL::common::VehicleState BuildVehicleState(
    const valet_parking_config_t& config,
    uint64_t stamp_ms,
    const RuntimeVehicleInput& vehicle_input) {
  TL::common::VehicleState vehicle_state;
  vehicle_state.x = vehicle_input.has_vehicle_state
      ? vehicle_input.x
      : config.fake_vehicle_x;
  vehicle_state.y = vehicle_input.has_vehicle_state
      ? vehicle_input.y
      : config.fake_vehicle_y;
  vehicle_state.z = vehicle_input.has_vehicle_state ? vehicle_input.z : 0.0;
  vehicle_state.yaw = vehicle_input.has_vehicle_state
      ? NormalizeAngle(vehicle_input.heading)
      : config.fake_vehicle_theta;
  vehicle_state.heading = vehicle_state.yaw;
  const uint64_t effective_stamp_ms =
      vehicle_input.has_vehicle_state && vehicle_input.stamp_ms != 0U
          ? vehicle_input.stamp_ms
          : stamp_ms;
  vehicle_state.timestamp = static_cast<double>(effective_stamp_ms) / 1000.0;
  vehicle_state.linear_velocity = vehicle_input.has_vehicle_state
      ? vehicle_input.linear_velocity
      : 0.0;
  vehicle_state.linear_acceleration = vehicle_input.has_vehicle_state
      ? vehicle_input.linear_acceleration
      : 0.0;
  vehicle_state.gear = vehicle_input.has_vehicle_state
      ? vehicle_input.gear
      : TL::common::GearPosition::GEAR_PARKING;
  vehicle_state.pose.position.x = vehicle_state.x;
  vehicle_state.pose.position.y = vehicle_state.y;
  vehicle_state.pose.position.z = vehicle_state.z;
  vehicle_state.pose.heading = vehicle_state.heading;
  vehicle_state.pose.linear_velocity.x = vehicle_state.linear_velocity;
  vehicle_state.pose.linear_acceleration.x = vehicle_state.linear_acceleration;
  return vehicle_state;
}

TL::common::PathPoint BuildStartPathPoint(
    const valet_parking_config_t& config,
    const RuntimeVehicleInput& vehicle_input) {
  TL::common::PathPoint start_point;
  start_point.x = vehicle_input.has_vehicle_state
      ? vehicle_input.x
      : config.fake_vehicle_x;
  start_point.y = vehicle_input.has_vehicle_state
      ? vehicle_input.y
      : config.fake_vehicle_y;
  start_point.z = vehicle_input.has_vehicle_state ? vehicle_input.z : 0.0;
  start_point.theta = vehicle_input.has_vehicle_state
      ? NormalizeAngle(vehicle_input.heading)
      : config.fake_vehicle_theta;
  start_point.kappa = 0.0;
  start_point.s = 0.0;
  start_point.x_derivative = std::cos(start_point.theta);
  start_point.y_derivative = std::sin(start_point.theta);
  return start_point;
}

std::vector<TL::planning::SimpleStaticObstacle> BuildStaticObstacles(
    const std::vector<RuntimeObstacleInput>& obstacles) {
  std::vector<TL::planning::SimpleStaticObstacle> output;
  output.reserve(obstacles.size());
  for (const RuntimeObstacleInput& obstacle : obstacles) {
    if (obstacle.is_dynamic) {
      continue;
    }
    output.emplace_back(obstacle.id, obstacle.type, obstacle.center_x,
                        obstacle.center_y, obstacle.heading,
                        obstacle.length, obstacle.width);
  }
  return output;
}

std::vector<TL::planning::SimpleMovingObstacle> BuildMovingObstacles(
    const std::vector<RuntimeObstacleInput>& obstacles) {
  std::vector<TL::planning::SimpleMovingObstacle> output;
  output.reserve(obstacles.size());
  for (const RuntimeObstacleInput& obstacle : obstacles) {
    if (!obstacle.is_dynamic) {
      continue;
    }
    output.emplace_back(obstacle.id, obstacle.type, obstacle.center_x,
                        obstacle.center_y, obstacle.heading,
                        obstacle.length, obstacle.width,
                        obstacle.velocity_x, obstacle.velocity_y);
  }
  return output;
}

std::vector<std::pair<TL::common::math::LineSegment2d, double>>
BuildObstacleSegments(
    const TL::planning::RoiDeciderOutput& roi_output,
    const std::vector<RuntimeObstacleInput>& obstacles) {
  std::vector<std::pair<TL::common::math::LineSegment2d, double>> segments =
      roi_output.obs_segments;
  for (const RuntimeObstacleInput& obstacle : obstacles) {
    TL::common::math::Box2d box(
        TL::common::math::Vec2d(obstacle.center_x, obstacle.center_y),
        obstacle.heading, obstacle.length, obstacle.width);
    std::vector<TL::common::math::Vec2d> corners;
    box.GetAllCorners(&corners);
    if (corners.size() != 4U) {
      continue;
    }
    for (std::size_t i = 0U; i < corners.size(); ++i) {
      const TL::common::math::Vec2d& start = corners[i];
      const TL::common::math::Vec2d& end = corners[(i + 1U) % corners.size()];
      segments.emplace_back(TL::common::math::LineSegment2d(start, end), 0.0);
    }
  }
  return segments;
}

TL::planning_internal::PathType MapPathProviderPathType(int path_type) {
  using TL::planning_internal::PathType;
  switch (path_type) {
    case static_cast<int>(PathType::SEARCH_PATH):
      return PathType::SEARCH_PATH;
    case static_cast<int>(PathType::TRACE_PATH):
      return PathType::TRACE_PATH;
    case static_cast<int>(PathType::GEOMETRY):
      return PathType::GEOMETRY;
    case static_cast<int>(PathType::GEOMETRY_ADJUST):
      return PathType::GEOMETRY_ADJUST;
    case static_cast<int>(PathType::CRUISE_PATH):
      return PathType::CRUISE_PATH;
    case static_cast<int>(PathType::SCS_GEOMETRY):
      return PathType::SCS_GEOMETRY;
    case static_cast<int>(PathType::ILQR_PATH):
      return PathType::ILQR_PATH;
    case static_cast<int>(PathType::SEARCH_EXTENSION_PATH):
      return PathType::SEARCH_EXTENSION_PATH;
    case static_cast<int>(PathType::DEFAULT):
    default:
      return PathType::DEFAULT;
  }
}

TL::planning::DiscretizedPath NormalizeDiscretizedPath(
    const TL::planning::DiscretizedPath& source_path) {
  std::vector<TL::common::PathPoint> normalized_points;
  normalized_points.reserve(source_path.size());

  double accumulated_s = 0.0;
  TL::common::PathPoint previous_point;
  for (std::size_t i = 0U; i < source_path.size(); ++i) {
    TL::common::PathPoint point = source_path[i];
    point.theta = NormalizeAngle(point.theta);
    if (i > 0U) {
      accumulated_s += std::hypot(point.x - previous_point.x,
                                  point.y - previous_point.y);
    }
    point.s = accumulated_s;
    point.x_derivative = std::cos(point.theta);
    point.y_derivative = std::sin(point.theta);
    normalized_points.push_back(point);
    previous_point = point;
  }

  return TL::planning::DiscretizedPath(std::move(normalized_points));
}

std::vector<TL::planning::PathGearPair> NormalizePathProviderPathSet(
    const std::vector<TL::planning::PathGearPair>& path_set) {
  std::vector<TL::planning::PathGearPair> normalized_path_set;
  normalized_path_set.reserve(path_set.size());
  for (const TL::planning::PathGearPair& path_pair : path_set) {
    normalized_path_set.emplace_back(NormalizeDiscretizedPath(path_pair.first),
                                     path_pair.second);
  }
  return normalized_path_set;
}

TL::planning::PartitionInput BuildPathPartitionInput(
    const valet_parking_config_t& config,
    const InputMetadata& metadata,
    const TL::planning::RoiDeciderOutput& roi_output,
    const TL::planning::OpenSpacePathOutput& path_output,
    const RuntimeVehicleInput& vehicle_input,
    const std::vector<RuntimeObstacleInput>& obstacles,
    TL::soc::GearPosition last_published_gear,
    bool replan_triggered_by_speed_plan,
    const TL::planning_internal::AvpSpeedPlanCollisionInfo&
        speed_plan_collision_info) {
  TL::planning::PartitionInput input;
  input.vehicle_state =
      BuildVehicleState(config, metadata.data_stamp_ms, vehicle_input);
  input.planning_start_point = BuildStartPathPoint(config, vehicle_input);
  input.pub_gear = last_published_gear;
  input.path_result.path_set =
      NormalizePathProviderPathSet(path_output.partitioned_path);
  input.path_result.path_idx = 0U;
  input.path_result.point_idx = 0U;
  input.path_result.path_shift = false;
  input.path_result.path_type = MapPathProviderPathType(path_output.path_type);
  input.path_result.replan_status = path_output.replan_status;
  input.parking_scenario_type = roi_output.scenario_type;
  input.is_parking_inwards = roi_output.is_parking_inwards;
  input.end_pose =
      roi_output.has_fine_tuned ? roi_output.fine_tuned_end_pose
                                : roi_output.end_pose;
  input.dest_region_with_angle = roi_output.dest_region;
  input.obstacles_segments_vec = BuildObstacleSegments(roi_output, obstacles);
  input.is_vehicle_stand_still =
      std::fabs(input.vehicle_state.linear_velocity) < 1.0e-3;
  input.replan_triggered_by_speed_plan = replan_triggered_by_speed_plan;
  input.guard_triggered = false;
  input.input_replan_status = path_output.replan_status;
  input.speed_plan_collision_info = speed_plan_collision_info;
  input.parking_action_type = 0;
  return input;
}

TL::planning::SpeedOptimizerInput BuildSpeedOptimizerInput(
    const valet_parking_config_t& config,
    const InputMetadata& metadata,
    const TL::planning::RoiDeciderOutput& roi_output,
    const TL::planning::PartitionOutput& partition_output,
    const RuntimeVehicleInput& vehicle_input,
    const std::vector<RuntimeObstacleInput>& obstacles,
    const TL::planning::OpenSpaceSpeedOptimizerConfig& speed_config,
    bool has_last_frame,
    double last_frame_timestamp,
    double last_planning_start_relative_time,
    bool is_rpa_direct_mode) {
  TL::planning::SpeedOptimizerInput input;
  input.discretized_path =
      NormalizeDiscretizedPath(partition_output.chosen_partitioned_path.first);
  input.partitioned_paths = partition_output.partitioned_paths;
  input.gear = partition_output.chosen_partitioned_path.second;
  input.is_gear_changed = partition_output.is_gear_changed;
  input.is_stop_path = partition_output.is_stop_path;
  input.start_point.path_point = BuildStartPathPoint(config, vehicle_input);
  input.start_point.v =
      vehicle_input.has_vehicle_state ? vehicle_input.linear_velocity : 0.0;
  input.start_point.a = vehicle_input.has_vehicle_state
      ? vehicle_input.linear_acceleration
      : 0.0;
  input.start_point.relative_time = 0.0;
  input.vehicle_state =
      BuildVehicleState(config, metadata.data_stamp_ms, vehicle_input);
  input.is_vehicle_stand_still =
      std::fabs(input.vehicle_state.linear_velocity) < 1.0e-3;
  input.env_structured_info.is_parking_inwards = roi_output.is_parking_inwards;
  input.env_structured_info.parking_scenario_type = roi_output.scenario_type;
  input.is_forward = input.gear == TL::soc::GearPosition::GEAR_DRIVE;
  input.is_mirror_fold = partition_output.is_mirror_fold;
  input.is_rpa_direct_mode = is_rpa_direct_mode;
  input.static_obstacles = BuildStaticObstacles(obstacles);
  input.moving_obstacles = BuildMovingObstacles(obstacles);
  input.config = speed_config;
  input.has_last_frame = has_last_frame;
  input.last_frame_timestamp = last_frame_timestamp;
  input.last_planning_start_relative_time =
      last_planning_start_relative_time;
  return input;
}

double SelectDirectDistance(double distance_m) {
  constexpr double kDefaultDirectDistanceM = 3.0;
  constexpr double kMaxDirectDistanceM = 20.0;
  if (!std::isfinite(distance_m) || distance_m <= 0.0) {
    return kDefaultDirectDistanceM;
  }
  return std::min(distance_m, kMaxDirectDistanceM);
}

double SelectDirectSpeed(double speed_mps) {
  constexpr double kDefaultDirectSpeedMps = 0.8;
  constexpr double kMaxDirectSpeedMps = 3.0;
  if (!std::isfinite(speed_mps) || speed_mps <= 0.0) {
    return kDefaultDirectSpeedMps;
  }
  return std::min(speed_mps, kMaxDirectSpeedMps);
}

DirectSpeedOptimizerProfile BuildDirectSpeedOptimizerProfile(double speed_mps) {
  DirectSpeedOptimizerProfile profile;
  profile.selected_speed_mps = SelectDirectSpeed(speed_mps);
  profile.max_sample_speed = profile.selected_speed_mps;
  profile.min_sample_speed =
      std::max(0.05, profile.selected_speed_mps * 0.6);
  if (profile.min_sample_speed >= profile.max_sample_speed) {
    profile.min_sample_speed = profile.max_sample_speed * 0.5;
  }
  return profile;
}

TL::planning::OpenSpaceSpeedOptimizerConfig BuildDirectSpeedOptimizerConfig(
    const TL::planning::OpenSpaceSpeedOptimizerConfig& base_config,
    const DirectSpeedOptimizerProfile& profile) {
  TL::planning::OpenSpaceSpeedOptimizerConfig config = base_config;
  ApplyDirectSpeedBound(profile, &config.apa_speed_bound_info.forward_info);
  ApplyDirectSpeedBound(profile, &config.apa_speed_bound_info.reverse_info);
  ApplyDirectSpeedBound(profile,
                        &config.rpa_direct_speed_bound_info.forward_info);
  ApplyDirectSpeedBound(profile,
                        &config.rpa_direct_speed_bound_info.reverse_info);
  config.speed_upper_bound =
      std::max(config.speed_upper_bound, profile.max_sample_speed);
  return config;
}

TL::soc::GearPosition TargetGearForDirectCommand(bool forward) {
  return forward ? TL::soc::GearPosition::GEAR_DRIVE
                 : TL::soc::GearPosition::GEAR_REVERSE;
}

TL::planning::OpenSpaceStraightPathInput BuildOpenSpaceStraightPathInput(
    const valet_parking_config_t& config,
    const InputMetadata& metadata,
    const RuntimeVehicleInput& vehicle_input,
    const ParkingCommand& command,
    bool forward) {
  TL::planning::OpenSpaceStraightPathInput input;
  input.mode = forward
      ? TL::planning::OpenSpaceStraightPathMode::DIRECT_FORWARD
      : TL::planning::OpenSpaceStraightPathMode::DIRECT_BACKWARD;
  input.vehicle_state =
      BuildVehicleState(config, metadata.data_stamp_ms, vehicle_input);
  input.planning_start_point.path_point =
      BuildStartPathPoint(config, vehicle_input);
  input.planning_start_point.v =
      vehicle_input.has_vehicle_state ? vehicle_input.linear_velocity : 0.0;
  input.planning_start_point.a = vehicle_input.has_vehicle_state
      ? vehicle_input.linear_acceleration
      : 0.0;
  input.planning_start_point.relative_time = 0.0;
  input.is_vehicle_stand_still =
      std::fabs(input.vehicle_state.linear_velocity) < 1.0e-3;
  input.direct_move_length_override =
      SelectDirectDistance(command.direct_distance_m());
  return input;
}

void BuildPartitionOutputFromStraightPath(
    const TL::planning::OpenSpaceStraightPathOutput& straight_output,
    TL::planning::PartitionOutput* partition_output) {
  if (partition_output == nullptr) {
    return;
  }
  *partition_output = TL::planning::PartitionOutput();
  partition_output->chosen_partitioned_path =
      straight_output.chosen_partitioned_path;
  partition_output->partitioned_paths.path_set.clear();
  partition_output->partitioned_paths.path_set.push_back(
      straight_output.chosen_partitioned_path);
  partition_output->partitioned_paths.path_idx = 0U;
  partition_output->partitioned_paths.point_idx = 0U;
  partition_output->partitioned_paths.path_shift = false;
  partition_output->partitioned_paths.path_type =
      TL::planning_internal::PathType::DEFAULT;
  partition_output->partitioned_paths.replan_status = 0U;
  partition_output->chosen_path_idx = {0U, 0U};
  partition_output->path_decision =
      TL::planning::OpenSpacePathDecision::CHOOSE_NEW_PATH;
  partition_output->path_decision_debug = straight_output.diagnostic;
  partition_output->is_stop_path = straight_output.is_stop_path;
  partition_output->is_gear_changed = straight_output.is_gear_changed;
  partition_output->destination_reached = false;
  partition_output->finish_status =
      TL::planning_internal::OpenSpaceDebug::UNKNOWN;
  partition_output->is_mirror_fold = false;
}

}  // namespace valet_parking
