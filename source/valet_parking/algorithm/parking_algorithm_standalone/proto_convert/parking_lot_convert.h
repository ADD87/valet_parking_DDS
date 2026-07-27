/******************************************************************************
 * Parking Lot Proto to C++ Conversion Header
 *
 * Description: Convert perception_parking_lot.proto to C++ structures
 * Original Proto: proto/perception/perception_parking_lot.proto
 * Priority: P0 (Core)
 *
 * Date: 2025-12-15
 * Note: This file replaces Protobuf dependencies for standalone compilation
 *****************************************************************************/

#pragma once

#include <vector>
#include <string>
#include <cstdint>

namespace TL {
namespace perception {

/**
 * @brief Point3D structure (from common/types.proto)
 */
struct Point3D {
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
};

/**
 * @brief Parking Space Point (corner point of parking slot)
 */
struct PSPoint {
  /**
   * @brief Position of the point on parking slot
   */
  enum Position {
    TOP_LEFT = 0,
    TOP_RIGHT = 1,
    BOTTOM_LEFT = 2,
    BOTTOM_RIGHT = 3,
    STOP_LEFT = 4,
    STOP_RIGHT = 5
  };

  /**
   * @brief Quality of the point detection
   */
  enum Quality {
    LOW = 0,
    HIGH = 1
  };

  Point3D point;           ///< 3D coordinates of the point
  Position position = TOP_LEFT;  ///< Position identifier
  Quality quality = LOW;   ///< Detection quality
};

/**
 * @brief Single Parking Slot Information
 */
struct ParkingLotOut {
  /**
   * @brief Type of parking space
   */
  enum ParkType {
    VERTICAL = 0,   ///< Vertical parking (perpendicular)
    LATERAL = 1,    ///< Lateral parking (parallel)
    OBLIQUE = 2,    ///< Oblique parking (angled)
    NONE = 3        ///< Unknown type
  };

  /**
   * @brief Size classification of parking space
   */
  enum ParkSpaceSize {
    NORMAL = 0,     ///< Normal size
    SMALL = 1,      ///< Small size
    NARROW = 2      ///< Narrow size
  };

  /**
   * @brief Occupancy status of parking space
   */
  enum ParkStatus {
    FREE = 0,       ///< Available
    OCCUPIED = 1,   ///< Occupied
    UNKNOWN = 2     ///< Unknown status
  };

  /**
   * @brief Sensor type used for detection
   */
  enum SenType {
    CAMERA = 0,         ///< Camera detection
    MAP = 1,            ///< Map-based
    CAM_MAP_FUSION = 2, ///< Camera + Map fusion
    USS = 3             ///< Ultrasonic sensor
  };

  // Member fields
  uint32_t parking_seq = 0;           ///< Parking slot sequence number
  ParkType type = VERTICAL;           ///< Parking space type
  ParkStatus status = FREE;           ///< Occupancy status
  SenType sensor_type = CAMERA;       ///< Detection sensor type
  bool is_private_ps = false;         ///< Is private/fixed parking slot
  std::vector<PSPoint> pts_vrf;       ///< Corner points in vehicle reference frame
  double time_creation = 0.0;         ///< Creation timestamp
  std::vector<PSPoint> pts_enu;       ///< Corner points in ENU (world) frame
  double hmi_angle = 0.0;             ///< Parking slot heading angle (degrees)
  double hmi_depth = 0.0;             ///< Parking slot depth (meters)
  double hmi_width = 0.0;             ///< Parking slot width (meters)
  double hmi_direction = 0.0;         ///< Direction indicator
  double hmi_type = 0.0;              ///< Type indicator for HMI
  double hmi_status = 0.0;            ///< Status indicator for HMI
  ParkSpaceSize park_size = NORMAL;   ///< Size classification
};

/**
 * @brief Parking Path Point (for traced path)
 */
struct ParkingPathPoint {
  double x = 0.0;             ///< X coordinate (meters)
  double y = 0.0;             ///< Y coordinate (meters)
  double z = 0.0;             ///< Z coordinate (meters)
  double yaw = 0.0;           ///< Heading angle (radians)
  double accumulate_s = 0.0;  ///< Accumulated distance from start (meters)
  uint32_t gear = 0;          ///< Gear position (0=P, 1=R, 2=N, 3=D, etc.)
};

/**
 * @brief Header structure (simplified from common/header.proto)
 */
struct Header {
  int32_t seq = 0;                ///< Sequence number
  std::string frame_id = "";      ///< Frame ID
  double publish_stamp = 0.0;     ///< Publish timestamp
  double data_stamp = 0.0;        ///< Data timestamp
};

/**
 * @brief Array of Parking Lots (main message)
 */
struct ParkingLotOutArray {
  Header header;                          ///< Message header
  uint32_t count = 0;                     ///< Number of parking lots
  uint32_t locSeq = 0;                    ///< Localization sequence
  uint32_t opt_parking_seq = 0;           ///< Optimal parking slot sequence number
  std::vector<ParkingLotOut> parking_lots; ///< Array of parking slots
  bool is_valid = false;                  ///< Data validity flag
  uint32_t path_point_size = 0;           ///< Number of path points
  std::vector<ParkingPathPoint> traced_path; ///< Historical traced path
  bool hpp_cruising_to_parking = false;   ///< HPP cruising to parking flag
  bool enable_trigger = false;            ///< Enable trigger flag

  /**
   * @brief Check if message has optimal parking sequence
   * @return true if opt_parking_seq > 0
   */
  bool has_opt_parking_seq() const {
    return opt_parking_seq > 0;
  }

  /**
   * @brief Get number of parking lots
   * @return Size of parking_lots vector
   */
  size_t parking_lots_size() const {
    return parking_lots.size();
  }

  /**
   * @brief Add a new parking lot
   * @return Pointer to the newly added parking lot
   */
  ParkingLotOut* add_parking_lots() {
    parking_lots.emplace_back();
    return &parking_lots.back();
  }

  /**
   * @brief Clear all parking lots
   */
  void clear_parking_lots() {
    parking_lots.clear();
  }
};

}  // namespace perception
}  // namespace TL
