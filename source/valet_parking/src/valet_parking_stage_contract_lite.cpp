#include "valet_parking_stage_contract_lite.h"

#include <sstream>

namespace valet_parking {

namespace {

const char* BoolText(bool value) {
  return value ? "true" : "false";
}

std::string StageFinishStateName(
    const StageFinishOutputContractLite& evaluation) {
  if (evaluation.ready_to_finish) {
    return "READY";
  }
  if (evaluation.ready_condition) {
    return "HOLDING";
  }
  return "WAITING";
}

std::string OriginalFlowBranchForTask(const std::string& task) {
  if (task == "OPEN_SPACE_STRAIGHT_PATH") {
    return "direct_open_space";
  }
  if (task == "STAGE_FINISH_HOLD") {
    return "stage_finish_hold";
  }
  if (task == "UNSUPPORTED_PARKING_OUT") {
    return "unsupported_parking_out";
  }
  return "stage_control_override";
}

std::string OriginalFlowBranchForFallback(
    const std::string& fallback_event) {
  if (fallback_event == "open_space_straight_path_failed" ||
      fallback_event == "direct_speed_optimizer_failed") {
    return "direct_open_space";
  }
  if (fallback_event == "path_provider_failed" ||
      fallback_event == "precheck_failed" ||
      fallback_event == "path_partition_failed" ||
      fallback_event == "speed_optimizer_failed") {
    return "normal_open_space_task";
  }
  return "normal_open_space_early_input";
}

void AppendRuntimeLifecycleContractImpl(
    const StageRuntimeLifecycleContractLite& runtime_lifecycle,
    std::ostringstream* stream) {
  if (stream == nullptr) {
    return;
  }
  *stream << ", runtime_lifecycle_contract="
          << "lightweight_stage_runtime_projection"
          << ", runtime_lifecycle_event=" << runtime_lifecycle.event
          << ", stage_exit_action="
          << runtime_lifecycle.stage_exit_action
          << ", path_history_available="
          << BoolText(runtime_lifecycle.path_history_available)
          << ", path_history_action="
          << runtime_lifecycle.path_history_action
          << ", speed_frame_available="
          << BoolText(runtime_lifecycle.speed_frame_available)
          << ", speed_frame_action="
          << runtime_lifecycle.speed_frame_action
          << ", direct_command_active="
          << BoolText(runtime_lifecycle.direct_command_active)
          << ", direct_state_action="
          << runtime_lifecycle.direct_state_action;
}

std::string BuildStageControlReason(const ParkingCommand& command,
                                    const std::string& action) {
  std::ostringstream stream;
  stream << "STAGE_CONTROL " << action;
  if (command.parking_seq() != 0U) {
    stream << ", parking_seq=" << command.parking_seq();
  }
  if (!command.reason().empty()) {
    stream << ", command_reason=" << command.reason();
  }
  return stream.str();
}

}  // namespace

void AppendRuntimeLifecycleContract(
    const StageRuntimeLifecycleContractLite& runtime_lifecycle,
    std::ostringstream* stream) {
  AppendRuntimeLifecycleContractImpl(runtime_lifecycle, stream);
}

std::string PathDecisionToString(
    TL::planning::OpenSpacePathDecision decision) {
  switch (decision) {
    case TL::planning::OpenSpacePathDecision::TASK_FINISH:
      return "TASK_FINISH";
    case TL::planning::OpenSpacePathDecision::TRACK_ABNORMAL:
      return "TRACK_ABNORMAL";
    case TL::planning::OpenSpacePathDecision::NO_VALID_PATH:
      return "NO_VALID_PATH";
    case TL::planning::OpenSpacePathDecision::CHOOSE_NEW_PATH:
      return "CHOOSE_NEW_PATH";
    case TL::planning::OpenSpacePathDecision::CHOOSE_HISTORY_PATH:
      return "CHOOSE_HISTORY_PATH";
    case TL::planning::OpenSpacePathDecision::PREPARE_FINISH:
      return "PREPARE_FINISH";
    case TL::planning::OpenSpacePathDecision::UNKOWN:
    default:
      return "UNKOWN";
  }
}

std::string FinishStatusToString(
    TL::planning_internal::OpenSpaceDebug::FinishStatus status) {
  using TL::planning_internal::OpenSpaceDebug;
  switch (status) {
    case OpenSpaceDebug::REACH_TARGET:
      return "REACH_TARGET";
    case OpenSpaceDebug::COLLISION_FINISH:
      return "COLLISION_FINISH";
    case OpenSpaceDebug::PREFINISH_BRAKING:
      return "PREFINISH_BRAKING";
    case OpenSpaceDebug::REACH_WHEEL_MASK:
      return "REACH_WHEEL_MASK";
    case OpenSpaceDebug::BLOCK_BY_CURB_IN_SPOT:
      return "BLOCK_BY_CURB_IN_SPOT";
    case OpenSpaceDebug::BLOCK_BY_CAR_IN_SPOT:
      return "BLOCK_BY_CAR_IN_SPOT";
    case OpenSpaceDebug::OVER_TIME:
      return "OVER_TIME";
    case OpenSpaceDebug::LARGE_ANGLE:
      return "LARGE_ANGLE";
    case OpenSpaceDebug::FAR_AWAY:
      return "FAR_AWAY";
    case OpenSpaceDebug::VEHICEL_MOVING:
      return "VEHICEL_MOVING";
    case OpenSpaceDebug::OUT_OF_PARK_LOT:
      return "OUT_OF_PARK_LOT";
    case OpenSpaceDebug::UNKNOWN:
    default:
      return "UNKNOWN";
  }
}

bool IsFinishSuccessStatus(
    TL::planning_internal::OpenSpaceDebug::FinishStatus status) {
  using TL::planning_internal::OpenSpaceDebug;
  switch (status) {
    case OpenSpaceDebug::REACH_TARGET:
    case OpenSpaceDebug::COLLISION_FINISH:
    case OpenSpaceDebug::REACH_WHEEL_MASK:
    case OpenSpaceDebug::BLOCK_BY_CURB_IN_SPOT:
    case OpenSpaceDebug::BLOCK_BY_CAR_IN_SPOT:
      return true;
    case OpenSpaceDebug::UNKNOWN:
    case OpenSpaceDebug::PREFINISH_BRAKING:
    case OpenSpaceDebug::OVER_TIME:
    case OpenSpaceDebug::LARGE_ANGLE:
    case OpenSpaceDebug::FAR_AWAY:
    case OpenSpaceDebug::VEHICEL_MOVING:
    case OpenSpaceDebug::OUT_OF_PARK_LOT:
    default:
      return false;
  }
}

std::string TrajectoryTypeName(PlanningTrajectoryType trajectory_type) {
  switch (trajectory_type) {
    case PlanningTrajectoryType::TRAJECTORY_TYPE_NORMAL:
      return "NORMAL";
    case PlanningTrajectoryType::TRAJECTORY_TYPE_PATH_FALLBACK:
      return "PATH_FALLBACK";
    case PlanningTrajectoryType::TRAJECTORY_TYPE_SPEED_FALLBACK:
      return "SPEED_FALLBACK";
    case PlanningTrajectoryType::TRAJECTORY_TYPE_PATH_REUSED:
      return "PATH_REUSED";
    case PlanningTrajectoryType::TRAJECTORY_TYPE_SHORT_PATH:
      return "SHORT_PATH";
    case PlanningTrajectoryType::TRAJECTORY_TYPE_PATH_LANE_KEEP:
      return "PATH_LANE_KEEP";
    case PlanningTrajectoryType::TRAJECTORY_TYPE_PATH_LANE_CHANGE:
      return "PATH_LANE_CHANGE";
    case PlanningTrajectoryType::TRAJECTORY_TYPE_PATH_PULL_OVER:
      return "PATH_PULL_OVER";
    case PlanningTrajectoryType::TRAJECTORY_TYPE_UNKNOWN:
    default:
      return "UNKNOWN";
  }
}

void AppendMissionStateContract(const std::string& mission_state,
                                const std::string& next_stage,
                                bool finish_scenario_intent,
                                std::ostringstream* stream) {
  if (stream == nullptr) {
    return;
  }
  *stream << ", mission_state_contract=lightweight_stage_projection"
          << ", mission_state=" << mission_state
          << ", next_stage=" << next_stage
          << ", finish_scenario_intent="
          << BoolText(finish_scenario_intent)
          << ", finish_scenario_contract=diagnostic_only";
}

void AppendStageProjectionContract(
    const std::string& record_type,
    std::ostringstream* stream,
    const StageProcessContextLite* stage_context) {
  if (stream == nullptr) {
    return;
  }
  *stream << ", stage_contract=lightweight_valet_parking_stage_projection"
          << ", stage_contract_record=" << record_type
          << ", status_transport=replan_reason_text"
          << ", dds_field_extension=required_before_vehicle_integration"
          << ", original_flow_reference="
          << "ValetParkingStageParking.Process>Stage.ExecuteTaskOnOpenSpace"
          << ", original_flow_contract=lightweight_node_projection";
  if (stage_context != nullptr) {
    AppendStageProcessContract(*stage_context, stream);
  }
}

void AppendFunctionManagerProjectionContract(
    const StageFunctionManagerProjectionContractLite& projection,
    std::ostringstream* stream) {
  if (stream == nullptr) {
    return;
  }
  *stream << ", function_manager_source=" << projection.source
          << ", function_manager_sys_mode=" << projection.sys_mode
          << ", function_manager_sys_command=" << projection.sys_command
          << ", function_manager_sys_run_state="
          << projection.sys_run_state
          << ", function_manager_sys_warning_info="
          << projection.sys_warning_info
          << ", function_manager_parking_type="
          << projection.parking_type
          << ", function_manager_command="
          << projection.parking_command
          << ", function_manager_reset_history="
          << BoolText(projection.reset_history);
}

std::string BuildFunctionManagerProjectionReason(
    const StageFunctionManagerProjectionContractLite& projection) {
  std::ostringstream stream;
  stream << "FUNCTION_MANAGER_INPUT";
  AppendFunctionManagerProjectionContract(projection, &stream);
  return stream.str();
}

std::string StageParkingStatus(
    const TL::planning::PartitionOutput& partition_output,
    const TL::planning::SpeedOptimizerOutput* speed_output,
    const StageFinishOutputContractLite* finish_evaluation) {
  if (finish_evaluation != nullptr &&
      finish_evaluation->ready_to_finish) {
    return "mission_finished";
  }
  if (partition_output.finish_status ==
      TL::planning_internal::OpenSpaceDebug::PREFINISH_BRAKING) {
    return "prepare_finish";
  }
  if (speed_output != nullptr) {
    switch (speed_output->interactive_stage) {
      case TL::planning_internal::AvpSpeedPlanCollisionInfo::WAITOBSTACLE:
        return "wait_obstacle";
      case TL::planning_internal::AvpSpeedPlanCollisionInfo::WAITREPLAN:
        return "wait_replan";
      case TL::planning_internal::AvpSpeedPlanCollisionInfo::RUNNING:
      case TL::planning_internal::AvpSpeedPlanCollisionInfo::INIT:
      default:
        break;
    }
  }
  if (partition_output.is_stop_path) {
    return "stop_by_path_partition";
  }
  return "running";
}

std::string StageStatusFromParkingStatus(const std::string& parking_status) {
  if (parking_status == "mission_finished") {
    return "mission_finished";
  }
  if (parking_status == "wait_obstacle") {
    return "waiting_obstacle";
  }
  if (parking_status == "wait_replan") {
    return "waiting_replan";
  }
  if (parking_status == "prepare_finish") {
    return "prepare_finish";
  }
  if (parking_status == "stop_by_path_partition") {
    return "stopped_by_path_partition";
  }
  return "running";
}

std::string MissionStateFromParkingStatus(const std::string& parking_status) {
  if (parking_status == "mission_finished") {
    return "MISSION_FINISHED";
  }
  if (parking_status == "wait_obstacle") {
    return "WAIT_OBSTACLE";
  }
  if (parking_status == "wait_replan") {
    return "WAIT_REPLAN";
  }
  if (parking_status == "prepare_finish") {
    return "PREPARE_FINISH";
  }
  if (parking_status == "stop_by_path_partition") {
    return "STOP_BY_PATH_PARTITION";
  }
  return "MISSION_RUNNING";
}

PlanningTrajectoryType StageTrajectoryType(
    const TL::planning::PartitionOutput& partition_output,
    const TL::planning::SpeedOptimizerOutput* speed_output,
    const StageFinishOutputContractLite* finish_evaluation) {
  if ((finish_evaluation != nullptr &&
       finish_evaluation->ready_to_finish) ||
      partition_output.destination_reached ||
      IsFinishSuccessStatus(partition_output.finish_status) ||
      partition_output.is_stop_path) {
    return PlanningTrajectoryType::TRAJECTORY_TYPE_SHORT_PATH;
  }
  return speed_output == nullptr
             ? PlanningTrajectoryType::TRAJECTORY_TYPE_SPEED_FALLBACK
             : PlanningTrajectoryType::TRAJECTORY_TYPE_NORMAL;
}

std::string BuildOpenSpaceStageOutputContract(
    const TL::planning::PartitionOutput& partition_output,
    const TL::planning::SpeedOptimizerOutput* speed_output,
    const StageFinishOutputContractLite* finish_evaluation,
    const StageFunctionManagerProjectionContractLite* function_projection,
    const StageRuntimeLifecycleContractLite& runtime_lifecycle,
    const StageProcessContextLite* stage_context) {
  const std::string parking_status =
      StageParkingStatus(partition_output, speed_output, finish_evaluation);
  const PlanningTrajectoryType trajectory_type =
      StageTrajectoryType(partition_output, speed_output, finish_evaluation);
  const TL::soc::GearPosition target_gear =
      speed_output == nullptr ? partition_output.chosen_partitioned_path.second
                              : speed_output->trajectory_gear.second;

  std::ostringstream stream;
  stream << "STAGE_OUTPUT open_space";
  AppendStageProjectionContract("open_space_output", &stream,
                                stage_context);
  stream << ", original_flow_branch=normal_open_space"
         << ", stage_status=" << StageStatusFromParkingStatus(parking_status)
         << ", task_chain=ROI_DECIDER>PATH_PROVIDER>PATH_PARTITION";
  if (speed_output != nullptr) {
    stream << ">SPEED_OPTIMIZER";
  } else {
    stream << ", speed_optimizer=fallback";
  }
  stream << ", path_decision="
         << PathDecisionToString(partition_output.path_decision)
         << ", finish_status="
         << FinishStatusToString(partition_output.finish_status)
         << ", destination_reached="
         << BoolText(partition_output.destination_reached)
         << ", target_gear=" << static_cast<int>(target_gear)
         << ", trajectory_type=" << TrajectoryTypeName(trajectory_type)
         << ", parking_status=" << parking_status;
  const bool finish_scenario_intent =
      finish_evaluation != nullptr && finish_evaluation->ready_to_finish;
  AppendMissionStateContract(
      MissionStateFromParkingStatus(parking_status),
      finish_scenario_intent ? "FINISH" : "PARKING",
      finish_scenario_intent, &stream);
  if (finish_evaluation != nullptr) {
    stream << ", finish_condition=destination_reached_and_standstill"
           << ", finish_ready="
           << BoolText(finish_evaluation->ready_to_finish)
           << ", finish_ready_condition="
           << BoolText(finish_evaluation->ready_condition)
           << ", finish_consecutive_frames="
           << finish_evaluation->consecutive_ready_frames
           << ", finish_required_frames="
           << finish_evaluation->required_consecutive_frames
           << ", vehicle_standstill="
           << BoolText(finish_evaluation->vehicle_standstill)
           << ", stage_finish_state="
           << StageFinishStateName(*finish_evaluation);
  }
  if (function_projection != nullptr) {
    AppendFunctionManagerProjectionContract(*function_projection, &stream);
  }
  AppendRuntimeLifecycleContractImpl(runtime_lifecycle, &stream);
  if (speed_output != nullptr) {
    stream << ", speed_interactive_stage="
           << TL::planning_internal::SpeedTaskInteractiveStage_Name(
                  speed_output->interactive_stage);
  }
  stream << ", finish_priority=finish_over_interactive";
  return stream.str();
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
    const StageFunctionManagerProjectionContractLite* function_projection,
    const StageRuntimeLifecycleContractLite& runtime_lifecycle,
    const StageProcessContextLite* stage_context) {
  std::ostringstream stream;
  stream << "STAGE_OUTPUT fallback";
  AppendStageProjectionContract("fallback_output", &stream, stage_context);
  stream << ", fallback_event=" << fallback_event
         << ", fallback_action=" << fallback_action
         << ", original_flow_branch="
         << OriginalFlowBranchForFallback(fallback_event)
         << ", stage_status=" << stage_status
         << ", trajectory_type=" << TrajectoryTypeName(trajectory_type)
         << ", parking_status=" << parking_status;
  AppendMissionStateContract(mission_state, next_stage,
                             finish_scenario_intent, &stream);
  if (function_projection != nullptr) {
    AppendFunctionManagerProjectionContract(*function_projection, &stream);
  }
  AppendRuntimeLifecycleContractImpl(runtime_lifecycle, &stream);
  return stream.str();
}

std::string BuildEarlyEstopFallbackContract(
    const std::string& fallback_event,
    const std::string& parking_status,
    const StageFunctionManagerProjectionContractLite& function_projection,
    const StageRuntimeLifecycleContractLite& runtime_lifecycle,
    const StageProcessContextLite* stage_context) {
  return BuildFallbackStageOutputContract(
      fallback_event, "publish_estop", "fallback_estop", parking_status,
      PlanningTrajectoryType::TRAJECTORY_TYPE_UNKNOWN, "MISSION_ESTOP",
      "PARKING", false, &function_projection, runtime_lifecycle,
      stage_context);
}

void AppendStageControlContract(
    const ParkingCommand& command,
    const std::string& action,
    const std::string& stage_status,
    const std::string& task,
    std::ostringstream* stream,
    const StageProcessContextLite* stage_context) {
  if (stream == nullptr) {
    return;
  }
  *stream << BuildStageControlReason(command, action)
          << ", command_action=" << action
          << ", stage_status=" << stage_status
          << ", skip=ROI_PATH_PROVIDER_PATH_PARTITION"
          << ", task=" << task
          << ", original_flow_branch=" << OriginalFlowBranchForTask(task)
          << ", reset_history=" << BoolText(command.reset_history());
  AppendStageProjectionContract("stage_control", stream, stage_context);
}

}  // namespace valet_parking
