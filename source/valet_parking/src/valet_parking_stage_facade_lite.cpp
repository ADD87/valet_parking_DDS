#include "valet_parking_stage_facade_lite.h"

#include "planning/tasks/optimizers/open_space_path_generation/open_space_path_provider.h"
#include "planning/tasks/optimizers/open_space_path_partition/open_space_path_partition.h"
#include "planning/tasks/optimizers/open_space_speed_optimizer/open_space_speed_optimizer.h"

namespace valet_parking {

namespace {

void ApplyFacadeBranch(const StageFacadeInputLite& facade_input,
                       StageProcessInputLite* stage_input) {
  if (stage_input == nullptr) {
    return;
  }

  switch (facade_input.branch) {
    case StageFacadeBranchLite::kDirectOpenSpace:
      stage_input->record_type = "direct_control";
      stage_input->branch = "direct_open_space";
      stage_input->task_entry = "OPEN_SPACE_STRAIGHT_PATH";
      stage_input->task_chain = "OPEN_SPACE_STRAIGHT_PATH>SPEED_OPTIMIZER";
      break;
    case StageFacadeBranchLite::kStageControlOverride:
      stage_input->record_type = "stage_control_override";
      stage_input->branch = "stage_control_override";
      stage_input->task_entry = "STAGE_CONTROL";
      stage_input->task_chain = "STAGE_CONTROL_STOP";
      stage_input->execute_task_on_open_space = false;
      stage_input->is_ready_to_finish_stage_called =
          facade_input.finish_requested;
      stage_input->ready_to_finish = facade_input.finish_requested;
      stage_input->finish_scenario_intent = facade_input.finish_requested;
      break;
    case StageFacadeBranchLite::kStageFinishHold:
      stage_input->record_type = "stage_finish_hold";
      stage_input->branch = "stage_finish_hold";
      stage_input->task_entry = "STAGE_FINISH_HOLD";
      stage_input->task_chain = "STAGE_FINISH_HOLD";
      stage_input->execute_task_on_open_space = false;
      stage_input->ready_to_finish = true;
      stage_input->finish_scenario_intent = true;
      break;
    case StageFacadeBranchLite::kNormalOpenSpace:
    default:
      break;
  }
}

void ApplyFunctionManager(const StageFunctionManagerLite& function_manager,
                          StageProcessContextLite* context) {
  if (context == nullptr) {
    return;
  }
  context->function_manager = function_manager;
}

}  // namespace

std::size_t CountPathProviderPoints(
    const TL::planning::OpenSpacePathOutput& path_output) {
  std::size_t count = 0U;
  for (const auto& path_pair : path_output.partitioned_path) {
    count += path_pair.first.size();
  }
  return count;
}

StageProcessContextLite BuildStageProcessContextLite(
    const StageFacadeInputLite& facade_input) {
  StageProcessInputLite stage_input;
  stage_input.frame = facade_input.frame;
  stage_input.open_space_info = facade_input.open_space_info;
  stage_input.planning_context = facade_input.planning_context;
  stage_input.function_manager = facade_input.function_manager;
  stage_input.hmi_state = facade_input.hmi_state;
  stage_input.collision = facade_input.collision;
  stage_input.wheel_mask = facade_input.wheel_mask;

  ApplyFacadeBranch(facade_input, &stage_input);
  return ValetParkingStageParkingStageLite().Process(stage_input);
}

void MarkStageEarlyInputFallback(const std::string& fallback_event,
                                 StageProcessContextLite* context) {
  if (context == nullptr) {
    return;
  }
  context->record_type = "early_input_fallback";
  context->branch = "normal_open_space_early_input";
  context->task_entry = "INPUT_VALIDATION";
  context->task_chain = "INPUT_VALIDATION";
  context->execute_task_on_open_space_called = false;
  context->planning_context.fallback_event = fallback_event;
}

void MarkStageFallback(const std::string& fallback_event,
                       StageProcessContextLite* context) {
  if (context == nullptr) {
    return;
  }
  context->record_type = "fallback_output";
  context->planning_context.fallback_event = fallback_event;
}

void MarkStageSelectedParkingLot(uint32_t parking_seq,
                                 StageProcessContextLite* context) {
  if (context == nullptr) {
    return;
  }
  context->open_space_info.target_parking_seq = parking_seq;
}

void MarkStageRoiOutput(uint32_t parking_seq,
                        StageProcessContextLite* context) {
  if (context == nullptr) {
    return;
  }
  context->open_space_info.target_parking_seq = parking_seq;
  context->open_space_info.roi_decider_output_ready = true;
}

void MarkStagePathProviderOutput(
    const TL::planning::OpenSpacePathOutput& path_output,
    StageProcessContextLite* context) {
  if (context == nullptr) {
    return;
  }
  context->open_space_info.path_provider_output_ready = true;
  context->open_space_info.chosen_path_points =
      CountPathProviderPoints(path_output);
}

void MarkStagePathPartitionOutput(
    const TL::planning::PartitionOutput& partition_output,
    StageProcessContextLite* context) {
  if (context == nullptr) {
    return;
  }
  context->open_space_info.path_partition_output_ready = true;
  context->open_space_info.destination_reached =
      partition_output.destination_reached;
  context->open_space_info.chosen_path_points =
      partition_output.chosen_partitioned_path.first.size();
}

void MarkStageStraightPathOutput(
    const TL::planning::PartitionOutput& partition_output,
    StageProcessContextLite* context) {
  if (context == nullptr) {
    return;
  }
  context->open_space_info.straight_path_output_ready = true;
  context->open_space_info.path_partition_output_ready = true;
  context->open_space_info.chosen_path_points =
      partition_output.chosen_partitioned_path.first.size();
}

void MarkStageSpeedOutput(
    const TL::planning::SpeedOptimizerOutput& speed_output,
    StageProcessContextLite* context) {
  if (context == nullptr) {
    return;
  }
  context->open_space_info.speed_optimizer_output_ready = true;
  context->open_space_info.chosen_path_points =
      speed_output.trajectory_gear.first.NumOfPoints();
}

void MarkStageFinishEvaluation(const StageFinishEvaluationLite& evaluation,
                               StageProcessContextLite* context) {
  if (context == nullptr) {
    return;
  }
  context->is_ready_to_finish_stage_called = true;
  context->planning_context.stage_exit_requested =
      evaluation.ready_to_finish ||
      context->planning_context.stage_exit_requested;
  context->finish_scenario_projected = evaluation.ready_to_finish;
  context->next_stage = evaluation.ready_to_finish ? "FINISH" : "PARKING";
}

void MarkStageDirectRelease(bool ready_to_finish,
                            const StageFunctionManagerLite& function_manager,
                            StageProcessContextLite* context) {
  if (context == nullptr) {
    return;
  }
  context->record_type = "direct_release";
  context->branch = "direct_open_space";
  context->task_entry = "DIRECT_COMMAND_RELEASE";
  context->task_chain = "DIRECT_COMMAND_RELEASE";
  context->execute_task_on_open_space_called = false;
  context->is_ready_to_finish_stage_called = true;
  context->finish_scenario_projected = ready_to_finish;
  context->next_stage = ready_to_finish ? "FINISH" : "PARKING";
  context->planning_context.stage_exit_requested = ready_to_finish;
  ApplyFunctionManager(function_manager, context);
}

void MarkStageFinishHold(const StageFunctionManagerLite& function_manager,
                         StageProcessContextLite* context) {
  if (context == nullptr) {
    return;
  }
  context->record_type = "stage_finish_hold";
  context->branch = "stage_finish_hold";
  context->task_entry = "STAGE_FINISH_HOLD";
  context->task_chain = "STAGE_FINISH_HOLD";
  context->execute_task_on_open_space_called = false;
  context->is_ready_to_finish_stage_called = true;
  context->finish_scenario_projected = true;
  context->next_stage = "FINISH";
  context->planning_context.stage_exit_requested = true;
  ApplyFunctionManager(function_manager, context);
}

}  // namespace valet_parking
