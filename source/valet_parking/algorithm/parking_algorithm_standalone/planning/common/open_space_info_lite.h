/******************************************************************************
 * Copyright 2019 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

/**
 * @file open_space_info_lite.h
 * @brief 独立编译版：open_space_info 的轻量级子集
 *
 * 包含 PathGearPair, DestRegionWithAng, ParkingScenarioType, PartitionedPath,
 * FreeSpaceSegment, OpenSpaceEnvStructuredInfo 等类型定义
 * 避免引入 hdmap、obstacle、trajectory 等重型依赖
 **/

#pragma once

#include <cstddef>
#include <cstdint>
#include <tuple>
#include <utility>
#include <vector>

#include "common/math/line_segment2d.h"
#include "common/math/polygon2d.h"
#include "planning/common/path/discretized_path.h"
#include "proto_convert/chassis_convert.h"
#include "proto_convert/planning_internal_convert.h"

// ========== FreeSpaceOut 枚举定义 ==========
// 独立编译改造：从 perception_freespace.pb.h 提取
// 原 Proto: message FreeSpaceOut 中的嵌套枚举
// 注：必须放在 TL::perception 命名空间，与原 proto 保持一致
namespace TL {
namespace perception {

struct FreeSpaceOut {
  enum ClassType {
    UNKOWN_CLASS = 0,
    VEHICLE = 1,
    PEDESTRAIN = 2,
    CYCLIST = 3,
    CONE_POLE = 4,
    CURBSTONE = 5,
    OTHER_CLASS = 6,
  };

  enum SensorType {
    CAMERA = 0,
    LIDAR = 1,
    USS = 2,
    LIDAR_CAM_FUSION = 3,
    USS_LIDAR_FUSION = 4,
    USS_LIDAR_CAM_FUSION = 5,
  };

  enum HeightType {
    UNKOWN_HEIGHT = 0,
    OVERDRIVABLE = 1,
    UNDERDRIVABLE = 2,
  };
};

/**
 * @brief FreeSpaceOutArray 存根
 * 独立编译改造：完整的 FreeSpaceOutArray 在 proto 中定义
 * 此为简化存根，使代码可以编译通过
 * 碰撞检测使用 FreeSpaceSegment（由 ROI 阶段预处理）
 */
struct FreeSpaceOutArray {
  // 存根：速度优化器通过 FreeSpaceSegment 间接使用 FreeSpace 数据
  // 实际数据由 ROI decider 预处理后通过 FreeSpaceSegment 传入
};

}  // namespace perception
}  // namespace TL

// ========== 泊车规划轻量类型 ==========
namespace TL {
namespace planning {

using PathGearPair = std::pair<DiscretizedPath, soc::GearPosition>;

// destregion: xy polygon, from_ang, to_ang
using DestRegionWithAng = std::tuple<common::math::Polygon2d, double, double>;

enum ParkingScenarioType {
  DEFAULT_TYPE = 0,
  LEFT_VERTICAL_PARKING_IN = 1,
  RIGHT_VERTICAL_PARKING_IN = 2,
  LEFT_LATERAL_PARKING_IN = 3,
  RIGHT_LATERAL_PARKING_IN = 4,
  LEFT_OBLIQUE_PARKING_IN = 5,
  RIGHT_OBLIQUE_PARKING_IN = 6,
  LEFT_VERTICAL_PARKING_OUT = 7,
  RIGHT_VERTICAL_PARKING_OUT = 8,
  FORWARD_VERTICAL_PARKING_OUT = 9,
  BACKWARD_VERTICAL_PARKING_OUT = 10,
  LEFT_LATERAL_PARKING_OUT = 11,
  RIGHT_LATERAL_PARKING_OUT = 12,
  LEFT_OBLIQUE_PARKING_OUT = 13,
  RIGHT_OBLIQUE_PARKING_OUT = 14,
  FORWARD_OBLIQUE_PARKING_OUT = 15,
  BACKWARD_OBLIQUE_PARKING_OUT = 16,
  REVERSE_TURN = 20,
  LEFT_U_TYPE_TURN = 21,
  RIGHT_U_TYPE_TURN = 22,
  FREESPACE_FORWARD_EXPLORATION = 23,
  CONTROL_CALIBRATION_MODE = 100
};

// ========== ScenarioDiffcultyType 枚举 ==========
// 独立编译改造：从 open_space_info.h 提取
enum ScenarioDiffcultyType {
  NORMAL_SCENARIO = 0,
  DEADEND_SCENARIO = 1,
  NARROW_SPOT_SCENARIO = 2,
  NARROW_PASSAGE_SCENARIO = 4,
};

struct PartitionedPath {
  size_t path_idx = 0;
  size_t point_idx = 0;
  bool path_shift = false;
  std::vector<PathGearPair> path_set;
  planning_internal::PathType path_type =
      planning_internal::PathType::DEFAULT;
  uint32_t replan_status = 0;
};

// ========== OpenSpaceEnvStructuredInfo ==========
// 独立编译改造：从 open_space_info.h 提取轻量版本

struct OpenSpaceEnvStructuredInfo {
  bool is_out_roi = false;
  bool is_parking_inwards = false;
  bool is_in_nns_adjust_scenario = false;
  uint32_t parking_scenario_diffculty_type = NORMAL_SCENARIO;
  ParkingScenarioType parking_scenario_type = ParkingScenarioType::DEFAULT_TYPE;
};

// ========== FreeSpaceSegment ==========
// 独立编译改造：从 open_space_info.h 提取
// FreeSpace 边界段信息（由 ROI 阶段预处理）

struct FreeSpaceSegment {
  int index_in_freespace_out = -1;
  int index_in_keypoints = -1;
  bool isLinkObjFusion = false;
  uint32_t obstacleId = 0;
  perception::FreeSpaceOut::ClassType cls_type =
      perception::FreeSpaceOut::UNKOWN_CLASS;
  perception::FreeSpaceOut::SensorType sensor_type =
      perception::FreeSpaceOut::CAMERA;
  perception::FreeSpaceOut::HeightType height_type =
      perception::FreeSpaceOut::UNKOWN_HEIGHT;
  common::math::LineSegment2d segment;
};

}  // namespace planning
}  // namespace TL
