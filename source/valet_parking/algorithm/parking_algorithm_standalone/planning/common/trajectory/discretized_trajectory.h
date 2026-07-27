#ifndef PLANNING_COMMON_TRAJECTORY_DISCRETIZED_TRAJECTORY_H
#define PLANNING_COMMON_TRAJECTORY_DISCRETIZED_TRAJECTORY_H

/******************************************************************************
 * Copyright 2017 The Apollo Authors. All Rights Reserved.
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
 * @file
 **/

#pragma once

#include <vector>

#include "common/file/log.h"
#include "common/math/vec2d.h"

// 独立编译改造：将 RepeatedPtrField<TrajectoryPoint> 替换为 std::vector<TrajectoryPoint>
// 消除 protobuf 运行时依赖，TrajectoryPoint 现在是纯 C++ 结构体
#include "proto_convert/pnc_point_convert.h"
#include "proto_convert/chassis_convert.h"

namespace TL {
namespace planning {
using TL::common::TrajectoryPoint;

class DiscretizedTrajectory {
 public:
  DiscretizedTrajectory() = default;

  // 独立编译改造：ADCTrajectory 构造函数已移除（来自 planning.pb.h，不再使用）

  explicit DiscretizedTrajectory(
      const std::vector<TrajectoryPoint>& trajectory_points);

  void SetTrajectoryPoints(
      const std::vector<TrajectoryPoint>& trajectory_points);

  void SetStopTrajectory(double x, double y, double theta, double kappa,
                         double start_time = 0.0, double acc = 0.0);

  virtual ~DiscretizedTrajectory() = default;

  virtual TrajectoryPoint StartPoint() const;

  virtual double GetTemporalLength() const;

  virtual double GetSpatialLength() const;

  virtual TrajectoryPoint Evaluate(double relative_time) const;

  size_t QueryLowerBoundPoint(double relative_time,
                              double epsilon = 1.0e-5) const;

  virtual size_t QueryNearestPoint(const common::math::Vec2d& position) const;

  double QueryMatchedRelativeTime(const common::math::Vec2d& position) const;

  size_t QueryNearestPointWithBuffer(const common::math::Vec2d& position,
                                     double buffer) const;

  virtual void AppendTrajectoryPoint(const TrajectoryPoint& trajectory_point);

  virtual TrajectoryPoint* AppendTrajectoryPoint(double relative_time);

  void PrependTrajectoryPoints(
      const std::vector<TrajectoryPoint>& trajectory_points) {
    if (!trajectory_.empty() && trajectory_points.size() > 1) {
      ACHECK(trajectory_points.rbegin()->relative_time <
             trajectory_.begin()->relative_time);
    }
    std::vector<TrajectoryPoint> merged;
    merged.reserve(trajectory_points.size() + trajectory_.size());
    merged.insert(merged.end(), trajectory_points.begin(), trajectory_points.end());
    merged.insert(merged.end(), trajectory_.begin(), trajectory_.end());
    trajectory_ = std::move(merged);
  }

  const TrajectoryPoint& TrajectoryPointAt(size_t index) const;

  /**
   * @brief Get the Trajectory Segment Size object
   *
   * @param init_gear init gear of trajectory
   * @return int trajectory segment size
   */
  // 独立编译改造：Chassis_GearPosition -> soc::GearPosition
  size_t GetTrajectorySegmentSize(
      const TL::soc::GearPosition& init_gear =
          soc::GearPosition::GEAR_PARKING) const;

  inline size_t NumOfPoints() const { return trajectory_.size(); }

  inline void clear() { trajectory_.clear(); }

  inline bool empty() const { return trajectory_.empty(); }

  inline const TrajectoryPoint& front() const { return trajectory_.front(); }

  inline const TrajectoryPoint& back() const { return trajectory_.back(); }

  inline auto begin() const { return trajectory_.begin(); }

  inline auto end() const { return trajectory_.end(); }

  inline int size() const { return static_cast<int>(trajectory_.size()); }

  inline const TrajectoryPoint& at(int index) const { return trajectory_[index]; }

  inline const TrajectoryPoint& operator[](int index) const { return trajectory_[index]; }

  // 独立编译改造：add() 替换 RepeatedPtrField::Add()
  inline TrajectoryPoint* add() {
    trajectory_.emplace_back();
    return &trajectory_.back();
  }

  inline TrajectoryPoint* Mutable(int index) { return &trajectory_[index]; }

 protected:
  std::vector<TrajectoryPoint> trajectory_;
};

}  // namespace planning
}  // namespace TL

#endif  // PLANNING_COMMON_TRAJECTORY_DISCRETIZED_TRAJECTORY_H
