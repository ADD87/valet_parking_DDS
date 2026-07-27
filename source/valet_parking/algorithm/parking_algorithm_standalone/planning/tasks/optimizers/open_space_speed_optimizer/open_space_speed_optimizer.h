/**
 * @file open_space_speed_optimizer.h
 * @brief 独立编译改造版 - 开放空间速度优化器
 *
 * 改造要点：
 * - 移除 SpeedOptimizer/Task 基类继承
 * - 移除 Frame, DependencyInjector 依赖
 * - 使用 SpeedOptimizerInput/Output 替代
 * - ThreadPool -> std::thread
 * - TaskConfig -> OpenSpaceSpeedOptimizerConfig 直接注入
 */
#pragma once

#include <algorithm>
#include <limits>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include "planning/common/path/discretized_path.h"
#include "planning/common/speed/speed_data.h"
#include "planning/common/trajectory/discretized_trajectory.h"
#include "planning/tasks/optimizers/open_space_speed_optimizer/path_handle.h"
#include "planning/tasks/optimizers/open_space_speed_optimizer/speed_optimizer_types.h"
#include "planning/tasks/optimizers/open_space_speed_optimizer/st_sample_curves.h"
#include "proto_convert/chassis_convert.h"
#include "proto_convert/open_space_speed_optimizer_config_convert.h"
#include "proto_convert/planning_internal_convert.h"
#include "proto_convert/pnc_point_convert.h"

namespace TL {
namespace planning {

using planning_internal::AvpSpeedPlanCollisionInfo;
using planning_internal::STSampleDebug;

/**
 * @brief 独立编译版速度优化器 - 无Apollo框架依赖
 *
 * 核心流程：PathHandle碰撞检测 -> ST采样 -> 最优速度曲线 -> 轨迹合成
 */
class OpenSpaceSpeedOptimizer {
 public:
  explicit OpenSpaceSpeedOptimizer(const OpenSpaceSpeedOptimizerConfig& config);
  ~OpenSpaceSpeedOptimizer() = default;

  bool Execute(const SpeedOptimizerInput& input, SpeedOptimizerOutput* output);
  void Reset();

 private:
  void GenerateStopTrajectory(
      const SpeedOptimizerInput& input,
      TrajGearPair* trajectory_gear_ptr);

  bool GenerateBackUpTrajectory(
      const StSampleParams& sample_params,
      const DiscretizedPath& candidate_path,
      const SpeedOptimizerInput& input,
      TrajGearPair* traj_gear_ptr);

  bool CombinePathAndSpeed(
      bool is_forward,
      const DiscretizedPath& path_points,
      const SpeedData& speed_points,
      bool is_gear_changed,
      double offset_s,
      double offset_relative_t,
      DiscretizedTrajectory* discretized_trajectory_ptr);

  static std::string SpeedPlanPreCheck(
      const DiscretizedPath& path,
      const soc::GearPosition& gear,
      bool is_stop_path);

  void UpdateSampleParams(
      const common::TrajectoryPoint& start_point,
      bool is_forward,
      double end_s,
      double start_s = 0.0,
      double end_v = 0.0);

  bool SampleStCurves(const StSampleParams& sample_params);
  bool GetBestCurveIdx(int* best_idx);

  bool SampleTrajectory(
      const StSampleParams& sample_params,
      const DiscretizedPath& candidate_path,
      const SpeedOptimizerInput& input,
      TrajGearPair* traj_gear_ptr);

  bool GenerateTrajectory(
      const StSampleParams& sample_params,
      const DiscretizedPath& candidate_path,
      const SpeedOptimizerInput& input,
      TrajGearPair* traj_gear_ptr,
      std::string* msg);

  void UpdateStDebugInfo(
      double start_v, double start_acc,
      double total_s, double end_s,
      double actual_v, double actual_acc,
      const std::vector<double>& speed_limits,
      double speed_limit_unit_s);

  void InitInteractiveStage(const soc::GearPosition& gear);
  double CalDiffTimeFromLast(const SpeedOptimizerInput& input);

 private:
  OpenSpaceSpeedOptimizerConfig config_;
  PathHandle path_handle_;
  bool is_forward_ = true;
  const double trajectory_unit_t_ = 0.1;
  AvpSpeedPlanCollisionInfo::SpeedTaskInteractiveStage interactive_stage_ =
      AvpSpeedPlanCollisionInfo::INIT;

  OpenSpaceSpeedOptimizerConfig::SpeedBoundInfo speed_bound_info_;
  StSampleParams st_sample_params_;
  StSampleCurves st_sample_curves_;
  std::shared_ptr<StCurve> last_curve_;
  std::vector<std::pair<size_t, double>> min_costs_;
  STSampleDebug st_debug_info_;
};

}  // namespace planning
}  // namespace TL
