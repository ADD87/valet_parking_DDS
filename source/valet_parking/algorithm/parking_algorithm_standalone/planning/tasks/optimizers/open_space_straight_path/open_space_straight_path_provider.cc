#include "planning/tasks/optimizers/open_space_straight_path/open_space_straight_path_provider.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <sstream>
#include <string>

namespace TL {
namespace planning {

namespace {

constexpr double kMinStepSize = 1.0e-3;
constexpr double kMaxDirectMoveLength = 20.0;

bool IsFiniteVehicleState(const common::VehicleState& vehicle_state) {
  return std::isfinite(vehicle_state.x) && std::isfinite(vehicle_state.y) &&
         std::isfinite(vehicle_state.heading) &&
         std::isfinite(vehicle_state.linear_velocity);
}

const char* ModeToString(OpenSpaceStraightPathMode mode) {
  switch (mode) {
    case OpenSpaceStraightPathMode::DIRECT_FORWARD:
      return "DIRECT_FORWARD";
    case OpenSpaceStraightPathMode::DIRECT_BACKWARD:
      return "DIRECT_BACKWARD";
  }
  return "UNKNOWN";
}

}  // namespace

OpenSpaceStraightPathProvider::OpenSpaceStraightPathProvider(
    const OpenSpaceStraightPathConfig& config)
    : config_(config) {}

void OpenSpaceStraightPathProvider::Reset() {
  has_last_mode_ = false;
  last_mode_ = OpenSpaceStraightPathMode::DIRECT_FORWARD;
  original_direction_ = common::math::Vec2d();
  original_point_ = common::math::Vec2d();
}

soc::GearPosition OpenSpaceStraightPathProvider::TargetGear(
    OpenSpaceStraightPathMode mode) {
  return mode == OpenSpaceStraightPathMode::DIRECT_FORWARD
             ? soc::GearPosition::GEAR_DRIVE
             : soc::GearPosition::GEAR_REVERSE;
}

double OpenSpaceStraightPathProvider::MovingDirection(
    OpenSpaceStraightPathMode mode) {
  return mode == OpenSpaceStraightPathMode::DIRECT_FORWARD ? 1.0 : -1.0;
}

double OpenSpaceStraightPathProvider::ResolveDirectMoveLength(
    const OpenSpaceStraightPathConfig& config, double override_length) {
  const double requested =
      std::isfinite(override_length) && override_length > 0.0
          ? override_length
          : config.direct_move_length;
  if (!std::isfinite(requested) || requested <= 0.0) {
    return OpenSpaceStraightPathConfig::GetDefault().direct_move_length;
  }
  return std::min(requested, kMaxDirectMoveLength);
}

common::Status OpenSpaceStraightPathProvider::Process(
    const OpenSpaceStraightPathInput& input,
    OpenSpaceStraightPathOutput* const output) {
  if (output == nullptr) {
    return common::Status(
        common::ErrorCode::PLANNER_PARKING_STRAIGHTPATH_ERROR,
        "OpenSpaceStraightPathProvider output is nullptr");
  }
  output->Reset();

  if (!IsFiniteVehicleState(input.vehicle_state)) {
    return common::Status(
        common::ErrorCode::PLANNER_PARKING_STRAIGHTPATH_ERROR,
        "OpenSpaceStraightPathProvider vehicle state is non-finite");
  }

  const double move_direction = MovingDirection(input.mode);
  output->chosen_partitioned_path.second = TargetGear(input.mode);
  auto* path_data = &output->chosen_partitioned_path.first;
  auto start_point = input.planning_start_point;

  std::string stop_reason;
  if (NeedStopDecision(move_direction, input.vehicle_state,
                       input.is_vehicle_stand_still, &stop_reason)) {
    output->is_stop_path = true;
    UpdateOpenSpaceStartPoint(input.vehicle_state, &start_point, output);
    path_data->GenerateStopPath(start_point.path_point.x,
                                start_point.path_point.y,
                                start_point.path_point.theta,
                                start_point.path_point.kappa);
    output->diagnostic = std::string("OPEN_SPACE_STRAIGHT_PATH stop_path, ") +
                         stop_reason;
    return common::Status::OK();
  }

  if (!has_last_mode_ || last_mode_ != input.mode) {
    original_point_ =
        common::math::Vec2d(input.vehicle_state.x, input.vehicle_state.y);
    original_direction_ =
        common::math::Vec2d::CreateUnitVec2d(input.vehicle_state.heading);
    last_mode_ = input.mode;
    has_last_mode_ = true;
    output->direction_changed = true;
    UpdateOpenSpaceStartPoint(input.vehicle_state, &start_point, output);
  }

  const double direct_move_length =
      ResolveDirectMoveLength(config_, input.direct_move_length_override);
  GenerateDirectMovingPath(start_point, move_direction, direct_move_length,
                           path_data);

  std::ostringstream stream;
  stream << "OPEN_SPACE_STRAIGHT_PATH direct_moving"
         << ", mode=" << ModeToString(input.mode)
         << ", direction_changed="
         << (output->direction_changed ? "true" : "false")
         << ", points=" << path_data->size()
         << ", direct_move_length=" << direct_move_length
         << ", step=" << std::max(config_.interpolation_step_size, kMinStepSize)
         << ", gear=" << static_cast<int>(output->chosen_partitioned_path.second)
         << ", stop_path=false";
  output->diagnostic = stream.str();
  return common::Status::OK();
}

bool OpenSpaceStraightPathProvider::NeedStopDecision(
    const double moving_direction,
    const common::VehicleState& vehicle_state,
    const bool is_vehicle_stand_still,
    std::string* const reason) const {
  const soc::GearPosition target_gear =
      moving_direction > 0.0 ? soc::GearPosition::GEAR_DRIVE
                             : soc::GearPosition::GEAR_REVERSE;
  if (target_gear != vehicle_state.gear) {
    if (reason != nullptr) {
      std::ostringstream stream;
      stream << "target_gear_mismatch target="
             << static_cast<int>(target_gear)
             << ", actual=" << static_cast<int>(vehicle_state.gear);
      *reason = stream.str();
    }
    return true;
  }

  const double longitudinal_speed = vehicle_state.linear_velocity;
  if ((longitudinal_speed * moving_direction < 0.0) &&
      !is_vehicle_stand_still) {
    if (reason != nullptr) {
      std::ostringstream stream;
      stream << "velocity_direction_conflict velocity="
             << longitudinal_speed
             << ", moving_direction=" << moving_direction;
      *reason = stream.str();
    }
    return true;
  }
  if (reason != nullptr) {
    *reason = "target_gear_and_velocity_match";
  }
  return false;
}

void OpenSpaceStraightPathProvider::UpdateOpenSpaceStartPoint(
    const common::VehicleState& vehicle_state,
    common::TrajectoryPoint* const start_point,
    OpenSpaceStraightPathOutput* const output) const {
  if (start_point == nullptr) {
    return;
  }
  start_point->path_point.x = vehicle_state.x;
  start_point->path_point.y = vehicle_state.y;
  start_point->path_point.theta = vehicle_state.heading;
  start_point->path_point.s = 0.0;
  start_point->path_point.kappa = 0.0;
  start_point->path_point.dkappa = 0.0;
  start_point->path_point.ddkappa = 0.0;
  if (output != nullptr) {
    output->is_gear_changed = true;
  }
}

void OpenSpaceStraightPathProvider::GenerateDirectMovingPath(
    const common::TrajectoryPoint& start_point,
    const double direction,
    const double direct_move_length,
    DiscretizedPath* const path_data) const {
  if (path_data == nullptr) {
    return;
  }

  const common::math::Vec2d start_to_original(
      start_point.path_point.x - original_point_.x(),
      start_point.path_point.y - original_point_.y());
  const common::math::Vec2d start_point_projection =
      start_to_original.InnerProd(original_direction_) * original_direction_ +
      original_point_;

  double temp_x = start_point_projection.x();
  double temp_y = start_point_projection.y();
  const double theta = original_direction_.Angle();
  const double step_size =
      std::max(config_.interpolation_step_size, kMinStepSize);
  const std::size_t num_of_knots =
      static_cast<std::size_t>(direct_move_length / step_size) + 1U;

  path_data->clear();
  path_data->reserve(num_of_knots);
  double temp_s = 0.0;
  for (std::size_t i = 0U; i < num_of_knots; ++i) {
    common::PathPoint path_point;
    path_point.x = temp_x;
    path_point.y = temp_y;
    path_point.theta = theta;
    path_point.kappa = 0.0;
    path_point.s = temp_s;
    path_point.x_derivative = std::cos(theta);
    path_point.y_derivative = std::sin(theta);
    path_data->emplace_back(path_point);
    temp_x += direction * step_size * std::cos(theta);
    temp_y += direction * step_size * std::sin(theta);
    temp_s += step_size;
  }
}

}  // namespace planning
}  // namespace TL
