#include "valet_parking_stage_process_lite.h"

#include <sstream>

namespace valet_parking {

namespace {

const char* BoolText(bool value) {
  return value ? "true" : "false";
}

}  // namespace

StageProcessContextLite ValetParkingStageParkingStageLite::Process(
    const StageProcessInputLite& input) const {
  StageProcessContextLite context;
  context.record_type = input.record_type;
  context.branch = input.branch;
  context.task_entry = input.task_entry;
  context.task_chain = input.task_chain;
  context.process_called = true;
  context.frame = input.frame;
  context.open_space_info = input.open_space_info;
  context.planning_context = input.planning_context;
  context.function_manager = input.function_manager;
  context.hmi_state = input.hmi_state;
  context.collision = input.collision;
  context.wheel_mask = input.wheel_mask;

  SetParkingType(input, &context);
  ExecuteTaskOnOpenSpace(input, &context);
  IsReadyToFinishStage(input, &context);
  FinishScenario(input, &context);
  return context;
}

void ValetParkingStageParkingStageLite::SetParkingType(
    const StageProcessInputLite& input,
    StageProcessContextLite* context) const {
  if (context == nullptr) {
    return;
  }
  context->set_parking_type_called = true;
  context->open_space_info.parking_type = input.function_manager.parking_type;
}

void ValetParkingStageParkingStageLite::ExecuteTaskOnOpenSpace(
    const StageProcessInputLite& input,
    StageProcessContextLite* context) const {
  if (context == nullptr) {
    return;
  }
  context->execute_task_on_open_space_called =
      input.execute_task_on_open_space;
  context->branch = input.branch;
  context->task_entry = input.task_entry;
  context->task_chain = input.task_chain;
}

void ValetParkingStageParkingStageLite::IsReadyToFinishStage(
    const StageProcessInputLite& input,
    StageProcessContextLite* context) const {
  if (context == nullptr) {
    return;
  }
  context->is_ready_to_finish_stage_called =
      input.is_ready_to_finish_stage_called;
  context->planning_context.stage_exit_requested =
      input.planning_context.stage_exit_requested || input.ready_to_finish;
}

void ValetParkingStageParkingStageLite::FinishScenario(
    const StageProcessInputLite& input,
    StageProcessContextLite* context) const {
  if (context == nullptr) {
    return;
  }
  context->finish_scenario_projected = input.finish_scenario_intent;
  context->next_stage = input.finish_scenario_intent ? "FINISH" : "PARKING";
}

void AppendStageProcessContract(const StageProcessContextLite& context,
                                std::ostringstream* stream) {
  if (stream == nullptr) {
    return;
  }

  *stream << ", stage_process_contract=lightweight_stage_skeleton"
          << ", stage_process_record=" << context.record_type
          << ", stage_process_methods="
          << "Process>SetParkingType>ExecuteTaskOnOpenSpace>"
          << "IsReadyToFinishStage>FinishScenario"
          << ", stage_process_called=" << BoolText(context.process_called)
          << ", stage_set_parking_type_called="
          << BoolText(context.set_parking_type_called)
          << ", stage_execute_task_on_open_space_called="
          << BoolText(context.execute_task_on_open_space_called)
          << ", stage_is_ready_to_finish_called="
          << BoolText(context.is_ready_to_finish_stage_called)
          << ", stage_finish_scenario_projected="
          << BoolText(context.finish_scenario_projected)
          << ", stage_skeleton_branch=" << context.branch
          << ", stage_skeleton_task_entry=" << context.task_entry
          << ", stage_skeleton_task_chain=" << context.task_chain
          << ", stage_skeleton_next_stage=" << context.next_stage
          << ", frame_lite_contract=stub_frame_bridge"
          << ", frame_lite_seq=" << context.frame.seq
          << ", frame_lite_id=" << context.frame.frame_id
          << ", frame_lite_data_stamp_ms=" << context.frame.data_stamp_ms
          << ", frame_lite_selected_slot_valid="
          << BoolText(context.frame.selected_slot_valid)
          << ", frame_lite_selected_slot_count="
          << context.frame.selected_slot_count
          << ", frame_lite_parking_lot_count="
          << context.frame.parking_lot_count
          << ", frame_lite_opt_parking_seq="
          << context.frame.selected_parking_seq
          << ", frame_lite_vehicle_state_valid="
          << BoolText(context.frame.vehicle_state_valid)
          << ", frame_lite_vehicle_state_source="
          << context.frame.vehicle_state_source
          << ", frame_lite_obstacle_count=" << context.frame.obstacle_count
          << ", open_space_info_lite_contract=stub_open_space_bridge"
          << ", open_space_info_lite_open_space_trajectory="
          << BoolText(context.open_space_info.open_space_trajectory)
          << ", open_space_info_lite_parking_type="
          << context.open_space_info.parking_type
          << ", open_space_info_lite_target_seq="
          << context.open_space_info.target_parking_seq
          << ", open_space_info_lite_roi_ready="
          << BoolText(context.open_space_info.roi_decider_output_ready)
          << ", open_space_info_lite_path_ready="
          << BoolText(context.open_space_info.path_provider_output_ready)
          << ", open_space_info_lite_partition_ready="
          << BoolText(context.open_space_info.path_partition_output_ready)
          << ", open_space_info_lite_speed_ready="
          << BoolText(context.open_space_info.speed_optimizer_output_ready)
          << ", open_space_info_lite_straight_ready="
          << BoolText(context.open_space_info.straight_path_output_ready)
          << ", open_space_info_lite_destination_reached="
          << BoolText(context.open_space_info.destination_reached)
          << ", open_space_info_lite_chosen_path_points="
          << context.open_space_info.chosen_path_points
          << ", planning_context_lite_contract=stub_planning_context_bridge"
          << ", planning_context_lite_path_history_available="
          << BoolText(context.planning_context.path_history_available)
          << ", planning_context_lite_path_id="
          << context.planning_context.path_id
          << ", planning_context_lite_speed_frame_available="
          << BoolText(context.planning_context.speed_frame_available)
          << ", planning_context_lite_stage_exit_requested="
          << BoolText(context.planning_context.stage_exit_requested)
          << ", planning_context_lite_direct_command_active="
          << BoolText(context.planning_context.direct_command_active)
          << ", planning_context_lite_processed_frames="
          << context.planning_context.processed_frames
          << ", planning_context_lite_fallback_event="
          << context.planning_context.fallback_event
          << ", function_manager_lite_contract=stub_function_manager_bridge"
          << ", function_manager_lite_source="
          << context.function_manager.source
          << ", function_manager_lite_sys_command="
          << context.function_manager.sys_command
          << ", function_manager_lite_sys_run_state="
          << context.function_manager.sys_run_state
          << ", function_manager_lite_parking_type="
          << context.function_manager.parking_type
          << ", function_manager_lite_command="
          << context.function_manager.command
          << ", hmi_state_lite_contract=stub_hmi_bridge"
          << ", hmi_state_lite_state=" << context.hmi_state.state
          << ", hmi_state_lite_source=" << context.hmi_state.source
          << ", collision_result_lite_contract="
          << context.collision.contract
          << ", collision_result_lite_risk="
          << BoolText(context.collision.collision_risk)
          << ", collision_result_lite_source=" << context.collision.source
          << ", wheel_mask_lite_contract=" << context.wheel_mask.contract
          << ", wheel_mask_lite_valid=" << BoolText(context.wheel_mask.valid)
          << ", wheel_mask_lite_source=" << context.wheel_mask.source;
}

}  // namespace valet_parking
