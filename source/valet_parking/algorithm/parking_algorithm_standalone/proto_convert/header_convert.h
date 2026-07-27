/******************************************************************************
 * Header Proto to C++ Conversion Header
 *
 * Description: Convert common/header.proto to C++ structures
 * Original Proto: proto/common/header.proto
 * Priority: P1 (Message metadata)
 *
 * Date: 2025-12-15
 * Note: This file replaces Protobuf dependencies for standalone compilation
 *****************************************************************************/

#pragma once

#include "error_code_convert.h"  // 独立编译改造：使用统一的 StatusPb 定义
#include <string>
#include <cstdint>

namespace TL {
namespace common {

// 独立编译改造：StatusPb 已在 error_code_convert.h 中定义，此处不再重复

/**
 * @brief Sensor Timestamp
 */
struct SensorStamp {
  uint64_t camera_timestamp = 0;      ///< Camera timestamp (microseconds)
  uint64_t lidar_timestamp = 0;       ///< Lidar timestamp (microseconds)
  uint64_t radar_timestamp = 0;       ///< Radar timestamp (microseconds)
  uint64_t ultrasonic_timestamp = 0;  ///< Ultrasonic timestamp (microseconds)
};

/**
 * @brief Message Header (for all messages)
 */
struct Header {
  uint32_t sequence_num = 0;        ///< Sequence number
  std::string frame_id = "";        ///< Frame ID (coordinate frame)
  double timestamp_sec = 0.0;       ///< Timestamp in seconds
  uint64_t lidar_timestamp = 0;     ///< Lidar timestamp (microseconds)
  uint64_t camera_timestamp = 0;    ///< Camera timestamp (microseconds)
  uint64_t radar_timestamp = 0;     ///< Radar timestamp (microseconds)
  std::string module_name = "";     ///< Module name (e.g., "planning", "control")
  StatusPb status;                  ///< Status information
  SensorStamp sensor_stamp;         ///< Sensor timestamps
};

}  // namespace common
}  // namespace TL
