#pragma once

#include "planning/tasks/deciders/open_space_decider/open_space_roi_decider.h"
#include "planning/tasks/optimizers/open_space_path_generation/open_space_path_provider.h"
#include "planning/tasks/optimizers/open_space_path_partition/open_space_path_partition.h"
#include "planning/tasks/optimizers/open_space_speed_optimizer/open_space_speed_optimizer.h"
#include "planning/tasks/optimizers/open_space_straight_path/open_space_straight_path_provider.h"
#include "valet_parking_c_api.h"
#include "valet_parking_topics.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace valet_parking {

struct RuntimeVehicleInput {
  bool has_vehicle_state{false};
  uint64_t stamp_ms{0U};
  double x{0.0};
  double y{0.0};
  double z{0.0};
  double heading{0.0};
  double linear_velocity{0.0};
  double linear_acceleration{0.0};
  TL::soc::GearPosition gear{TL::soc::GearPosition::GEAR_PARKING};
};

struct RuntimeObstacleInput {
  int id{0};
  TL::planning::PerceptionObstacle_Type type{
      TL::planning::PerceptionObstacle_Type_UNKNOWN_UNMOVABLE};
  bool is_dynamic{false};
  double center_x{0.0};
  double center_y{0.0};
  double heading{0.0};
  double length{0.0};
  double width{0.0};
  double velocity_x{0.0};
  double velocity_y{0.0};
};

struct InputMetadata {
  uint64_t seq{0U};
  std::string frame_id{"selected_slot"};
  uint64_t publish_stamp_ms{0U};
  uint64_t data_stamp_ms{0U};
  std::string status_reason{"valid selected_slot sample"};
};

struct DirectSpeedOptimizerProfile {
  double selected_speed_mps{0.8};
  double min_sample_speed{0.3};
  double max_sample_speed{0.8};
};

TL::common::VehicleState BuildVehicleState(
    const valet_parking_config_t& config,
    uint64_t stamp_ms,
    const RuntimeVehicleInput& vehicle_input);

TL::common::PathPoint BuildStartPathPoint(
    const valet_parking_config_t& config,
    const RuntimeVehicleInput& vehicle_input);

std::vector<TL::planning::SimpleStaticObstacle> BuildStaticObstacles(
    const std::vector<RuntimeObstacleInput>& obstacles);

std::vector<TL::planning::SimpleMovingObstacle> BuildMovingObstacles(
    const std::vector<RuntimeObstacleInput>& obstacles);

std::vector<std::pair<TL::common::math::LineSegment2d, double>>
BuildObstacleSegments(
    const TL::planning::RoiDeciderOutput& roi_output,
    const std::vector<RuntimeObstacleInput>& obstacles);

TL::planning_internal::PathType MapPathProviderPathType(int path_type);

TL::planning::DiscretizedPath NormalizeDiscretizedPath(
    const TL::planning::DiscretizedPath& source_path);

std::vector<TL::planning::PathGearPair> NormalizePathProviderPathSet(
    const std::vector<TL::planning::PathGearPair>& path_set);

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
        speed_plan_collision_info);

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
    bool is_rpa_direct_mode);

double SelectDirectDistance(double distance_m);
double SelectDirectSpeed(double speed_mps);

DirectSpeedOptimizerProfile BuildDirectSpeedOptimizerProfile(double speed_mps);

TL::planning::OpenSpaceSpeedOptimizerConfig BuildDirectSpeedOptimizerConfig(
    const TL::planning::OpenSpaceSpeedOptimizerConfig& base_config,
    const DirectSpeedOptimizerProfile& profile);

TL::soc::GearPosition TargetGearForDirectCommand(bool forward);

TL::planning::OpenSpaceStraightPathInput BuildOpenSpaceStraightPathInput(
    const valet_parking_config_t& config,
    const InputMetadata& metadata,
    const RuntimeVehicleInput& vehicle_input,
    const ParkingCommand& command,
    bool forward);

void BuildPartitionOutputFromStraightPath(
    const TL::planning::OpenSpaceStraightPathOutput& straight_output,
    TL::planning::PartitionOutput* partition_output);

}  // namespace valet_parking
