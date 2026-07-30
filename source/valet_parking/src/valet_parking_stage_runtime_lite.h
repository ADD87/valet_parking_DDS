#pragma once

#include "valet_parking_function_manager_lite.h"
#include "valet_parking_stage_contract_lite.h"
#include "valet_parking_stage_facade_lite.h"
#include "valet_parking_stage_finish_lite.h"

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <string>

namespace valet_parking {

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
    uint64_t processed_frames);

StageRuntimeLifecycleContractLite BuildStageRuntimeLifecycleContractLite(
    bool path_history_available,
    bool has_last_speed_frame,
    bool direct_command_active,
    const std::string& runtime_lifecycle_event,
    const std::string& stage_exit_action,
    const std::string& path_history_action,
    const std::string& speed_frame_action,
    const std::string& direct_state_action);

void AppendRuntimeLifecycleContract(const std::string& event,
                                    const std::string& stage_exit_action,
                                    const std::string& path_history_action,
                                    const std::string& speed_frame_action,
                                    const std::string& direct_state_action,
                                    bool path_history_available,
                                    bool speed_frame_available,
                                    bool direct_command_active,
                                    std::ostringstream* stream);

std::string BuildOpenSpaceStageOutputContract(
    const TL::planning::PartitionOutput& partition_output,
    const TL::planning::SpeedOptimizerOutput* speed_output,
    const StageFinishEvaluation* finish_evaluation,
    const FunctionManagerProjection* function_projection,
    bool path_history_available,
    bool has_last_speed_frame,
    bool direct_command_active,
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
    const FunctionManagerProjection* function_projection,
    bool path_history_available,
    bool has_last_speed_frame,
    bool direct_command_active,
    const std::string& runtime_lifecycle_event,
    const std::string& stage_exit_action,
    const std::string& path_history_action,
    const std::string& speed_frame_action,
    const std::string& direct_state_action,
    const StageProcessContextLite* stage_context = nullptr);

std::string BuildEarlyEstopFallbackContract(
    const std::string& fallback_event,
    const std::string& parking_status,
    const FunctionManagerProjection& function_projection,
    bool path_history_available,
    bool has_last_speed_frame,
    bool direct_command_active,
    const StageProcessContextLite* stage_context = nullptr);

}  // namespace valet_parking
