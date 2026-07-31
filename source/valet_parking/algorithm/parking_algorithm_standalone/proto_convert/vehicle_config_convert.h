/******************************************************************************
 * Vehicle Config Proto to C++ Conversion Header
 *
 * Description: Convert common/vehicle_config.proto to C++ structures
 * Original Proto: proto/common/vehicle_config.proto
 * Priority: P1 (Vehicle parameters for planning and control)
 *
 * Date: 2025-12-19
 * Note: This file replaces Protobuf dependencies for standalone compilation.
 *       Vehicle configuration includes dimensions, dynamics, and sensor positions.
 *****************************************************************************/

#pragma once

#include "header_convert.h"
#include "types_convert.h"
#include <cmath>
#include <string>
#include <vector>
#include <limits>

namespace TL {
namespace common {

/**
 * @brief Vehicle brand enumeration
 */
enum class VehicleBrand {
  LINCOLN_MKZ = 0,
  NEOLIX = 1,
  NETA_EP31 = 2,
  NETA_EP40 = 3,
  NETA_EP41 = 4,
};

/**
 * @brief Vehicle identification information
 */
struct VehicleID {
  std::string vin;              ///< Vehicle Identification Number
  std::string plate;            ///< License plate
  std::string other_unique_id;  ///< Other unique identifier
};

/**
 * @brief Latency parameters for control systems
 * 
 * All values in seconds (s)
 */
struct LatencyParam {
  double dead_time = 0.0;      ///< Dead time (delay before response)
  double rise_time = 0.0;      ///< Time to rise from 10% to 90%
  double peak_time = 0.0;      ///< Time to reach peak overshoot
  double settling_time = 0.0;  ///< Time to settle within tolerance
};

/**
 * @brief Ultrasonic sensor position
 * 
 * Position defined in front-right-down (FRD) frame
 */
struct UltrasonicPosition {
  Point2D point;  ///< Position in FRD frame (x, y in meters)
  double seta = 0.0;  ///< Orientation angle in degrees, range [-180, 180]
};

/**
 * @brief Array of ultrasonic sensor positions
 * 
 * Up to 12 ultrasonic sensors
 */
struct UltrasonicPositionArray {
  UltrasonicPosition ultrasonic_s1;
  UltrasonicPosition ultrasonic_s2;
  UltrasonicPosition ultrasonic_s3;
  UltrasonicPosition ultrasonic_s4;
  UltrasonicPosition ultrasonic_s5;
  UltrasonicPosition ultrasonic_s6;
  UltrasonicPosition ultrasonic_s7;
  UltrasonicPosition ultrasonic_s8;
  UltrasonicPosition ultrasonic_s9;
  UltrasonicPosition ultrasonic_s10;
  UltrasonicPosition ultrasonic_s11;
  UltrasonicPosition ultrasonic_s12;
};

/**
 * @brief Steer ratio segment configuration
 * 
 * Maps steering wheel angle to wheel angle with segmented ratios
 */
struct SteerRatioSegmentConf {
  std::vector<double> steer_segment;        ///< Steer angle segments (radians)
  std::vector<double> steer_ratio_segment;  ///< Steer ratios for each segment
};

/**
 * @brief Steering wheel speed limit segments
 * 
 * Maps vehicle speed to maximum steering wheel angular velocity
 */
struct SteerWheelSpeedSegment {
  std::vector<double> vehicle_speed_segment;             ///< Vehicle speed segments (m/s)
  std::vector<double> steering_wheel_speed_limit_segment;  ///< Speed limits (rad/s)
};

/**
 * @brief Transform between coordinate frames
 * 
 * Represents a rigid body transformation (translation + rotation)
 */
struct Transform {
  std::string source_frame;  ///< Source frame ID
  std::string target_frame;  ///< Target frame ID (child frame)
  Point3D translation;       ///< Position of target in source frame (meters)
  Quaternion rotation;       ///< Rotation from source to target frame
};

/**
 * @brief Extrinsic calibration for sensors
 * 
 * Collection of transforms between vehicle frame and sensor frames
 */
struct Extrinsics {
  std::vector<Transform> transforms;  ///< Array of coordinate frame transforms
};

/**
 * @brief Vehicle parameters
 * 
 * Complete vehicle configuration including dimensions, dynamics, and control limits.
 * By default, all measurements are in SI units (meters, m/s, radians, etc.).
 * Car center point is the reference point (center of rear axle).
 */
struct VehicleParam {
  // ========================================
  // Identification
  // ========================================
  VehicleBrand brand = VehicleBrand::NETA_EP31;
  VehicleID vehicle_id;

  // ========================================
  // Vehicle Dimensions (meters)
  // ========================================
  // Reference point: center of rear axle
  double front_edge_to_center = std::numeric_limits<double>::quiet_NaN();  ///< Front bumper to center
  double back_edge_to_center = std::numeric_limits<double>::quiet_NaN();   ///< Rear bumper to center
  double left_edge_to_center = std::numeric_limits<double>::quiet_NaN();   ///< Left edge to center
  double right_edge_to_center = std::numeric_limits<double>::quiet_NaN();  ///< Right edge to center

  double length = std::numeric_limits<double>::quiet_NaN();  ///< Total vehicle length
  double width = std::numeric_limits<double>::quiet_NaN();   ///< Total vehicle width
  double height = std::numeric_limits<double>::quiet_NaN();  ///< Total vehicle height

  // ========================================
  // Vehicle Dynamics
  // ========================================
  double min_turn_radius = std::numeric_limits<double>::quiet_NaN();  ///< Minimum turning radius (m)
  double max_acceleration = std::numeric_limits<double>::quiet_NaN(); ///< Max acceleration (m/s²)
  double max_deceleration = std::numeric_limits<double>::quiet_NaN(); ///< Max deceleration (m/s²)

  // ========================================
  // Steering Parameters
  // ========================================
  double max_steer_angle = std::numeric_limits<double>::quiet_NaN();       ///< Max steering angle (rad)
  double max_steer_angle_rate = std::numeric_limits<double>::quiet_NaN();  ///< Max steer rate (rad/s)
  double min_steer_angle_rate = std::numeric_limits<double>::quiet_NaN();  ///< Min steer rate (rad/s)
  double steer_ratio = std::numeric_limits<double>::quiet_NaN();           ///< Steering wheel to wheel ratio
  double wheel_base = std::numeric_limits<double>::quiet_NaN();            ///< Front to rear axle distance (m)

  // ========================================
  // Wheel Parameters
  // ========================================
  double wheel_rolling_radius = std::numeric_limits<double>::quiet_NaN();  ///< Effective rolling radius (m)

  // ========================================
  // Control Thresholds
  // ========================================
  float max_abs_speed_when_stopped = std::numeric_limits<float>::quiet_NaN();  ///< Min speed considered stopped (m/s)

  double brake_deadzone = 0.0;     ///< Min brake percentage to activate
  double throttle_deadzone = 0.0;  ///< Min throttle percentage to activate
  double steer_deadzone = 0.0;     ///< Min steer percentage to activate
  double steer_offset = 0.0;       ///< Steering offset correction

  // ========================================
  // Control Latency Parameters
  // ========================================
  LatencyParam steering_latency_param;  ///< Steering system latency
  LatencyParam throttle_latency_param;  ///< Throttle system latency
  LatencyParam brake_latency_param;     ///< Brake system latency

  // ========================================
  // Calibration Offsets
  // ========================================
  double heading_offset = std::numeric_limits<double>::quiet_NaN();      ///< Heading calibration offset (rad)
  double eye_heading_offset = std::numeric_limits<double>::quiet_NaN();  ///< Camera heading offset (rad)
  double localization_offset = std::numeric_limits<double>::quiet_NaN(); ///< Localization offset (m)

  // ========================================
  // Steering Torque Parameters
  // ========================================
  double steer_2_torque_right = std::numeric_limits<double>::quiet_NaN();  ///< Steer to torque (right)
  double steer_2_torque_left = std::numeric_limits<double>::quiet_NaN();   ///< Steer to torque (left)
  double max_steer_torque = std::numeric_limits<double>::quiet_NaN();      ///< Max steering torque (Nm)

  // ========================================
  // Sensor Configurations
  // ========================================
  UltrasonicPositionArray ultrasonic_position;  ///< Ultrasonic sensor array
  SteerRatioSegmentConf steer_ratio_segment;    ///< Segmented steer ratio config
  SteerWheelSpeedSegment steer_wheel_speed_segment;  ///< Steering speed limits

  // ========================================
  // Additional Parameters
  // ========================================
  double max_steer_angle_offset_compensate = 0.0;  ///< Max steer offset compensation
  double mirror_to_center = std::numeric_limits<double>::quiet_NaN();  ///< Mirror to center distance (m)
  double mirror_radius = std::numeric_limits<double>::quiet_NaN();     ///< Mirror radius (m)

  // ========================================
  // Helper Methods
  // ========================================
  
  /**
   * @brief Check if vehicle dimensions are valid
   */
  bool IsValid() const {
    return !std::isnan(length) && !std::isnan(width) && 
           !std::isnan(wheel_base) && !std::isnan(max_steer_angle);
  }

  /**
   * @brief Get vehicle bounding box half-lengths
   */
  void GetBoundingBox(double& half_length, double& half_width) const {
    half_length = length / 2.0;
    half_width = width / 2.0;
  }

  /**
   * @brief Calculate front overhang (front_edge_to_center - wheel_base)
   */
  double GetFrontOverhang() const {
    return front_edge_to_center - wheel_base;
  }

  /**
   * @brief Calculate rear overhang (back_edge_to_center)
   */
  double GetRearOverhang() const {
    return back_edge_to_center;
  }
};

/**
 * @brief Complete vehicle configuration
 * 
 * Top-level message containing vehicle parameters and extrinsic calibrations
 */
struct VehicleConfig {
  Header header;            ///< Message header with timestamp
  VehicleParam vehicle_param;  ///< Vehicle parameters
  Extrinsics extrinsics;    ///< Sensor extrinsic calibrations
};

}  // namespace common
}  // namespace TL
