/******************************************************************************
 * Vehicle State Proto to C++ Conversion Header
 *
 * Description: Convert common/vehicle_state.proto to C++ structures
 * Original Proto: proto/common/vehicle_state.proto
 * Priority: P1 (Vehicle state information)
 *
 * Date: 2025-12-15
 * Note: This file replaces Protobuf dependencies for standalone compilation
 *****************************************************************************/

#pragma once

#include "header_convert.h"
#include "types_convert.h"
#include "chassis_convert.h"  // 独立编译改造: 复用 soc::GearPosition

namespace TL {
namespace common {

/**
 * @brief Pose structure
 */
struct Pose {
  Point3D position;      ///< Position in world frame
  Quaternion orientation;///< Orientation
  Point3D linear_velocity;      ///< Linear velocity (m/s)
  Point3D angular_velocity;     ///< Angular velocity (rad/s)
  Point3D linear_acceleration;  ///< Linear acceleration (m/s^2)
  double heading = 0.0;  ///< Heading angle (radians)
};

/**
 * @brief Vehicle Signal (simplified)
 */
struct VehicleSignal {
  bool turn_signal_left = false;   ///< Left turn signal
  bool turn_signal_right = false;  ///< Right turn signal
  bool horn = false;               ///< Horn
  bool emergency_light = false;    ///< Emergency flasher
};

/**
 * @brief Gear Position — 复用 soc::GearPosition 避免类型不兼容
 */
using GearPosition = TL::soc::GearPosition;

/**
 * @brief Driving Mode — 复用 soc::DrivingMode 避免类型不兼容
 */
using DrivingMode = TL::soc::DrivingMode;

/**
 * @brief Vehicle State Message
 */
struct VehicleState {
  // Position (world frame)
  double x = 0.0;             ///< X coordinate (meters)
  double y = 0.0;             ///< Y coordinate (meters)
  double z = 0.0;             ///< Z coordinate (meters)
  double timestamp = 0.0;     ///< Timestamp (seconds)

  // Orientation
  double roll = 0.0;          ///< Roll angle (radians)
  double pitch = 0.0;         ///< Pitch angle (radians)
  double yaw = 0.0;           ///< Yaw angle (radians)
  double heading = 0.0;       ///< Heading angle (radians, same as yaw typically)

  // Motion state
  double kappa = 0.0;              ///< Curvature (1/meters)
  double linear_velocity = 0.0;    ///< Linear velocity (m/s)
  double angular_velocity = 0.0;   ///< Angular velocity (rad/s)
  double linear_acceleration = 0.0;///< Linear acceleration (m/s^2)

  // Vehicle control state
  GearPosition gear = GearPosition::GEAR_PARKING;  ///< Gear position
  DrivingMode driving_mode = DrivingMode::COMPLETE_MANUAL; ///< Driving mode
  double steering_percentage = 0.0; ///< Steering percentage [-100, 100]
  double linear_steer_ratio = 0.0;  ///< Steering ratio

  // Additional info
  Pose pose;                         ///< Full pose information
  VehicleSignal signal;              ///< Signal lights
  Header localization_header;        ///< Localization header
  Header header;                     ///< Message header
  bool is_vehicle_reference_frame = false; ///< Is in vehicle frame

  // Chassis reference (if needed, can be null)
  // ChassisSimplified* chassis = nullptr; ///< Pointer to full chassis data
};

}  // namespace common
}  // namespace TL
