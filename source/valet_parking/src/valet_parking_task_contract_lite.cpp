#include "valet_parking_task_contract_lite.h"

#include "valet_parking_stage_contract_lite.h"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <tuple>
#include <vector>

namespace valet_parking {

namespace {

const char* BoolText(bool value) {
  return value ? "true" : "false";
}

std::string JoinThreadPathIds(const std::vector<int>& path_ids) {
  if (path_ids.empty()) {
    return "[]";
  }
  std::ostringstream stream;
  stream << "[";
  for (std::size_t i = 0U; i < path_ids.size(); ++i) {
    if (i > 0U) {
      stream << ",";
    }
    stream << path_ids[i];
  }
  stream << "]";
  return stream.str();
}

}  // namespace

std::size_t DestRegionPointCount(
    const TL::planning::DestRegionWithAng& dest_region) {
  return static_cast<std::size_t>(std::get<0>(dest_region).num_points());
}

double DestRegionArea(const TL::planning::DestRegionWithAng& dest_region) {
  return std::fabs(std::get<0>(dest_region).area());
}

std::size_t CountPathProviderOutputPoints(
    const TL::planning::OpenSpacePathOutput& output) {
  std::size_t point_count = 0U;
  for (const TL::planning::PathGearPair& path_pair :
       output.partitioned_path) {
    point_count += path_pair.first.size();
  }
  return point_count;
}

void AppendDestRegionContract(
    const TL::planning::DestRegionWithAng& dest_region,
    std::ostringstream* stream) {
  if (stream == nullptr) {
    return;
  }
  *stream << ", dest_region_points=" << DestRegionPointCount(dest_region)
          << ", dest_region_area=" << std::fixed << std::setprecision(3)
          << DestRegionArea(dest_region)
          << ", dest_region_angle=[" << std::get<1>(dest_region)
          << "," << std::get<2>(dest_region) << "]";
}

void AppendRoiOpenSpaceInfoContract(
    uint32_t path_info_id,
    const TL::planning::RoiDeciderOutput& roi_output,
    std::ostringstream* stream) {
  if (stream == nullptr) {
    return;
  }
  *stream << ", open_space_info_contract=roi_output"
          << ", open_space_path_info_id=" << path_info_id
          << ", xy_bounds_size=" << roi_output.xy_bounds.size()
          << ", origin=(" << std::fixed << std::setprecision(3)
          << roi_output.origin_point.x() << ","
          << roi_output.origin_point.y() << ","
          << roi_output.origin_heading << ")";
  AppendDestRegionContract(roi_output.dest_region, stream);
}

void AppendOpenSpaceTaskContract(const std::string& record_type,
                                 std::ostringstream* stream) {
  if (stream == nullptr) {
    return;
  }
  *stream << ", task_contract=lightweight_open_space_task_projection"
          << ", task_contract_record=" << record_type
          << ", task_contract_chain="
          << "ROI_DECIDER>PATH_PROVIDER>PATH_PARTITION>SPEED_OPTIMIZER"
          << ", task_status_transport=replan_reason_text";
}

void AppendDirectTaskContract(const std::string& record_type,
                              std::ostringstream* stream) {
  if (stream == nullptr) {
    return;
  }
  *stream << ", task_contract=lightweight_direct_task_projection"
          << ", task_contract_record=" << record_type
          << ", task_contract_chain=OPEN_SPACE_STRAIGHT_PATH>SPEED_OPTIMIZER"
          << ", task_status_transport=replan_reason_text"
          << ", original_flow_branch=direct_open_space";
}

void AppendSpeedOptimizerTaskContract(bool is_rpa_direct_mode,
                                      std::ostringstream* stream) {
  if (is_rpa_direct_mode) {
    AppendDirectTaskContract("direct_speed_optimizer_output", stream);
  } else {
    AppendOpenSpaceTaskContract("speed_optimizer_output", stream);
  }
}

void AppendThreadedProviderDiagnostics(
    const TL::planning::OpenSpacePathProviderDiagnostics& diagnostics,
    std::ostringstream* stream) {
  if (stream == nullptr) {
    return;
  }
  *stream << ", threaded=" << BoolText(diagnostics.threaded)
          << ", provider_status=" << diagnostics.provider_status
          << ", target_plan="
          << (diagnostics.target_plan_submitted ? "submitted" : "none")
          << ", target_output="
          << (diagnostics.target_output_ready ? "ready" : "pending")
          << ", target_source="
          << (diagnostics.target_used_candidate_result
                  ? "preplan_candidate"
                  : (diagnostics.target_generated_in_target_thread
                         ? "target_thread"
                         : "unknown"))
          << ", target_timeout=" << BoolText(diagnostics.target_timed_out)
          << ", target_cancel="
          << BoolText(diagnostics.target_cancel_requested)
          << ", wait_s=" << diagnostics.wait_time_s
          << ", thread_path_ids="
          << JoinThreadPathIds(diagnostics.thread_path_ids);
}

void AppendPathProviderAttemptDiagnostics(
    const PathProviderAttemptContractLite& attempt,
    std::ostringstream* stream) {
  if (stream == nullptr) {
    return;
  }
  *stream << ", replan=" << attempt.replan_text
          << ", reason=" << attempt.decision_reason
          << ", warm_start=" << attempt.warm_start_source
          << ", warm_start_reject="
          << attempt.warm_start_reject_reason
          << ", warm_start_points=" << attempt.warm_start_points
          << ", warm_start_history_points="
          << attempt.warm_start_history_points
          << ", warm_start_s=" << attempt.warm_start_match_s
          << ", warm_start_l=" << attempt.warm_start_match_l
          << ", trace_adjust="
          << BoolText(attempt.trace_adjust_enabled)
          << ", trace_adjust_source=" << attempt.trace_adjust_source
          << ", trace_adjust_reject="
          << attempt.trace_adjust_reject_reason
          << ", trace_adjust_points=" << attempt.trace_adjust_points
          << ", trace_adjust_path_length="
          << attempt.trace_adjust_path_length
          << ", trace_adjust_min_length="
          << attempt.trace_adjust_min_length
          << ", external_vehicle=" << BoolText(attempt.external_vehicle)
          << ", external_obstacles=" << attempt.obstacle_count;
}

void AppendPlanningContextPathProjection(
    const PlanningContextPathProjectionLite& projection,
    const std::string& history_state,
    std::ostringstream* stream) {
  if (stream == nullptr) {
    return;
  }
  *stream << ", planning_context_contract=path_provider_runtime_projection"
          << ", path_history_state=" << history_state
          << ", path_history_available=" << BoolText(projection.has_history)
          << ", planning_context_path_id=" << projection.path_id
          << ", planning_context_replan_reason=" << projection.replan_text
          << ", target_update_writeback="
          << BoolText(projection.replan_text == "TARGET_UPDATE")
          << ", planning_context_history_points="
          << projection.history_points;
}

std::string BuildRoiReason(const TL::planning::RoiDeciderOutput& output,
                           uint32_t path_info_id) {
  std::ostringstream stream;
  stream << "ROI_DECIDER ok";
  AppendOpenSpaceTaskContract("roi_decider_output", &stream);
  stream << ", scenario=" << static_cast<int>(output.scenario_type)
         << ", lot_status=" << static_cast<int>(output.lot_status)
         << ", target=(" << std::fixed << std::setprecision(3)
         << output.end_pose.x << "," << output.end_pose.y << ","
         << output.end_pose.theta << ")"
         << ", end_pose=(" << output.end_pose.x << ","
         << output.end_pose.y << "," << output.end_pose.theta << ")"
         << ", fine_tuned=" << BoolText(output.has_fine_tuned)
         << ", slot_inner_fs_valid="
         << BoolText(output.is_slot_inner_fs_valid)
         << ", scenario_difficulty=" << output.scenario_difficulty
         << ", obs_segments=" << output.obs_segments.size()
         << ", linked_obs_segments=" << output.linked_obs_segments.size()
         << ", high_curb_obs_segments="
         << output.high_curb_obs_segments.size()
         << ", roi_wall_segments=" << output.roi_wall_segments.size()
         << ", virtual_obs_segments="
         << output.virtual_obs_segments.size();
  AppendRoiOpenSpaceInfoContract(path_info_id, output, &stream);
  return stream.str();
}

std::string BuildPathProviderPreCheckReason(
    const PathProviderPreCheckResult& result) {
  std::ostringstream stream;
  if (!result.ok) {
    stream << "PATH_PROVIDER_PRECHECK failed: " << result.reason;
    AppendOpenSpaceTaskContract("path_provider_precheck", &stream);
    stream << ", collision_contract=geometry_precheck_only"
           << ", collision_input_source=roi_and_external_segments"
           << ", wheel_mask_contract=not_exposed_in_current_mvp"
           << ", wheel_mask_input_source=none"
           << ", wheel_mask_idl_extension=required_before_vehicle_integration";
    return stream.str();
  }
  stream << "PATH_PROVIDER_PRECHECK ok"
         << ", xy_bounds_span=" << std::fixed << std::setprecision(3)
         << result.xy_width << "x" << result.xy_height
         << ", dest_points=" << result.dest_region_point_count
         << ", obstacle_segments=" << result.obstacle_segment_count
         << ", roi_segments=" << result.roi_obstacle_segment_count
         << ", external_segments=" << result.external_obstacle_segment_count
         << ", near_start_segments=" << result.obstacle_near_start_count
         << ", near_end_segments=" << result.obstacle_near_end_count
         << ", vehicle_has_state=" << BoolText(result.vehicle_has_state);
  AppendOpenSpaceTaskContract("path_provider_precheck", &stream);
  stream << ", collision_contract=geometry_precheck_only"
         << ", collision_input_source=roi_and_external_segments"
         << ", wheel_mask_contract=not_exposed_in_current_mvp"
         << ", wheel_mask_input_source=none"
         << ", wheel_mask_idl_extension=required_before_vehicle_integration";
  return stream.str();
}

std::string BuildPathProviderReuseReason(
    const TL::planning::OpenSpacePathOutput& path_output,
    uint32_t parking_seq,
    const TL::planning::RoiDeciderOutput& roi_output,
    const PathProviderHistoryContractLite& history,
    const PlanningContextPathProjectionLite& projection) {
  std::ostringstream stream;
  stream << "PATH_PROVIDER ok";
  AppendOpenSpaceTaskContract("path_provider_output", &stream);
  stream << ", partitions=" << path_output.partitioned_path.size()
         << ", points=" << CountPathProviderOutputPoints(path_output)
         << ", path_type=" << path_output.path_type
         << ", parking_seq=" << parking_seq
         << ", open_space_info_contract=path_provider_output"
         << ", path_info_id=" << parking_seq;
  AppendDestRegionContract(roi_output.dest_region, &stream);
  stream << ", smoothed=false"
         << ", history=reused"
         << ", replan=" << history.replan_text
         << ", warm_start=not_applied"
         << ", last_warm_start_points="
         << history.last_warm_start_points
         << ", last_strategy_init_move="
         << history.last_strategy_init_moving_direction
         << ", generated_count=" << history.generated_count
         << ", reused_count=" << history.reused_count
         << ", external_vehicle=" << BoolText(history.external_vehicle)
         << ", external_obstacles=" << history.obstacle_count;
  AppendPlanningContextPathProjection(projection, "reused", &stream);
  return stream.str();
}

std::string BuildPathProviderFailureReason(
    const std::string& message,
    const PathProviderAttemptContractLite& attempt,
    std::size_t preplan_candidate_count,
    const TL::planning::OpenSpacePathProviderDiagnostics* diagnostics) {
  std::ostringstream stream;
  stream << "PATH_PROVIDER failed: " << message;
  AppendOpenSpaceTaskContract("path_provider_output", &stream);
  AppendPathProviderAttemptDiagnostics(attempt, &stream);
  stream << ", preplan_candidates=" << preplan_candidate_count;
  if (diagnostics != nullptr) {
    AppendThreadedProviderDiagnostics(*diagnostics, &stream);
  }
  return stream.str();
}

std::string BuildPathProviderOutputFailureReason(
    const std::string& message,
    const TL::planning::OpenSpacePathOutput& path_output,
    const PathProviderAttemptContractLite& attempt,
    std::size_t preplan_candidate_count,
    const TL::planning::OpenSpacePathProviderDiagnostics& diagnostics) {
  std::ostringstream stream;
  stream << "PATH_PROVIDER failed: " << message
         << ", partitions=" << path_output.partitioned_path.size()
         << ", points=" << CountPathProviderOutputPoints(path_output);
  AppendOpenSpaceTaskContract("path_provider_output", &stream);
  AppendPathProviderAttemptDiagnostics(attempt, &stream);
  stream << ", preplan_candidates=" << preplan_candidate_count;
  AppendThreadedProviderDiagnostics(diagnostics, &stream);
  return stream.str();
}

std::string BuildPathProviderGeneratedReason(
    const TL::planning::OpenSpacePathOutput& path_output,
    uint32_t parking_seq,
    const TL::planning::RoiDeciderOutput& roi_output,
    const PathProviderAttemptContractLite& attempt,
    uint64_t generated_count,
    uint64_t reused_count,
    std::size_t preplan_candidate_count,
    const PlanningContextPathProjectionLite& projection,
    const TL::planning::OpenSpacePathProviderDiagnostics& diagnostics) {
  std::ostringstream stream;
  stream << "PATH_PROVIDER ok";
  AppendOpenSpaceTaskContract("path_provider_output", &stream);
  stream << ", partitions=" << path_output.partitioned_path.size()
         << ", points=" << CountPathProviderOutputPoints(path_output)
         << ", path_type=" << path_output.path_type
         << ", parking_seq=" << parking_seq
         << ", open_space_info_contract=path_provider_output"
         << ", path_info_id=" << parking_seq;
  AppendDestRegionContract(roi_output.dest_region, &stream);
  stream << ", smoothed=" << BoolText(path_output.has_smoothed)
         << ", history=generated"
         << ", replan=" << attempt.replan_text
         << ", reason=" << attempt.decision_reason
         << ", warm_start=" << attempt.warm_start_source
         << ", warm_start_reject="
         << attempt.warm_start_reject_reason
         << ", warm_start_points=" << attempt.warm_start_points
         << ", splice_points=" << attempt.splice_path_points
         << ", warm_start_history_points="
         << attempt.warm_start_history_points
         << ", warm_start_s=" << attempt.warm_start_match_s
         << ", warm_start_l=" << attempt.warm_start_match_l
         << ", warm_start_path_s=[" << attempt.warm_start_path_front_s
         << "," << attempt.warm_start_path_back_s << "]"
         << ", strategy_init_move="
         << attempt.strategy_init_moving_direction
         << ", strategy_init_path="
         << attempt.strategy_init_path_direction
         << ", strategy_kappa_cost="
         << BoolText(attempt.strategy_enable_init_kappa_cost)
         << ", strategy_limit_steer="
         << BoolText(attempt.strategy_limit_init_steer_margin)
         << ", strategy_disable_search="
         << BoolText(attempt.strategy_disable_search)
         << ", trace_adjust=" << BoolText(attempt.trace_adjust_enabled)
         << ", trace_adjust_source=" << attempt.trace_adjust_source
         << ", trace_adjust_reject="
         << attempt.trace_adjust_reject_reason
         << ", trace_adjust_points=" << attempt.trace_adjust_points
         << ", trace_adjust_path_length="
         << attempt.trace_adjust_path_length
         << ", trace_adjust_min_length="
         << attempt.trace_adjust_min_length
         << ", trace_adjust_target_s=" << attempt.trace_adjust_target_s
         << ", trace_adjust_finish_l="
         << attempt.trace_adjust_finish_l_threshold
         << ", trace_adjust_finish_theta="
         << attempt.trace_adjust_finish_theta_threshold
         << ", trace_adjust_bounds=";
  if (attempt.trace_adjust_enabled) {
    stream << "[-" << attempt.trace_adjust_bound
           << "," << attempt.trace_adjust_bound
           << ",-" << attempt.trace_adjust_bound
           << "," << attempt.trace_adjust_bound << "]";
  } else {
    stream << "none";
  }
  stream << ", generated_count=" << generated_count
         << ", reused_count=" << reused_count
         << ", preplan_candidates=" << preplan_candidate_count
         << ", external_vehicle=" << BoolText(attempt.external_vehicle)
         << ", external_obstacles=" << attempt.obstacle_count;
  AppendPlanningContextPathProjection(projection, "generated", &stream);
  AppendThreadedProviderDiagnostics(diagnostics, &stream);
  return stream.str();
}

std::string BuildPathPartitionForcedFailureReason() {
  std::ostringstream stream;
  stream << "PATH_PARTITION failed: forced_by_smoke_env";
  AppendOpenSpaceTaskContract("path_partition_output", &stream);
  return stream.str();
}

std::string BuildPathPartitionInsufficientReason(
    const TL::planning::PartitionOutput& partition_output) {
  std::ostringstream stream;
  stream << "PATH_PARTITION failed: insufficient chosen path points"
         << ", decision=" << static_cast<int>(partition_output.path_decision)
         << ", chosen_points="
         << partition_output.chosen_partitioned_path.first.size();
  return stream.str();
}

std::string BuildPathPartitionOutputReason(
    const TL::planning::PartitionOutput& partition_output,
    bool vehicle_has_state,
    std::size_t obstacle_count,
    const std::string& compact_debug) {
  const std::size_t chosen_points =
      partition_output.chosen_partitioned_path.first.size();
  std::ostringstream stream;
  stream << "PATH_PARTITION ok";
  AppendOpenSpaceTaskContract("path_partition_output", &stream);
  stream << ", decision=" << static_cast<int>(partition_output.path_decision)
         << ", decision_name="
         << PathDecisionToString(partition_output.path_decision)
         << ", finish=" << static_cast<int>(partition_output.finish_status)
         << ", finish_name="
         << FinishStatusToString(partition_output.finish_status)
         << ", destination_reached="
         << BoolText(partition_output.destination_reached)
         << ", chosen_idx=(" << partition_output.chosen_path_idx.first
         << "," << partition_output.chosen_path_idx.second << ")"
         << ", open_space_info_contract=path_partition_output"
         << ", partitioned_paths="
         << partition_output.partitioned_paths.path_set.size()
         << ", chosen_path_contract=chosen_partitioned_path"
         << ", chosen_points=" << chosen_points
         << ", chosen_path_points=" << chosen_points
         << ", gear="
         << static_cast<int>(partition_output.chosen_partitioned_path.second)
         << ", chosen_path_gear="
         << static_cast<int>(partition_output.chosen_partitioned_path.second)
         << ", gear_changed=" << BoolText(partition_output.is_gear_changed)
         << ", stop_path=" << BoolText(partition_output.is_stop_path)
         << ", external_vehicle=" << BoolText(vehicle_has_state)
         << ", external_obstacles=" << obstacle_count;
  if (!compact_debug.empty()) {
    stream << ", debug=" << compact_debug;
  }
  return stream.str();
}

std::string BuildSpeedOptimizerFailureReason(const std::string& message,
                                             bool is_rpa_direct_mode) {
  std::ostringstream stream;
  stream << "SPEED_OPTIMIZER failed: " << message;
  AppendSpeedOptimizerTaskContract(is_rpa_direct_mode, &stream);
  return stream.str();
}

std::string BuildSpeedOptimizerInsufficientReason(
    std::size_t trajectory_points,
    const std::string& message,
    bool is_rpa_direct_mode) {
  std::ostringstream stream;
  stream << "SPEED_OPTIMIZER failed: insufficient trajectory points"
         << ", points=" << trajectory_points
         << ", message=" << message;
  AppendSpeedOptimizerTaskContract(is_rpa_direct_mode, &stream);
  return stream.str();
}

std::string BuildSpeedOptimizerOutputReason(
    const TL::planning::PartitionOutput& partition_output,
    const TL::planning::SpeedOptimizerOutput& speed_output,
    bool is_rpa_direct_mode,
    bool has_last_frame,
    bool vehicle_has_state,
    std::size_t obstacle_count,
    const std::string& compact_message) {
  const auto& trajectory = speed_output.trajectory_gear.first;
  const std::size_t trajectory_points = trajectory.NumOfPoints();
  const double total_time =
      trajectory.TrajectoryPointAt(trajectory_points - 1).relative_time;
  const double total_s =
      trajectory.TrajectoryPointAt(trajectory_points - 1).path_point.s -
      trajectory.TrajectoryPointAt(0).path_point.s;

  std::ostringstream stream;
  stream << "SPEED_OPTIMIZER ok";
  AppendSpeedOptimizerTaskContract(is_rpa_direct_mode, &stream);
  stream << ", points=" << trajectory_points
         << ", duration=" << std::fixed << std::setprecision(3)
         << total_time
         << ", distance=" << total_s
         << ", gear=" << static_cast<int>(speed_output.trajectory_gear.second)
         << ", open_space_info_contract=speed_optimizer_output"
         << ", chosen_path_points="
         << partition_output.chosen_partitioned_path.first.size()
         << ", partitioned_paths="
         << partition_output.partitioned_paths.path_set.size()
         << ", stop_path=" << BoolText(partition_output.is_stop_path)
         << ", speed_optimizer_trajectory_points=" << trajectory_points
         << ", wheel_mask_considered=false"
         << ", stage=" << static_cast<int>(speed_output.interactive_stage)
         << ", stage_name="
         << TL::planning_internal::SpeedTaskInteractiveStage_Name(
                speed_output.interactive_stage)
         << ", last_frame=" << BoolText(has_last_frame)
         << ", external_vehicle=" << BoolText(vehicle_has_state)
         << ", external_obstacles=" << obstacle_count
         << ", message=" << compact_message;
  return stream.str();
}

std::string BuildOpenSpaceStraightPathFailureReason(
    const std::string& message) {
  std::ostringstream stream;
  stream << "OPEN_SPACE_STRAIGHT_PATH failed: " << message;
  AppendDirectTaskContract("open_space_straight_path_output", &stream);
  return stream.str();
}

std::string BuildOpenSpaceStraightPathInsufficientReason(
    std::size_t path_points,
    const std::string& diagnostic) {
  std::ostringstream stream;
  stream << "OPEN_SPACE_STRAIGHT_PATH failed: insufficient path points"
         << ", points=" << path_points
         << ", diagnostic=" << diagnostic;
  AppendDirectTaskContract("open_space_straight_path_output", &stream);
  return stream.str();
}

std::string BuildOpenSpaceStraightPathOutputReason(
    const TL::planning::OpenSpaceStraightPathOutput& straight_output,
    bool vehicle_has_state,
    double direct_speed) {
  std::ostringstream stream;
  stream << straight_output.diagnostic
         << ", gear_changed=" << BoolText(straight_output.is_gear_changed)
         << ", external_vehicle=" << BoolText(vehicle_has_state)
         << ", direct_speed=" << direct_speed;
  AppendDirectTaskContract("open_space_straight_path_output", &stream);
  return stream.str();
}

}  // namespace valet_parking
