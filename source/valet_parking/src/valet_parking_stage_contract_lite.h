#pragma once

#include "planning/tasks/optimizers/open_space_path_partition/open_space_path_partition.h"
#include "planning/tasks/optimizers/open_space_speed_optimizer/open_space_speed_optimizer.h"
#include "valet_parking_stage_process_lite.h"
#include "valet_parking_topics.h"

#include <cstdint>
#include <iosfwd>
#include <string>

namespace valet_parking {

struct StageFunctionManagerProjectionContractLite {
  std::string source{"selected_slot"};
  std::string parking_command{"NONE"};
  std::string sys_mode{"RPA"};
  std::string sys_command{"PARKINCONTROL"};
  std::string sys_run_state{"PARKSTART"};
  std::string sys_warning_info{"NO_WARNING"};
  std::string parking_type{"PARKING_IN"};
  bool reset_history{false};
};

struct StageRuntimeLifecycleContractLite {
  std::string event{"normal_open_space"};
  std::string stage_exit_action{"continue_parking"};
  std::string path_history_action{"keep_for_reuse"};
  std::string speed_frame_action{"keep_for_speed_warm_start"};
  std::string direct_state_action{"already_clear"};
  bool path_history_available{false};
  bool speed_frame_available{false};
  bool direct_command_active{false};
};

struct StageFinishOutputContractLite {
  bool destination_reached{false};
  bool vehicle_standstill{false};
  bool ready_condition{false};
  bool ready_to_finish{false};
  uint32_t consecutive_ready_frames{0U};
  uint32_t required_consecutive_frames{0U};
};

std::string PathDecisionToString(
    TL::planning::OpenSpacePathDecision decision);
std::string FinishStatusToString(
    TL::planning_internal::OpenSpaceDebug::FinishStatus status);
bool IsFinishSuccessStatus(
    TL::planning_internal::OpenSpaceDebug::FinishStatus status);
std::string TrajectoryTypeName(PlanningTrajectoryType trajectory_type);

void AppendMissionStateContract(const std::string& mission_state,
                                const std::string& next_stage,
                                bool finish_scenario_intent,
                                std::ostringstream* stream);
void AppendStageProjectionContract(
    const std::string& record_type,
    std::ostringstream* stream,
    const StageProcessContextLite* stage_context = nullptr);
void AppendFunctionManagerProjectionContract(
    const StageFunctionManagerProjectionContractLite& projection,
    std::ostringstream* stream);
void AppendRuntimeLifecycleContract(
    const StageRuntimeLifecycleContractLite& runtime_lifecycle,
    std::ostringstream* stream);
std::string BuildFunctionManagerProjectionReason(
    const StageFunctionManagerProjectionContractLite& projection);

std::string StageParkingStatus(
    const TL::planning::PartitionOutput& partition_output,
    const TL::planning::SpeedOptimizerOutput* speed_output,
    const StageFinishOutputContractLite* finish_evaluation);
std::string StageStatusFromParkingStatus(const std::string& parking_status);
std::string MissionStateFromParkingStatus(const std::string& parking_status);
PlanningTrajectoryType StageTrajectoryType(
    const TL::planning::PartitionOutput& partition_output,
    const TL::planning::SpeedOptimizerOutput* speed_output,
    const StageFinishOutputContractLite* finish_evaluation);

std::string BuildOpenSpaceStageOutputContract(
    const TL::planning::PartitionOutput& partition_output,
    const TL::planning::SpeedOptimizerOutput* speed_output,
    const StageFinishOutputContractLite* finish_evaluation,
    const StageFunctionManagerProjectionContractLite* function_projection,
    const StageRuntimeLifecycleContractLite& runtime_lifecycle,
    const StageProcessContextLite* stage_context = nullptr);

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
    const StageProcessContextLite* stage_context = nullptr);

std::string BuildEarlyEstopFallbackContract(
    const std::string& fallback_event,
    const std::string& parking_status,
    const StageFunctionManagerProjectionContractLite& function_projection,
    const StageRuntimeLifecycleContractLite& runtime_lifecycle,
    const StageProcessContextLite* stage_context = nullptr);

void AppendStageControlContract(
    const ParkingCommand& command,
    const std::string& action,
    const std::string& stage_status,
    const std::string& task,
    std::ostringstream* stream,
    const StageProcessContextLite* stage_context = nullptr);

}  // namespace valet_parking
