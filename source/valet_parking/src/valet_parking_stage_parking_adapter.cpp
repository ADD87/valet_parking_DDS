#include "valet_parking_stage_parking_adapter.h"

#include "planning/open_space/vehicle_param.h"
#include "planning/tasks/deciders/open_space_decider/open_space_roi_decider.h"
#include "planning/tasks/optimizers/open_space_path_generation/open_space_path_generator.h"
#include "planning/tasks/optimizers/open_space_path_partition/open_space_path_partition.h"
#include "proto_convert/parking_lot_convert.h"
#include "proto_convert/vehicle_state_convert.h"
#include "valet_parking_topics.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace valet_parking {
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

const ParkingLot* SelectParkingLot(const SelectedSlot& sample) {
  const auto& lots = sample.parking_lots();
  if (lots.empty()) {
    return nullptr;
  }

  const uint32_t selected_seq = sample.opt_parking_seq();
  for (const auto& lot : lots) {
    if (lot.parking_seq() == selected_seq) {
      return &lot;
    }
  }

  return &lots.front();
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

TL::common::VehicleState BuildVehicleState(const valet_parking_config_t& config,
                                           uint64_t stamp_ms) {
  TL::common::VehicleState vehicle_state;
  vehicle_state.x = config.fake_vehicle_x;
  vehicle_state.y = config.fake_vehicle_y;
  vehicle_state.z = 0.0;
  vehicle_state.yaw = config.fake_vehicle_theta;
  vehicle_state.heading = config.fake_vehicle_theta;
  vehicle_state.timestamp = static_cast<double>(stamp_ms) / 1000.0;
  vehicle_state.gear = TL::common::GearPosition::GEAR_PARKING;
  vehicle_state.pose.position.x = vehicle_state.x;
  vehicle_state.pose.position.y = vehicle_state.y;
  vehicle_state.pose.position.z = vehicle_state.z;
  vehicle_state.pose.heading = vehicle_state.heading;
  return vehicle_state;
}

TL::common::PathPoint BuildStartPathPoint(const valet_parking_config_t& config) {
  TL::common::PathPoint start_point;
  start_point.x = config.fake_vehicle_x;
  start_point.y = config.fake_vehicle_y;
  start_point.z = 0.0;
  start_point.theta = config.fake_vehicle_theta;
  start_point.kappa = 0.0;
  start_point.s = 0.0;
  start_point.x_derivative = std::cos(start_point.theta);
  start_point.y_derivative = std::sin(start_point.theta);
  return start_point;
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

TL::planning::OpenSpacePathInput BuildPathProviderInput(
    const valet_parking_config_t& config,
    const InputMetadata& metadata,
    uint32_t parking_seq,
    const TL::planning::RoiDeciderOutput& roi_output) {
  TL::planning::OpenSpacePathInput input;
  input.path_id = static_cast<int>(parking_seq);
  input.replan_status = 0U;
  input.rotate_angle = roi_output.origin_heading;
  input.translate_origin = roi_output.origin_point;
  input.start_point = BuildStartPathPoint(config);
  input.end_pose =
      roi_output.has_fine_tuned ? roi_output.fine_tuned_end_pose
                                : roi_output.end_pose;
  input.xy_bounds = roi_output.xy_bounds;
  input.xy_bounds_is_local = true;
  input.obstacles_segments_vec = roi_output.obs_segments;
  input.dest_region_with_angle = roi_output.dest_region;
  input.path_strategy.init_moving_direction = -1;
  input.path_strategy.disable_search = false;
  input.path_strategy.path_search_strategy.init_path_direction = 0;
  input.path_strategy.path_search_strategy.is_plan_from_start = true;
  input.path_strategy.path_search_strategy.space_structure =
      BuildSpaceStructure(roi_output.scenario_type);
  input.path_strategy.path_search_strategy.park_direction =
      TL::planning::ParkDirection::PARKIN;
  (void)metadata;
  return input;
}

std::size_t CountPathProviderPoints(
    const TL::planning::OpenSpacePathOutput& path_output) {
  std::size_t count = 0U;
  for (const auto& path_pair : path_output.partitioned_path) {
    count += path_pair.first.size();
  }
  return count;
}

bool RunPathProvider(const valet_parking_config_t& config,
                     const InputMetadata& metadata,
                     uint32_t parking_seq,
                     const TL::planning::RoiDeciderOutput& roi_output,
                     TL::planning::OpenSpacePathOutput* path_output,
                     std::string* status_reason) {
  if (path_output == nullptr || status_reason == nullptr) {
    return false;
  }

  try {
    const TL::planning::OpenSpacePathInput input =
        BuildPathProviderInput(config, metadata, parking_seq, roi_output);
    std::atomic<bool> early_stop_flag(false);
    TL::planning::OpenSpacePathGenerator generator(
        BuildPathProviderConfig(), TL::planning::LoadEP30VehicleParam());
    generator.Plan(early_stop_flag, input, path_output);
  } catch (const std::exception& ex) {
    *status_reason = std::string("PATH_PROVIDER failed: exception: ") + ex.what();
    return false;
  } catch (...) {
    *status_reason = "PATH_PROVIDER failed: unknown exception";
    return false;
  }

  const std::size_t point_count = CountPathProviderPoints(*path_output);
  if (!path_output->error_msg.empty()) {
    *status_reason = "PATH_PROVIDER failed: " + path_output->error_msg;
    return false;
  }
  if (path_output->partitioned_path.empty() || point_count < 2U) {
    std::ostringstream stream;
    stream << "PATH_PROVIDER failed: insufficient path points"
           << ", partitions=" << path_output->partitioned_path.size()
           << ", points=" << point_count;
    *status_reason = stream.str();
    return false;
  }

  std::ostringstream stream;
  stream << "PATH_PROVIDER ok"
         << ", partitions=" << path_output->partitioned_path.size()
         << ", points=" << point_count
         << ", path_type=" << path_output->path_type
         << ", smoothed=false";
  *status_reason = stream.str();
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
    const TL::planning::OpenSpacePathOutput& path_output) {
  TL::planning::PartitionInput input;
  input.vehicle_state = BuildVehicleState(config, metadata.data_stamp_ms);
  input.planning_start_point = BuildStartPathPoint(config);
  input.pub_gear = TL::soc::GearPosition::GEAR_PARKING;
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
  input.obstacles_segments_vec = roi_output.obs_segments;
  input.is_vehicle_stand_still = true;
  input.replan_triggered_by_speed_plan = false;
  input.guard_triggered = false;
  input.input_replan_status = path_output.replan_status;
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

bool RunPathPartition(const valet_parking_config_t& config,
                      const InputMetadata& metadata,
                      const TL::planning::RoiDeciderOutput& roi_output,
                      const TL::planning::OpenSpacePathOutput& path_output,
                      TL::planning::PartitionOutput* partition_output,
                      std::string* status_reason) {
  if (partition_output == nullptr || status_reason == nullptr) {
    return false;
  }

  try {
    TL::planning::OpenSpacePathPartition partition(
        TL::planning::OpenSpacePathPartitionConfig::GetDefault(),
        TL::planning::LoadEP30VehicleParam());
    const TL::common::Status reset_status = partition.Reset();
    if (!reset_status.ok()) {
      *status_reason =
          "PATH_PARTITION failed: reset: " + reset_status.error_message();
      return false;
    }

    const TL::planning::PartitionInput input =
        BuildPathPartitionInput(config, metadata, roi_output, path_output);
    const TL::common::Status status =
        partition.Execute(input, partition_output);
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
    std::ostringstream stream;
    stream << "PATH_PARTITION failed: insufficient chosen path points"
           << ", decision=" << static_cast<int>(partition_output->path_decision)
           << ", chosen_points=" << chosen_points;
    *status_reason = stream.str();
    return false;
  }

  std::ostringstream stream;
  stream << "PATH_PARTITION ok"
         << ", decision=" << static_cast<int>(partition_output->path_decision)
         << ", finish=" << static_cast<int>(partition_output->finish_status)
         << ", chosen_idx=(" << partition_output->chosen_path_idx.first
         << "," << partition_output->chosen_path_idx.second << ")"
         << ", chosen_points=" << chosen_points
         << ", gear=" << static_cast<int>(partition_output->chosen_partitioned_path.second)
         << ", gear_changed=" << (partition_output->is_gear_changed ? "true" : "false")
         << ", stop_path=" << (partition_output->is_stop_path ? "true" : "false");
  const std::string debug = CompactDebugText(partition_output->path_decision_debug);
  if (!debug.empty()) {
    stream << ", debug=" << debug;
  }
  *status_reason = stream.str();
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
  output.trajectory_type(PlanningTrajectoryType::TRAJECTORY_TYPE_NORMAL);
  return output;
}

std::string BuildRoiReason(const TL::planning::RoiDeciderOutput& output) {
  std::ostringstream stream;
  stream << "ROI_DECIDER ok"
         << ", scenario=" << static_cast<int>(output.scenario_type)
         << ", lot_status=" << static_cast<int>(output.lot_status)
         << ", target=(" << std::fixed << std::setprecision(3)
         << output.end_pose.x << "," << output.end_pose.y << ","
         << output.end_pose.theta << ")";
  return stream.str();
}

}  // namespace

ValetParkingStageParkingAdapter::ValetParkingStageParkingAdapter(
    const valet_parking_config_t& config)
    : config_(config) {}

bool ValetParkingStageParkingAdapter::Process(const SelectedSlot& input_sample,
                                               PlanningTrajectory* output_sample,
                                               std::string* status_reason) const {
  if (output_sample == nullptr || status_reason == nullptr) {
    return false;
  }

  InputMetadata metadata = BuildMetadata(input_sample);
  if (!input_sample.is_valid()) {
    metadata.status_reason = "selected_slot.is_valid is false";
    *status_reason = metadata.status_reason;
    *output_sample = BuildEstopTrajectory(config_, metadata, metadata.status_reason);
    return true;
  }

  const auto& lots = input_sample.parking_lots();
  if (input_sample.count() == 0U || lots.empty()) {
    metadata.status_reason = "selected_slot has no parking_lots";
    *status_reason = metadata.status_reason;
    *output_sample = BuildEstopTrajectory(config_, metadata, metadata.status_reason);
    return true;
  }

  if (static_cast<std::size_t>(input_sample.count()) > lots.size()) {
    metadata.status_reason = "selected_slot count exceeds parking_lots size";
    *status_reason = metadata.status_reason;
    *output_sample = BuildEstopTrajectory(config_, metadata, metadata.status_reason);
    return true;
  }

  const ParkingLot* selected_lot = SelectParkingLot(input_sample);
  if (selected_lot == nullptr) {
    metadata.status_reason = "selected_slot selected lot is unavailable";
    *status_reason = metadata.status_reason;
    *output_sample = BuildEstopTrajectory(config_, metadata, metadata.status_reason);
    return true;
  }

  TL::perception::ParkingLotOut parking_lot;
  if (!ConvertParkingLot(*selected_lot, &parking_lot, &metadata.status_reason)) {
    *status_reason = metadata.status_reason;
    *output_sample = BuildEstopTrajectory(config_, metadata, metadata.status_reason);
    return true;
  }

  const TL::common::VehicleState vehicle_state =
      BuildVehicleState(config_, metadata.data_stamp_ms);
  TL::planning::OpenSpaceRoiDecider roi_decider(
      TL::planning::LoadEP30VehicleParam(),
      TL::planning::RoiDeciderConfig::GetDefault());
  TL::planning::RoiDeciderOutput roi_output;

  const int roi_ret = roi_decider.Process(parking_lot, vehicle_state, &roi_output);
  if (roi_ret != 0) {
    metadata.status_reason = "ROI_DECIDER failed: ret=" + std::to_string(roi_ret) +
                             ", lot_status=" +
                             std::to_string(static_cast<int>(roi_output.lot_status));
    *status_reason = metadata.status_reason;
    *output_sample = BuildEstopTrajectory(config_, metadata, metadata.status_reason);
    return true;
  }

  const std::string roi_reason = BuildRoiReason(roi_output);
  TL::planning::OpenSpacePathOutput path_output;
  std::string path_provider_reason;
  if (RunPathProvider(config_, metadata, selected_lot->parking_seq(),
                      roi_output, &path_output, &path_provider_reason)) {
    TL::planning::PartitionOutput partition_output;
    std::string path_partition_reason;
    if (RunPathPartition(config_, metadata, roi_output, path_output,
                         &partition_output, &path_partition_reason)) {
      metadata.status_reason =
          path_partition_reason + "; " + path_provider_reason + "; " +
          roi_reason;
      *status_reason = metadata.status_reason;
      *output_sample = BuildTrajectoryFromPathPartition(
          config_, metadata, partition_output, metadata.status_reason);
      return true;
    }

    metadata.status_reason =
        path_partition_reason + "; fallback to PATH_PROVIDER; " +
        path_provider_reason + "; " + roi_reason;
    *status_reason = metadata.status_reason;
    *output_sample = BuildTrajectoryFromPathProvider(
        config_, metadata, path_output, metadata.status_reason);
    return true;
  }

  metadata.status_reason =
      path_provider_reason + "; fallback to ROI seed; " + roi_reason;
  *status_reason = metadata.status_reason;
  *output_sample =
      BuildTrajectoryToTarget(config_, metadata, roi_output.end_pose, false,
                              metadata.status_reason);
  return true;
}

}  // namespace valet_parking
