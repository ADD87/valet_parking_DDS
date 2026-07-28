#pragma once

#include <string>

#include "common/math/vec2d.h"
#include "common/status/status.h"
#include "planning/common/open_space_info_lite.h"
#include "planning/common/path/discretized_path.h"
#include "proto_convert/pnc_point_convert.h"
#include "proto_convert/vehicle_state_convert.h"

namespace TL {
namespace planning {

enum class OpenSpaceStraightPathMode {
  DIRECT_FORWARD = 0,
  DIRECT_BACKWARD = 1,
};

struct OpenSpaceStraightPathConfig {
  double direct_move_length = 3.0;
  double interpolation_step_size = 0.1;
  double standstill_velocity_threshold = 1.0e-3;

  static OpenSpaceStraightPathConfig GetDefault() {
    return OpenSpaceStraightPathConfig();
  }
};

struct OpenSpaceStraightPathInput {
  OpenSpaceStraightPathMode mode{OpenSpaceStraightPathMode::DIRECT_FORWARD};
  common::VehicleState vehicle_state;
  common::TrajectoryPoint planning_start_point;
  bool is_vehicle_stand_still{true};
  double direct_move_length_override{0.0};
};

struct OpenSpaceStraightPathOutput {
  PathGearPair chosen_partitioned_path;
  bool is_stop_path{false};
  bool is_gear_changed{false};
  bool direction_changed{false};
  std::string diagnostic;

  void Reset() {
    chosen_partitioned_path = PathGearPair();
    is_stop_path = false;
    is_gear_changed = false;
    direction_changed = false;
    diagnostic.clear();
  }
};

class OpenSpaceStraightPathProvider {
 public:
  explicit OpenSpaceStraightPathProvider(
      const OpenSpaceStraightPathConfig& config =
          OpenSpaceStraightPathConfig::GetDefault());

  common::Status Process(const OpenSpaceStraightPathInput& input,
                         OpenSpaceStraightPathOutput* output);
  void Reset();

 private:
  static soc::GearPosition TargetGear(OpenSpaceStraightPathMode mode);
  static double MovingDirection(OpenSpaceStraightPathMode mode);
  static double ResolveDirectMoveLength(
      const OpenSpaceStraightPathConfig& config, double override_length);

  bool NeedStopDecision(double moving_direction,
                        const common::VehicleState& vehicle_state,
                        bool is_vehicle_stand_still,
                        std::string* reason) const;
  void UpdateOpenSpaceStartPoint(const common::VehicleState& vehicle_state,
                                 common::TrajectoryPoint* start_point,
                                 OpenSpaceStraightPathOutput* output) const;
  void GenerateDirectMovingPath(const common::TrajectoryPoint& start_point,
                                double direction,
                                double direct_move_length,
                                DiscretizedPath* path_data) const;

  OpenSpaceStraightPathConfig config_;
  bool has_last_mode_{false};
  OpenSpaceStraightPathMode last_mode_{OpenSpaceStraightPathMode::DIRECT_FORWARD};
  common::math::Vec2d original_direction_;
  common::math::Vec2d original_point_;
};

}  // namespace planning
}  // namespace TL
