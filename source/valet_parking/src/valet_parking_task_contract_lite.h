#pragma once

#include "planning/tasks/deciders/open_space_decider/open_space_roi_decider.h"
#include "planning/tasks/optimizers/open_space_path_generation/open_space_path_provider.h"
#include "planning/tasks/optimizers/open_space_path_partition/open_space_path_partition.h"
#include "planning/tasks/optimizers/open_space_speed_optimizer/open_space_speed_optimizer.h"
#include "planning/tasks/optimizers/open_space_straight_path/open_space_straight_path_provider.h"

#include <cstdint>
#include <cstddef>
#include <iosfwd>
#include <string>

namespace valet_parking {

struct PathProviderPreCheckResult {
  bool ok{true};
  std::string reason{"accepted"};
  std::size_t obstacle_segment_count{0U};
  std::size_t roi_obstacle_segment_count{0U};
  std::size_t external_obstacle_segment_count{0U};
  std::size_t obstacle_near_start_count{0U};
  std::size_t obstacle_near_end_count{0U};
  std::size_t dest_region_point_count{0U};
  double xy_width{0.0};
  double xy_height{0.0};
  bool vehicle_has_state{false};
};

struct PathProviderAttemptContractLite {
  std::string replan_text;
  std::string decision_reason;
  std::string warm_start_source;
  std::string warm_start_reject_reason;
  std::size_t warm_start_points{0U};
  std::size_t splice_path_points{0U};
  std::size_t warm_start_history_points{0U};
  double warm_start_match_s{0.0};
  double warm_start_match_l{0.0};
  double warm_start_path_front_s{0.0};
  double warm_start_path_back_s{0.0};
  int strategy_init_moving_direction{0};
  int strategy_init_path_direction{0};
  bool strategy_enable_init_kappa_cost{false};
  bool strategy_limit_init_steer_margin{false};
  bool strategy_disable_search{false};
  bool trace_adjust_enabled{false};
  std::string trace_adjust_source;
  std::string trace_adjust_reject_reason;
  std::size_t trace_adjust_points{0U};
  double trace_adjust_path_length{0.0};
  double trace_adjust_min_length{0.0};
  double trace_adjust_target_s{0.0};
  double trace_adjust_finish_l_threshold{0.0};
  double trace_adjust_finish_theta_threshold{0.0};
  double trace_adjust_bound{0.0};
  bool external_vehicle{false};
  std::size_t obstacle_count{0U};
};

struct PathProviderHistoryContractLite {
  std::string replan_text;
  std::size_t last_warm_start_points{0U};
  int last_strategy_init_moving_direction{0};
  uint64_t generated_count{0U};
  uint64_t reused_count{0U};
  bool external_vehicle{false};
  std::size_t obstacle_count{0U};
};

struct PlanningContextPathProjectionLite {
  bool has_history{false};
  int path_id{-1};
  std::string replan_text;
  std::size_t history_points{0U};
};

std::size_t DestRegionPointCount(
    const TL::planning::DestRegionWithAng& dest_region);
double DestRegionArea(const TL::planning::DestRegionWithAng& dest_region);
std::size_t CountPathProviderOutputPoints(
    const TL::planning::OpenSpacePathOutput& output);

void AppendDestRegionContract(
    const TL::planning::DestRegionWithAng& dest_region,
    std::ostringstream* stream);
void AppendRoiOpenSpaceInfoContract(
    uint32_t path_info_id,
    const TL::planning::RoiDeciderOutput& roi_output,
    std::ostringstream* stream);
void AppendOpenSpaceTaskContract(const std::string& record_type,
                                 std::ostringstream* stream);
void AppendDirectTaskContract(const std::string& record_type,
                              std::ostringstream* stream);
void AppendSpeedOptimizerTaskContract(bool is_rpa_direct_mode,
                                      std::ostringstream* stream);
void AppendThreadedProviderDiagnostics(
    const TL::planning::OpenSpacePathProviderDiagnostics& diagnostics,
    std::ostringstream* stream);
void AppendPathProviderAttemptDiagnostics(
    const PathProviderAttemptContractLite& attempt,
    std::ostringstream* stream);
void AppendPlanningContextPathProjection(
    const PlanningContextPathProjectionLite& projection,
    const std::string& history_state,
    std::ostringstream* stream);

std::string BuildRoiReason(const TL::planning::RoiDeciderOutput& output,
                           uint32_t path_info_id);
std::string BuildPathProviderPreCheckReason(
    const PathProviderPreCheckResult& result);
std::string BuildPathProviderReuseReason(
    const TL::planning::OpenSpacePathOutput& path_output,
    uint32_t parking_seq,
    const TL::planning::RoiDeciderOutput& roi_output,
    const PathProviderHistoryContractLite& history,
    const PlanningContextPathProjectionLite& projection);
std::string BuildPathProviderFailureReason(
    const std::string& message,
    const PathProviderAttemptContractLite& attempt,
    std::size_t preplan_candidate_count,
    const TL::planning::OpenSpacePathProviderDiagnostics* diagnostics);
std::string BuildPathProviderOutputFailureReason(
    const std::string& message,
    const TL::planning::OpenSpacePathOutput& path_output,
    const PathProviderAttemptContractLite& attempt,
    std::size_t preplan_candidate_count,
    const TL::planning::OpenSpacePathProviderDiagnostics& diagnostics);
std::string BuildPathProviderGeneratedReason(
    const TL::planning::OpenSpacePathOutput& path_output,
    uint32_t parking_seq,
    const TL::planning::RoiDeciderOutput& roi_output,
    const PathProviderAttemptContractLite& attempt,
    uint64_t generated_count,
    uint64_t reused_count,
    std::size_t preplan_candidate_count,
    const PlanningContextPathProjectionLite& projection,
    const TL::planning::OpenSpacePathProviderDiagnostics& diagnostics);

std::string BuildPathPartitionForcedFailureReason();
std::string BuildPathPartitionInsufficientReason(
    const TL::planning::PartitionOutput& partition_output);
std::string BuildPathPartitionOutputReason(
    const TL::planning::PartitionOutput& partition_output,
    bool vehicle_has_state,
    std::size_t obstacle_count,
    const std::string& compact_debug);

std::string BuildSpeedOptimizerFailureReason(const std::string& message,
                                             bool is_rpa_direct_mode);
std::string BuildSpeedOptimizerInsufficientReason(
    std::size_t trajectory_points,
    const std::string& message,
    bool is_rpa_direct_mode);
std::string BuildSpeedOptimizerOutputReason(
    const TL::planning::PartitionOutput& partition_output,
    const TL::planning::SpeedOptimizerOutput& speed_output,
    bool is_rpa_direct_mode,
    bool has_last_frame,
    bool vehicle_has_state,
    std::size_t obstacle_count,
    const std::string& compact_message);

std::string BuildOpenSpaceStraightPathFailureReason(
    const std::string& message);
std::string BuildOpenSpaceStraightPathInsufficientReason(
    std::size_t path_points,
    const std::string& diagnostic);
std::string BuildOpenSpaceStraightPathOutputReason(
    const TL::planning::OpenSpaceStraightPathOutput& straight_output,
    bool vehicle_has_state,
    double direct_speed);

}  // namespace valet_parking
