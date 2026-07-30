#include "valet_parking_stage_runtime_lite.h"

#include <sstream>

namespace valet_parking {

namespace {

bool IsStageControlOverrideCommandMode(ParkingCommandMode mode) {
  switch (mode) {
    case ParkingCommandMode::PARKING_COMMAND_PAUSE:
    case ParkingCommandMode::PARKING_COMMAND_BRAKE:
    case ParkingCommandMode::PARKING_COMMAND_FINISH:
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_LEFT:
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_RIGHT:
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_FRONT:
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_BACK:
      return true;
    case ParkingCommandMode::PARKING_COMMAND_NONE:
    case ParkingCommandMode::PARKING_COMMAND_PARKING_IN:
    case ParkingCommandMode::PARKING_COMMAND_DIRECT_FORWARD:
    case ParkingCommandMode::PARKING_COMMAND_DIRECT_BACKWARD:
      return false;
  }
  return false;
}

StageFacadeBranchLite StageFacadeBranchForInput(
    const ParkingCommand* command_sample,
    bool stage_exit_requested) {
  const bool has_command =
      command_sample != nullptr && command_sample->is_valid();
  if (has_command && IsDirectCommandMode(command_sample->mode())) {
    return StageFacadeBranchLite::kDirectOpenSpace;
  }
  if (has_command &&
      IsStageControlOverrideCommandMode(command_sample->mode())) {
    return StageFacadeBranchLite::kStageControlOverride;
  }
  if (!has_command && stage_exit_requested) {
    return StageFacadeBranchLite::kStageFinishHold;
  }
  return StageFacadeBranchLite::kNormalOpenSpace;
}

}  // namespace

StageFacadeInputLite BuildStageFacadeInputLite(
    const ParkingCommand* command_sample,
    uint64_t seq,
    const std::string& frame_id,
    uint64_t data_stamp_ms,
    bool selected_slot_valid,
    uint32_t selected_slot_count,
    std::size_t parking_lot_count,
    uint32_t selected_parking_seq,
    const FunctionManagerProjection& function_projection,
    bool vehicle_state_valid,
    std::size_t obstacle_count,
    bool path_history_available,
    int path_id,
    bool has_last_speed_frame,
    bool stage_exit_requested,
    bool direct_command_active,
    uint64_t processed_frames) {
  StageFacadeInputLite input;
  input.branch = StageFacadeBranchForInput(command_sample,
                                           stage_exit_requested);
  input.frame.seq = seq;
  input.frame.frame_id = frame_id;
  input.frame.data_stamp_ms = data_stamp_ms;
  input.frame.selected_slot_valid = selected_slot_valid;
  input.frame.selected_slot_count = selected_slot_count;
  input.frame.parking_lot_count = parking_lot_count;
  input.frame.selected_parking_seq = selected_parking_seq;
  input.frame.vehicle_state_valid = vehicle_state_valid;
  input.frame.vehicle_state_source =
      vehicle_state_valid ? "dds_aux_vehicle" : "config_fake_vehicle";
  input.frame.obstacle_count = obstacle_count;

  input.open_space_info.parking_type = function_projection.parking_type;
  input.open_space_info.target_parking_seq = selected_parking_seq;

  input.planning_context.path_history_available = path_history_available;
  input.planning_context.path_id = path_id;
  input.planning_context.speed_frame_available = has_last_speed_frame;
  input.planning_context.stage_exit_requested = stage_exit_requested;
  input.planning_context.direct_command_active = direct_command_active;
  input.planning_context.processed_frames = processed_frames;

  input.function_manager =
      BuildStageFunctionManagerLite(function_projection);

  const bool has_command =
      command_sample != nullptr && command_sample->is_valid();
  input.finish_requested =
      has_command &&
      command_sample->mode() == ParkingCommandMode::PARKING_COMMAND_FINISH;
  return input;
}

StageRuntimeLifecycleContractLite BuildStageRuntimeLifecycleContractLite(
    bool path_history_available,
    bool has_last_speed_frame,
    bool direct_command_active,
    const std::string& runtime_lifecycle_event,
    const std::string& stage_exit_action,
    const std::string& path_history_action,
    const std::string& speed_frame_action,
    const std::string& direct_state_action) {
  StageRuntimeLifecycleContractLite lite;
  lite.event = runtime_lifecycle_event;
  lite.stage_exit_action = stage_exit_action;
  lite.path_history_action = path_history_action;
  lite.speed_frame_action = speed_frame_action;
  lite.direct_state_action = direct_state_action;
  lite.path_history_available = path_history_available;
  lite.speed_frame_available = has_last_speed_frame;
  lite.direct_command_active = direct_command_active;
  return lite;
}

void AppendRuntimeLifecycleContract(const std::string& event,
                                    const std::string& stage_exit_action,
                                    const std::string& path_history_action,
                                    const std::string& speed_frame_action,
                                    const std::string& direct_state_action,
                                    bool path_history_available,
                                    bool speed_frame_available,
                                    bool direct_command_active,
                                    std::ostringstream* stream) {
  AppendRuntimeLifecycleContract(
      BuildStageRuntimeLifecycleContractLite(
          path_history_available, speed_frame_available,
          direct_command_active, event, stage_exit_action,
          path_history_action, speed_frame_action, direct_state_action),
      stream);
}

std::string BuildOpenSpaceStageOutputContract(
    const TL::planning::PartitionOutput& partition_output,
    const TL::planning::SpeedOptimizerOutput* speed_output,
    const StageFinishEvaluation* finish_evaluation,
    const FunctionManagerProjection* function_projection,
    bool path_history_available,
    bool has_last_speed_frame,
    bool direct_command_active,
    const StageProcessContextLite* stage_context) {
  StageFinishOutputContractLite finish_lite;
  const StageFinishOutputContractLite* finish_lite_ptr = nullptr;
  if (finish_evaluation != nullptr) {
    finish_lite = BuildStageFinishOutputContractLite(*finish_evaluation);
    finish_lite_ptr = &finish_lite;
  }

  StageFunctionManagerProjectionContractLite function_lite;
  const StageFunctionManagerProjectionContractLite* function_lite_ptr =
      nullptr;
  if (function_projection != nullptr) {
    function_lite =
        BuildFunctionManagerProjectionContractLite(*function_projection);
    function_lite_ptr = &function_lite;
  }

  const bool finish_ready =
      finish_evaluation != nullptr && finish_evaluation->ready_to_finish;
  const StageRuntimeLifecycleContractLite runtime_lifecycle =
      BuildStageRuntimeLifecycleContractLite(
          path_history_available, has_last_speed_frame,
          direct_command_active,
          finish_ready ? "stage_finish_ready" : "normal_open_space",
          finish_ready ? "latch_finish_hold_after_publish"
                       : "continue_parking",
          finish_ready ? "keep_until_stage_reset" : "keep_for_reuse",
          finish_ready ? "keep_until_stage_reset"
                       : "keep_for_speed_warm_start",
          direct_command_active ? "unexpected_active" : "already_clear");
  return BuildOpenSpaceStageOutputContract(
      partition_output, speed_output, finish_lite_ptr, function_lite_ptr,
      runtime_lifecycle, stage_context);
}

std::string BuildFallbackStageOutputContract(
    const std::string& fallback_event,
    const std::string& fallback_action,
    const std::string& stage_status,
    const std::string& parking_status,
    PlanningTrajectoryType trajectory_type,
    const std::string& mission_state,
    const std::string& next_stage,
    bool finish_scenario_intent,
    const FunctionManagerProjection* function_projection,
    bool path_history_available,
    bool has_last_speed_frame,
    bool direct_command_active,
    const std::string& runtime_lifecycle_event,
    const std::string& stage_exit_action,
    const std::string& path_history_action,
    const std::string& speed_frame_action,
    const std::string& direct_state_action,
    const StageProcessContextLite* stage_context) {
  StageFunctionManagerProjectionContractLite function_lite;
  const StageFunctionManagerProjectionContractLite* function_lite_ptr =
      nullptr;
  if (function_projection != nullptr) {
    function_lite =
        BuildFunctionManagerProjectionContractLite(*function_projection);
    function_lite_ptr = &function_lite;
  }
  const StageRuntimeLifecycleContractLite runtime_lifecycle =
      BuildStageRuntimeLifecycleContractLite(
          path_history_available, has_last_speed_frame,
          direct_command_active, runtime_lifecycle_event,
          stage_exit_action, path_history_action, speed_frame_action,
          direct_state_action);
  return BuildFallbackStageOutputContract(
      fallback_event, fallback_action, stage_status, parking_status,
      trajectory_type, mission_state, next_stage, finish_scenario_intent,
      function_lite_ptr, runtime_lifecycle, stage_context);
}

std::string BuildEarlyEstopFallbackContract(
    const std::string& fallback_event,
    const std::string& parking_status,
    const FunctionManagerProjection& function_projection,
    bool path_history_available,
    bool has_last_speed_frame,
    bool direct_command_active,
    const StageProcessContextLite* stage_context) {
  const StageRuntimeLifecycleContractLite runtime_lifecycle =
      BuildStageRuntimeLifecycleContractLite(
          path_history_available, has_last_speed_frame,
          direct_command_active, fallback_event + "_fallback",
          "stay_in_parking_stage", "reset_on_early_failure",
          "reset_on_early_failure",
          direct_command_active ? "reset_failed_direct_command"
                                : "already_clear");
  return BuildEarlyEstopFallbackContract(
      fallback_event, parking_status,
      BuildFunctionManagerProjectionContractLite(function_projection),
      runtime_lifecycle, stage_context);
}

}  // namespace valet_parking
