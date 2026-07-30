#include "valet_parking_stage_parking_adapter.h"

#include "planning/open_space/vehicle_param.h"
#include "planning/tasks/deciders/open_space_decider/open_space_roi_decider.h"
#include "planning/tasks/optimizers/open_space_path_generation/open_space_path_provider.h"
#include "planning/tasks/optimizers/open_space_path_partition/open_space_path_partition.h"
#include "planning/tasks/optimizers/open_space_speed_optimizer/open_space_speed_optimizer.h"
#include "planning/tasks/optimizers/open_space_straight_path/open_space_straight_path_provider.h"
#include "proto_convert/parking_lot_convert.h"
#include "proto_convert/vehicle_state_convert.h"
#include "valet_parking_function_manager_lite.h"
#include "valet_parking_stage_contract_lite.h"
#include "valet_parking_stage_facade_lite.h"
#include "valet_parking_stage_finish_lite.h"
#include "valet_parking_task_contract_lite.h"
#include "valet_parking_topics.h"

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iomanip>
#include <limits>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace valet_parking {

namespace {

constexpr uint32_t kMaxExternalObstacleCount = 128U;
constexpr double kDefaultPathProviderTimeoutS = 8.5;
constexpr const char* kPathProviderTimeoutEnv =
    "VALET_PARKING_PATH_PROVIDER_TIMEOUT_S";
constexpr const char* kForcePathPartitionFailEnv =
    "VALET_PARKING_FORCE_PATH_PARTITION_FAIL";
constexpr const char* kForceSpeedOptimizerFailEnv =
    "VALET_PARKING_FORCE_SPEED_OPTIMIZER_FAIL";
constexpr const char* kForceRoiDeciderFailEnv =
    "VALET_PARKING_FORCE_ROI_DECIDER_FAIL";
constexpr const char* kForceStraightPathFailEnv =
    "VALET_PARKING_FORCE_STRAIGHT_PATH_FAIL";
constexpr double kStageFinishStandstillThresholdMps = 0.05;
constexpr uint32_t kStageFinishRequiredConsecutiveFrames = 2U;
constexpr double kPathProviderPreCheckObstacleNearPoseThresholdM = 0.75;

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

struct PathProviderRuntimeState {
  void Reset() {
    has_history = false;
    path_id = -1;
    start_point = TL::common::PathPoint();
    end_pose = TL::common::PathPoint();
    obstacle_signature = 0U;
    last_output.Reset();
    threaded_provider.reset();
    last_splice_gear = TL::soc::GearPosition::GEAR_NONE;
    last_warm_start_points = 0U;
    last_strategy_init_moving_direction = 0;
    generated_count = 0U;
    reused_count = 0U;
  }

  bool has_history{false};
  int path_id{-1};
  TL::common::PathPoint start_point;
  TL::common::PathPoint end_pose;
  uint64_t obstacle_signature{0U};
  TL::planning::OpenSpacePathOutput last_output;
  std::unique_ptr<TL::planning::OpenSpacePathProvider> threaded_provider;
  TL::soc::GearPosition last_splice_gear{
      TL::soc::GearPosition::GEAR_NONE};
  std::size_t last_warm_start_points{0U};
  int last_strategy_init_moving_direction{0};
  uint64_t generated_count{0U};
  uint64_t reused_count{0U};
};

struct DirectCommandRuntimeState {
  void Reset() {
    active = false;
    last_mode = ParkingCommandMode::PARKING_COMMAND_NONE;
  }

  bool active{false};
  ParkingCommandMode last_mode{ParkingCommandMode::PARKING_COMMAND_NONE};
};

TL::soc::GearPosition MapApiGearPosition(
    valet_parking_gear_position_t gear) {
  switch (gear) {
    case VALET_PARKING_GEAR_NEUTRAL:
      return TL::soc::GearPosition::GEAR_NEUTRAL;
    case VALET_PARKING_GEAR_DRIVE:
      return TL::soc::GearPosition::GEAR_DRIVE;
    case VALET_PARKING_GEAR_REVERSE:
      return TL::soc::GearPosition::GEAR_REVERSE;
    case VALET_PARKING_GEAR_PARKING:
      return TL::soc::GearPosition::GEAR_PARKING;
    case VALET_PARKING_GEAR_LOW:
      return TL::soc::GearPosition::GEAR_LOW;
    case VALET_PARKING_GEAR_INVALID:
      return TL::soc::GearPosition::GEAR_INVALID;
    case VALET_PARKING_GEAR_NONE:
      return TL::soc::GearPosition::GEAR_NONE;
  }
  return TL::soc::GearPosition::GEAR_INVALID;
}

TL::planning::PerceptionObstacle_Type MapApiObstacleType(
    valet_parking_obstacle_type_t type) {
  switch (type) {
    case VALET_PARKING_OBSTACLE_UNKNOWN:
      return TL::planning::PerceptionObstacle_Type_UNKNOWN;
    case VALET_PARKING_OBSTACLE_UNKNOWN_MOVABLE:
      return TL::planning::PerceptionObstacle_Type_UNKNOWN_MOVABLE;
    case VALET_PARKING_OBSTACLE_UNKNOWN_UNMOVABLE:
      return TL::planning::PerceptionObstacle_Type_UNKNOWN_UNMOVABLE;
    case VALET_PARKING_OBSTACLE_PEDESTRIAN:
      return TL::planning::PerceptionObstacle_Type_PEDESTRIAN;
    case VALET_PARKING_OBSTACLE_BICYCLE:
      return TL::planning::PerceptionObstacle_Type_BICYCLE;
    case VALET_PARKING_OBSTACLE_VEHICLE:
      return TL::planning::PerceptionObstacle_Type_VEHICLE;
  }
  return TL::planning::PerceptionObstacle_Type_UNKNOWN;
}

bool IsFiniteVehicleState(const valet_parking_vehicle_state_t& input) {
  return std::isfinite(input.x) && std::isfinite(input.y) &&
         std::isfinite(input.z) && std::isfinite(input.heading) &&
         std::isfinite(input.linear_velocity) &&
         std::isfinite(input.linear_acceleration);
}

bool IsFiniteObstacle(const valet_parking_obstacle_t& input) {
  return std::isfinite(input.center_x) && std::isfinite(input.center_y) &&
         std::isfinite(input.heading) && std::isfinite(input.length) &&
         std::isfinite(input.width) && std::isfinite(input.velocity_x) &&
         std::isfinite(input.velocity_y);
}

}  // namespace

struct ValetParkingStageParkingAdapter::RuntimeContext {
  RuntimeContext()
      : vehicle_param(TL::planning::LoadEP30VehicleParam()),
        path_partition_config(
            TL::planning::OpenSpacePathPartitionConfig::GetDefault()),
        path_partition(path_partition_config, vehicle_param),
        straight_path_provider(
            TL::planning::OpenSpaceStraightPathConfig::GetDefault()),
        speed_config(
            TL::planning::OpenSpaceSpeedOptimizerConfig::GetDefault()),
        speed_optimizer(speed_config) {
    ResetPlanningState();
  }

  bool ResetPlanningState() {
    const TL::common::Status partition_status = path_partition.Reset();
    path_partition_ready = partition_status.ok();
    path_partition_reset_message = partition_status.error_message();
    speed_optimizer.Reset();
    has_last_speed_frame = false;
    last_frame_timestamp = 0.0;
    last_planning_start_relative_time = 0.0;
    last_speed_plan_collision_info =
        TL::planning_internal::AvpSpeedPlanCollisionInfo();
    replan_triggered_by_speed_plan = false;
    current_path_has_collision_risk = false;
    last_published_gear = TL::soc::GearPosition::GEAR_PARKING;
    path_provider.Reset();
    stage_finish.Reset();
    direct_command.Reset();
    stage_exit_requested = false;
    straight_path_provider.Reset();
    return path_partition_ready;
  }

  int UpdateVehicleState(const valet_parking_vehicle_state_t& input) {
    if (input.is_valid == 0 || !IsFiniteVehicleState(input)) {
      return VALET_PARKING_ERR_INVALID_ARG;
    }

    vehicle_input.has_vehicle_state = true;
    vehicle_input.stamp_ms = input.stamp_ms;
    vehicle_input.x = input.x;
    vehicle_input.y = input.y;
    vehicle_input.z = input.z;
    vehicle_input.heading = input.heading;
    vehicle_input.linear_velocity = input.linear_velocity;
    vehicle_input.linear_acceleration = input.linear_acceleration;
    vehicle_input.gear = MapApiGearPosition(input.gear);
    return VALET_PARKING_OK;
  }

  void ClearVehicleState() {
    vehicle_input = RuntimeVehicleInput();
  }

  int UpdateObstacles(const valet_parking_obstacle_t* input_obstacles,
                      uint32_t obstacle_count) {
    if (obstacle_count > 0U && input_obstacles == nullptr) {
      return VALET_PARKING_ERR_INVALID_ARG;
    }
    if (obstacle_count > kMaxExternalObstacleCount) {
      return VALET_PARKING_ERR_INVALID_ARG;
    }

    std::vector<RuntimeObstacleInput> next_obstacles;
    next_obstacles.reserve(obstacle_count);
    for (uint32_t i = 0U; i < obstacle_count; ++i) {
      const valet_parking_obstacle_t& input = input_obstacles[i];
      if (!IsFiniteObstacle(input) || input.length <= 0.0 ||
          input.width <= 0.0) {
        return VALET_PARKING_ERR_INVALID_ARG;
      }

      RuntimeObstacleInput obstacle;
      obstacle.id = static_cast<int>(input.id);
      obstacle.type = MapApiObstacleType(input.type);
      obstacle.is_dynamic = input.is_dynamic != 0;
      obstacle.center_x = input.center_x;
      obstacle.center_y = input.center_y;
      obstacle.heading = input.heading;
      obstacle.length = input.length;
      obstacle.width = input.width;
      obstacle.velocity_x = input.velocity_x;
      obstacle.velocity_y = input.velocity_y;
      next_obstacles.push_back(obstacle);
    }

    obstacles = std::move(next_obstacles);
    return VALET_PARKING_OK;
  }

  void ClearObstacles() {
    obstacles.clear();
  }

  void UpdateAfterPartitionFallback(
      const TL::planning::PartitionOutput& partition_output) {
    speed_optimizer.Reset();
    has_last_speed_frame = false;
    last_frame_timestamp = 0.0;
    last_planning_start_relative_time = 0.0;
    last_speed_plan_collision_info =
        TL::planning_internal::AvpSpeedPlanCollisionInfo();
    replan_triggered_by_speed_plan = false;
    current_path_has_collision_risk = false;
    last_published_gear = partition_output.chosen_partitioned_path.second;
    ++processed_frames;
  }

  void UpdateAfterSpeedOptimizer(
      uint64_t data_stamp_ms,
      const TL::planning::SpeedOptimizerOutput& speed_output) {
    has_last_speed_frame = true;
    last_frame_timestamp = static_cast<double>(data_stamp_ms) / 1000.0;
    last_planning_start_relative_time = 0.0;
    last_speed_plan_collision_info = speed_output.speed_plan_collision_info;
    replan_triggered_by_speed_plan =
        speed_output.replan_triggered_by_speed_plan;
    current_path_has_collision_risk =
        speed_output.current_path_has_collision_risk;
    last_published_gear = speed_output.trajectory_gear.second;
    ++processed_frames;
  }

  TL::planning::VehicleParam vehicle_param;
  TL::planning::OpenSpacePathPartitionConfig path_partition_config;
  TL::planning::OpenSpacePathPartition path_partition;
  TL::planning::OpenSpaceStraightPathProvider straight_path_provider;
  TL::planning::OpenSpaceSpeedOptimizerConfig speed_config;
  TL::planning::OpenSpaceSpeedOptimizer speed_optimizer;
  PathProviderRuntimeState path_provider;
  StageFinishRuntimeState stage_finish;
  DirectCommandRuntimeState direct_command;
  bool stage_exit_requested{false};
  TL::planning_internal::AvpSpeedPlanCollisionInfo
      last_speed_plan_collision_info;
  RuntimeVehicleInput vehicle_input;
  std::vector<RuntimeObstacleInput> obstacles;
  TL::soc::GearPosition last_published_gear{
      TL::soc::GearPosition::GEAR_PARKING};
  bool path_partition_ready{false};
  std::string path_partition_reset_message;
  bool has_last_speed_frame{false};
  double last_frame_timestamp{0.0};
  double last_planning_start_relative_time{0.0};
  bool replan_triggered_by_speed_plan{false};
  bool current_path_has_collision_risk{false};
  uint64_t processed_frames{0U};
};

namespace {

constexpr int kTrajectoryPointCount = 21;

struct InputMetadata {
  uint64_t seq{0U};
  std::string frame_id{"selected_slot"};
  uint64_t publish_stamp_ms{0U};
  uint64_t data_stamp_ms{0U};
  std::string status_reason{"valid selected_slot sample"};
};

uint64_t NowMilliseconds() {
  return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()).count());
}

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

Header MakeHeader(uint64_t seq,
                  const std::string& frame_id,
                  uint64_t publish_stamp_ms,
                  uint64_t data_stamp_ms) {
  Header header;
  header.seq(seq);
  header.frame_id(frame_id);
  header.publish_stamp_ms(publish_stamp_ms);
  header.data_stamp_ms(data_stamp_ms);
  return header;
}

std::string JoinParkingLotSeqs(const std::vector<ParkingLot>& lots) {
  if (lots.empty()) {
    return "[]";
  }
  std::ostringstream stream;
  stream << "[";
  for (std::size_t i = 0U; i < lots.size(); ++i) {
    if (i > 0U) {
      stream << ",";
    }
    stream << lots[i].parking_seq();
  }
  stream << "]";
  return stream.str();
}

const ParkingLot* SelectParkingLot(const SelectedSlot& sample,
                                   std::string* status_reason) {
  const auto& lots = sample.parking_lots();
  if (lots.empty()) {
    if (status_reason != nullptr) {
      *status_reason = "selected_slot has no parking_lots";
    }
    return nullptr;
  }

  const uint32_t selected_seq = sample.opt_parking_seq();
  for (const auto& lot : lots) {
    if (lot.parking_seq() == selected_seq) {
      return &lot;
    }
  }

  if (status_reason != nullptr) {
    std::ostringstream stream;
    stream << "selected_slot opt_parking_seq is unavailable"
           << ", opt_parking_seq=" << selected_seq
           << ", available_parking_seq=" << JoinParkingLotSeqs(lots);
    *status_reason = stream.str();
  }
  return nullptr;
}

InputMetadata BuildMetadata(const SelectedSlot& sample) {
  InputMetadata metadata;
  const Header& input_header = sample.header();
  metadata.seq = input_header.seq();
  metadata.frame_id = input_header.frame_id().empty()
      ? std::string("selected_slot")
      : input_header.frame_id();
  metadata.publish_stamp_ms = input_header.publish_stamp_ms();
  metadata.data_stamp_ms = input_header.data_stamp_ms();

  const uint64_t now_ms = NowMilliseconds();
  if (metadata.seq == 0U) {
    metadata.seq = now_ms;
  }
  if (metadata.publish_stamp_ms == 0U) {
    metadata.publish_stamp_ms = now_ms;
  }
  if (metadata.data_stamp_ms == 0U) {
    metadata.data_stamp_ms = metadata.publish_stamp_ms;
  }
  return metadata;
}

bool IsStageControlOverrideCommandMode(ParkingCommandMode mode) {
  switch (mode) {
    case ParkingCommandMode::PARKING_COMMAND_PAUSE:
    case ParkingCommandMode::PARKING_COMMAND_BRAKE:
    case ParkingCommandMode::PARKING_COMMAND_FINISH:
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_LEFT:
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_RIGHT:
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_FRONT:
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_BACK:
      return true;
    case ParkingCommandMode::PARKING_COMMAND_NONE:
    case ParkingCommandMode::PARKING_COMMAND_PARKING_IN:
    case ParkingCommandMode::PARKING_COMMAND_DIRECT_FORWARD:
    case ParkingCommandMode::PARKING_COMMAND_DIRECT_BACKWARD:
      return false;
    default:
      return false;
  }
}

StageFacadeBranchLite StageFacadeBranchForInput(
    const ParkingCommand* command_sample,
    bool stage_exit_requested) {
  const bool has_command =
      command_sample != nullptr && command_sample->is_valid();
  if (has_command && IsDirectCommandMode(command_sample->mode())) {
    return StageFacadeBranchLite::kDirectOpenSpace;
  }
  if (has_command &&
      IsStageControlOverrideCommandMode(command_sample->mode())) {
    return StageFacadeBranchLite::kStageControlOverride;
  }
  if (!has_command && stage_exit_requested) {
    return StageFacadeBranchLite::kStageFinishHold;
  }
  return StageFacadeBranchLite::kNormalOpenSpace;
}

StageFacadeInputLite BuildStageFacadeInputLite(
    const InputMetadata& metadata,
    const SelectedSlot& input_sample,
    const ParkingCommand* command_sample,
    const FunctionManagerProjection& function_projection,
    const RuntimeVehicleInput& vehicle_input,
    std::size_t obstacle_count,
    const PathProviderRuntimeState& provider_state,
    bool has_last_speed_frame,
    bool stage_exit_requested,
    bool direct_command_active,
    uint64_t processed_frames) {
  StageFacadeInputLite input;
  input.branch = StageFacadeBranchForInput(command_sample,
                                           stage_exit_requested);
  input.frame.seq = metadata.seq;
  input.frame.frame_id = metadata.frame_id;
  input.frame.data_stamp_ms = metadata.data_stamp_ms;
  input.frame.selected_slot_valid = input_sample.is_valid();
  input.frame.selected_slot_count = input_sample.count();
  input.frame.parking_lot_count = input_sample.parking_lots().size();
  input.frame.selected_parking_seq = input_sample.opt_parking_seq();
  input.frame.vehicle_state_valid = vehicle_input.has_vehicle_state;
  input.frame.vehicle_state_source =
      vehicle_input.has_vehicle_state ? "dds_aux_vehicle" : "config_fake_vehicle";
  input.frame.obstacle_count = obstacle_count;

  input.open_space_info.parking_type = function_projection.parking_type;
  input.open_space_info.target_parking_seq = input_sample.opt_parking_seq();

  input.planning_context.path_history_available = provider_state.has_history;
  input.planning_context.path_id = provider_state.path_id;
  input.planning_context.speed_frame_available = has_last_speed_frame;
  input.planning_context.stage_exit_requested = stage_exit_requested;
  input.planning_context.direct_command_active = direct_command_active;
  input.planning_context.processed_frames = processed_frames;

  input.function_manager.source = function_projection.source;
  input.function_manager.sys_command = function_projection.sys_command;
  input.function_manager.sys_run_state = function_projection.sys_run_state;
  input.function_manager.parking_type = function_projection.parking_type;
  input.function_manager.command = function_projection.parking_command;
  input.function_manager.reset_history = function_projection.reset_history;

  const bool has_command =
      command_sample != nullptr && command_sample->is_valid();
  const ParkingCommandMode command_mode =
      has_command ? command_sample->mode()
                  : ParkingCommandMode::PARKING_COMMAND_NONE;
  input.finish_requested =
      has_command && command_mode == ParkingCommandMode::PARKING_COMMAND_FINISH;

  return input;
}

TL::perception::PSPoint::Position MapPointPosition(PsPointPosition position) {
  switch (position) {
    case PsPointPosition::PS_POSITION_TOP_LEFT:
      return TL::perception::PSPoint::TOP_LEFT;
    case PsPointPosition::PS_POSITION_TOP_RIGHT:
      return TL::perception::PSPoint::TOP_RIGHT;
    case PsPointPosition::PS_POSITION_BOTTOM_LEFT:
      return TL::perception::PSPoint::BOTTOM_LEFT;
    case PsPointPosition::PS_POSITION_BOTTOM_RIGHT:
      return TL::perception::PSPoint::BOTTOM_RIGHT;
    case PsPointPosition::PS_POSITION_STOP_LEFT:
      return TL::perception::PSPoint::STOP_LEFT;
    case PsPointPosition::PS_POSITION_STOP_RIGHT:
      return TL::perception::PSPoint::STOP_RIGHT;
  }
  return TL::perception::PSPoint::TOP_LEFT;
}

TL::perception::PSPoint::Quality MapPointQuality(PsPointQuality quality) {
  switch (quality) {
    case PsPointQuality::PS_QUALITY_LOW:
      return TL::perception::PSPoint::LOW;
    case PsPointQuality::PS_QUALITY_HIGH:
      return TL::perception::PSPoint::HIGH;
  }
  return TL::perception::PSPoint::LOW;
}

TL::perception::ParkingLotOut::ParkType MapParkingType(ParkingType type) {
  switch (type) {
    case ParkingType::PARKING_TYPE_VERTICAL:
      return TL::perception::ParkingLotOut::VERTICAL;
    case ParkingType::PARKING_TYPE_LATERAL:
      return TL::perception::ParkingLotOut::LATERAL;
    case ParkingType::PARKING_TYPE_OBLIQUE:
      return TL::perception::ParkingLotOut::OBLIQUE;
    case ParkingType::PARKING_TYPE_NONE:
      return TL::perception::ParkingLotOut::NONE;
  }
  return TL::perception::ParkingLotOut::NONE;
}

TL::perception::ParkingLotOut::ParkStatus MapParkingStatus(ParkingStatus status) {
  switch (status) {
    case ParkingStatus::PARKING_STATUS_FREE:
      return TL::perception::ParkingLotOut::FREE;
    case ParkingStatus::PARKING_STATUS_OCCUPIED:
      return TL::perception::ParkingLotOut::OCCUPIED;
    case ParkingStatus::PARKING_STATUS_UNKOWN:
      return TL::perception::ParkingLotOut::UNKNOWN;
  }
  return TL::perception::ParkingLotOut::UNKNOWN;
}

TL::perception::ParkingLotOut::SenType MapParkingSensorType(ParkingSensorType type) {
  switch (type) {
    case ParkingSensorType::PARKING_SENSOR_CAMERA:
      return TL::perception::ParkingLotOut::CAMERA;
    case ParkingSensorType::PARKING_SENSOR_MAP:
      return TL::perception::ParkingLotOut::MAP;
    case ParkingSensorType::PARKING_SENSOR_CAM_MAP_FUSION:
      return TL::perception::ParkingLotOut::CAM_MAP_FUSION;
    case ParkingSensorType::PARKING_SENSOR_USS:
      return TL::perception::ParkingLotOut::USS;
  }
  return TL::perception::ParkingLotOut::CAMERA;
}

TL::perception::ParkingLotOut::ParkSpaceSize MapParkingSpaceSize(ParkingSpaceSize size) {
  switch (size) {
    case ParkingSpaceSize::PARKING_SPACE_SIZE_NORMAL:
      return TL::perception::ParkingLotOut::NORMAL;
    case ParkingSpaceSize::PARKING_SPACE_SIZE_SMALL:
      return TL::perception::ParkingLotOut::SMALL;
    case ParkingSpaceSize::PARKING_SPACE_SIZE_NARROW:
      return TL::perception::ParkingLotOut::NARROW;
  }
  return TL::perception::ParkingLotOut::NORMAL;
}

TL::perception::PSPoint ConvertPoint(const PsPoint& input) {
  TL::perception::PSPoint output;
  output.point.x = input.point().x();
  output.point.y = input.point().y();
  output.point.z = input.point().z();
  output.position = MapPointPosition(input.position());
  output.quality = MapPointQuality(input.quality());
  return output;
}

std::vector<TL::perception::PSPoint> ConvertPoints(const std::vector<PsPoint>& input) {
  std::vector<TL::perception::PSPoint> output;
  output.reserve(input.size());
  for (const PsPoint& point : input) {
    output.push_back(ConvertPoint(point));
  }
  return output;
}

bool HasUsableCorners(const std::vector<TL::perception::PSPoint>& points) {
  if (points.size() < 4U) {
    return false;
  }

  bool has_top_left = false;
  bool has_top_right = false;
  bool has_bottom_left = false;
  bool has_bottom_right = false;
  for (const TL::perception::PSPoint& point : points) {
    if (!std::isfinite(point.point.x) || !std::isfinite(point.point.y) ||
        !std::isfinite(point.point.z)) {
      return false;
    }

    switch (point.position) {
      case TL::perception::PSPoint::TOP_LEFT:
        has_top_left = true;
        break;
      case TL::perception::PSPoint::TOP_RIGHT:
        has_top_right = true;
        break;
      case TL::perception::PSPoint::BOTTOM_LEFT:
        has_bottom_left = true;
        break;
      case TL::perception::PSPoint::BOTTOM_RIGHT:
        has_bottom_right = true;
        break;
      case TL::perception::PSPoint::STOP_LEFT:
      case TL::perception::PSPoint::STOP_RIGHT:
        break;
    }
  }

  return has_top_left && has_top_right && has_bottom_left && has_bottom_right;
}

bool HasUsableCornerGeometry(
    const std::vector<TL::perception::PSPoint>& points) {
  constexpr double kMinCornerSpan = 1.0e-3;
  constexpr double kMinCornerArea = 1.0e-4;
  double min_x = std::numeric_limits<double>::infinity();
  double min_y = std::numeric_limits<double>::infinity();
  double max_x = -std::numeric_limits<double>::infinity();
  double max_y = -std::numeric_limits<double>::infinity();
  std::vector<TL::common::math::Vec2d> corners;
  corners.reserve(points.size());

  for (const TL::perception::PSPoint& point : points) {
    if (point.position != TL::perception::PSPoint::TOP_LEFT &&
        point.position != TL::perception::PSPoint::TOP_RIGHT &&
        point.position != TL::perception::PSPoint::BOTTOM_LEFT &&
        point.position != TL::perception::PSPoint::BOTTOM_RIGHT) {
      continue;
    }
    min_x = std::min(min_x, point.point.x);
    min_y = std::min(min_y, point.point.y);
    max_x = std::max(max_x, point.point.x);
    max_y = std::max(max_y, point.point.y);
    corners.emplace_back(point.point.x, point.point.y);
  }

  if (corners.size() < 4U || !std::isfinite(min_x) || !std::isfinite(min_y) ||
      !std::isfinite(max_x) || !std::isfinite(max_y) ||
      max_x - min_x < kMinCornerSpan || max_y - min_y < kMinCornerSpan) {
    return false;
  }

  TL::common::math::Vec2d center(0.0, 0.0);
  for (const TL::common::math::Vec2d& corner : corners) {
    center += corner;
  }
  center /= static_cast<double>(corners.size());
  std::sort(corners.begin(), corners.end(),
            [&center](const TL::common::math::Vec2d& lhs,
                      const TL::common::math::Vec2d& rhs) {
              return std::atan2(lhs.y() - center.y(), lhs.x() - center.x()) <
                     std::atan2(rhs.y() - center.y(), rhs.x() - center.x());
            });

  const TL::common::math::Polygon2d corner_polygon(corners);
  return std::isfinite(corner_polygon.area()) &&
         std::fabs(corner_polygon.area()) >= kMinCornerArea;
}

bool ConvertParkingLot(const ParkingLot& input,
                       TL::perception::ParkingLotOut* output,
                       std::string* status_reason) {
  if (output == nullptr || status_reason == nullptr) {
    return false;
  }

  TL::perception::ParkingLotOut lot;
  lot.parking_seq = input.parking_seq();
  lot.type = MapParkingType(input.type());
  lot.status = MapParkingStatus(input.status());
  lot.sensor_type = MapParkingSensorType(input.sensor_type());
  lot.is_private_ps = input.is_private_ps();
  lot.time_creation = input.time_creation();
  lot.hmi_angle = input.hmi_angle();
  lot.hmi_depth = input.hmi_depth();
  lot.hmi_width = input.hmi_width();
  lot.hmi_direction = input.hmi_direction();
  lot.hmi_type = input.hmi_type();
  lot.hmi_status = input.hmi_status();
  lot.park_size = MapParkingSpaceSize(input.park_size());

  const std::vector<TL::perception::PSPoint> enu_points = ConvertPoints(input.pts_enu());
  const std::vector<TL::perception::PSPoint> vrf_points = ConvertPoints(input.pts_vrf());
  lot.pts_enu = enu_points;
  lot.pts_vrf = !enu_points.empty() ? enu_points : vrf_points;

  if (!HasUsableCorners(lot.pts_vrf)) {
    *status_reason = "selected parking lot has insufficient or invalid corner points";
    return false;
  }

  if (!HasUsableCornerGeometry(lot.pts_vrf)) {
    *status_reason = "selected parking lot corner geometry is degenerate";
    return false;
  }

  if (!std::isfinite(lot.hmi_angle) || !std::isfinite(lot.hmi_depth) ||
      !std::isfinite(lot.hmi_width)) {
    *status_reason = "selected parking lot contains non-finite hmi geometry";
    return false;
  }

  if (lot.hmi_depth <= 0.0 || lot.hmi_width <= 0.0) {
    *status_reason = "selected parking lot geometry must be positive";
    return false;
  }

  *output = std::move(lot);
  return true;
}

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

bool IsFiniteVec2d(const TL::common::math::Vec2d& point) {
  return std::isfinite(point.x()) && std::isfinite(point.y());
}

bool IsFinitePathPose(const TL::common::PathPoint& point) {
  return std::isfinite(point.x) && std::isfinite(point.y) &&
         std::isfinite(point.z) && std::isfinite(point.theta);
}

bool ValidateVehicleNearParkingLot(
    const TL::perception::ParkingLotOut& parking_lot,
    const RuntimeVehicleInput& vehicle_input,
    std::string* status_reason) {
  if (!vehicle_input.has_vehicle_state) {
    return true;
  }
  if (status_reason == nullptr) {
    return false;
  }

  double min_x = std::numeric_limits<double>::infinity();
  double min_y = std::numeric_limits<double>::infinity();
  double max_x = -std::numeric_limits<double>::infinity();
  double max_y = -std::numeric_limits<double>::infinity();
  for (const TL::perception::PSPoint& point : parking_lot.pts_vrf) {
    min_x = std::min(min_x, point.point.x);
    min_y = std::min(min_y, point.point.y);
    max_x = std::max(max_x, point.point.x);
    max_y = std::max(max_y, point.point.y);
  }
  if (!std::isfinite(min_x) || !std::isfinite(min_y) ||
      !std::isfinite(max_x) || !std::isfinite(max_y) ||
      min_x >= max_x || min_y >= max_y) {
    *status_reason = "vehicle_lot_precheck failed: invalid parking lot AABB";
    return false;
  }

  const double span_x = max_x - min_x;
  const double span_y = max_y - min_y;
  const double margin = std::max(20.0, 4.0 * std::max(span_x, span_y));
  if (vehicle_input.x < min_x - margin || vehicle_input.x > max_x + margin ||
      vehicle_input.y < min_y - margin || vehicle_input.y > max_y + margin) {
    std::ostringstream stream;
    stream << "vehicle_lot_precheck failed: vehicle outside selected lot "
           << "envelope, vehicle=(" << vehicle_input.x << ","
           << vehicle_input.y << "), lot_aabb=[" << min_x << "," << max_x
           << "," << min_y << "," << max_y << "], margin=" << margin;
    *status_reason = stream.str();
    return false;
  }
  return true;
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

PathProviderPreCheckResult MakePathProviderPreCheckFailure(
    const std::string& reason) {
  PathProviderPreCheckResult result;
  result.ok = false;
  result.reason = reason;
  return result;
}

TL::common::math::Vec2d TransformPointToRoiLocal(
    const TL::common::math::Vec2d& origin_point,
    double origin_heading,
    double x,
    double y) {
  const double dx = x - origin_point.x();
  const double dy = y - origin_point.y();
  const double cos_angle = std::cos(-origin_heading);
  const double sin_angle = std::sin(-origin_heading);
  return TL::common::math::Vec2d(
      dx * cos_angle - dy * sin_angle,
      dx * sin_angle + dy * cos_angle);
}

bool IsLocalPointNearBounds(const TL::common::math::Vec2d& point,
                            const std::vector<double>& xy_bounds,
                            double margin) {
  return point.x() >= xy_bounds[0] - margin &&
         point.x() <= xy_bounds[1] + margin &&
         point.y() >= xy_bounds[2] - margin &&
         point.y() <= xy_bounds[3] + margin;
}

PathProviderPreCheckResult RunPathProviderPreCheck(
    const valet_parking_config_t& config,
    const TL::planning::RoiDeciderOutput& roi_output,
    const RuntimeVehicleInput& vehicle_input,
    const std::vector<RuntimeObstacleInput>& obstacles) {
  if (!IsFiniteVec2d(roi_output.origin_point) ||
      !std::isfinite(roi_output.origin_heading)) {
    return MakePathProviderPreCheckFailure("non_finite_roi_origin");
  }

  if (roi_output.xy_bounds.size() < 4U) {
    std::ostringstream stream;
    stream << "xy_bounds_size=" << roi_output.xy_bounds.size();
    return MakePathProviderPreCheckFailure(stream.str());
  }
  for (std::size_t i = 0U; i < 4U; ++i) {
    if (!std::isfinite(roi_output.xy_bounds[i])) {
      std::ostringstream stream;
      stream << "non_finite_xy_bounds[" << i << "]";
      return MakePathProviderPreCheckFailure(stream.str());
    }
  }
  if (roi_output.xy_bounds[0] >= roi_output.xy_bounds[1] ||
      roi_output.xy_bounds[2] >= roi_output.xy_bounds[3]) {
    std::ostringstream stream;
    stream << "invalid_xy_bounds_order=["
           << roi_output.xy_bounds[0] << "," << roi_output.xy_bounds[1]
           << "," << roi_output.xy_bounds[2] << ","
           << roi_output.xy_bounds[3] << "]";
    return MakePathProviderPreCheckFailure(stream.str());
  }

  PathProviderPreCheckResult result;
  result.vehicle_has_state = vehicle_input.has_vehicle_state;
  result.xy_width = roi_output.xy_bounds[1] - roi_output.xy_bounds[0];
  result.xy_height = roi_output.xy_bounds[3] - roi_output.xy_bounds[2];
  constexpr double kMinRoiSpan = 0.50;
  if (result.xy_width < kMinRoiSpan || result.xy_height < kMinRoiSpan) {
    std::ostringstream stream;
    stream << "xy_bounds_too_small="
           << result.xy_width << "x" << result.xy_height;
    return MakePathProviderPreCheckFailure(stream.str());
  }

  const TL::common::PathPoint start_point =
      BuildStartPathPoint(config, vehicle_input);
  const TL::common::PathPoint end_pose =
      roi_output.has_fine_tuned ? roi_output.fine_tuned_end_pose
                                : roi_output.end_pose;
  if (!IsFinitePathPose(start_point)) {
    return MakePathProviderPreCheckFailure("non_finite_start_point");
  }
  if (!IsFinitePathPose(end_pose)) {
    return MakePathProviderPreCheckFailure("non_finite_end_pose");
  }

  const TL::common::math::Vec2d start_local = TransformPointToRoiLocal(
      roi_output.origin_point, roi_output.origin_heading,
      start_point.x, start_point.y);
  const TL::common::math::Vec2d end_local = TransformPointToRoiLocal(
      roi_output.origin_point, roi_output.origin_heading,
      end_pose.x, end_pose.y);
  if (!IsFiniteVec2d(start_local) || !IsFiniteVec2d(end_local)) {
    return MakePathProviderPreCheckFailure("non_finite_local_pose");
  }

  constexpr double kLocalBoundsMargin = 10.0;
  if (!IsLocalPointNearBounds(start_local, roi_output.xy_bounds,
                              kLocalBoundsMargin)) {
    std::ostringstream stream;
    stream << "start_outside_xy_bounds_local=("
           << start_local.x() << "," << start_local.y() << ")";
    return MakePathProviderPreCheckFailure(stream.str());
  }
  if (!IsLocalPointNearBounds(end_local, roi_output.xy_bounds,
                              kLocalBoundsMargin)) {
    std::ostringstream stream;
    stream << "end_outside_xy_bounds_local=("
           << end_local.x() << "," << end_local.y() << ")";
    return MakePathProviderPreCheckFailure(stream.str());
  }

  const TL::common::math::Polygon2d& dest_polygon =
      std::get<0>(roi_output.dest_region);
  result.dest_region_point_count =
      static_cast<std::size_t>(dest_polygon.num_points());
  if (result.dest_region_point_count < 3U) {
    std::ostringstream stream;
    stream << "dest_region_points=" << result.dest_region_point_count;
    return MakePathProviderPreCheckFailure(stream.str());
  }
  if (!std::isfinite(std::get<1>(roi_output.dest_region)) ||
      !std::isfinite(std::get<2>(roi_output.dest_region))) {
    return MakePathProviderPreCheckFailure("non_finite_dest_region_angle");
  }
  if (!std::isfinite(dest_polygon.area()) ||
      std::fabs(dest_polygon.area()) < 1.0e-6) {
    return MakePathProviderPreCheckFailure("invalid_dest_region_area");
  }
  for (const TL::common::math::Vec2d& point : dest_polygon.points()) {
    if (!IsFiniteVec2d(point)) {
      return MakePathProviderPreCheckFailure("non_finite_dest_region_point");
    }
  }

  const std::vector<std::pair<TL::common::math::LineSegment2d, double>>
      obstacle_segments = BuildObstacleSegments(roi_output, obstacles);
  result.obstacle_segment_count = obstacle_segments.size();
  result.roi_obstacle_segment_count = roi_output.obs_segments.size();
  if (result.obstacle_segment_count >= result.roi_obstacle_segment_count) {
    result.external_obstacle_segment_count =
        result.obstacle_segment_count - result.roi_obstacle_segment_count;
  }
  // 128 box obstacles generate 512 segments; keep that boundary out of search.
  constexpr std::size_t kMaxPathProviderObstacleSegments = 500U;
  if (result.obstacle_segment_count > kMaxPathProviderObstacleSegments) {
    std::ostringstream stream;
    stream << "too_many_obstacle_segments="
           << result.obstacle_segment_count;
    return MakePathProviderPreCheckFailure(stream.str());
  }
  constexpr double kObstacleLocalBoundsMargin = 250.0;
  for (std::size_t i = 0U; i < obstacle_segments.size(); ++i) {
    const TL::common::math::LineSegment2d& segment =
        obstacle_segments[i].first;
    const double buffer = obstacle_segments[i].second;
    if (!IsFiniteVec2d(segment.start()) || !IsFiniteVec2d(segment.end()) ||
        !std::isfinite(segment.length()) || segment.length() <= 1.0e-6) {
      std::ostringstream stream;
      stream << "invalid_obstacle_segment[" << i << "]";
      return MakePathProviderPreCheckFailure(stream.str());
    }
    if (!std::isfinite(buffer) || buffer < 0.0) {
      std::ostringstream stream;
      stream << "invalid_obstacle_buffer[" << i << "]=" << buffer;
      return MakePathProviderPreCheckFailure(stream.str());
    }
    const TL::common::math::Vec2d segment_start_local =
        TransformPointToRoiLocal(roi_output.origin_point,
                                 roi_output.origin_heading,
                                 segment.start().x(), segment.start().y());
    const TL::common::math::Vec2d segment_end_local =
        TransformPointToRoiLocal(roi_output.origin_point,
                                 roi_output.origin_heading,
                                 segment.end().x(), segment.end().y());
    if (!IsFiniteVec2d(segment_start_local) ||
        !IsFiniteVec2d(segment_end_local)) {
      std::ostringstream stream;
      stream << "non_finite_obstacle_segment_local[" << i << "]";
      return MakePathProviderPreCheckFailure(stream.str());
    }
    if (!IsLocalPointNearBounds(segment_start_local, roi_output.xy_bounds,
                                kObstacleLocalBoundsMargin) ||
        !IsLocalPointNearBounds(segment_end_local, roi_output.xy_bounds,
                                kObstacleLocalBoundsMargin)) {
      std::ostringstream stream;
      stream << "obstacle_segment_outside_xy_bounds[" << i << "]"
             << " local_start=(" << segment_start_local.x() << ","
             << segment_start_local.y() << ")"
             << ", local_end=(" << segment_end_local.x() << ","
             << segment_end_local.y() << ")";
      return MakePathProviderPreCheckFailure(stream.str());
    }
    if (i >= result.roi_obstacle_segment_count) {
      const TL::common::math::LineSegment2d local_segment(
          segment_start_local, segment_end_local);
      if (local_segment.DistanceTo(start_local) <=
          kPathProviderPreCheckObstacleNearPoseThresholdM) {
        ++result.obstacle_near_start_count;
      }
      if (local_segment.DistanceTo(end_local) <=
          kPathProviderPreCheckObstacleNearPoseThresholdM) {
        ++result.obstacle_near_end_count;
      }
    }
  }

  return result;
}

::GearPosition ConvertGear(TL::soc::GearPosition gear) {
  switch (gear) {
    case TL::soc::GearPosition::GEAR_DRIVE:
      return ::GearPosition::GEAR_DRIVE;
    case TL::soc::GearPosition::GEAR_REVERSE:
      return ::GearPosition::GEAR_REVERSE;
    case TL::soc::GearPosition::GEAR_NEUTRAL:
      return ::GearPosition::GEAR_NEUTRAL;
    case TL::soc::GearPosition::GEAR_PARKING:
      return ::GearPosition::GEAR_PARKING;
    case TL::soc::GearPosition::GEAR_LOW:
      return ::GearPosition::GEAR_LOW;
    case TL::soc::GearPosition::GEAR_NONE:
      return ::GearPosition::GEAR_NONE;
    case TL::soc::GearPosition::GEAR_INVALID:
      return ::GearPosition::GEAR_INVALID;
  }
  return ::GearPosition::GEAR_INVALID;
}

TL::planning::SpaceStructure BuildSpaceStructure(
    TL::planning::ParkingScenarioType scenario_type) {
  switch (scenario_type) {
    case TL::planning::LEFT_LATERAL_PARKING_IN:
    case TL::planning::RIGHT_LATERAL_PARKING_IN:
    case TL::planning::LEFT_LATERAL_PARKING_OUT:
    case TL::planning::RIGHT_LATERAL_PARKING_OUT:
      return TL::planning::SpaceStructure::LAT_PARK_LOT;
    case TL::planning::LEFT_VERTICAL_PARKING_IN:
    case TL::planning::RIGHT_VERTICAL_PARKING_IN:
    case TL::planning::LEFT_OBLIQUE_PARKING_IN:
    case TL::planning::RIGHT_OBLIQUE_PARKING_IN:
    case TL::planning::LEFT_VERTICAL_PARKING_OUT:
    case TL::planning::RIGHT_VERTICAL_PARKING_OUT:
    case TL::planning::FORWARD_VERTICAL_PARKING_OUT:
    case TL::planning::BACKWARD_VERTICAL_PARKING_OUT:
    case TL::planning::LEFT_OBLIQUE_PARKING_OUT:
    case TL::planning::RIGHT_OBLIQUE_PARKING_OUT:
    case TL::planning::FORWARD_OBLIQUE_PARKING_OUT:
    case TL::planning::BACKWARD_OBLIQUE_PARKING_OUT:
      return TL::planning::SpaceStructure::VER_PARK_LOT;
    case TL::planning::DEFAULT_TYPE:
    case TL::planning::REVERSE_TURN:
    case TL::planning::LEFT_U_TYPE_TURN:
    case TL::planning::RIGHT_U_TYPE_TURN:
    case TL::planning::FREESPACE_FORWARD_EXPLORATION:
    case TL::planning::CONTROL_CALIBRATION_MODE:
      return TL::planning::SpaceStructure::DEFAULT;
  }
  return TL::planning::SpaceStructure::DEFAULT;
}

TL::planning::HybridAStarConfig BuildPathProviderConfig() {
  TL::planning::HybridAStarConfig config = TL::planning::GetParkingFastConfig();
  config.max_iterations = 20000;
  config.accept_exploration_time = 3.0;
  config.max_exploration_time = 8.0;
  config.dead_end_scenario_max_exploration_time = 8.0;
  config.max_search_time = 8.0;
  return config;
}

double ReadPathProviderTimeoutS() {
  const char* env_value = std::getenv(kPathProviderTimeoutEnv);
  if (env_value == nullptr || env_value[0] == '\0') {
    return kDefaultPathProviderTimeoutS;
  }

  errno = 0;
  char* end = nullptr;
  const double parsed = std::strtod(env_value, &end);
  if (errno != 0 || end == nullptr || *end != '\0' ||
      !std::isfinite(parsed) || parsed <= 0.0 || parsed > 60.0) {
    return kDefaultPathProviderTimeoutS;
  }
  return parsed;
}

bool IsSmokeFlagEnabled(const char* name) {
  const char* env_value = std::getenv(name);
  if (env_value == nullptr || env_value[0] == '\0') {
    return false;
  }

  const std::string value(env_value);
  return value == "1" || value == "true" || value == "TRUE" ||
         value == "yes" || value == "YES" || value == "on" ||
         value == "ON";
}

TL::planning::OpenSpacePathProvider* EnsureThreadedPathProvider(
    PathProviderRuntimeState* provider_state) {
  if (provider_state == nullptr) {
    return nullptr;
  }
  if (provider_state->threaded_provider == nullptr) {
    TL::planning::OpenSpacePathProviderConfig provider_config;
    provider_config.hybrid_config = BuildPathProviderConfig();
    provider_config.vehicle_param = TL::planning::LoadEP30VehicleParam();
    provider_config.search_thread_num = 4U;
    provider_config.target_plan_timeout_s = ReadPathProviderTimeoutS();
    provider_state->threaded_provider =
        std::make_unique<TL::planning::OpenSpacePathProvider>(
            provider_config);
  }
  return provider_state->threaded_provider.get();
}

constexpr uint32_t kReplanNoValidPath = 1U;
constexpr uint32_t kReplanTargetUpdate = 2U;
constexpr uint32_t kReplanBlockByStaticObstacle = 4U;
constexpr uint32_t kReplanDynamicReplan = 256U;
constexpr uint32_t kReplanTraceReplan = 512U;
constexpr uint32_t kReplanForSpeedWarn = 4096U;
constexpr double kTraceAdjustBound = 30.0;
constexpr double kTraceAdjustLatDiffThreshold = 0.05;
constexpr double kTraceAdjustThetaDiffThreshold = 0.02;
constexpr double kTraceAdjustTargetS = 8.0;
constexpr double kMinTraceAdjustPathLength = 0.50;

uint64_t HashCombine(uint64_t seed, uint64_t value) {
  constexpr uint64_t kMagic = 0x9e3779b97f4a7c15ULL;
  return seed ^ (value + kMagic + (seed << 6U) + (seed >> 2U));
}

uint64_t QuantizeForHash(double value, double resolution) {
  if (!std::isfinite(value) || resolution <= 0.0) {
    return 0U;
  }
  return static_cast<uint64_t>(
      static_cast<int64_t>(std::llround(value / resolution)));
}

uint64_t BuildObstacleSignature(
    const std::vector<RuntimeObstacleInput>& obstacles) {
  uint64_t signature = 1469598103934665603ULL;
  signature = HashCombine(signature, static_cast<uint64_t>(obstacles.size()));
  for (const RuntimeObstacleInput& obstacle : obstacles) {
    signature = HashCombine(signature, static_cast<uint64_t>(obstacle.id));
    signature = HashCombine(signature, static_cast<uint64_t>(obstacle.type));
    signature = HashCombine(signature, obstacle.is_dynamic ? 1ULL : 0ULL);
    signature = HashCombine(signature,
                            QuantizeForHash(obstacle.center_x, 0.01));
    signature = HashCombine(signature,
                            QuantizeForHash(obstacle.center_y, 0.01));
    signature = HashCombine(signature,
                            QuantizeForHash(obstacle.heading, 0.001));
    signature = HashCombine(signature,
                            QuantizeForHash(obstacle.length, 0.01));
    signature = HashCombine(signature,
                            QuantizeForHash(obstacle.width, 0.01));
    signature = HashCombine(signature,
                            QuantizeForHash(obstacle.velocity_x, 0.01));
    signature = HashCombine(signature,
                            QuantizeForHash(obstacle.velocity_y, 0.01));
  }
  return signature;
}

bool IsSamePathPoint(const TL::common::PathPoint& lhs,
                     const TL::common::PathPoint& rhs,
                     double distance_threshold,
                     double heading_threshold) {
  return std::hypot(lhs.x - rhs.x, lhs.y - rhs.y) < distance_threshold &&
         std::fabs(NormalizeAngle(lhs.theta - rhs.theta)) < heading_threshold;
}

bool HasUsablePathOutput(
    const TL::planning::OpenSpacePathOutput& path_output) {
  std::size_t point_count = 0U;
  for (const auto& path_pair : path_output.partitioned_path) {
    point_count += path_pair.first.size();
  }
  return path_output.error_msg.empty() &&
         !path_output.partitioned_path.empty() &&
         point_count >= 2U;
}

bool IsHistoryPathReusable(
    const PathProviderRuntimeState& state,
    const TL::common::PathPoint& start_point) {
  if (!state.has_history || !HasUsablePathOutput(state.last_output)) {
    return false;
  }

  for (const TL::planning::PathGearPair& path_pair :
       state.last_output.partitioned_path) {
    const TL::planning::DiscretizedPath& path = path_pair.first;
    if (path.empty()) {
      continue;
    }
    if (path.IsPointIn(start_point, 0.30, 0.35) ||
        TL::planning::DiscretizedPath::IsSamePoint(path.front(), start_point,
                                                   0.30, 0.35)) {
      return true;
    }
  }
  return false;
}

void AppendReason(std::string* text, const std::string& reason) {
  if (text == nullptr || reason.empty()) {
    return;
  }
  if (!text->empty()) {
    *text += "|";
  }
  *text += reason;
}

std::string ReplanStatusToString(uint32_t status) {
  if (status == 0U) {
    return "NONE";
  }

  std::string text;
  if ((status & kReplanNoValidPath) != 0U) {
    AppendReason(&text, "NO_VALID_PATH");
  }
  if ((status & kReplanTargetUpdate) != 0U) {
    AppendReason(&text, "TARGET_UPDATE");
  }
  if ((status & kReplanBlockByStaticObstacle) != 0U) {
    AppendReason(&text, "BLOCK_BY_STATIC_OBSTACLE");
  }
  if ((status & kReplanDynamicReplan) != 0U) {
    AppendReason(&text, "DYNAMIC_REPLAN");
  }
  if ((status & kReplanTraceReplan) != 0U) {
    AppendReason(&text, "TRACE_REPLAN");
  }
  if ((status & kReplanForSpeedWarn) != 0U) {
    AppendReason(&text, "REPLAN_FOR_SPEED_WARN");
  }
  return text.empty() ? "UNKNOWN" : text;
}

int GearToMovingDirection(TL::soc::GearPosition gear) {
  if (gear == TL::soc::GearPosition::GEAR_DRIVE) {
    return 1;
  }
  if (gear == TL::soc::GearPosition::GEAR_REVERSE) {
    return -1;
  }
  return 0;
}

void RebaseWarmStartPath(const TL::common::PathPoint& start_point,
                         TL::planning::DiscretizedPath* warm_start_path) {
  if (warm_start_path == nullptr || warm_start_path->empty()) {
    return;
  }

  warm_start_path->front() = start_point;
  warm_start_path->front().s = 0.0;
  warm_start_path->front().theta =
      NormalizeAngle(warm_start_path->front().theta);
  warm_start_path->front().x_derivative =
      std::cos(warm_start_path->front().theta);
  warm_start_path->front().y_derivative =
      std::sin(warm_start_path->front().theta);

  double accumulated_s = 0.0;
  for (std::size_t i = 1U; i < warm_start_path->size(); ++i) {
    TL::common::PathPoint& current_point = (*warm_start_path)[i];
    const TL::common::PathPoint& previous_point = (*warm_start_path)[i - 1U];
    accumulated_s += std::hypot(current_point.x - previous_point.x,
                                current_point.y - previous_point.y);
    current_point.s = accumulated_s;
    current_point.theta = NormalizeAngle(current_point.theta);
    current_point.x_derivative = std::cos(current_point.theta);
    current_point.y_derivative = std::sin(current_point.theta);
  }
}

bool ShouldUseHistoryWarmStart(uint32_t replan_status) {
  const uint32_t unsafe_history_reasons =
      kReplanTargetUpdate | kReplanBlockByStaticObstacle |
      kReplanDynamicReplan | kReplanForSpeedWarn;
  return (replan_status & kReplanTraceReplan) != 0U &&
         (replan_status & unsafe_history_reasons) == 0U;
}

struct WarmStartBuildDiagnostics {
  std::string reject_reason{"not_attempted"};
  std::size_t history_points{0U};
  double start_s{0.0};
  double start_l{0.0};
  double path_front_s{0.0};
  double path_back_s{0.0};
};

struct HistoryPathProjection {
  double s{0.0};
  double l{0.0};
  double path_length{0.0};
  std::size_t next_point_index{0U};
};

void UpdateWarmStartDiagnostics(
    WarmStartBuildDiagnostics* diagnostics,
    const std::string& reason,
    const TL::planning::DiscretizedPath* path,
    const TL::common::SLPoint* start_sl) {
  if (diagnostics == nullptr) {
    return;
  }
  diagnostics->reject_reason = reason;
  if (path != nullptr) {
    diagnostics->history_points = path->size();
    if (!path->empty()) {
      diagnostics->path_front_s = path->front().s;
      diagnostics->path_back_s = path->back().s;
    }
  }
  if (start_sl != nullptr) {
    diagnostics->start_s = start_sl->s;
    diagnostics->start_l = start_sl->l;
  }
}

void UpdateWarmStartProjectionDiagnostics(
    WarmStartBuildDiagnostics* diagnostics,
    const std::string& reason,
    const TL::planning::DiscretizedPath& path,
    const HistoryPathProjection& projection) {
  if (diagnostics == nullptr) {
    return;
  }
  diagnostics->reject_reason = reason;
  diagnostics->history_points = path.size();
  diagnostics->start_s = projection.s;
  diagnostics->start_l = projection.l;
  diagnostics->path_front_s = 0.0;
  diagnostics->path_back_s = projection.path_length;
}

bool ProjectPointToPathGeometry(
    const TL::planning::DiscretizedPath& path,
    const TL::common::PathPoint& point,
    HistoryPathProjection* projection) {
  if (projection == nullptr || path.size() < 2U) {
    return false;
  }

  constexpr double kMinSegmentLength = 1.0e-6;
  double accumulated_s = 0.0;
  double best_distance_square = std::numeric_limits<double>::infinity();
  bool found_projection = false;
  HistoryPathProjection best_projection;

  for (std::size_t i = 0U; i + 1U < path.size(); ++i) {
    const TL::common::PathPoint& segment_start = path[i];
    const TL::common::PathPoint& segment_end = path[i + 1U];
    const double dx = segment_end.x - segment_start.x;
    const double dy = segment_end.y - segment_start.y;
    const double length_square = dx * dx + dy * dy;
    if (length_square < kMinSegmentLength * kMinSegmentLength) {
      continue;
    }

    const double segment_length = std::sqrt(length_square);
    const double point_dx = point.x - segment_start.x;
    const double point_dy = point.y - segment_start.y;
    double ratio = (point_dx * dx + point_dy * dy) / length_square;
    ratio = std::max(0.0, std::min(1.0, ratio));
    const double projection_x = segment_start.x + ratio * dx;
    const double projection_y = segment_start.y + ratio * dy;
    const double distance_dx = point.x - projection_x;
    const double distance_dy = point.y - projection_y;
    const double distance_square =
        distance_dx * distance_dx + distance_dy * distance_dy;
    if (distance_square < best_distance_square) {
      best_distance_square = distance_square;
      best_projection.s = accumulated_s + ratio * segment_length;
      best_projection.l = (dx * point_dy - dy * point_dx) / segment_length;
      best_projection.next_point_index =
          ratio >= 1.0 ? i + 2U : i + 1U;
      found_projection = true;
    }
    accumulated_s += segment_length;
  }

  if (!found_projection || accumulated_s < kMinSegmentLength) {
    return false;
  }
  best_projection.path_length = accumulated_s;
  if (best_projection.next_point_index > path.size()) {
    best_projection.next_point_index = path.size();
  }
  *projection = best_projection;
  return true;
}

bool BuildWarmStartPathFromHistory(
    const PathProviderRuntimeState& state,
    const TL::common::PathPoint& start_point,
    uint32_t replan_status,
    TL::planning::DiscretizedPath* warm_start_path,
    TL::soc::GearPosition* warm_start_gear,
    WarmStartBuildDiagnostics* diagnostics) {
  if (warm_start_path == nullptr || warm_start_gear == nullptr) {
    return false;
  }

  warm_start_path->clear();
  *warm_start_gear = TL::soc::GearPosition::GEAR_NONE;
  UpdateWarmStartDiagnostics(diagnostics, "not_attempted", nullptr, nullptr);
  if (!HasUsablePathOutput(state.last_output)) {
    UpdateWarmStartDiagnostics(diagnostics, "no_history_path", nullptr,
                               nullptr);
    return false;
  }
  if (!ShouldUseHistoryWarmStart(replan_status)) {
    UpdateWarmStartDiagnostics(diagnostics, "disallowed_replan_status",
                               nullptr, nullptr);
    return false;
  }

  constexpr double kMaxWarmStartLatOffset = 1.20;
  constexpr double kWarmStartSBuffer = 0.50;
  constexpr double kMinWarmStartLength = 0.50;

  for (const TL::planning::PathGearPair& path_pair :
       state.last_output.partitioned_path) {
    const TL::planning::DiscretizedPath& path = path_pair.first;
    if (path.size() < 2U) {
      UpdateWarmStartDiagnostics(diagnostics, "history_path_too_short", &path,
                                 nullptr);
      continue;
    }

    HistoryPathProjection projection;
    if (!ProjectPointToPathGeometry(path, start_point, &projection)) {
      UpdateWarmStartDiagnostics(diagnostics, "projection_failed", &path,
                                 nullptr);
      continue;
    }
    const bool start_projects_to_path =
        std::fabs(projection.l) < kMaxWarmStartLatOffset &&
        projection.s > -kWarmStartSBuffer &&
        projection.s < projection.path_length + kWarmStartSBuffer;
    if (!start_projects_to_path) {
      const std::string reason =
          std::fabs(projection.l) >= kMaxWarmStartLatOffset
              ? "lateral_offset_large"
              : "s_out_of_range";
      UpdateWarmStartProjectionDiagnostics(diagnostics, reason, path,
                                           projection);
      continue;
    }

    if (projection.next_point_index >= path.size()) {
      UpdateWarmStartProjectionDiagnostics(diagnostics, "no_tail_after_start",
                                           path, projection);
      continue;
    }

    auto iter = path.begin() +
                static_cast<std::ptrdiff_t>(projection.next_point_index);
    warm_start_path->assign(iter, path.end());
    if (warm_start_path->empty()) {
      UpdateWarmStartProjectionDiagnostics(diagnostics,
                                           "empty_tail_after_assign", path,
                                           projection);
      continue;
    }
    RebaseWarmStartPath(start_point, warm_start_path);
    if (warm_start_path->back().s - warm_start_path->front().s <
        kMinWarmStartLength) {
      warm_start_path->clear();
      UpdateWarmStartProjectionDiagnostics(diagnostics, "tail_too_short", path,
                                           projection);
      continue;
    }

    *warm_start_gear = path_pair.second;
    UpdateWarmStartProjectionDiagnostics(diagnostics, "accepted", path,
                                         projection);
    return true;
  }
  if (diagnostics != nullptr &&
      diagnostics->reject_reason == "not_attempted") {
    UpdateWarmStartDiagnostics(diagnostics, "no_matching_history_path",
                               nullptr, nullptr);
  }
  return false;
}

struct PathProviderStrategySummary {
  std::size_t warm_start_points{0U};
  std::size_t splice_path_points{0U};
  TL::soc::GearPosition splice_gear{TL::soc::GearPosition::GEAR_NONE};
  int init_moving_direction{0};
  int init_path_direction{0};
  bool enable_init_kappa_cost{false};
  bool limit_init_steer_margin{false};
  bool disable_search{false};
  std::string warm_start_source{"none"};
  std::string warm_start_reject_reason{"not_attempted"};
  std::size_t warm_start_history_points{0U};
  double warm_start_match_s{0.0};
  double warm_start_match_l{0.0};
  double warm_start_path_front_s{0.0};
  double warm_start_path_back_s{0.0};
  bool trace_adjust_enabled{false};
  std::size_t trace_adjust_points{0U};
  double trace_adjust_target_s{0.0};
  double trace_adjust_finish_l_threshold{0.0};
  double trace_adjust_finish_theta_threshold{0.0};
  double trace_adjust_bound{0.0};
  double trace_adjust_path_length{0.0};
  double trace_adjust_min_length{kMinTraceAdjustPathLength};
  std::string trace_adjust_source{"none"};
  std::string trace_adjust_reject_reason{"not_attempted"};
};

struct TraceAdjustDecision {
  bool enabled{false};
  std::string reject_reason{"not_attempted"};
  double path_length{0.0};
};

TraceAdjustDecision EvaluateTraceAdjust(
    uint32_t replan_status,
    const TL::planning::DiscretizedPath& trace_path) {
  TraceAdjustDecision decision;
  if ((replan_status & kReplanTraceReplan) == 0U) {
    decision.reject_reason = "not_trace_replan";
    return decision;
  }
  if (!ShouldUseHistoryWarmStart(replan_status)) {
    decision.reject_reason = "unsafe_replan_status";
    return decision;
  }
  if (trace_path.empty()) {
    decision.reject_reason = "no_trace_path";
    return decision;
  }
  if (trace_path.size() <= 1U) {
    decision.reject_reason = "trace_path_too_short";
    return decision;
  }

  decision.path_length = trace_path.Length();
  if (!std::isfinite(decision.path_length) ||
      decision.path_length < kMinTraceAdjustPathLength) {
    decision.reject_reason = "trace_path_length_short";
    return decision;
  }

  decision.enabled = true;
  decision.reject_reason = "accepted";
  return decision;
}

void ApplyLocalTraceAdjustSearchStrategy(
    const TL::planning::DiscretizedPath& trace_path,
    TL::planning::PathSearchStrategy* search_strategy) {
  if (search_strategy == nullptr) {
    return;
  }
  TL::planning::TraceAdjustSearchStrategy& trace_adjust =
      search_strategy->trace_adjust_search_strategy;
  trace_adjust.is_trace_adjust = true;
  trace_adjust.trace_path = trace_path;
  trace_adjust.finish_l_threshold = kTraceAdjustLatDiffThreshold;
  trace_adjust.finish_theta_threshold = kTraceAdjustThetaDiffThreshold;
  trace_adjust.target_s = kTraceAdjustTargetS;
  trace_adjust.xy_bounds = {-kTraceAdjustBound, kTraceAdjustBound,
                            -kTraceAdjustBound, kTraceAdjustBound};
  search_strategy->cut_off_strategy = 0;
  search_strategy->is_plan_from_start = true;
}

void ApplyPathProviderStrategy(
    const TL::planning::RoiDeciderOutput& roi_output,
    const PathProviderRuntimeState& provider_state,
    const uint32_t replan_status,
    const TL::soc::GearPosition warm_start_gear,
    const TL::planning::DiscretizedPath& trace_path,
    TL::planning::PathStrategy* path_strategy,
    PathProviderStrategySummary* summary) {
  if (path_strategy == nullptr) {
    return;
  }

  const std::size_t warm_start_points = trace_path.size();
  path_strategy->path_search_strategy.Reset();
  path_strategy->disable_search = false;
  path_strategy->init_moving_direction =
      warm_start_points > 1U ? GearToMovingDirection(warm_start_gear) : 0;

  TL::planning::PathSearchStrategy& search_strategy =
      path_strategy->path_search_strategy;
  search_strategy.init_path_direction = 0;
  search_strategy.enable_init_kappa_cost = warm_start_points > 1U;
  search_strategy.limit_init_steer_margin = provider_state.has_history;
  search_strategy.is_plan_from_start = true;
  search_strategy.space_structure = BuildSpaceStructure(roi_output.scenario_type);
  search_strategy.park_direction =
      roi_output.is_parking_inwards ? TL::planning::ParkDirection::PARKIN
                                    : TL::planning::ParkDirection::NODIRECTION;
  const TraceAdjustDecision trace_adjust_decision =
      EvaluateTraceAdjust(replan_status, trace_path);
  if (trace_adjust_decision.enabled) {
    ApplyLocalTraceAdjustSearchStrategy(trace_path, &search_strategy);
  }

  if (summary != nullptr) {
    summary->warm_start_points = warm_start_points;
    summary->splice_path_points = warm_start_points;
    summary->splice_gear = warm_start_gear;
    summary->init_moving_direction = path_strategy->init_moving_direction;
    summary->init_path_direction = search_strategy.init_path_direction;
    summary->enable_init_kappa_cost =
        search_strategy.enable_init_kappa_cost;
    summary->limit_init_steer_margin =
        search_strategy.limit_init_steer_margin;
    summary->disable_search = path_strategy->disable_search;
    summary->warm_start_source =
        warm_start_points > 1U ? "history_splice" : "none";
    summary->trace_adjust_enabled =
        search_strategy.trace_adjust_search_strategy.is_trace_adjust;
    summary->trace_adjust_points =
        summary->trace_adjust_enabled ? trace_path.size() : 0U;
    summary->trace_adjust_target_s =
        summary->trace_adjust_enabled ? kTraceAdjustTargetS : 0.0;
    summary->trace_adjust_finish_l_threshold =
        summary->trace_adjust_enabled ? kTraceAdjustLatDiffThreshold : 0.0;
    summary->trace_adjust_finish_theta_threshold =
        summary->trace_adjust_enabled ? kTraceAdjustThetaDiffThreshold : 0.0;
    summary->trace_adjust_bound =
        summary->trace_adjust_enabled ? kTraceAdjustBound : 0.0;
    summary->trace_adjust_path_length = trace_adjust_decision.path_length;
    summary->trace_adjust_min_length = kMinTraceAdjustPathLength;
    summary->trace_adjust_source =
        summary->trace_adjust_enabled ? "history_warm_start" : "none";
    summary->trace_adjust_reject_reason =
        trace_adjust_decision.reject_reason;
  }
}

struct PathProviderDecision {
  TL::common::PathPoint start_point;
  TL::common::PathPoint end_pose;
  uint64_t obstacle_signature{0U};
  uint32_t replan_status{0U};
  bool can_reuse_history{false};
  std::string reason;
};

PathProviderDecision BuildPathProviderDecision(
    const valet_parking_config_t& config,
    uint32_t parking_seq,
    const TL::planning::RoiDeciderOutput& roi_output,
    const RuntimeVehicleInput& vehicle_input,
    const std::vector<RuntimeObstacleInput>& obstacles,
    bool replan_triggered_by_speed_plan,
    bool current_path_has_collision_risk,
    const PathProviderRuntimeState& state) {
  PathProviderDecision decision;
  decision.start_point = BuildStartPathPoint(config, vehicle_input);
  decision.end_pose =
      roi_output.has_fine_tuned ? roi_output.fine_tuned_end_pose
                                : roi_output.end_pose;
  decision.obstacle_signature = BuildObstacleSignature(obstacles);

  if (!state.has_history || !HasUsablePathOutput(state.last_output)) {
    decision.replan_status |= kReplanNoValidPath;
    AppendReason(&decision.reason, "no_history");
  } else {
    if (state.path_id != static_cast<int>(parking_seq) ||
        !IsSamePathPoint(state.end_pose, decision.end_pose, 0.05, 0.03)) {
      decision.replan_status |= kReplanTargetUpdate;
      AppendReason(&decision.reason, "target_update");
    }

    if (state.obstacle_signature != decision.obstacle_signature) {
      decision.replan_status |= kReplanBlockByStaticObstacle;
      AppendReason(&decision.reason, "obstacles_changed");
    }

    if (!IsHistoryPathReusable(state, decision.start_point)) {
      decision.replan_status |= kReplanTraceReplan;
      AppendReason(&decision.reason, "history_start_mismatch");
    }
  }

  if (replan_triggered_by_speed_plan) {
    decision.replan_status |= kReplanForSpeedWarn;
    AppendReason(&decision.reason, "speed_replan");
  }
  if (current_path_has_collision_risk) {
    decision.replan_status |= kReplanDynamicReplan;
    AppendReason(&decision.reason, "collision_risk");
  }

  decision.can_reuse_history =
      state.has_history && decision.replan_status == 0U &&
      IsHistoryPathReusable(state, decision.start_point);
  if (decision.reason.empty()) {
    decision.reason = decision.can_reuse_history ? "history_valid" : "NONE";
  }
  return decision;
}

TL::planning::OpenSpacePathInput BuildPathProviderInput(
    const InputMetadata& metadata,
    uint32_t parking_seq,
    const TL::planning::RoiDeciderOutput& roi_output,
    const std::vector<RuntimeObstacleInput>& obstacles,
    const PathProviderRuntimeState& provider_state,
    uint32_t replan_status,
    const TL::common::PathPoint& start_point,
    const TL::common::PathPoint& end_pose,
    PathProviderStrategySummary* strategy_summary) {
  TL::planning::OpenSpacePathInput input;
  input.path_id = static_cast<int>(parking_seq);
  input.replan_status = replan_status;
  input.rotate_angle = roi_output.origin_heading;
  input.translate_origin = roi_output.origin_point;
  input.start_point = start_point;
  input.end_pose = end_pose;
  input.xy_bounds = roi_output.xy_bounds;
  input.xy_bounds_is_local = true;
  input.obstacles_segments_vec = BuildObstacleSegments(roi_output, obstacles);
  input.dest_region_with_angle = roi_output.dest_region;
  TL::soc::GearPosition warm_start_gear =
      TL::soc::GearPosition::GEAR_NONE;
  WarmStartBuildDiagnostics warm_start_diagnostics;
  BuildWarmStartPathFromHistory(provider_state, start_point, replan_status,
                                &input.warm_start_path, &warm_start_gear,
                                &warm_start_diagnostics);
  ApplyPathProviderStrategy(roi_output, provider_state, replan_status,
                            warm_start_gear, input.warm_start_path,
                            &input.path_strategy, strategy_summary);
  if (strategy_summary != nullptr) {
    strategy_summary->warm_start_reject_reason =
        warm_start_diagnostics.reject_reason;
    strategy_summary->warm_start_history_points =
        warm_start_diagnostics.history_points;
    strategy_summary->warm_start_match_s = warm_start_diagnostics.start_s;
    strategy_summary->warm_start_match_l = warm_start_diagnostics.start_l;
    strategy_summary->warm_start_path_front_s =
        warm_start_diagnostics.path_front_s;
    strategy_summary->warm_start_path_back_s =
        warm_start_diagnostics.path_back_s;
  }
  (void)metadata;
  return input;
}

std::vector<TL::planning::OpenSpacePathInput> BuildPathProviderPrePlanInputs(
    const valet_parking_config_t& config,
    const InputMetadata& metadata,
    const SelectedSlot& selected_slot,
    uint32_t selected_parking_seq,
    const TL::common::VehicleState& vehicle_state,
    const RuntimeVehicleInput& vehicle_input,
    const std::vector<RuntimeObstacleInput>& obstacles,
    const PathProviderRuntimeState& provider_state) {
  std::vector<TL::planning::OpenSpacePathInput> inputs;
  const auto& lots = selected_slot.parking_lots();
  if (lots.size() <= 1U) {
    return inputs;
  }

  TL::planning::OpenSpaceRoiDecider roi_decider(
      TL::planning::LoadEP30VehicleParam(),
      TL::planning::RoiDeciderConfig::GetDefault());
  inputs.reserve(lots.size() - 1U);
  for (const ParkingLot& candidate_lot_sample : lots) {
    if (candidate_lot_sample.parking_seq() == selected_parking_seq) {
      continue;
    }

    TL::perception::ParkingLotOut candidate_lot;
    std::string ignored_reason;
    if (!ConvertParkingLot(candidate_lot_sample, &candidate_lot,
                           &ignored_reason)) {
      continue;
    }

    TL::planning::RoiDeciderOutput candidate_roi_output;
    if (roi_decider.Process(candidate_lot, vehicle_state,
                            &candidate_roi_output) != 0) {
      continue;
    }

    const PathProviderPreCheckResult precheck_result =
        RunPathProviderPreCheck(config, candidate_roi_output, vehicle_input,
                                obstacles);
    if (!precheck_result.ok) {
      continue;
    }

    const PathProviderDecision decision = BuildPathProviderDecision(
        config, candidate_lot_sample.parking_seq(), candidate_roi_output,
        vehicle_input, obstacles, false, false, provider_state);
    PathProviderStrategySummary ignored_strategy_summary;
    inputs.push_back(BuildPathProviderInput(
        metadata, candidate_lot_sample.parking_seq(), candidate_roi_output,
        obstacles, provider_state, decision.replan_status,
        decision.start_point, decision.end_pose, &ignored_strategy_summary));
  }
  return inputs;
}

PathProviderAttemptContractLite MakePathProviderAttemptContractLite(
    const PathProviderDecision& decision,
    const std::string& replan_text,
    const PathProviderStrategySummary& strategy_summary,
    bool vehicle_has_state,
    std::size_t obstacle_count) {
  PathProviderAttemptContractLite attempt;
  attempt.replan_text = replan_text;
  attempt.decision_reason = decision.reason;
  attempt.warm_start_source = strategy_summary.warm_start_source;
  attempt.warm_start_reject_reason =
      strategy_summary.warm_start_reject_reason;
  attempt.warm_start_points = strategy_summary.warm_start_points;
  attempt.splice_path_points = strategy_summary.splice_path_points;
  attempt.warm_start_history_points =
      strategy_summary.warm_start_history_points;
  attempt.warm_start_match_s = strategy_summary.warm_start_match_s;
  attempt.warm_start_match_l = strategy_summary.warm_start_match_l;
  attempt.warm_start_path_front_s =
      strategy_summary.warm_start_path_front_s;
  attempt.warm_start_path_back_s =
      strategy_summary.warm_start_path_back_s;
  attempt.strategy_init_moving_direction =
      strategy_summary.init_moving_direction;
  attempt.strategy_init_path_direction = strategy_summary.init_path_direction;
  attempt.strategy_enable_init_kappa_cost =
      strategy_summary.enable_init_kappa_cost;
  attempt.strategy_limit_init_steer_margin =
      strategy_summary.limit_init_steer_margin;
  attempt.strategy_disable_search = strategy_summary.disable_search;
  attempt.trace_adjust_enabled = strategy_summary.trace_adjust_enabled;
  attempt.trace_adjust_source = strategy_summary.trace_adjust_source;
  attempt.trace_adjust_reject_reason =
      strategy_summary.trace_adjust_reject_reason;
  attempt.trace_adjust_points = strategy_summary.trace_adjust_points;
  attempt.trace_adjust_path_length = strategy_summary.trace_adjust_path_length;
  attempt.trace_adjust_min_length = strategy_summary.trace_adjust_min_length;
  attempt.trace_adjust_target_s = strategy_summary.trace_adjust_target_s;
  attempt.trace_adjust_finish_l_threshold =
      strategy_summary.trace_adjust_finish_l_threshold;
  attempt.trace_adjust_finish_theta_threshold =
      strategy_summary.trace_adjust_finish_theta_threshold;
  attempt.trace_adjust_bound = strategy_summary.trace_adjust_bound;
  attempt.external_vehicle = vehicle_has_state;
  attempt.obstacle_count = obstacle_count;
  return attempt;
}

PathProviderHistoryContractLite MakePathProviderHistoryContractLite(
    const PathProviderRuntimeState& provider_state,
    const std::string& replan_text,
    bool vehicle_has_state,
    std::size_t obstacle_count) {
  PathProviderHistoryContractLite history;
  history.replan_text = replan_text;
  history.last_warm_start_points = provider_state.last_warm_start_points;
  history.last_strategy_init_moving_direction =
      provider_state.last_strategy_init_moving_direction;
  history.generated_count = provider_state.generated_count;
  history.reused_count = provider_state.reused_count;
  history.external_vehicle = vehicle_has_state;
  history.obstacle_count = obstacle_count;
  return history;
}

PlanningContextPathProjectionLite MakePlanningContextPathProjectionLite(
    const PathProviderRuntimeState& provider_state,
    const std::string& replan_text) {
  PlanningContextPathProjectionLite projection;
  projection.has_history = provider_state.has_history;
  projection.path_id = provider_state.path_id;
  projection.replan_text = replan_text;
  projection.history_points =
      CountPathProviderOutputPoints(provider_state.last_output);
  return projection;
}

bool RunPathProvider(const valet_parking_config_t& config,
                     const InputMetadata& metadata,
                     uint32_t parking_seq,
                     const TL::planning::RoiDeciderOutput& roi_output,
                     const RuntimeVehicleInput& vehicle_input,
                     const std::vector<RuntimeObstacleInput>& obstacles,
                     bool replan_triggered_by_speed_plan,
                     bool current_path_has_collision_risk,
                     const std::vector<TL::planning::OpenSpacePathInput>&
                         preplan_inputs,
                     PathProviderRuntimeState* provider_state,
                     TL::planning::OpenSpacePathOutput* path_output,
                     std::string* status_reason) {
  if (provider_state == nullptr || path_output == nullptr ||
      status_reason == nullptr) {
    return false;
  }

  const PathProviderDecision decision = BuildPathProviderDecision(
      config, parking_seq, roi_output, vehicle_input, obstacles,
      replan_triggered_by_speed_plan, current_path_has_collision_risk,
      *provider_state);
  const std::string replan_text =
      ReplanStatusToString(decision.replan_status);

  if (decision.can_reuse_history) {
    *path_output = provider_state->last_output;
    path_output->replan_status = 0U;
    ++provider_state->reused_count;

    const PathProviderHistoryContractLite history_contract =
        MakePathProviderHistoryContractLite(
            *provider_state, replan_text, vehicle_input.has_vehicle_state,
            obstacles.size());
    const PlanningContextPathProjectionLite planning_projection =
        MakePlanningContextPathProjectionLite(*provider_state, replan_text);
    *status_reason = BuildPathProviderReuseReason(
        *path_output, parking_seq, roi_output, history_contract,
        planning_projection);
    return true;
  }

  PathProviderStrategySummary strategy_summary;
  TL::planning::OpenSpacePathProviderDiagnostics provider_diagnostics;
  PathProviderAttemptContractLite attempt_contract =
      MakePathProviderAttemptContractLite(
          decision, replan_text, strategy_summary,
          vehicle_input.has_vehicle_state, obstacles.size());
  try {
    const TL::planning::OpenSpacePathInput input =
        BuildPathProviderInput(metadata, parking_seq, roi_output, obstacles,
                               *provider_state, decision.replan_status,
                               decision.start_point, decision.end_pose,
                               &strategy_summary);
    attempt_contract = MakePathProviderAttemptContractLite(
        decision, replan_text, strategy_summary,
        vehicle_input.has_vehicle_state, obstacles.size());
    TL::planning::OpenSpacePathProvider* provider =
        EnsureThreadedPathProvider(provider_state);
    if (provider == nullptr) {
      *status_reason = BuildPathProviderFailureReason(
          "threaded provider unavailable", attempt_contract,
          preplan_inputs.size(), nullptr);
      return false;
    }
    provider->PrePlan(preplan_inputs);
    const TL::common::Status provider_status =
        provider->Plan(input, path_output, &provider_diagnostics);
    if (!provider_status.ok()) {
      *status_reason = BuildPathProviderFailureReason(
          provider_status.error_message(), attempt_contract,
          preplan_inputs.size(), &provider_diagnostics);
      return false;
    }
  } catch (const std::exception& ex) {
    *status_reason = BuildPathProviderFailureReason(
        std::string("exception: ") + ex.what(), attempt_contract,
        preplan_inputs.size(), &provider_diagnostics);
    return false;
  } catch (...) {
    *status_reason = BuildPathProviderFailureReason(
        "unknown exception", attempt_contract, preplan_inputs.size(),
        &provider_diagnostics);
    return false;
  }

  const std::size_t point_count = CountPathProviderPoints(*path_output);
  if (!path_output->error_msg.empty()) {
    *status_reason = BuildPathProviderOutputFailureReason(
        path_output->error_msg, *path_output, attempt_contract,
        preplan_inputs.size(), provider_diagnostics);
    return false;
  }
  if (path_output->partitioned_path.empty() || point_count < 2U) {
    *status_reason = BuildPathProviderOutputFailureReason(
        "insufficient path points", *path_output, attempt_contract,
        preplan_inputs.size(), provider_diagnostics);
    return false;
  }

  if (path_output->replan_status == 0U) {
    path_output->replan_status = decision.replan_status;
  }
  provider_state->has_history = true;
  provider_state->path_id = static_cast<int>(parking_seq);
  provider_state->start_point = decision.start_point;
  provider_state->end_pose = decision.end_pose;
  provider_state->obstacle_signature = decision.obstacle_signature;
  provider_state->last_output = *path_output;
  provider_state->last_splice_gear = strategy_summary.splice_gear;
  provider_state->last_warm_start_points =
      strategy_summary.warm_start_points;
  provider_state->last_strategy_init_moving_direction =
      strategy_summary.init_moving_direction;
  ++provider_state->generated_count;

  const PlanningContextPathProjectionLite planning_projection =
      MakePlanningContextPathProjectionLite(*provider_state, replan_text);
  *status_reason = BuildPathProviderGeneratedReason(
      *path_output, parking_seq, roi_output, attempt_contract,
      provider_state->generated_count, provider_state->reused_count,
      preplan_inputs.size(), planning_projection, provider_diagnostics);
  return true;
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

std::string CompactDebugText(std::string text) {
  for (char& ch : text) {
    if (ch == '\r' || ch == '\n' || ch == '\t') {
      ch = ' ';
    }
  }
  return text;
}

StageRuntimeLifecycleContractLite BuildStageRuntimeLifecycleContractLite(
    const PathProviderRuntimeState* provider_state,
    bool has_last_speed_frame,
    bool direct_command_active,
    const std::string& runtime_lifecycle_event,
    const std::string& stage_exit_action,
    const std::string& path_history_action,
    const std::string& speed_frame_action,
    const std::string& direct_state_action) {
  StageRuntimeLifecycleContractLite lite;
  lite.event = runtime_lifecycle_event;
  lite.stage_exit_action = stage_exit_action;
  lite.path_history_action = path_history_action;
  lite.speed_frame_action = speed_frame_action;
  lite.direct_state_action = direct_state_action;
  lite.path_history_available =
      provider_state != nullptr && provider_state->has_history;
  lite.speed_frame_available = has_last_speed_frame;
  lite.direct_command_active = direct_command_active;
  return lite;
}

void AppendRuntimeLifecycleContract(const std::string& event,
                                    const std::string& stage_exit_action,
                                    const std::string& path_history_action,
                                    const std::string& speed_frame_action,
                                    const std::string& direct_state_action,
                                    bool path_history_available,
                                    bool speed_frame_available,
                                    bool direct_command_active,
                                    std::ostringstream* stream) {
  StageRuntimeLifecycleContractLite lite;
  lite.event = event;
  lite.stage_exit_action = stage_exit_action;
  lite.path_history_action = path_history_action;
  lite.speed_frame_action = speed_frame_action;
  lite.direct_state_action = direct_state_action;
  lite.path_history_available = path_history_available;
  lite.speed_frame_available = speed_frame_available;
  lite.direct_command_active = direct_command_active;
  AppendRuntimeLifecycleContract(lite, stream);
}

std::string BuildOpenSpaceStageOutputContract(
    const TL::planning::PartitionOutput& partition_output,
    const TL::planning::SpeedOptimizerOutput* speed_output,
    const StageFinishEvaluation* finish_evaluation,
    const FunctionManagerProjection* function_projection,
    const PathProviderRuntimeState* provider_state,
    bool has_last_speed_frame,
    bool direct_command_active,
    const StageProcessContextLite* stage_context = nullptr) {
  StageFinishOutputContractLite finish_lite;
  const StageFinishOutputContractLite* finish_lite_ptr = nullptr;
  if (finish_evaluation != nullptr) {
    finish_lite = BuildStageFinishOutputContractLite(*finish_evaluation);
    finish_lite_ptr = &finish_lite;
  }

  StageFunctionManagerProjectionContractLite function_lite;
  const StageFunctionManagerProjectionContractLite* function_lite_ptr =
      nullptr;
  if (function_projection != nullptr) {
    function_lite =
        BuildFunctionManagerProjectionContractLite(*function_projection);
    function_lite_ptr = &function_lite;
  }

  const bool finish_ready =
      finish_evaluation != nullptr && finish_evaluation->ready_to_finish;
  const StageRuntimeLifecycleContractLite runtime_lifecycle =
      BuildStageRuntimeLifecycleContractLite(
          provider_state, has_last_speed_frame, direct_command_active,
          finish_ready ? "stage_finish_ready" : "normal_open_space",
          finish_ready ? "latch_finish_hold_after_publish"
                       : "continue_parking",
          finish_ready ? "keep_until_stage_reset" : "keep_for_reuse",
          finish_ready ? "keep_until_stage_reset"
                       : "keep_for_speed_warm_start",
          direct_command_active ? "unexpected_active" : "already_clear");
  return BuildOpenSpaceStageOutputContract(
      partition_output, speed_output, finish_lite_ptr, function_lite_ptr,
      runtime_lifecycle, stage_context);
}

std::string BuildFallbackStageOutputContract(
    const std::string& fallback_event,
    const std::string& fallback_action,
    const std::string& stage_status,
    const std::string& parking_status,
    PlanningTrajectoryType trajectory_type,
    const std::string& mission_state,
    const std::string& next_stage,
    bool finish_scenario_intent,
    const FunctionManagerProjection* function_projection,
    const PathProviderRuntimeState* provider_state,
    bool has_last_speed_frame,
    bool direct_command_active,
    const std::string& runtime_lifecycle_event,
    const std::string& stage_exit_action,
    const std::string& path_history_action,
    const std::string& speed_frame_action,
    const std::string& direct_state_action,
    const StageProcessContextLite* stage_context = nullptr) {
  StageFunctionManagerProjectionContractLite function_lite;
  const StageFunctionManagerProjectionContractLite* function_lite_ptr =
      nullptr;
  if (function_projection != nullptr) {
    function_lite =
        BuildFunctionManagerProjectionContractLite(*function_projection);
    function_lite_ptr = &function_lite;
  }
  const StageRuntimeLifecycleContractLite runtime_lifecycle =
      BuildStageRuntimeLifecycleContractLite(
          provider_state, has_last_speed_frame, direct_command_active,
          runtime_lifecycle_event, stage_exit_action, path_history_action,
          speed_frame_action, direct_state_action);
  return BuildFallbackStageOutputContract(
      fallback_event, fallback_action, stage_status, parking_status,
      trajectory_type, mission_state, next_stage, finish_scenario_intent,
      function_lite_ptr, runtime_lifecycle, stage_context);
}

std::string BuildEarlyEstopFallbackContract(
    const std::string& fallback_event,
    const std::string& parking_status,
    const FunctionManagerProjection& function_projection,
    const PathProviderRuntimeState* provider_state,
    bool has_last_speed_frame,
    bool direct_command_active,
    const StageProcessContextLite* stage_context = nullptr) {
  const StageRuntimeLifecycleContractLite runtime_lifecycle =
      BuildStageRuntimeLifecycleContractLite(
          provider_state, has_last_speed_frame, direct_command_active,
          fallback_event + "_fallback", "stay_in_parking_stage",
          "reset_on_early_failure", "reset_on_early_failure",
          direct_command_active ? "reset_failed_direct_command"
                                : "already_clear");
  return BuildEarlyEstopFallbackContract(
      fallback_event, parking_status,
      BuildFunctionManagerProjectionContractLite(function_projection),
      runtime_lifecycle, stage_context);
}

bool RunPathPartition(const valet_parking_config_t& config,
                      const InputMetadata& metadata,
                      const TL::planning::RoiDeciderOutput& roi_output,
                      const TL::planning::OpenSpacePathOutput& path_output,
                      const RuntimeVehicleInput& vehicle_input,
                      const std::vector<RuntimeObstacleInput>& obstacles,
                      TL::soc::GearPosition last_published_gear,
                      bool replan_triggered_by_speed_plan,
                      const TL::planning_internal::AvpSpeedPlanCollisionInfo&
                          speed_plan_collision_info,
                      TL::planning::OpenSpacePathPartition* partition,
                      TL::planning::PartitionOutput* partition_output,
                      std::string* status_reason) {
  if (partition == nullptr || partition_output == nullptr ||
      status_reason == nullptr) {
    return false;
  }

  if (IsSmokeFlagEnabled(kForcePathPartitionFailEnv)) {
    *status_reason = BuildPathPartitionForcedFailureReason();
    return false;
  }

  try {
    const TL::planning::PartitionInput input =
        BuildPathPartitionInput(config, metadata, roi_output, path_output,
                                vehicle_input, obstacles,
                                last_published_gear,
                                replan_triggered_by_speed_plan,
                                speed_plan_collision_info);
    const TL::common::Status status =
        partition->Execute(input, partition_output);
    if (!status.ok()) {
      *status_reason =
          "PATH_PARTITION failed: " + status.error_message();
      return false;
    }
  } catch (const std::exception& ex) {
    *status_reason = std::string("PATH_PARTITION failed: exception: ") +
                     ex.what();
    return false;
  } catch (...) {
    *status_reason = "PATH_PARTITION failed: unknown exception";
    return false;
  }

  const std::size_t chosen_points =
      partition_output->chosen_partitioned_path.first.size();
  if (chosen_points < 2U) {
    *status_reason = BuildPathPartitionInsufficientReason(*partition_output);
    return false;
  }

  const std::string debug = CompactDebugText(partition_output->path_decision_debug);
  *status_reason = BuildPathPartitionOutputReason(
      *partition_output, vehicle_input.has_vehicle_state, obstacles.size(),
      debug);
  return true;
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

bool RunSpeedOptimizer(const valet_parking_config_t& config,
                       const InputMetadata& metadata,
                       const TL::planning::RoiDeciderOutput& roi_output,
                       const TL::planning::PartitionOutput& partition_output,
                       const RuntimeVehicleInput& vehicle_input,
                       const std::vector<RuntimeObstacleInput>& obstacles,
                       const TL::planning::OpenSpaceSpeedOptimizerConfig&
                           speed_config,
                       bool has_last_frame,
                       double last_frame_timestamp,
                       double last_planning_start_relative_time,
                       bool is_rpa_direct_mode,
                       TL::planning::OpenSpaceSpeedOptimizer* optimizer,
                       TL::planning::SpeedOptimizerOutput* speed_output,
                       std::string* status_reason) {
  if (optimizer == nullptr || speed_output == nullptr ||
      status_reason == nullptr) {
    return false;
  }

  if (IsSmokeFlagEnabled(kForceSpeedOptimizerFailEnv)) {
    *status_reason = BuildSpeedOptimizerFailureReason(
        "forced_by_smoke_env", is_rpa_direct_mode);
    return false;
  }

  try {
    const TL::planning::SpeedOptimizerInput input =
        BuildSpeedOptimizerInput(config, metadata, roi_output,
                                 partition_output, vehicle_input, obstacles,
                                 speed_config,
                                 has_last_frame, last_frame_timestamp,
                                 last_planning_start_relative_time,
                                 is_rpa_direct_mode);
    const bool ok = optimizer->Execute(input, speed_output);
    if (!ok || !speed_output->success) {
      *status_reason = BuildSpeedOptimizerFailureReason(
          speed_output->message, is_rpa_direct_mode);
      return false;
    }
  } catch (const std::exception& ex) {
    *status_reason = BuildSpeedOptimizerFailureReason(
        std::string("exception: ") + ex.what(), is_rpa_direct_mode);
    return false;
  } catch (...) {
    *status_reason = BuildSpeedOptimizerFailureReason(
        "unknown exception", is_rpa_direct_mode);
    return false;
  }

  const std::size_t trajectory_points =
      speed_output->trajectory_gear.first.NumOfPoints();
  if (trajectory_points < 2U) {
    *status_reason = BuildSpeedOptimizerInsufficientReason(
        trajectory_points, speed_output->message, is_rpa_direct_mode);
    return false;
  }

  *status_reason = BuildSpeedOptimizerOutputReason(
      partition_output, *speed_output, is_rpa_direct_mode, has_last_frame,
      vehicle_input.has_vehicle_state, obstacles.size(),
      CompactDebugText(speed_output->message));
  return true;
}

struct FlattenedPathProviderPoint {
  TL::common::PathPoint path_point;
  TL::soc::GearPosition gear{TL::soc::GearPosition::GEAR_INVALID};
};

void AppendFlattenedPathPair(const TL::planning::PathGearPair& path_pair,
                             std::vector<FlattenedPathProviderPoint>* points) {
  if (points == nullptr) {
    return;
  }
  for (const TL::common::PathPoint& path_point : path_pair.first) {
      if (!points->empty()) {
        const TL::common::PathPoint& previous = points->back().path_point;
        const double dx = path_point.x - previous.x;
        const double dy = path_point.y - previous.y;
        if (std::sqrt(dx * dx + dy * dy) < 1.0e-4 &&
            std::fabs(NormalizeAngle(path_point.theta - previous.theta)) <
                1.0e-4) {
          continue;
        }
      }
      FlattenedPathProviderPoint flattened;
      flattened.path_point = path_point;
      flattened.gear = path_pair.second;
      points->push_back(flattened);
  }
}

std::vector<FlattenedPathProviderPoint> FlattenPathProviderOutput(
    const TL::planning::OpenSpacePathOutput& path_output) {
  std::vector<FlattenedPathProviderPoint> points;
  points.reserve(CountPathProviderPoints(path_output));
  for (const auto& path_pair : path_output.partitioned_path) {
    AppendFlattenedPathPair(path_pair, &points);
  }
  return points;
}

std::vector<FlattenedPathProviderPoint> FlattenPathPartitionOutput(
    const TL::planning::PartitionOutput& partition_output) {
  std::vector<FlattenedPathProviderPoint> points;
  points.reserve(partition_output.chosen_partitioned_path.first.size());
  AppendFlattenedPathPair(partition_output.chosen_partitioned_path, &points);
  return points;
}

PlanningTrajectory BuildTrajectoryToTarget(const valet_parking_config_t& config,
                                           const InputMetadata& metadata,
                                           const TL::common::PathPoint& target,
                                           bool is_estop,
                                           const std::string& reason) {
  const double start_x = config.fake_vehicle_x;
  const double start_y = config.fake_vehicle_y;
  const double start_theta = config.fake_vehicle_theta;
  const double target_x = is_estop ? start_x : target.x;
  const double target_y = is_estop ? start_y : target.y;
  const double target_theta = is_estop ? start_theta : target.theta;
  const double dx = target_x - start_x;
  const double dy = target_y - start_y;
  const double total_path_length = is_estop ? 0.0 : std::sqrt(dx * dx + dy * dy);
  const double total_path_time = is_estop ? 0.0 : 2.0;
  const int point_count = is_estop ? 1 : kTrajectoryPointCount;
  const double nominal_speed = (total_path_time > 1e-6)
      ? (total_path_length / total_path_time)
      : 0.0;
  const double heading_delta = NormalizeAngle(target_theta - start_theta);

  std::vector<TrajectoryPoint> points;
  points.reserve(static_cast<std::size_t>(point_count));

  for (int i = 0; i < point_count; ++i) {
    const double ratio = (point_count <= 1)
        ? 0.0
        : static_cast<double>(i) / static_cast<double>(point_count - 1);
    const double x = start_x + ratio * dx;
    const double y = start_y + ratio * dy;
    const double theta = NormalizeAngle(start_theta + ratio * heading_delta);
    const double s = total_path_length * ratio;
    const double relative_time = total_path_time * ratio;

    PathPoint path_point;
    path_point.x(x);
    path_point.y(y);
    path_point.z(0.0);
    path_point.theta(theta);
    path_point.kappa(0.0);
    path_point.s(s);
    path_point.l(0.0);
    path_point.dkappa(0.0);
    path_point.ddkappa(0.0);
    path_point.lane_id("valet_parking_roi_seed");
    path_point.x_derivative(std::cos(theta));
    path_point.y_derivative(std::sin(theta));

    GaussianInfo gaussian_info;
    gaussian_info.sigma_x(0.3);
    gaussian_info.sigma_y(0.25);
    gaussian_info.correlation(0.0);
    gaussian_info.area_probability(0.95);
    gaussian_info.ellipse_a(0.5);
    gaussian_info.ellipse_b(0.25);
    gaussian_info.theta_a(theta);

    TrajectoryPoint trajectory_point;
    trajectory_point.path_point(path_point);
    trajectory_point.v(is_estop ? 0.0 : nominal_speed);
    trajectory_point.a(0.0);
    trajectory_point.relative_time(relative_time);
    trajectory_point.da(0.0);
    trajectory_point.steer(0.0);
    trajectory_point.gaussian_info(gaussian_info);
    points.push_back(trajectory_point);
  }

  const uint64_t publish_stamp_ms = NowMilliseconds();
  const uint64_t data_stamp_ms = metadata.data_stamp_ms == 0U
      ? publish_stamp_ms
      : metadata.data_stamp_ms;

  EStop estop;
  estop.is_estop(is_estop);
  estop.reason(reason);

  PlanningTrajectory output;
  output.header(MakeHeader(metadata.seq, metadata.frame_id, publish_stamp_ms, data_stamp_ms));
  output.total_path_length(total_path_length);
  output.total_path_time(total_path_time);
  output.trajectory_point(std::move(points));
  output.is_replan(false);
  output.replan_type(0U);
  output.replan_reason(reason);
  output.longitudinal_diff(dx);
  output.lateral_diff(dy);
  output.gear(is_estop ? GearPosition::GEAR_INVALID : GearPosition::GEAR_REVERSE);
  output.estop(std::move(estop));
  output.trajectory_type(is_estop ? PlanningTrajectoryType::TRAJECTORY_TYPE_UNKNOWN
                                  : PlanningTrajectoryType::TRAJECTORY_TYPE_NORMAL);
  return output;
}

PlanningTrajectory BuildEstopTrajectory(const valet_parking_config_t& config,
                                        const InputMetadata& metadata,
                                        const std::string& reason) {
  TL::common::PathPoint stop_pose;
  stop_pose.x = config.fake_vehicle_x;
  stop_pose.y = config.fake_vehicle_y;
  stop_pose.theta = config.fake_vehicle_theta;
  return BuildTrajectoryToTarget(config, metadata, stop_pose, true, reason);
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

struct DirectSpeedOptimizerProfile {
  double selected_speed_mps{0.8};
  double min_sample_speed{0.3};
  double max_sample_speed{0.8};
};

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

void ApplyDirectSpeedBound(
    const DirectSpeedOptimizerProfile& profile,
    TL::planning::OpenSpaceSpeedOptimizerConfig::SpeedBoundInfo* bound) {
  if (bound == nullptr) {
    return;
  }
  bound->min_sample_speed = profile.min_sample_speed;
  bound->max_sample_speed = profile.max_sample_speed;
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

bool RunOpenSpaceStraightPath(
    const valet_parking_config_t& config,
    const InputMetadata& metadata,
    const RuntimeVehicleInput& vehicle_input,
    const ParkingCommand& command,
    bool forward,
    TL::planning::OpenSpaceStraightPathProvider* provider,
    TL::planning::PartitionOutput* partition_output,
    std::string* status_reason) {
  if (provider == nullptr || partition_output == nullptr ||
      status_reason == nullptr) {
    return false;
  }

  if (IsSmokeFlagEnabled(kForceStraightPathFailEnv)) {
    *status_reason =
        BuildOpenSpaceStraightPathFailureReason("forced_by_smoke_env");
    return false;
  }

  TL::planning::OpenSpaceStraightPathOutput straight_output;
  try {
    const TL::planning::OpenSpaceStraightPathInput input =
        BuildOpenSpaceStraightPathInput(config, metadata, vehicle_input,
                                        command, forward);
    const TL::common::Status status =
        provider->Process(input, &straight_output);
    if (!status.ok()) {
      *status_reason =
          BuildOpenSpaceStraightPathFailureReason(status.error_message());
      return false;
    }
  } catch (const std::exception& ex) {
    *status_reason = BuildOpenSpaceStraightPathFailureReason(
        std::string("exception: ") + ex.what());
    return false;
  } catch (...) {
    *status_reason =
        BuildOpenSpaceStraightPathFailureReason("unknown exception");
    return false;
  }

  BuildPartitionOutputFromStraightPath(straight_output, partition_output);

  const std::size_t path_points =
      partition_output->chosen_partitioned_path.first.size();
  if (path_points < 2U) {
    *status_reason = BuildOpenSpaceStraightPathInsufficientReason(
        path_points, straight_output.diagnostic);
    return false;
  }

  *status_reason = BuildOpenSpaceStraightPathOutputReason(
      straight_output, vehicle_input.has_vehicle_state,
      SelectDirectSpeed(command.direct_speed_mps()));
  return true;
}

PlanningTrajectory BuildStageControlStopTrajectory(
    const valet_parking_config_t& config,
    const InputMetadata& metadata,
    const RuntimeVehicleInput& vehicle_input,
    const std::string& reason,
    ::GearPosition gear) {
  const TL::common::PathPoint start_point =
      BuildStartPathPoint(config, vehicle_input);

  PathPoint path_point;
  path_point.x(start_point.x);
  path_point.y(start_point.y);
  path_point.z(start_point.z);
  path_point.theta(NormalizeAngle(start_point.theta));
  path_point.kappa(0.0);
  path_point.s(0.0);
  path_point.l(0.0);
  path_point.dkappa(0.0);
  path_point.ddkappa(0.0);
  path_point.lane_id("valet_parking_stage_control_stop");
  path_point.x_derivative(std::cos(start_point.theta));
  path_point.y_derivative(std::sin(start_point.theta));

  GaussianInfo gaussian_info;
  gaussian_info.sigma_x(0.3);
  gaussian_info.sigma_y(0.25);
  gaussian_info.correlation(0.0);
  gaussian_info.area_probability(0.95);
  gaussian_info.ellipse_a(0.5);
  gaussian_info.ellipse_b(0.25);
  gaussian_info.theta_a(start_point.theta);

  TrajectoryPoint trajectory_point;
  trajectory_point.path_point(std::move(path_point));
  trajectory_point.v(0.0);
  trajectory_point.a(0.0);
  trajectory_point.relative_time(0.0);
  trajectory_point.da(0.0);
  trajectory_point.steer(0.0);
  trajectory_point.gaussian_info(std::move(gaussian_info));

  std::vector<TrajectoryPoint> points;
  points.push_back(std::move(trajectory_point));

  const uint64_t publish_stamp_ms = NowMilliseconds();
  const uint64_t data_stamp_ms = metadata.data_stamp_ms == 0U
      ? publish_stamp_ms
      : metadata.data_stamp_ms;

  EStop estop;
  estop.is_estop(false);
  estop.reason(reason);

  PlanningTrajectory output;
  output.header(MakeHeader(metadata.seq, metadata.frame_id, publish_stamp_ms,
                           data_stamp_ms));
  output.total_path_length(0.0);
  output.total_path_time(0.0);
  output.trajectory_point(std::move(points));
  output.is_replan(false);
  output.replan_type(0U);
  output.replan_reason(reason);
  output.longitudinal_diff(0.0);
  output.lateral_diff(0.0);
  output.gear(gear);
  output.estop(std::move(estop));
  output.trajectory_type(PlanningTrajectoryType::TRAJECTORY_TYPE_SHORT_PATH);
  return output;
}

PlanningTrajectory BuildTrajectoryFromPathProvider(
    const valet_parking_config_t& config,
    const InputMetadata& metadata,
    const TL::planning::OpenSpacePathOutput& path_output,
    const std::string& reason) {
  const std::vector<FlattenedPathProviderPoint> flattened_points =
      FlattenPathProviderOutput(path_output);
  if (flattened_points.size() < 2U) {
    return BuildEstopTrajectory(config, metadata,
                                reason + "; flattened path has too few points");
  }

  constexpr double kNominalSpeedMps = 1.0;
  double total_path_length = 0.0;
  std::vector<TrajectoryPoint> points;
  points.reserve(flattened_points.size());

  TL::common::PathPoint previous_source = flattened_points.front().path_point;
  for (std::size_t i = 0U; i < flattened_points.size(); ++i) {
    const TL::common::PathPoint& source = flattened_points[i].path_point;
    if (i > 0U) {
      const double dx = source.x - previous_source.x;
      const double dy = source.y - previous_source.y;
      total_path_length += std::sqrt(dx * dx + dy * dy);
    }

    PathPoint path_point;
    path_point.x(source.x);
    path_point.y(source.y);
    path_point.z(source.z);
    path_point.theta(NormalizeAngle(source.theta));
    path_point.kappa(source.kappa);
    path_point.s(total_path_length);
    path_point.l(source.l);
    path_point.dkappa(source.dkappa);
    path_point.ddkappa(source.ddkappa);
    path_point.lane_id("valet_parking_path_provider");
    path_point.x_derivative(std::cos(source.theta));
    path_point.y_derivative(std::sin(source.theta));

    GaussianInfo gaussian_info;
    gaussian_info.sigma_x(0.3);
    gaussian_info.sigma_y(0.25);
    gaussian_info.correlation(0.0);
    gaussian_info.area_probability(0.95);
    gaussian_info.ellipse_a(0.5);
    gaussian_info.ellipse_b(0.25);
    gaussian_info.theta_a(source.theta);

    TrajectoryPoint trajectory_point;
    trajectory_point.path_point(std::move(path_point));
    trajectory_point.v(kNominalSpeedMps);
    trajectory_point.a(0.0);
    trajectory_point.relative_time(total_path_length / kNominalSpeedMps);
    trajectory_point.da(0.0);
    trajectory_point.steer(0.0);
    trajectory_point.gaussian_info(std::move(gaussian_info));
    points.push_back(std::move(trajectory_point));
    previous_source = source;
  }

  const uint64_t publish_stamp_ms = NowMilliseconds();
  const uint64_t data_stamp_ms = metadata.data_stamp_ms == 0U
      ? publish_stamp_ms
      : metadata.data_stamp_ms;

  EStop estop;
  estop.is_estop(false);
  estop.reason(reason);

  const TL::common::PathPoint& first = flattened_points.front().path_point;
  const TL::common::PathPoint& last = flattened_points.back().path_point;

  PlanningTrajectory output;
  output.header(MakeHeader(metadata.seq, metadata.frame_id, publish_stamp_ms,
                           data_stamp_ms));
  output.total_path_length(total_path_length);
  output.total_path_time(total_path_length / kNominalSpeedMps);
  output.trajectory_point(std::move(points));
  output.is_replan(false);
  output.replan_type(0U);
  output.replan_reason(reason);
  output.longitudinal_diff(last.x - first.x);
  output.lateral_diff(last.y - first.y);
  output.gear(ConvertGear(flattened_points.front().gear));
  output.estop(std::move(estop));
  output.trajectory_type(PlanningTrajectoryType::TRAJECTORY_TYPE_NORMAL);
  return output;
}

PlanningTrajectory BuildTrajectoryFromPathPartition(
    const valet_parking_config_t& config,
    const InputMetadata& metadata,
    const TL::planning::PartitionOutput& partition_output,
    const std::string& reason) {
  const std::vector<FlattenedPathProviderPoint> flattened_points =
      FlattenPathPartitionOutput(partition_output);
  if (flattened_points.size() < 2U) {
    return BuildEstopTrajectory(config, metadata,
                                reason + "; chosen path has too few points");
  }

  constexpr double kNominalSpeedMps = 1.0;
  double total_path_length = 0.0;
  std::vector<TrajectoryPoint> points;
  points.reserve(flattened_points.size());

  TL::common::PathPoint previous_source = flattened_points.front().path_point;
  for (std::size_t i = 0U; i < flattened_points.size(); ++i) {
    const TL::common::PathPoint& source = flattened_points[i].path_point;
    if (i > 0U) {
      const double dx = source.x - previous_source.x;
      const double dy = source.y - previous_source.y;
      total_path_length += std::sqrt(dx * dx + dy * dy);
    }

    PathPoint path_point;
    path_point.x(source.x);
    path_point.y(source.y);
    path_point.z(source.z);
    path_point.theta(NormalizeAngle(source.theta));
    path_point.kappa(source.kappa);
    path_point.s(total_path_length);
    path_point.l(source.l);
    path_point.dkappa(source.dkappa);
    path_point.ddkappa(source.ddkappa);
    path_point.lane_id("valet_parking_path_partition");
    path_point.x_derivative(std::cos(source.theta));
    path_point.y_derivative(std::sin(source.theta));

    GaussianInfo gaussian_info;
    gaussian_info.sigma_x(0.3);
    gaussian_info.sigma_y(0.25);
    gaussian_info.correlation(0.0);
    gaussian_info.area_probability(0.95);
    gaussian_info.ellipse_a(0.5);
    gaussian_info.ellipse_b(0.25);
    gaussian_info.theta_a(source.theta);

    TrajectoryPoint trajectory_point;
    trajectory_point.path_point(std::move(path_point));
    trajectory_point.v(partition_output.is_stop_path ? 0.0 : kNominalSpeedMps);
    trajectory_point.a(0.0);
    trajectory_point.relative_time(partition_output.is_stop_path
                                       ? 0.0
                                       : total_path_length / kNominalSpeedMps);
    trajectory_point.da(0.0);
    trajectory_point.steer(0.0);
    trajectory_point.gaussian_info(std::move(gaussian_info));
    points.push_back(std::move(trajectory_point));
    previous_source = source;
  }

  const uint64_t publish_stamp_ms = NowMilliseconds();
  const uint64_t data_stamp_ms = metadata.data_stamp_ms == 0U
      ? publish_stamp_ms
      : metadata.data_stamp_ms;

  EStop estop;
  estop.is_estop(false);
  estop.reason(reason);

  const TL::common::PathPoint& first = flattened_points.front().path_point;
  const TL::common::PathPoint& last = flattened_points.back().path_point;

  PlanningTrajectory output;
  output.header(MakeHeader(metadata.seq, metadata.frame_id, publish_stamp_ms,
                           data_stamp_ms));
  output.total_path_length(total_path_length);
  output.total_path_time(partition_output.is_stop_path
                             ? 0.0
                             : total_path_length / kNominalSpeedMps);
  output.trajectory_point(std::move(points));
  output.is_replan(false);
  output.replan_type(0U);
  output.replan_reason(reason);
  output.longitudinal_diff(last.x - first.x);
  output.lateral_diff(last.y - first.y);
  output.gear(ConvertGear(flattened_points.front().gear));
  output.estop(std::move(estop));
  output.trajectory_type(partition_output.is_stop_path
                             ? PlanningTrajectoryType::TRAJECTORY_TYPE_SHORT_PATH
                             : PlanningTrajectoryType::TRAJECTORY_TYPE_NORMAL);
  return output;
}

PlanningTrajectory BuildTrajectoryFromSpeedOptimizer(
    const valet_parking_config_t& config,
    const InputMetadata& metadata,
    const TL::planning::SpeedOptimizerOutput& speed_output,
    const std::string& reason) {
  const TL::planning::DiscretizedTrajectory& source_trajectory =
      speed_output.trajectory_gear.first;
  if (source_trajectory.NumOfPoints() < 2U) {
    return BuildEstopTrajectory(config, metadata,
                                reason + "; speed trajectory has too few points");
  }

  double total_path_length = 0.0;
  std::vector<TrajectoryPoint> points;
  points.reserve(source_trajectory.NumOfPoints());

  TL::common::TrajectoryPoint previous_source =
      source_trajectory.TrajectoryPointAt(0);
  for (std::size_t i = 0U; i < source_trajectory.NumOfPoints(); ++i) {
    const TL::common::TrajectoryPoint& source =
        source_trajectory.TrajectoryPointAt(i);
    if (i > 0U) {
      const double dx = source.path_point.x - previous_source.path_point.x;
      const double dy = source.path_point.y - previous_source.path_point.y;
      total_path_length += std::sqrt(dx * dx + dy * dy);
    }

    PathPoint path_point;
    path_point.x(source.path_point.x);
    path_point.y(source.path_point.y);
    path_point.z(source.path_point.z);
    path_point.theta(NormalizeAngle(source.path_point.theta));
    path_point.kappa(source.path_point.kappa);
    path_point.s(total_path_length);
    path_point.l(source.path_point.l);
    path_point.dkappa(source.path_point.dkappa);
    path_point.ddkappa(source.path_point.ddkappa);
    path_point.lane_id("valet_parking_speed_optimizer");
    path_point.x_derivative(std::cos(source.path_point.theta));
    path_point.y_derivative(std::sin(source.path_point.theta));

    GaussianInfo gaussian_info;
    gaussian_info.sigma_x(0.3);
    gaussian_info.sigma_y(0.25);
    gaussian_info.correlation(0.0);
    gaussian_info.area_probability(0.95);
    gaussian_info.ellipse_a(0.5);
    gaussian_info.ellipse_b(0.25);
    gaussian_info.theta_a(source.path_point.theta);

    TrajectoryPoint trajectory_point;
    trajectory_point.path_point(std::move(path_point));
    trajectory_point.v(source.v);
    trajectory_point.a(source.a);
    trajectory_point.relative_time(source.relative_time);
    trajectory_point.da(source.da);
    trajectory_point.steer(source.steer);
    trajectory_point.gaussian_info(std::move(gaussian_info));
    points.push_back(std::move(trajectory_point));
    previous_source = source;
  }

  const uint64_t publish_stamp_ms = NowMilliseconds();
  const uint64_t data_stamp_ms = metadata.data_stamp_ms == 0U
      ? publish_stamp_ms
      : metadata.data_stamp_ms;

  EStop estop;
  estop.is_estop(false);
  estop.reason(reason);

  const TL::common::TrajectoryPoint& first =
      source_trajectory.TrajectoryPointAt(0);
  const TL::common::TrajectoryPoint& last =
      source_trajectory.TrajectoryPointAt(source_trajectory.NumOfPoints() - 1);

  PlanningTrajectory output;
  output.header(MakeHeader(metadata.seq, metadata.frame_id, publish_stamp_ms,
                           data_stamp_ms));
  output.total_path_length(total_path_length);
  output.total_path_time(last.relative_time);
  output.trajectory_point(std::move(points));
  output.is_replan(false);
  output.replan_type(0U);
  output.replan_reason(reason);
  output.longitudinal_diff(last.path_point.x - first.path_point.x);
  output.lateral_diff(last.path_point.y - first.path_point.y);
  output.gear(ConvertGear(speed_output.trajectory_gear.second));
  output.estop(std::move(estop));
  output.trajectory_type(PlanningTrajectoryType::TRAJECTORY_TYPE_NORMAL);
  return output;
}

}  // namespace

ValetParkingStageParkingAdapter::ValetParkingStageParkingAdapter(
    const valet_parking_config_t& config)
    : config_(config), runtime_(std::make_unique<RuntimeContext>()) {}

ValetParkingStageParkingAdapter::~ValetParkingStageParkingAdapter() = default;

int ValetParkingStageParkingAdapter::UpdateVehicleState(
    const valet_parking_vehicle_state_t& vehicle_state) {
  std::lock_guard<std::mutex> lock(runtime_mutex_);
  if (runtime_ == nullptr) {
    runtime_ = std::make_unique<RuntimeContext>();
  }
  return runtime_->UpdateVehicleState(vehicle_state);
}

int ValetParkingStageParkingAdapter::ClearVehicleState() {
  std::lock_guard<std::mutex> lock(runtime_mutex_);
  if (runtime_ == nullptr) {
    runtime_ = std::make_unique<RuntimeContext>();
  }
  runtime_->ClearVehicleState();
  return VALET_PARKING_OK;
}

int ValetParkingStageParkingAdapter::UpdateObstacles(
    const valet_parking_obstacle_t* obstacles,
    uint32_t obstacle_count) {
  std::lock_guard<std::mutex> lock(runtime_mutex_);
  if (runtime_ == nullptr) {
    runtime_ = std::make_unique<RuntimeContext>();
  }
  return runtime_->UpdateObstacles(obstacles, obstacle_count);
}

int ValetParkingStageParkingAdapter::ClearObstacles() {
  std::lock_guard<std::mutex> lock(runtime_mutex_);
  if (runtime_ == nullptr) {
    runtime_ = std::make_unique<RuntimeContext>();
  }
  runtime_->ClearObstacles();
  return VALET_PARKING_OK;
}

bool ValetParkingStageParkingAdapter::Process(const SelectedSlot& input_sample,
                                               const ParkingCommand* command_sample,
                                               PlanningTrajectory* output_sample,
                                               std::string* status_reason) {
  if (output_sample == nullptr || status_reason == nullptr) {
    return false;
  }

  std::lock_guard<std::mutex> lock(runtime_mutex_);
  if (runtime_ == nullptr) {
    runtime_ = std::make_unique<RuntimeContext>();
  }

  InputMetadata metadata = BuildMetadata(input_sample);
  FunctionManagerProjection function_projection =
      BuildFunctionManagerProjection(command_sample,
                                     runtime_->path_provider.has_history);
  auto build_stage_context = [&]() {
    return BuildStageProcessContextLite(
        BuildStageFacadeInputLite(
            metadata, input_sample, command_sample, function_projection,
            runtime_->vehicle_input, runtime_->obstacles.size(),
            runtime_->path_provider, runtime_->has_last_speed_frame,
            runtime_->stage_exit_requested, runtime_->direct_command.active,
            runtime_->processed_frames));
  };
  StageProcessContextLite stage_context = build_stage_context();
  if (command_sample != nullptr && command_sample->is_valid()) {
    const ParkingCommandMode command_mode = command_sample->mode();
    const std::string command_mode_text =
        ParkingCommandModeToString(command_mode);
    if (command_sample->reset_history()) {
      runtime_->ResetPlanningState();
    }
    function_projection =
        BuildFunctionManagerProjection(command_sample,
                                       runtime_->path_provider.has_history);
    stage_context = build_stage_context();

    if (command_mode == ParkingCommandMode::PARKING_COMMAND_DIRECT_FORWARD ||
        command_mode == ParkingCommandMode::PARKING_COMMAND_DIRECT_BACKWARD) {
      const bool forward =
          command_mode == ParkingCommandMode::PARKING_COMMAND_DIRECT_FORWARD;
      runtime_->direct_command.active = true;
      runtime_->direct_command.last_mode = command_mode;
      const DirectSpeedOptimizerProfile direct_speed_profile =
          BuildDirectSpeedOptimizerProfile(command_sample->direct_speed_mps());
      const TL::planning::OpenSpaceSpeedOptimizerConfig direct_speed_config =
          BuildDirectSpeedOptimizerConfig(runtime_->speed_config,
                                          direct_speed_profile);
      const DirectFinishEvaluation direct_finish_evaluation =
          BuildDirectFinishEvaluation(
              runtime_->vehicle_input.has_vehicle_state
                  ? runtime_->vehicle_input.linear_velocity
                  : 0.0,
              true, kStageFinishStandstillThresholdMps);
      std::ostringstream stream;
      AppendStageControlContract(*command_sample, command_mode_text,
                                 "direct_control",
                                 "OPEN_SPACE_STRAIGHT_PATH", &stream,
                                 &stage_context);
      AppendFunctionManagerProjectionContract(function_projection, &stream);
      AppendMissionStateContract("DIRECT_CONTROL_ACTIVE", "PARKING", false,
                                 &stream);
      stream << ", target_gear="
             << static_cast<int>(TargetGearForDirectCommand(forward))
             << ", direct_distance="
             << std::fixed << std::setprecision(3)
             << SelectDirectDistance(command_sample->direct_distance_m())
             << ", direct_speed="
             << direct_speed_profile.selected_speed_mps
             << ", direct_speed_bound_min="
             << direct_speed_profile.min_sample_speed
             << ", direct_speed_bound_max="
             << direct_speed_profile.max_sample_speed
             << ", trajectory_type=NORMAL";
      AppendDirectFinishContract(direct_finish_evaluation, &stream);
      AppendRuntimeLifecycleContract(
          "direct_control_active", "continue_direct_control",
          "preserve_ordinary_history", "update_from_direct_speed_optimizer",
          "keep_direct_command", runtime_->path_provider.has_history,
          runtime_->has_last_speed_frame, runtime_->direct_command.active,
          &stream);
      const std::string stage_control_reason = stream.str();

      TL::planning::PartitionOutput straight_partition_output;
      std::string straight_path_reason;
      if (RunOpenSpaceStraightPath(config_, metadata, runtime_->vehicle_input,
                                   *command_sample, forward,
                                   &runtime_->straight_path_provider,
                                   &straight_partition_output,
                                   &straight_path_reason)) {
        MarkStageStraightPathOutput(straight_partition_output, &stage_context);
        TL::planning::RoiDeciderOutput direct_roi_output;
        TL::planning::SpeedOptimizerOutput speed_output;
        std::string speed_optimizer_reason;
        if (RunSpeedOptimizer(config_, metadata, direct_roi_output,
                              straight_partition_output,
                              runtime_->vehicle_input, runtime_->obstacles,
                              direct_speed_config,
                              runtime_->has_last_speed_frame,
                              runtime_->last_frame_timestamp,
                              runtime_->last_planning_start_relative_time,
                              true,
                              &runtime_->speed_optimizer, &speed_output,
                              &speed_optimizer_reason)) {
          runtime_->UpdateAfterSpeedOptimizer(metadata.data_stamp_ms,
                                              speed_output);
          MarkStageSpeedOutput(speed_output, &stage_context);
          const std::string direct_parking_status =
              straight_partition_output.is_stop_path ? "direct_stop_path"
                                                     : "direct_moving";
          metadata.status_reason = speed_optimizer_reason + "; " +
                                   stage_control_reason +
                                   ", parking_status=" +
                                   direct_parking_status + "; " +
                                   straight_path_reason;
          *status_reason = metadata.status_reason;
          *output_sample = BuildTrajectoryFromSpeedOptimizer(
              config_, metadata, speed_output, metadata.status_reason);
          return true;
        }

        runtime_->UpdateAfterPartitionFallback(straight_partition_output);
        const std::string direct_parking_status =
            straight_partition_output.is_stop_path ? "direct_stop_path"
                                                   : "direct_moving";
        MarkStageFallback("direct_speed_optimizer_failed", &stage_context);
        const bool can_publish_straight_path =
            straight_partition_output.chosen_partitioned_path.first.size() >=
                2U &&
            !straight_partition_output.is_stop_path;
        metadata.status_reason =
            speed_optimizer_reason +
            "; fallback to OPEN_SPACE_STRAIGHT_PATH; " +
            BuildFallbackStageOutputContract(
                "direct_speed_optimizer_failed",
                can_publish_straight_path ? "publish_open_space_straight_path"
                                          : "publish_stage_control_stop",
                "direct_speed_optimizer_fallback",
                direct_parking_status,
                PlanningTrajectoryType::TRAJECTORY_TYPE_SPEED_FALLBACK,
                "DIRECT_CONTROL_FALLBACK", "PARKING", false,
                &function_projection, &runtime_->path_provider,
                runtime_->has_last_speed_frame,
                runtime_->direct_command.active,
                "direct_speed_optimizer_fallback",
                "continue_direct_control_with_fallback",
                "preserve_ordinary_history",
                "clear_failed_speed_frame", "keep_direct_command",
                &stage_context) +
            "; " +
            stage_control_reason + ", parking_status=" +
            direct_parking_status + "; " + straight_path_reason;
        *status_reason = metadata.status_reason;
        if (can_publish_straight_path) {
          *output_sample = BuildTrajectoryFromPathPartition(
              config_, metadata, straight_partition_output,
              metadata.status_reason);
        } else {
          *output_sample = BuildStageControlStopTrajectory(
              config_, metadata, runtime_->vehicle_input,
              metadata.status_reason, ::GearPosition::GEAR_PARKING);
        }
        return true;
      }

      const bool direct_active_before_reset = runtime_->direct_command.active;
      MarkStageFallback("open_space_straight_path_failed", &stage_context);
      runtime_->ResetPlanningState();
      metadata.status_reason =
          stage_control_reason + "; " + straight_path_reason + "; " +
          BuildFallbackStageOutputContract(
              "open_space_straight_path_failed",
              "publish_stage_control_stop",
              "direct_path_fallback", "direct_path_fallback_stop",
              PlanningTrajectoryType::TRAJECTORY_TYPE_SHORT_PATH,
              "DIRECT_CONTROL_FALLBACK", "PARKING", false,
              &function_projection, &runtime_->path_provider,
              runtime_->has_last_speed_frame, direct_active_before_reset,
              "direct_straight_path_fallback",
              "stay_in_parking_stage_with_stop",
              "reset_after_direct_path_failure",
              "reset_after_direct_path_failure",
              "reset_failed_direct_command", &stage_context);
      *status_reason = metadata.status_reason;
      *output_sample = BuildStageControlStopTrajectory(
          config_, metadata, runtime_->vehicle_input, metadata.status_reason,
          ::GearPosition::GEAR_PARKING);
      return true;
    }

    runtime_->direct_command.Reset();

    if (command_mode == ParkingCommandMode::PARKING_COMMAND_PAUSE ||
        command_mode == ParkingCommandMode::PARKING_COMMAND_BRAKE ||
        command_mode == ParkingCommandMode::PARKING_COMMAND_FINISH) {
      std::ostringstream stream;
      std::string stage_status = "paused";
      std::string parking_status = "stage_control_stop";
      if (command_mode == ParkingCommandMode::PARKING_COMMAND_BRAKE) {
        stage_status = "braking";
      } else if (command_mode == ParkingCommandMode::PARKING_COMMAND_FINISH) {
        stage_status = "mission_finished";
        parking_status = "mission_finished";
      }
      AppendStageControlContract(*command_sample, command_mode_text,
                                 stage_status, "STAGE_CONTROL_STOP", &stream,
                                 &stage_context);
      AppendFunctionManagerProjectionContract(function_projection, &stream);
      AppendMissionStateContract(
          command_mode == ParkingCommandMode::PARKING_COMMAND_FINISH
              ? "MISSION_FINISHED"
              : (command_mode == ParkingCommandMode::PARKING_COMMAND_BRAKE
                     ? "MISSION_BRAKING"
                     : "MISSION_PAUSED"),
          command_mode == ParkingCommandMode::PARKING_COMMAND_FINISH
              ? "FINISH"
              : "PARKING",
          command_mode == ParkingCommandMode::PARKING_COMMAND_FINISH, &stream);
      if (command_mode == ParkingCommandMode::PARKING_COMMAND_FINISH) {
        stream << ", MISSIONFINISHED"
               << ", finish_status=MISSIONFINISHED";
      }
      stream << ", trajectory_type=SHORT_PATH"
             << ", parking_status=" << parking_status;
      AppendRuntimeLifecycleContract(
          command_mode == ParkingCommandMode::PARKING_COMMAND_FINISH
              ? "finish_command_ready"
              : (command_mode == ParkingCommandMode::PARKING_COMMAND_BRAKE
                     ? "brake_hold"
                     : "pause_hold"),
          command_mode == ParkingCommandMode::PARKING_COMMAND_FINISH
              ? "latch_finish_hold_after_publish"
              : "hold_stage_control",
          "keep_until_stage_reset", "keep_until_stage_reset",
          "already_clear", runtime_->path_provider.has_history,
          runtime_->has_last_speed_frame, runtime_->direct_command.active,
          &stream);
      metadata.status_reason = stream.str();
      if (command_mode == ParkingCommandMode::PARKING_COMMAND_FINISH) {
        runtime_->stage_exit_requested = true;
      }
      *status_reason = metadata.status_reason;
      *output_sample = BuildStageControlStopTrajectory(
          config_, metadata, runtime_->vehicle_input, metadata.status_reason,
          ::GearPosition::GEAR_PARKING);
      return true;
    }

    if (command_mode == ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_LEFT ||
        command_mode == ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_RIGHT ||
        command_mode == ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_FRONT ||
        command_mode == ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_BACK) {
      std::ostringstream stream;
      AppendStageControlContract(*command_sample, command_mode_text,
                                 "unsupported", "UNSUPPORTED_PARKING_OUT",
                                 &stream, &stage_context);
      AppendFunctionManagerProjectionContract(function_projection, &stream);
      AppendMissionStateContract("UNSUPPORTED_PARKING_OUT", "PARKING",
                                 false, &stream);
      stream << ", trajectory_type=SHORT_PATH"
             << ", parking_status=parking_out_unsupported"
             << ", unsupported_in_mvp"
             << ", unsupported_reason=parking_out_not_in_mvp";
      AppendRuntimeLifecycleContract(
          "parking_out_unsupported", "stay_in_parking_stage",
          "keep_until_supported_parking_out", "keep_until_supported_parking_out",
          "already_clear", runtime_->path_provider.has_history,
          runtime_->has_last_speed_frame, runtime_->direct_command.active,
          &stream);
      metadata.status_reason = stream.str();
      *status_reason = metadata.status_reason;
      *output_sample = BuildStageControlStopTrajectory(
          config_, metadata, runtime_->vehicle_input, metadata.status_reason,
          ::GearPosition::GEAR_PARKING);
      return true;
    }
  }

  if (!input_sample.is_valid()) {
    MarkStageEarlyInputFallback("selected_slot_invalid", &stage_context);
    runtime_->ResetPlanningState();
    metadata.status_reason =
        "selected_slot.is_valid is false; " +
        BuildEarlyEstopFallbackContract(
            "selected_slot_invalid", "selected_slot_invalid",
            function_projection, &runtime_->path_provider,
            runtime_->has_last_speed_frame, runtime_->direct_command.active,
            &stage_context);
    *status_reason = metadata.status_reason;
    *output_sample = BuildEstopTrajectory(config_, metadata, metadata.status_reason);
    return true;
  }

  const auto& lots = input_sample.parking_lots();
  if (input_sample.count() == 0U || lots.empty()) {
    MarkStageEarlyInputFallback("selected_slot_empty", &stage_context);
    runtime_->ResetPlanningState();
    metadata.status_reason =
        "selected_slot has no parking_lots; " +
        BuildEarlyEstopFallbackContract(
            "selected_slot_empty", "selected_slot_empty",
            function_projection, &runtime_->path_provider,
            runtime_->has_last_speed_frame, runtime_->direct_command.active,
            &stage_context);
    *status_reason = metadata.status_reason;
    *output_sample = BuildEstopTrajectory(config_, metadata, metadata.status_reason);
    return true;
  }

  if (static_cast<std::size_t>(input_sample.count()) > lots.size()) {
    MarkStageEarlyInputFallback("selected_slot_count_overflow",
                                &stage_context);
    runtime_->ResetPlanningState();
    metadata.status_reason =
        "selected_slot count exceeds parking_lots size; " +
        BuildEarlyEstopFallbackContract(
            "selected_slot_count_overflow",
            "selected_slot_count_overflow", function_projection,
            &runtime_->path_provider, runtime_->has_last_speed_frame,
            runtime_->direct_command.active, &stage_context);
    *status_reason = metadata.status_reason;
    *output_sample = BuildEstopTrajectory(config_, metadata, metadata.status_reason);
    return true;
  }

  if (runtime_->direct_command.active &&
      IsDirectCommandMode(runtime_->direct_command.last_mode)) {
    const ParkingCommandMode released_mode = runtime_->direct_command.last_mode;
    const DirectFinishEvaluation direct_finish_evaluation =
        BuildDirectFinishEvaluation(
            runtime_->vehicle_input.has_vehicle_state
                ? runtime_->vehicle_input.linear_velocity
                : 0.0,
            false, kStageFinishStandstillThresholdMps);
    const FunctionManagerProjection released_projection =
        BuildReleasedDirectFunctionProjection(released_mode);
    const std::string released_action =
        DirectReleasedActionName(released_mode);

    std::ostringstream stream;
    stream << "STAGE_CONTROL " << released_action
           << ", command_action=" << released_action
           << ", stage_status="
           << (direct_finish_evaluation.ready_to_finish
                   ? "mission_finished"
                   : "direct_release_waiting")
           << ", skip=ROI_PATH_PROVIDER_PATH_PARTITION"
           << ", task=DIRECT_COMMAND_RELEASE"
           << ", original_flow_branch=direct_open_space"
           << ", reset_history=false";
    MarkStageDirectRelease(direct_finish_evaluation.ready_to_finish,
                           BuildStageFunctionManagerLite(released_projection),
                           &stage_context);
    AppendStageProjectionContract("stage_control", &stream, &stage_context);
    AppendFunctionManagerProjectionContract(released_projection, &stream);
    AppendMissionStateContract(
        direct_finish_evaluation.ready_to_finish ? "DIRECT_FINISH_READY"
                                                 : "DIRECT_FINISH_WAITING",
        direct_finish_evaluation.ready_to_finish ? "FINISH" : "PARKING",
        direct_finish_evaluation.ready_to_finish, &stream);
    AppendDirectFinishContract(direct_finish_evaluation, &stream);
    AppendRuntimeLifecycleContract(
        direct_finish_evaluation.ready_to_finish ? "direct_release_ready"
                                                 : "direct_release_waiting",
        direct_finish_evaluation.ready_to_finish
            ? "reset_planning_state_after_publish"
            : "wait_for_direct_release_ready",
        direct_finish_evaluation.ready_to_finish ? "reset_after_publish"
                                                 : "keep_until_release_ready",
        direct_finish_evaluation.ready_to_finish ? "reset_after_publish"
                                                 : "keep_until_release_ready",
        direct_finish_evaluation.ready_to_finish ? "reset_after_publish"
                                                 : "keep_release_waiting",
        runtime_->path_provider.has_history, runtime_->has_last_speed_frame,
        runtime_->direct_command.active, &stream);
    stream << ", previous_direct_command="
           << ParkingCommandModeToString(released_mode)
           << ", trajectory_type=SHORT_PATH"
           << ", parking_status="
           << (direct_finish_evaluation.ready_to_finish
                   ? "mission_finished"
                   : "direct_finish_waiting");

    metadata.status_reason = stream.str();
    if (direct_finish_evaluation.ready_to_finish) {
      runtime_->ResetPlanningState();
    }
    *status_reason = metadata.status_reason;
    *output_sample = BuildStageControlStopTrajectory(
        config_, metadata, runtime_->vehicle_input, metadata.status_reason,
        ::GearPosition::GEAR_PARKING);
    return true;
  }

  if (runtime_->stage_exit_requested) {
    const FunctionManagerProjection finish_hold_projection =
        BuildStageFinishHoldFunctionProjection();

    std::ostringstream stream;
    stream << "STAGE_CONTROL FINISH_HOLD"
           << ", command_action=FINISH_HOLD"
           << ", stage_status=mission_finished"
           << ", skip=ROI_PATH_PROVIDER_PATH_PARTITION"
           << ", task=STAGE_FINISH_HOLD"
           << ", original_flow_branch=stage_finish_hold"
           << ", reset_history=false";
    MarkStageFinishHold(BuildStageFunctionManagerLite(finish_hold_projection),
                        &stage_context);
    AppendStageProjectionContract("stage_control", &stream, &stage_context);
    AppendFunctionManagerProjectionContract(finish_hold_projection, &stream);
    AppendMissionStateContract("MISSION_FINISHED", "FINISH", true, &stream);
    AppendRuntimeLifecycleContract(
        "stage_finish_hold", "hold_until_reset_history_or_new_stage",
        "keep_until_stage_reset", "keep_until_stage_reset", "already_clear",
        runtime_->path_provider.has_history, runtime_->has_last_speed_frame,
        runtime_->direct_command.active, &stream);
    stream << ", trajectory_type=SHORT_PATH"
           << ", parking_status=mission_finished";

    metadata.status_reason = stream.str();
    *status_reason = metadata.status_reason;
    *output_sample = BuildStageControlStopTrajectory(
        config_, metadata, runtime_->vehicle_input, metadata.status_reason,
        ::GearPosition::GEAR_PARKING);
    return true;
  }

  std::string selected_lot_reason;
  const ParkingLot* selected_lot =
      SelectParkingLot(input_sample, &selected_lot_reason);
  if (selected_lot == nullptr) {
    MarkStageEarlyInputFallback("selected_lot_unavailable", &stage_context);
    runtime_->ResetPlanningState();
    metadata.status_reason =
        selected_lot_reason + "; " +
        BuildEarlyEstopFallbackContract(
            "selected_lot_unavailable", "selected_lot_unavailable",
            function_projection, &runtime_->path_provider,
            runtime_->has_last_speed_frame, runtime_->direct_command.active,
            &stage_context);
    *status_reason = metadata.status_reason;
    *output_sample = BuildEstopTrajectory(config_, metadata, metadata.status_reason);
    return true;
  }
  MarkStageSelectedParkingLot(selected_lot->parking_seq(), &stage_context);

  TL::perception::ParkingLotOut parking_lot;
  if (!ConvertParkingLot(*selected_lot, &parking_lot, &metadata.status_reason)) {
    const std::string failure_reason = metadata.status_reason;
    MarkStageEarlyInputFallback("parking_lot_convert_failed",
                                &stage_context);
    runtime_->ResetPlanningState();
    metadata.status_reason =
        failure_reason + "; " +
        BuildEarlyEstopFallbackContract(
            "parking_lot_convert_failed",
            "parking_lot_convert_failed", function_projection,
            &runtime_->path_provider, runtime_->has_last_speed_frame,
            runtime_->direct_command.active, &stage_context);
    *status_reason = metadata.status_reason;
    *output_sample = BuildEstopTrajectory(config_, metadata, metadata.status_reason);
    return true;
  }
  if (!ValidateVehicleNearParkingLot(parking_lot, runtime_->vehicle_input,
                                     &metadata.status_reason)) {
    const std::string failure_reason = metadata.status_reason;
    MarkStageEarlyInputFallback("vehicle_lot_precheck_failed",
                                &stage_context);
    runtime_->ResetPlanningState();
    metadata.status_reason =
        failure_reason + "; " +
        BuildEarlyEstopFallbackContract(
            "vehicle_lot_precheck_failed",
            "vehicle_lot_precheck_failed", function_projection,
            &runtime_->path_provider, runtime_->has_last_speed_frame,
            runtime_->direct_command.active, &stage_context);
    *status_reason = metadata.status_reason;
    *output_sample =
        BuildEstopTrajectory(config_, metadata, metadata.status_reason);
    return true;
  }

  const TL::common::VehicleState vehicle_state =
      BuildVehicleState(config_, metadata.data_stamp_ms,
                        runtime_->vehicle_input);
  TL::planning::OpenSpaceRoiDecider roi_decider(
      TL::planning::LoadEP30VehicleParam(),
      TL::planning::RoiDeciderConfig::GetDefault());
  TL::planning::RoiDeciderOutput roi_output;

  if (IsSmokeFlagEnabled(kForceRoiDeciderFailEnv)) {
    std::ostringstream stream;
    stream << "ROI_DECIDER failed: forced_by_smoke_env";
    AppendOpenSpaceTaskContract("roi_decider_output", &stream);
    MarkStageEarlyInputFallback("roi_decider_failed", &stage_context);
    runtime_->ResetPlanningState();
    metadata.status_reason =
        stream.str() + "; " +
        BuildEarlyEstopFallbackContract(
            "roi_decider_failed", "roi_decider_failed",
            function_projection, &runtime_->path_provider,
            runtime_->has_last_speed_frame, runtime_->direct_command.active,
            &stage_context);
    *status_reason = metadata.status_reason;
    *output_sample =
        BuildEstopTrajectory(config_, metadata, metadata.status_reason);
    return true;
  }

  const int roi_ret = roi_decider.Process(parking_lot, vehicle_state, &roi_output);
  if (roi_ret != 0) {
    MarkStageEarlyInputFallback("roi_decider_failed", &stage_context);
    runtime_->ResetPlanningState();
    metadata.status_reason = "ROI_DECIDER failed: ret=" + std::to_string(roi_ret) +
                             ", lot_status=" +
                             std::to_string(static_cast<int>(roi_output.lot_status)) +
                             "; " +
                             BuildEarlyEstopFallbackContract(
                                 "roi_decider_failed", "roi_decider_failed",
                                 function_projection,
                                 &runtime_->path_provider,
                                 runtime_->has_last_speed_frame,
                                 runtime_->direct_command.active,
                                 &stage_context);
    *status_reason = metadata.status_reason;
    *output_sample = BuildEstopTrajectory(config_, metadata, metadata.status_reason);
    return true;
  }

  const std::string roi_reason =
      BuildRoiReason(roi_output, selected_lot->parking_seq());
  MarkStageRoiOutput(selected_lot->parking_seq(), &stage_context);
  const PathProviderPreCheckResult precheck_result =
      RunPathProviderPreCheck(config_, roi_output, runtime_->vehicle_input,
                              runtime_->obstacles);
  const std::string precheck_reason =
      BuildPathProviderPreCheckReason(precheck_result);
  if (!precheck_result.ok) {
    MarkStageFallback("precheck_failed", &stage_context);
    runtime_->ResetPlanningState();
    metadata.status_reason = precheck_reason + "; " +
                             BuildFallbackStageOutputContract(
                                 "precheck_failed", "publish_estop",
                                 "fallback_estop", "precheck_failed",
                                 PlanningTrajectoryType::
                                     TRAJECTORY_TYPE_UNKNOWN,
                                 "MISSION_ESTOP", "PARKING", false,
                                 &function_projection,
                                 &runtime_->path_provider,
                                 runtime_->has_last_speed_frame,
                                 runtime_->direct_command.active,
                                 "precheck_fallback",
                                 "stay_in_parking_stage",
                                 "reset_on_precheck_failure",
                                 "reset_on_precheck_failure",
                                 "already_clear", &stage_context) +
                             "; " +
                             BuildFunctionManagerProjectionReason(
                                 function_projection) +
                             "; " + roi_reason;
    *status_reason = metadata.status_reason;
    *output_sample =
        BuildEstopTrajectory(config_, metadata, metadata.status_reason);
    return true;
  }

  TL::planning::OpenSpacePathOutput path_output;
  std::string path_provider_reason;
  const std::vector<TL::planning::OpenSpacePathInput> preplan_inputs =
      BuildPathProviderPrePlanInputs(config_, metadata, input_sample,
                                     selected_lot->parking_seq(),
                                     vehicle_state, runtime_->vehicle_input,
                                     runtime_->obstacles,
                                     runtime_->path_provider);
  if (RunPathProvider(config_, metadata, selected_lot->parking_seq(),
                      roi_output, runtime_->vehicle_input,
                      runtime_->obstacles,
                      runtime_->replan_triggered_by_speed_plan,
                      runtime_->current_path_has_collision_risk,
                      preplan_inputs,
                      &runtime_->path_provider, &path_output,
                      &path_provider_reason)) {
    MarkStagePathProviderOutput(path_output, &stage_context);
    TL::planning::PartitionOutput partition_output;
    std::string path_partition_reason;
    if (RunPathPartition(config_, metadata, roi_output, path_output,
                         runtime_->vehicle_input, runtime_->obstacles,
                         runtime_->last_published_gear,
                         runtime_->replan_triggered_by_speed_plan,
                         runtime_->last_speed_plan_collision_info,
                         &runtime_->path_partition,
                         &partition_output, &path_partition_reason)) {
      MarkStagePathPartitionOutput(partition_output, &stage_context);
      const StageFinishEvaluation stage_finish_evaluation =
          UpdateStageFinishEvaluation(
              partition_output,
              runtime_->vehicle_input.has_vehicle_state
                  ? runtime_->vehicle_input.linear_velocity
                  : 0.0,
              kStageFinishStandstillThresholdMps,
              kStageFinishRequiredConsecutiveFrames,
              &runtime_->stage_finish);
      MarkStageFinishEvaluation(
          BuildStageFinishEvaluationLite(stage_finish_evaluation),
          &stage_context);
      TL::planning::SpeedOptimizerOutput speed_output;
      std::string speed_optimizer_reason;
      if (RunSpeedOptimizer(config_, metadata, roi_output, partition_output,
                            runtime_->vehicle_input, runtime_->obstacles,
                            runtime_->speed_config,
                            runtime_->has_last_speed_frame,
                            runtime_->last_frame_timestamp,
                            runtime_->last_planning_start_relative_time,
                            false,
                            &runtime_->speed_optimizer,
                            &speed_output, &speed_optimizer_reason)) {
        runtime_->UpdateAfterSpeedOptimizer(metadata.data_stamp_ms,
                                            speed_output);
        MarkStageSpeedOutput(speed_output, &stage_context);
        if (stage_finish_evaluation.ready_to_finish) {
          runtime_->stage_exit_requested = true;
        }
        metadata.status_reason =
            speed_optimizer_reason + "; " +
            BuildOpenSpaceStageOutputContract(partition_output,
                                              &speed_output,
                                              &stage_finish_evaluation,
                                              &function_projection,
                                              &runtime_->path_provider,
                                              runtime_->has_last_speed_frame,
                                              runtime_->direct_command.active,
                                              &stage_context) +
            "; " + path_partition_reason + "; " + path_provider_reason +
            "; " + precheck_reason + "; " + roi_reason;
        *status_reason = metadata.status_reason;
        *output_sample = BuildTrajectoryFromSpeedOptimizer(
            config_, metadata, speed_output, metadata.status_reason);
        return true;
      }

      runtime_->UpdateAfterPartitionFallback(partition_output);
      if (stage_finish_evaluation.ready_to_finish) {
        runtime_->stage_exit_requested = true;
      }
      MarkStageFallback("speed_optimizer_failed", &stage_context);
      metadata.status_reason =
          speed_optimizer_reason + "; fallback to PATH_PARTITION; " +
          BuildFallbackStageOutputContract(
              "speed_optimizer_failed", "publish_path_partition_path",
              stage_finish_evaluation.ready_to_finish
                  ? "mission_finished"
                  : "speed_optimizer_fallback",
              stage_finish_evaluation.ready_to_finish
                  ? "mission_finished"
                  : "path_partition_fallback",
              StageTrajectoryType(partition_output, nullptr,
                                  &stage_finish_evaluation),
              stage_finish_evaluation.ready_to_finish ? "MISSION_FINISHED"
                                                       : "MISSION_FALLBACK",
              stage_finish_evaluation.ready_to_finish ? "FINISH" : "PARKING",
              stage_finish_evaluation.ready_to_finish,
              &function_projection, &runtime_->path_provider,
              runtime_->has_last_speed_frame,
              runtime_->direct_command.active, "speed_optimizer_fallback",
              stage_finish_evaluation.ready_to_finish
                  ? "latch_finish_hold_after_publish"
                  : "continue_parking_with_fallback",
              stage_finish_evaluation.ready_to_finish
                  ? "keep_until_stage_reset"
                  : "keep_path_partition_fallback",
              stage_finish_evaluation.ready_to_finish
                  ? "keep_until_stage_reset"
                  : "clear_failed_speed_frame",
              "already_clear", &stage_context) +
          "; " +
          BuildOpenSpaceStageOutputContract(partition_output, nullptr,
                                            &stage_finish_evaluation,
                                            &function_projection,
                                            &runtime_->path_provider,
                                            runtime_->has_last_speed_frame,
                                            runtime_->direct_command.active,
                                            &stage_context) + "; " +
          path_partition_reason + "; " + path_provider_reason + "; " +
          precheck_reason + "; " + roi_reason;
      *status_reason = metadata.status_reason;
      *output_sample = BuildTrajectoryFromPathPartition(
          config_, metadata, partition_output, metadata.status_reason);
      return true;
    }

    runtime_->ResetPlanningState();
    MarkStageFallback("path_partition_failed", &stage_context);
    metadata.status_reason =
        path_partition_reason + "; fallback to PATH_PROVIDER; " +
        BuildFallbackStageOutputContract(
            "path_partition_failed", "publish_path_provider_path",
            "path_partition_fallback", "path_provider_fallback",
            PlanningTrajectoryType::TRAJECTORY_TYPE_PATH_FALLBACK,
            "MISSION_FALLBACK", "PARKING", false,
            &function_projection, &runtime_->path_provider,
            runtime_->has_last_speed_frame, runtime_->direct_command.active,
            "path_partition_fallback", "continue_parking_with_fallback",
            "reset_after_path_partition_failure",
            "reset_after_path_partition_failure", "already_clear",
            &stage_context) +
        "; " +
        BuildFunctionManagerProjectionReason(function_projection) + "; " +
        path_provider_reason + "; " + precheck_reason + "; " + roi_reason;
    *status_reason = metadata.status_reason;
    *output_sample = BuildTrajectoryFromPathProvider(
        config_, metadata, path_output, metadata.status_reason);
    return true;
  }

  runtime_->ResetPlanningState();
  MarkStageFallback("path_provider_failed", &stage_context);
  metadata.status_reason =
      path_provider_reason + "; fallback to ROI seed; " +
      BuildFallbackStageOutputContract(
          "path_provider_failed", "publish_roi_seed",
          "path_provider_fallback", "roi_seed_fallback",
          PlanningTrajectoryType::TRAJECTORY_TYPE_PATH_FALLBACK,
          "MISSION_FALLBACK", "PARKING", false,
          &function_projection, &runtime_->path_provider,
          runtime_->has_last_speed_frame, runtime_->direct_command.active,
          "path_provider_fallback", "continue_parking_with_fallback",
          "reset_after_path_provider_failure",
          "reset_after_path_provider_failure", "already_clear",
          &stage_context) +
      "; " +
      BuildFunctionManagerProjectionReason(function_projection) + "; " +
      precheck_reason + "; " + roi_reason;
  *status_reason = metadata.status_reason;
  *output_sample =
      BuildTrajectoryToTarget(config_, metadata, roi_output.end_pose, false,
                              metadata.status_reason);
  return true;
}

}  // namespace valet_parking
