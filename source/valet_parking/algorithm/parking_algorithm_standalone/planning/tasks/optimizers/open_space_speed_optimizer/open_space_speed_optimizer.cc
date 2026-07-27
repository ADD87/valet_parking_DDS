/**
 * @file open_space_speed_optimizer.cc
 * @brief 独立编译改造版 - 开放空间速度优化器实现
 *
 * 改造要点：
 * - Process(Frame*) -> Execute(SpeedOptimizerInput, SpeedOptimizerOutput*)
 * - frame_->xxx -> input.xxx
 * - ThreadPool::ForEach -> std::vector<std::thread>
 * - config_.open_space_speed_optimizer_config().xxx() -> config_.xxx
 * - FLAGS_xxx -> config_.xxx
 * - set_speed_optimizer_trajectory -> output->trajectory_gear
 * - mutable_debug_instance -> output->st_debug_info
 */

#include "planning/tasks/optimizers/open_space_speed_optimizer/open_space_speed_optimizer.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <ctime>
#include <iterator>
#include <limits>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_set>
#include <utility>
#include <vector>

#include "planning/tasks/optimizers/open_space_speed_optimizer/path_handle.h"
#include "planning/tasks/optimizers/open_space_speed_optimizer/st_sample_cost.h"

#include "common/configs/vehicle_config_helper.h"
#include "common/math/double_type.h"
#include "common/math/math_utils.h"
#include "planning/common/path/discretized_path.h"
#include "planning/common/speed/speed_data.h"
#include "planning/common/trajectory/discretized_trajectory.h"
#include "planning/common/trajectory1d/piecewise_acceleration_trajectory1d.h"
#include "planning/tasks/optimizers/open_space_speed_optimizer/st_sample_curves.h"
#include "proto_convert/pnc_point_convert.h"

namespace TL {
namespace planning {

using TL::common::TrajectoryPoint;
using TL::common::math::double_type::DefinitelyGreater;
using TL::common::math::double_type::DefinitelyLess;

// 独立编译改造：kMinSampleS 已在 st_sample_curves.h 中定义，此处不再重复

// ============================================================================
// Constructor - 独立编译改造：直接接收配置结构体
// ============================================================================
OpenSpaceSpeedOptimizer::OpenSpaceSpeedOptimizer(
    const OpenSpaceSpeedOptimizerConfig& config)
    : config_(config),
      path_handle_(config),
      trajectory_unit_t_(config.tarjectory_unit_t) {}

// ============================================================================
// Execute - 独立编译改造：替代 Process(Frame*)
// ============================================================================
bool OpenSpaceSpeedOptimizer::Execute(const SpeedOptimizerInput& input,
                                      SpeedOptimizerOutput* output) {
  if (nullptr == output) {
    AERROR << "output is nullptr!";
    return false;
  }

  st_debug_info_.Clear();

  // 独立编译改造：从 input 获取路径和档位（原 frame->open_space_info()）
  const DiscretizedPath& complete_path = input.discretized_path;
  const soc::GearPosition& gear = input.gear;
  TrajGearPair trajectory_gear;
  trajectory_gear.second = gear;

  InitInteractiveStage(gear);

  auto pre_check_msg = SpeedPlanPreCheck(
      complete_path, gear, input.is_stop_path);
  if (!pre_check_msg.empty()) {
    GenerateStopTrajectory(input, &trajectory_gear);
    AERROR << pre_check_msg;
    st_debug_info_.set_message(pre_check_msg);
    output->trajectory_gear = trajectory_gear;
    output->st_debug_info = st_debug_info_;
    output->success = true;
    output->message = pre_check_msg;
    return true;
  }

  // 独立编译改造：start_point 从 input 获取
  auto start_point = input.start_point;
  is_forward_ = (gear == soc::GearPosition::GEAR_DRIVE);
  if (input.is_gear_changed) {
    start_point.v = 0.0;
    start_point.a = 0.0;
    last_curve_.reset();
  }

  // 独立编译改造：speed_bound_info 直接从 input.config 获取
  speed_bound_info_ = is_forward_
      ? input.config.apa_speed_bound_info.forward_info
      : input.config.apa_speed_bound_info.reverse_info;

  auto vehicle_state = input.vehicle_state;

  // 独立编译改造：PathHandle 已改造，直接使用 SpeedOptimizerInput/Output
  SpeedOptimizerOutput path_handle_output;
  path_handle_output.interactive_stage = interactive_stage_;
  auto path_handle_msg = path_handle_.Process(input, &path_handle_output);

  if (!path_handle_msg.empty()) {
    AERROR << path_handle_msg;
    GenerateStopTrajectory(input, &trajectory_gear);
    st_debug_info_.set_message(path_handle_msg);
    output->trajectory_gear = trajectory_gear;
    output->st_debug_info = st_debug_info_;
    output->success = true;
    output->message = path_handle_msg;
    return true;
  }

  // 从 PathHandle 输出获取候选路径和交互状态
  DiscretizedPath candidate_path = path_handle_output.candidate_path;
  interactive_stage_ = path_handle_output.interactive_stage;
  output->speed_plan_collision_info = path_handle_output.speed_plan_collision_info;
  output->replan_triggered_by_speed_plan = path_handle_output.replan_triggered_by_speed_plan;
  output->current_path_has_collision_risk = path_handle_output.current_path_has_collision_risk;
  output->future_collision_point = path_handle_output.future_collision_point;

  UpdateStDebugInfo(start_point.v, start_point.a, complete_path.Length(),
                    candidate_path.Length(), vehicle_state.linear_velocity,
                    vehicle_state.linear_acceleration,
                    path_handle_.GetSpeedLimits(),
                    path_handle_.GetSpeedLimitUnitS());

  if (AvpSpeedPlanCollisionInfo::INIT != interactive_stage_) {
    std::string msg = "interactive is " +
                      planning_internal::SpeedTaskInteractiveStage_Name(
                          interactive_stage_);
    GenerateStopTrajectory(input, &trajectory_gear);
    st_debug_info_.set_message(msg);
    output->trajectory_gear = trajectory_gear;
    output->st_debug_info = st_debug_info_;
    output->interactive_stage = interactive_stage_;
    output->success = true;
    output->message = msg;
    return true;
  }

  UpdateSampleParams(start_point, is_forward_, candidate_path.Length());
  std::string msg;
  if (!GenerateTrajectory(st_sample_params_, candidate_path, input,
                          &trajectory_gear, &msg)) {
    msg = "generate trajectory failed!";
    AERROR << msg;
    st_debug_info_.set_message(msg);
    output->trajectory_gear = trajectory_gear;
    output->st_debug_info = st_debug_info_;
    output->success = false;
    output->message = msg;
    return false;
  }

  st_debug_info_.set_message(msg);
  output->trajectory_gear = trajectory_gear;
  output->st_debug_info = st_debug_info_;
  output->interactive_stage = interactive_stage_;
  output->success = true;
  output->message = msg;
  return true;
}

// ============================================================================
// Reset
// ============================================================================
void OpenSpaceSpeedOptimizer::Reset() {
  is_forward_ = true;
  interactive_stage_ = AvpSpeedPlanCollisionInfo::INIT;
  speed_bound_info_ = {};
  last_curve_ = nullptr;
  min_costs_.clear();
  st_debug_info_.Clear();
  AINFO << "[SPEED_OPT] open space optimizer is reset!";
}

// ============================================================================
// GenerateStopTrajectory - 独立编译改造：从 input 获取数据
// ============================================================================
void OpenSpaceSpeedOptimizer::GenerateStopTrajectory(
    const SpeedOptimizerInput& input,
    TrajGearPair* const trajectory_gear_ptr) {
  AINFO << "[SPEED_OPT] no space to move, stop now!";

  if (nullptr == trajectory_gear_ptr) {
    AERROR << "GenerateStopTrajectory init check failed";
    return;
  }

  static constexpr double kStopACC = 0.3;
  soc::GearPosition gear = trajectory_gear_ptr->second;
  double stop_acc = -kStopACC;
  if (soc::GearPosition::GEAR_REVERSE == gear) {
    stop_acc = kStopACC;
  }

  double offset_s = 0.0;
  double offset_relative_t = 0.0;
  if (!input.is_gear_changed) {
    offset_s = input.start_point.path_point.s;
    offset_relative_t = input.start_point.relative_time;
  }

  DiscretizedTrajectory& trajectory = trajectory_gear_ptr->first;
  trajectory.clear();
  TrajectoryPoint start_p = input.start_point;
  start_p.v = 0.0;
  start_p.a = stop_acc;

  if (!input.discretized_path.empty()) {
    auto kappa = input.discretized_path.front().kappa;
    start_p.path_point.kappa = kappa;
  } else {
    start_p.path_point.kappa = 0.0;
  }

  common::TrajectoryPoint trajectory_point;
  int publish_points = config_.publish_trajectory_points_number;
  for (int i = 0; i < publish_points; ++i) {
    trajectory_point = start_p;
    trajectory_point.path_point.s = (
        trajectory_point.path_point.s + offset_s);
    trajectory_point.relative_time = i * trajectory_unit_t_ +
                                       offset_relative_t;
    trajectory.AppendTrajectoryPoint(trajectory_point);
  }

  last_curve_.reset();
}

// ============================================================================
// GenerateBackUpTrajectory - 独立编译改造：从 input 获取数据
// ============================================================================
bool OpenSpaceSpeedOptimizer::GenerateBackUpTrajectory(
    const StSampleParams& sample_params,
    const DiscretizedPath& candidate_path,
    const SpeedOptimizerInput& input,
    TrajGearPair* const traj_gear_ptr) {
  if (nullptr == traj_gear_ptr) {
    AERROR << "input pointer is nullptr!";
    return false;
  }

  if (!DefinitelyGreater(sample_params.end_s, kMinSampleS)) {
    AERROR << "candidate path length is not greater 0.01!";
    return false;
  }

  static constexpr double kMinThreshold = 0.01;
  const double init_v = fabs(sample_params.start_v);
  if (!DefinitelyGreater(init_v, kMinThreshold)) {
    AERROR << "init_v is not greater than 0.01!";
    return false;
  }

  const double path_length = sample_params.end_s;
  const double dec = fmin(init_v * init_v / (2.0 * path_length),
                          fabs(common::VehicleConfigHelper::GetConfig()
                                   .vehicle_param()
                                   .max_deceleration));
  const double dec_t = init_v / dec;
  auto ptr_trajectory =
      std::make_unique<PiecewiseAccelerationTrajectory1d>(0, init_v);
  ptr_trajectory->AppendSegment(-dec, dec_t);
  std::vector<double> s;
  std::vector<double> ds;
  std::vector<double> dds;
  const size_t num_of_knots =
      static_cast<size_t>(dec_t / trajectory_unit_t_) + 1;
  s.reserve(num_of_knots);
  ds.reserve(num_of_knots);
  dds.reserve(num_of_knots);
  for (size_t i = 0; i < num_of_knots; ++i) {
    auto t = static_cast<double>(i) * trajectory_unit_t_;
    if (!DefinitelyLess(t, dec_t)) {
      s.push_back(path_length);
      ds.push_back(0.0);
      dds.push_back(dec_t);
      break;
    }
    s.push_back(ptr_trajectory->Evaluate(0, t));
    ds.push_back(ptr_trajectory->Evaluate(1, t));
    dds.push_back(ptr_trajectory->Evaluate(2, t));
  }

  SpeedData candidate_speed;
  candidate_speed.AppendSpeedPoint(s[0], 0.0, ds[0], dds[0], 0.0);
  for (size_t i = 1; i < num_of_knots; ++i) {
    if (DefinitelyLess(s[i], s[i - 1])) {
      AERROR << "unexpected decreasing s in speed data at time "
             << static_cast<double>(i) * trajectory_unit_t_
             << " with total time " << dec_t << " i = " << i;
      return false;
    }
    candidate_speed.AppendSpeedPoint(
        s[i], trajectory_unit_t_ * static_cast<double>(i), ds[i], dds[i],
        (dds[i] - dds[i - 1]) / trajectory_unit_t_);
    if (DefinitelyLess(path_length, s[i])) {
      break;
    }
  }

  // 独立编译改造：CombinePathAndSpeed 需要 offset 参数
  double offset_s = 0.0;
  double offset_relative_t = 0.0;
  if (!input.is_gear_changed) {
    offset_s = input.start_point.path_point.s;
    offset_relative_t = input.start_point.relative_time;
  }

  if (!CombinePathAndSpeed(is_forward_, candidate_path, candidate_speed,
                           input.is_gear_changed,
                           offset_s, offset_relative_t,
                           &traj_gear_ptr->first)) {
    AERROR << "combine path and speed failed!";
    return false;
  }

  last_curve_.reset();
  return true;
}

// ============================================================================
// CombinePathAndSpeed - 独立编译改造：offset参数化
// ============================================================================
bool OpenSpaceSpeedOptimizer::CombinePathAndSpeed(
    const bool is_forward,
    const DiscretizedPath& path_points,
    const SpeedData& speed_points,
    const bool is_gear_changed,
    const double offset_s,
    const double offset_relative_t,
    DiscretizedTrajectory* const discretized_trajectory_ptr) {
  if (nullptr == discretized_trajectory_ptr || speed_points.empty() ||
      path_points.empty()) {
    AERROR << "init check failed!";
    return false;
  }

  discretized_trajectory_ptr->clear();
  const auto trajectory_size =
      static_cast<size_t>(speed_points.TotalTime() / trajectory_unit_t_ + 1);

  common::SpeedPoint speed_point;
  common::PathPoint path_point;
  common::TrajectoryPoint trajectory_point;
  for (size_t i = 0; i < trajectory_size + 1; i++) {
    auto t = static_cast<double>(i) * trajectory_unit_t_;
    if (i < trajectory_size) {
      if (!speed_points.EvaluateByTime(t, &speed_point)) {
        AERROR << "Fail to get speed point with relative time " << t;
        return false;
      }
    } else {
      speed_point = speed_points.back();
      speed_point.t = t;
    }

    if (DefinitelyLess(path_points.Length(), speed_point.s)) {
      break;
    }

    path_point = path_points.Evaluate(speed_point.s);
    trajectory_point.path_point = path_point;
    trajectory_point.path_point.s = (
        trajectory_point.path_point.s + offset_s);
    trajectory_point.relative_time = speed_point.t + offset_relative_t;
    if (is_forward) {
      trajectory_point.v = speed_point.v;
      trajectory_point.a = speed_point.a;
    } else {
      trajectory_point.v = 0.0 - speed_point.v;
      trajectory_point.a = 0.0 - speed_point.a;
    }

    discretized_trajectory_ptr->AppendTrajectoryPoint(trajectory_point);
  }

  ADEBUG << "path length before combine " << path_points.Length();
  ADEBUG << "trajectory length after combine "
         << discretized_trajectory_ptr->GetSpatialLength();
  return true;
}

// ============================================================================
// SpeedPlanPreCheck - 独立编译改造：GearPosition 适配
// ============================================================================
std::string OpenSpaceSpeedOptimizer::SpeedPlanPreCheck(
    const DiscretizedPath& path,
    const soc::GearPosition& gear,
    const bool is_stop_path) {
  std::string msg;
  if (is_stop_path ||
      (soc::GearPosition::GEAR_DRIVE != gear &&
       soc::GearPosition::GEAR_REVERSE != gear) ||
      path.empty()) {
    std::stringstream ss;
    ss << "chosen path is stop path: " << is_stop_path
       << " gear: " << static_cast<int>(gear)
       << " path is empty: " << path.empty();
    return ss.str();
  }
  return msg;
}

// ============================================================================
// UpdateSampleParams - 独立编译改造：config_ 直接访问
// ============================================================================
void OpenSpaceSpeedOptimizer::UpdateSampleParams(
    const common::TrajectoryPoint& start_point, const bool is_forward,
    const double end_s, const double start_s, const double end_v) {
  double start_v = fabs(start_point.v);
  double start_acc = start_point.a;
  if (!is_forward) {
    start_acc = -start_point.a;
  }

  st_sample_params_.start_s = start_s;
  st_sample_params_.start_v = start_v;
  st_sample_params_.start_acc = start_acc;
  st_sample_params_.end_s = end_s;
  st_sample_params_.end_v = fabs(end_v);
  // 独立编译改造：config_ 直接访问成员（原 config_.open_space_speed_optimizer_config().xxx()）
  st_sample_params_.unit_acc = config_.sample_unit_acc;
  st_sample_params_.unit_max_v = config_.sample_unit_max_v;
  st_sample_params_.efficiency_cost = config_.efficiency_cost;
  st_sample_params_.acc_cost = config_.acc_cost;
  st_sample_params_.jerk_cost = config_.jerk_cost;
  st_sample_params_.over_speed_cost_max = config_.over_speed_cost_max;
  st_sample_params_.over_speed_cost_min = config_.over_speed_cost_min;
  st_sample_params_.diff_cost = config_.diff_cost;
  st_sample_params_.speed_bound_info = speed_bound_info_;

  if (nullptr != last_curve_) {
    // 独立编译改造：CalDiffTimeFromLast 需要 input（但这里没有）
    // 使用默认时间步长作为 fallback
    double diff_time = trajectory_unit_t_;
    last_curve_->UpdateOriginByDiffTime(diff_time);
  }
}

// ============================================================================
// SampleStCurves
// ============================================================================
bool OpenSpaceSpeedOptimizer::SampleStCurves(
    const StSampleParams& sample_params) {
  size_t curves_size = st_sample_curves_.SampleProcess(sample_params);
  return curves_size > 0;
}

// ============================================================================
// GetBestCurveIdx - 独立编译改造：ThreadPool -> std::thread
// ============================================================================
bool OpenSpaceSpeedOptimizer::GetBestCurveIdx(int* const best_idx) {
  if (best_idx == nullptr) {
    AERROR << "input pointer is nullptr!";
    return false;
  }
  static constexpr size_t kMinThreadSize = 4;
  static constexpr size_t kMaxThreadSize = 1024;

  *best_idx = -1;
  auto* st_curves = st_sample_curves_.GetMutableStSampleCurves();
  const auto st_curve_size = st_sample_curves_.GetStSampleCurveSize();
  auto thread_count = static_cast<size_t>(config_.sample_thread_size);
  thread_count =
      common::math::Clamp(thread_count, kMinThreadSize, kMaxThreadSize);
  min_costs_.reserve(thread_count);
  min_costs_.assign(thread_count, {0, std::numeric_limits<double>::max()});

  const size_t count_per_thread = st_curve_size / thread_count + 1;

  // 独立编译改造：ThreadPool::ForEach -> std::vector<std::thread>
  std::vector<std::thread> threads;
  threads.reserve(thread_count);
  for (size_t thread_index = 0; thread_index < thread_count; thread_index++) {
    threads.emplace_back([&, thread_index]() {
      const size_t start_index = thread_index * count_per_thread;
      const size_t end_index =
          std::min(start_index + count_per_thread, st_curve_size);
      StSampleCost st_sample_cost(
          st_sample_params_, path_handle_.GetSpeedLimits(), last_curve_,
          path_handle_.GetSpeedLimitUnitS(), trajectory_unit_t_ * 2.0,
          st_sample_curves_.GetMaxSampleT());
      double min_cost = std::numeric_limits<double>::max();
      size_t min_cost_index = 0;
      for (size_t i = start_index; i < end_index && i < st_curve_size; i++) {
        auto* st_curve = &st_curves->at(i);
        double cost = st_sample_cost.CalCurveCost(st_curve);
        if (DefinitelyLess(cost, min_cost)) {
          min_cost_index = i;
          min_cost = cost;
        }
      }
      if (thread_index < min_costs_.size()) {
        min_costs_[thread_index].first = min_cost_index;
        min_costs_[thread_index].second = min_cost;
      }
    });
  }
  for (auto& t : threads) {
    t.join();
  }

  std::sort(min_costs_.begin(), min_costs_.end(),
            [](const std::pair<size_t, double>& left,
               const std::pair<size_t, double>& right) {
              return left.second < right.second;
            });
  *best_idx = static_cast<int>(min_costs_.front().first);

  return (*best_idx) >= 0 && (*best_idx) < static_cast<int>(st_curve_size);
}

// ============================================================================
// SampleTrajectory - 独立编译改造：从 input 获取数据
// ============================================================================
bool OpenSpaceSpeedOptimizer::SampleTrajectory(
    const StSampleParams& sample_params,
    const DiscretizedPath& candidate_path,
    const SpeedOptimizerInput& input,
    TrajGearPair* const traj_gear_ptr) {
  if (traj_gear_ptr == nullptr) {
    AERROR << "input pointer is nullptr!";
    return false;
  }

  if (!SampleStCurves(sample_params)) {
    AERROR << "sample st curves failed!";
    return false;
  }

  int best_index = -1;
  if (!GetBestCurveIdx(&best_index)) {
    AERROR << "get best speed data failed!";
    return false;
  }
  ADEBUG << "sample curve size: " << st_sample_curves_.GetStSampleCurveSize()
         << " best curve idx is: " << best_index << " "
         << st_sample_curves_.GetStSampleCurves()[best_index].DebugInfo();

  const auto& best_curve = st_sample_curves_.GetStSampleCurves()[best_index];
  st_debug_info_.set_best_curve(best_curve.DebugInfo());
  if (nullptr != last_curve_) {
    st_debug_info_.set_last_curve(last_curve_->DebugInfo());
  }

  last_curve_ = best_curve.Clone();
  SpeedData candidate_speed;
  if (!best_curve.Discrete(trajectory_unit_t_, &candidate_speed)) {
    AERROR << "get speed data from best curve failed!";
    return false;
  }

  // 独立编译改造：CombinePathAndSpeed 需要 offset 参数
  double offset_s = 0.0;
  double offset_relative_t = 0.0;
  if (!input.is_gear_changed) {
    offset_s = input.start_point.path_point.s;
    offset_relative_t = input.start_point.relative_time;
  }

  if (!CombinePathAndSpeed(is_forward_, candidate_path, candidate_speed,
                           input.is_gear_changed,
                           offset_s, offset_relative_t,
                           &traj_gear_ptr->first)) {
    AERROR << "combine path and speed failed!";
    return false;
  }

  return true;
}

// ============================================================================
// GenerateTrajectory
// ============================================================================
bool OpenSpaceSpeedOptimizer::GenerateTrajectory(
    const StSampleParams& sample_params,
    const DiscretizedPath& candidate_path,
    const SpeedOptimizerInput& input,
    TrajGearPair* const traj_gear_ptr,
    std::string* const msg) {
  if (nullptr == traj_gear_ptr || nullptr == msg) {
    AERROR << "input pointer is nullptr!";
    return false;
  }

  *msg = "generate sample trajectory!";
  if (!SampleTrajectory(sample_params, candidate_path, input, traj_gear_ptr)) {
    *msg = "generate backup trajectory!";
    if (!GenerateBackUpTrajectory(sample_params, candidate_path, input,
                                  traj_gear_ptr)) {
      *msg = "sample and backup trajectory failed!";
      GenerateStopTrajectory(input, traj_gear_ptr);
    }
  }
  ADEBUG << *msg;
  return true;
}

// ============================================================================
// UpdateStDebugInfo
// ============================================================================
void OpenSpaceSpeedOptimizer::UpdateStDebugInfo(
    const double start_v, const double start_acc, const double total_s,
    const double end_s, const double actual_v, const double actual_acc,
    const std::vector<double>& speed_limits, const double speed_limit_unit_s) {
  st_debug_info_.set_start_v(start_v);
  st_debug_info_.set_start_acc(start_acc);
  st_debug_info_.set_total_s(total_s);
  st_debug_info_.set_end_s(end_s);
  st_debug_info_.set_actual_v(actual_v);
  st_debug_info_.set_actual_acc(actual_acc);
  for (size_t i = 0; i < speed_limits.size(); i++) {
    auto s = static_cast<double>(i) * speed_limit_unit_s;
    auto* speed_limit_p = st_debug_info_.add_speed_limit_points();
    speed_limit_p->set_s(s);
    speed_limit_p->set_limit_v(speed_limits[i]);
  }
}

// ============================================================================
// InitInteractiveStage - 独立编译改造：简化（无 local_view）
// ============================================================================
void OpenSpaceSpeedOptimizer::InitInteractiveStage(
    const soc::GearPosition& gear) {
  if (soc::GearPosition::GEAR_PARKING == gear) {
    interactive_stage_ = AvpSpeedPlanCollisionInfo::INIT;
  }
  // 独立编译改造：移除 frame_->local_view() 依赖
  // 原逻辑：检查 FunctionManagerIn 的 sys_run_state
  // 独立版本中无此信息，RUNNING 状态自动重置为 INIT
  if (AvpSpeedPlanCollisionInfo::RUNNING == interactive_stage_) {
    interactive_stage_ = AvpSpeedPlanCollisionInfo::INIT;
  }
}

// ============================================================================
// CalDiffTimeFromLast - 独立编译改造：从 input 获取时间信息
// ============================================================================
double OpenSpaceSpeedOptimizer::CalDiffTimeFromLast(
    const SpeedOptimizerInput& input) {
  if (input.has_last_frame) {
    return input.vehicle_state.timestamp +
           input.start_point.relative_time -
           input.last_frame_timestamp -
           input.last_planning_start_relative_time;
  }
  return 0.0;
}

}  // namespace planning
}  // namespace TL
