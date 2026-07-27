/******************************************************************************
 * Chassis Proto to C++ Conversion Header
 *
 * Description: Convert soc/chassis.proto to C++ structures (simplified for parking)
 * Original Proto: proto/soc/chassis.proto
 * Priority: P1 (Vehicle chassis information)
 *
 * Date: 2025-12-15
 * Note: This file replaces Protobuf dependencies for standalone compilation.
 *       Only includes fields needed for parking algorithm.
 *****************************************************************************/

#pragma once

#include "header_convert.h"
#include "types_convert.h"
#include <cmath>

namespace TL {
namespace soc {

/**
 * @brief Chassis Error Code
 */
enum class ChassisErrorCode {
  NO_ERROR = 0,
  CMD_NOT_IN_PERIOD = 1,
  CHASSIS_ERROR = 2,
  CHASSIS_ERROR_ON_STEER = 6,
  CHASSIS_ERROR_ON_BRAKE = 7,
  CHASSIS_ERROR_ON_THROTTLE = 8,
  CHASSIS_ERROR_ON_GEAR = 9,
  MANUAL_INTERVENTION = 3,
  CHASSIS_CAN_NOT_IN_PERIOD = 4,
  UNKNOWN_ERROR = 5
};

/**
 * @brief Driving Mode
 */
enum class DrivingMode {
  COMPLETE_MANUAL = 0,      ///< Human drive
  COMPLETE_AUTO_DRIVE = 1,  ///< Full autonomous
  AUTO_STEER_ONLY = 2,      ///< Only steer control
  AUTO_SPEED_ONLY = 3,      ///< Only speed control (throttle + brake)
  EMERGENCY_MODE = 4        ///< Emergency mode
};

/**
 * @brief Gear Position
 */
enum class GearPosition {
  GEAR_NEUTRAL = 0,
  GEAR_DRIVE = 1,
  GEAR_REVERSE = 2,
  GEAR_PARKING = 3,
  GEAR_LOW = 4,
  GEAR_INVALID = 5,
  GEAR_NONE = 6
};

/**
 * @brief Vehicle Signal (lights, horn, etc.)
 */
struct VehicleSignal {
  bool turn_signal_left = false;
  bool turn_signal_right = false;
  bool high_beam_signal = false;
  bool low_beam_signal = false;
  bool horn = false;
  bool emergency_light = false;
};

/**
 * @brief Wheel Speed Information
 */
struct WheelSpeed {
  enum WheelSpeedType {
    FORWARD = 0,
    BACKWARD = 1,
    STANDSTILL = 2,
    INVALID = 3
  };

  bool is_wheel_spd_rr_valid = false;
  WheelSpeedType wheel_direction_rr = INVALID;
  double wheel_spd_rr = 0.0;

  bool is_wheel_spd_rl_valid = false;
  WheelSpeedType wheel_direction_rl = INVALID;
  double wheel_spd_rl = 0.0;

  bool is_wheel_spd_fr_valid = false;
  WheelSpeedType wheel_direction_fr = INVALID;
  double wheel_spd_fr = 0.0;

  bool is_wheel_spd_fl_valid = false;
  WheelSpeedType wheel_direction_fl = INVALID;
  double wheel_spd_fl = 0.0;
};

/**
 * @brief Door Status
 */
struct DoorStatus {
  bool driver_door = false;       ///< Driver door open
  bool passenger_door = false;    ///< Front passenger door open
  bool rear_left_door = false;    ///< Rear left door open
  bool rear_right_door = false;   ///< Rear right door open
};

/**
 * @brief Ignition State
 */
enum class IgState {
  IG_OFF = 0,
  IG_ACC = 1,
  IG_ON = 2,
  IG_START = 3
};

/**
 * @brief Chassis Message (Simplified for Parking Algorithm)
 */
struct Chassis {
  // Engine and power
  bool engine_started = false;
  float engine_rpm = NAN;

  // Vehicle motion state
  float speed_mps = NAN;           ///< Vehicle speed (m/s)
  float odometer_m = NAN;          ///< Odometer (meters)
  int32_t fuel_range_m = 0;        ///< Fuel range (meters)

  // Control inputs (actual values)
  float throttle_percentage = NAN; ///< Throttle position [0, 100]
  float brake_percentage = NAN;    ///< Brake position [0, 100]
  float steering_percentage = NAN; ///< Steering [-100, 100]
  float steering_torque_nm = NAN;  ///< Steering torque (Nm)
  double steering_angle = 0.0;     ///< Steering angle (radians)
  double steering_rate = 0.0;      ///< Steering rate (rad/s)

  // Vehicle state
  bool parking_brake = false;
  bool wiper = false;
  DrivingMode driving_mode = DrivingMode::COMPLETE_MANUAL;
  ChassisErrorCode error_code = ChassisErrorCode::NO_ERROR;
  GearPosition gear_location = GearPosition::GEAR_PARKING;

  // Timestamps
  double steering_timestamp = 0.0;
  TL::common::Header header;

  // Signals and sensor data
  int32_t chassis_error_mask = 0;
  VehicleSignal signal;
  WheelSpeed wheel_speed;

  // Battery (for EV)
  int32_t battery_soc_percentage = -1;

  // IMU data (if available)
  double yaw_rate = 0.0;
  TL::common::Point3D imu_raw_ang_rate;
  TL::common::Point3D imu_raw_acc;
  TL::common::Point3D imu_vel;
  TL::common::Point3D imu_acc;
  TL::common::Point3D imu_ang;
  TL::common::Point3D imu_ang_rate;

  // Open space / parking specific
  bool enable_open_space_signal = false;
  bool hpp_reset_signal = false;

  // Safety features
  bool fcw = false;  ///< Forward Collision Warning triggered
  bool aeb = false;  ///< Autonomous Emergency Braking triggered

  // Door and cabin status
  DoorStatus door_status;
  IgState ig_state = IgState::IG_OFF;
  bool driver_buckle_status = false;
  uint32_t crash_status = 0;

  // Validity flags
  bool vcu_act_gear_position_valid = false;
  bool vcu_real_throttle_pos_valid = false;
  bool steering_angle_valid = false;
  bool steering_angle_speed_valid = false;

  /**
   * @brief Check if chassis has error
   * @return true if error exists
   */
  bool has_error() const {
    return error_code != ChassisErrorCode::NO_ERROR;
  }

  /**
   * @brief Check if vehicle is in autonomous mode
   * @return true if in autonomous mode
   */
  bool is_autonomous_mode() const {
    return driving_mode == DrivingMode::COMPLETE_AUTO_DRIVE ||
           driving_mode == DrivingMode::AUTO_STEER_ONLY ||
           driving_mode == DrivingMode::AUTO_SPEED_ONLY;
  }

  /**
   * @brief Check if vehicle is stopped
   * @return true if speed is near zero
   */
  bool is_stopped() const {
    return !std::isnan(speed_mps) && std::abs(speed_mps) < 0.1;
  }
};

}  // namespace soc
}  // namespace TL
