/******************************************************************************
 * Copyright 2019 The TL Authors. All Rights Reserved.
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
#pragma once

// 独立编译改造：完整保留 PathHandle 原始结构与碰撞检测逻辑
// 改造内容:
//   1. Proto include → convert/types include
//   2. Obstacle 依赖 → 独立版本中障碍物数组为空（无感知输入）
//   3. OpenSpaceInfo → 通过 SpeedOptimizerInput/Output 传递
//   4. config getter() → 直接成员访问
//   5. Clock::NowInSeconds() → std::chrono
// 原始文件备份: path_handle_original.h

#include <chrono>
#include <cstddef>
#include <limits>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "common/math/box2d.h"
#include "common/math/line_segment2d.h"
#include "planning/common/open_space_info_lite.h"
#include "planning/common/path/discretized_path.h"
#include "planning/tasks/optimizers/open_space_speed_optimizer/speed_optimizer_types.h"
#include "proto_convert/open_space_speed_optimizer_config_convert.h"
#include "proto_convert/planning_internal_convert.h"
#include "proto_convert/pnc_point_convert.h"
#include "proto_convert/vehicle_state_convert.h"

namespace TL {
namespace planning {

using common::math::Box2d;
// 独立编译改造：FreeSpaceOutArray 从 open_space_info_lite.h 获取
using perception::FreeSpaceOutArray;
// 独立编译改造：引入 planning_internal 命名空间中的类型
using planning_internal::AvpSpeedPlanCollisionInfo;
using planning_internal::PathType;

class PathHandle {
 public:
  using FsPointInfo = std::tuple<const common::Point2D*, int>;

  struct CollisionInfo {
    struct ObstacleCollisionInfo {
      bool is_collision = false;
      size_t collision_index = std::numeric_limits<size_t>::max();
      int collision_obstacle_id = 0;
      // 独立编译改造：使用 speed_optimizer_types.h 中定义的枚举
      PerceptionObstacle_Type collision_obstacle_type =
          PerceptionObstacle_Type_UNKNOWN;

      void Init(size_t path_size) {
        is_collision = false;
        collision_index = path_size - 1;
        collision_obstacle_id = 0;
        collision_obstacle_type = PerceptionObstacle_Type_UNKNOWN;
      }
    };

    struct FreeSpaceCollisionInfo {
      bool is_collision = false;
      size_t collision_index = std::numeric_limits<size_t>::max();
      FreeSpaceSegment freespace_segment;
      common::Point2D freespace_flu_point;

      void Init(size_t path_size) {
        is_collision = false;
        collision_index = path_size - 1;
      }
    };

    ObstacleCollisionInfo static_obstacle_collision_info;
    ObstacleCollisionInfo moving_obstacle_collision_info;
    ObstacleCollisionInfo outside_wheelmask_obstacle_collision_info;
    FreeSpaceCollisionInfo freespace_collision_info;
    bool is_collision = false;
    size_t first_collision_index = std::numeric_limits<size_t>::max();
    AvpSpeedPlanCollisionInfo::CollisionType collision_type =
        AvpSpeedPlanCollisionInfo::NO_COLLISION;
    double stop_reserve_distance = 0.0;
    double curr_collision_distance = std::numeric_limits<double>::max();

    void Init(size_t path_size) {
      static_obstacle_collision_info.Init(path_size);
      moving_obstacle_collision_info.Init(path_size);
      outside_wheelmask_obstacle_collision_info.Init(path_size);
      freespace_collision_info.Init(path_size);
      is_collision = false;
      first_collision_index = path_size - 1;
      collision_type = AvpSpeedPlanCollisionInfo::NO_COLLISION;
      stop_reserve_distance = 0.0;
      curr_collision_distance = std::numeric_limits<double>::max();
    }
  };

 public:
  explicit PathHandle(const OpenSpaceSpeedOptimizerConfig& config);
  ~PathHandle() = default;

  /**
   * @brief 独立编译版路径处理 - 使用 SpeedOptimizerInput/Output
   * 原 Process() 签名改造：移除 Obstacle/OpenSpaceInfo 指针依赖
   * input  速度优化器输入
   * output 速度优化器输出
   * return std::string 空字符串=成功, 否则为错误信息
   */
  std::string Process(const SpeedOptimizerInput& input,
                      SpeedOptimizerOutput* output);

  /**
   * @brief Get the Speed Limits object
   */
  const std::vector<double>& GetSpeedLimits() const { return speed_limits_; }

  /**
   * @brief Get the Speed Limit Unit S object
   */
  double GetSpeedLimitUnitS() const { return speed_limit_unit_s_; }

 private:
  /**
   * @brief init info
   */
  void Init(const planning::OpenSpaceEnvStructuredInfo&
                open_space_env_structured_info);

  /**
   * @brief cut off path by wheel mask info
   */
  bool CutOffPathByWheelMask(const DiscretizedPath& path, bool is_forward,
                             bool is_parking_inwards,
                             bool is_consider_wheel_mask,
                             const Box2d& wheel_mask_box,
                             DiscretizedPath* new_path);

  /**
   * @brief update collision buffer info
   */
  OpenSpaceSpeedOptimizerConfig::CollisionBufferInfo UpdateCollisionBuffer(
      bool is_forward) const;

  /**
   * @brief calculate lateral control diff
   */
  static void CalLateralBufferByControlDiff(
      const common::VehicleState& vehicle_state,
      const common::PathPoint& path_p, double* left_lateral_buffer,
      double* right_lateral_buffer);

  /**
   * @brief is path collision with freespaces
   * 独立编译改造：保留freespace碰撞检测逻辑
   */
  bool IsCollisionWithFreeSpaceSegment(
      const DiscretizedPath& path,
      const OpenSpaceSpeedOptimizerConfig::CollisionBufferInfo&
          collision_buffer,
      double left_control_diff, double right_control_diff,
      bool is_use_middle_buffer, bool is_mirror_fold,
      const PathType& path_type,
      size_t cur_path_idx,
      CollisionInfo::FreeSpaceCollisionInfo* collision_info);

  /**
   * @brief 简化版静态障碍物碰撞检测
   * 独立编译改造：替代原 IsCollisionWithStaticObstacle（依赖 Obstacle 类）
   * 使用 SimpleStaticObstacle 的 Box2d + Polygon2d overlap 检测
   */
  bool IsCollisionWithStaticObstacle(
      const DiscretizedPath& path, bool is_forward,
      CollisionInfo::ObstacleCollisionInfo* collision_info);

    /**
     * @brief 预生成动态障碍物预测包围盒
     * standalone 对齐原版 moving_obs_boxs_ 结构：
     * 先按时间片缓存预测 box，再在碰撞检测阶段复用。
     */
    void UpdateMovingObstacleBoxes();

  /**
   * @brief 简化版动态障碍物碰撞检测
   * standalone 最小实现：使用 moving obstacle box 与车辆 box overlap，
   * 仅用于打通 MOVING_OBSTACLE_COLLISION 分支。
   */
  bool IsCollisionWithMovingObstacle(
      const DiscretizedPath& path, bool is_forward,
      CollisionInfo::ObstacleCollisionInfo* collision_info);

  /**
   * @brief collision info decision by static/moving/freespace collision info
   */
  void CollisionInfoDecision(CollisionInfo* collision_info);

  /**
   * @brief update collision info (orchestrator)
   */
  bool UpdateCollisionInfo(
      const DiscretizedPath& path,
      const std::shared_ptr<const FreeSpaceOutArray>& freespace_out_array,
      const std::vector<size_t>& under_spot_low_fs_idxs,
      const std::vector<size_t>& high_curb_fs_idxs,
      const std::vector<std::pair<size_t, std::vector<size_t>>>& ignore_fs_idxs,
      const common::VehicleState& vehicle_state, bool is_forward,
      bool is_rpa_direct_mode, bool is_mirror_fold,
      const PartitionedPath& partitioned_paths, CollisionInfo* collision_info);

  /**
   * @brief update interactive stage
   */
  void UpdateInteractiveStage(
      bool is_vehicle_still, bool is_rpa_direct_mode,
      const CollisionInfo& collision_info,
      AvpSpeedPlanCollisionInfo::SpeedTaskInteractiveStage* interactive_stage);

  void UpdateIsUseMiddleBuffer(
      const AvpSpeedPlanCollisionInfo::SpeedTaskInteractiveStage&
          interactive_stage,
      bool curr_is_forward,
      const PathType& path_type);

  /**
   * @brief cutoff path by collision info
   */
  static bool CutOffPathByCollisionInfo(const DiscretizedPath& path,
                                        const CollisionInfo& collision_info,
                                        DiscretizedPath* candidate_path);

  /**
   * @brief calculate limit speed by s
   */
  static double CalLimitSpeedByS(
      double s,
      const OpenSpaceSpeedOptimizerConfig::SpeedBoundInfo& speed_bound_info);

  /**
   * @brief update speed limits
   */
  void UpdateSpeedLimits(
      const DiscretizedPath& path,
      const OpenSpaceSpeedOptimizerConfig::SpeedBoundInfo& speed_bound_info,
      bool is_forward,
      const std::vector<common::PathPoint>& limit_speed_path_points);

  void SmoothSpeedLimits();

  /**
   * @brief update debug info → writes to SpeedOptimizerOutput
   */
  void UpdateDebugInfo(
      const CollisionInfo& collision_info,
      const AvpSpeedPlanCollisionInfo::SpeedTaskInteractiveStage&
          interactive_stage,
      const common::PathPoint& future_collision_point, bool is_vehicle_still,
      double wheel_mask_distance, SpeedOptimizerOutput* output);

  /**
   * @brief update collision risk count
   */
  void UpdatePathCollisionRiskCount(
      const DiscretizedPath& path,
      const PathType& path_type,
      size_t cur_path_idx, bool is_mirror_fold, bool is_forward);

  /**
   * @brief update real_time lateral buffer
   */
  static void UpdateLateralBuffer(
      const OpenSpaceSpeedOptimizerConfig::CollisionBufferInfo&
          collision_buffer,
      bool is_use_middle_buffer, bool is_narrow_spot_scenario,
      bool is_lat_park_in, bool is_lat_park_out,
      const PathType& path_type,
      size_t cur_path_idx,
      std::pair<double, double>* lateral_buffer_for_vehicle,
      std::pair<double, double>* lateral_buffer_for_not_vehicle,
      std::pair<double, double>* lateral_buffer_for_low_fs);

  // 独立编译改造：保留 freespace 信息更新（不依赖 Obstacle 类）
  void UpdateFreeSpaceInfo(
      const DiscretizedPath& path,
      const std::shared_ptr<const FreeSpaceOutArray>& freespace_out_array,
      const std::vector<size_t>& under_spot_low_fs_idxs,
      const std::vector<size_t>& high_curb_fs_idxs,
      const std::vector<std::pair<size_t, std::vector<size_t>>>&
          ignore_fs_idxs);

 private:
  const OpenSpaceSpeedOptimizerConfig& config_;

  // 独立编译改造：简化版障碍物缓存
  // 替代原 moving_obs_ptrs_, static_obs_ptrs_, uss_obs_ptrs_, wheelmask_obs_ptrs_
  std::vector<SimpleStaticObstacle> static_obstacles_;
    std::vector<SimpleMovingObstacle> moving_obstacles_;
    std::vector<std::vector<common::math::Box2d>> moving_obstacle_boxes_;

  const double predict_unit_t_ = 0.2;
  const size_t predict_box_size_ = 0;

  // FreeSpace 段（保留，用于 freespace 碰撞检测）
  std::vector<FreeSpaceSegment> not_lidar_not_vehicle_fs_segments_;
  std::vector<FreeSpaceSegment> not_lidar_vehicle_fs_segments_;
  std::vector<FreeSpaceSegment> lidar_vehicle_fs_segments_;
  std::vector<FreeSpaceSegment> lidar_not_vehicle_fs_segments_;
  std::vector<FreeSpaceSegment> low_height_fs_segments_;
  std::vector<FreeSpaceSegment> high_height_curb_fs_segments_;

  std::vector<std::pair<common::math::LineSegment2d, double>>
      not_lidar_not_vehicle_fs_;
  std::vector<std::pair<common::math::LineSegment2d, double>>
      not_lidar_vehicle_fs_;
  std::vector<std::pair<common::math::LineSegment2d, double>> lidar_vehicle_fs_;
  std::vector<std::pair<common::math::LineSegment2d, double>>
      lidar_not_vehicle_fs_;
  std::vector<std::pair<common::math::LineSegment2d, double>> low_height_fs_;
  std::vector<std::pair<common::math::LineSegment2d, double>>
      high_height_curb_fs_;

  std::vector<std::pair<common::math::LineSegment2d, double>>
      all_freespace_segments_;

  bool pre_wheel_mask_valid_ = false;

  const double speed_limit_unit_s_ = 0.2;
  std::vector<double> speed_limits_;

  // 独立编译改造：Clock::NowInSeconds() → std::chrono
  double GetCurrentTimeSeconds() const {
    return std::chrono::duration<double>(
               std::chrono::steady_clock::now().time_since_epoch())
        .count();
  }
  double wait_replan_start_time_ = 0.0;
  double wait_replan_to_init_time_ = 0.0;
  double wait_obstacle_start_time_ = 0.0;

  int wait_obstacle_count_ = 0;
  int bigger_buffer_safe_count_ = 0;
  bool is_use_middle_buffer_ = false;

  bool is_narrow_spot_scenario_ = false;
  bool is_lateral_park_in_ = false;
  bool is_lateral_park_out_ = false;
  bool is_vertical_park_in_ = false;
  bool is_nns_adjust_ = false;
  std::pair<bool, bool> last_is_forward_{false, false};

  int current_path_has_collision_count_ = 0;
  size_t current_path_collision_index_ = 0;
};

}  // namespace planning
}  // namespace TL
