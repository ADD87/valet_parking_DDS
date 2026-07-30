#include "valet_parking_stage_finish_lite.h"

#include <cmath>
#include <limits>

namespace valet_parking {

StageFinishEvaluation UpdateStageFinishEvaluation(
    const TL::planning::PartitionOutput& partition_output,
    double vehicle_speed_mps,
    double standstill_threshold_mps,
    uint32_t required_consecutive_frames,
    StageFinishRuntimeState* finish_state) {
  StageFinishEvaluation evaluation;
  evaluation.destination_reached = partition_output.destination_reached;
  evaluation.vehicle_standstill =
      std::fabs(vehicle_speed_mps) <= standstill_threshold_mps;
  evaluation.ready_condition =
      evaluation.destination_reached && evaluation.vehicle_standstill;
  evaluation.required_consecutive_frames = required_consecutive_frames;

  if (finish_state != nullptr) {
    if (evaluation.ready_condition) {
      if (finish_state->consecutive_ready_frames <
          std::numeric_limits<uint32_t>::max()) {
        ++finish_state->consecutive_ready_frames;
      }
    } else {
      finish_state->consecutive_ready_frames = 0U;
    }
    finish_state->last_ready_condition = evaluation.ready_condition;
    finish_state->last_ready_to_finish =
        finish_state->consecutive_ready_frames >= required_consecutive_frames;
    evaluation.consecutive_ready_frames =
        finish_state->consecutive_ready_frames;
    evaluation.ready_to_finish = finish_state->last_ready_to_finish;
  } else {
    evaluation.consecutive_ready_frames =
        evaluation.ready_condition ? 1U : 0U;
    evaluation.ready_to_finish = evaluation.ready_condition;
  }
  return evaluation;
}

StageFinishEvaluationLite BuildStageFinishEvaluationLite(
    const StageFinishEvaluation& evaluation) {
  StageFinishEvaluationLite lite;
  lite.ready_condition = evaluation.ready_condition;
  lite.ready_to_finish = evaluation.ready_to_finish;
  lite.consecutive_ready_frames = evaluation.consecutive_ready_frames;
  lite.required_consecutive_frames = evaluation.required_consecutive_frames;
  return lite;
}

StageFinishOutputContractLite BuildStageFinishOutputContractLite(
    const StageFinishEvaluation& evaluation) {
  StageFinishOutputContractLite lite;
  lite.destination_reached = evaluation.destination_reached;
  lite.vehicle_standstill = evaluation.vehicle_standstill;
  lite.ready_condition = evaluation.ready_condition;
  lite.ready_to_finish = evaluation.ready_to_finish;
  lite.consecutive_ready_frames = evaluation.consecutive_ready_frames;
  lite.required_consecutive_frames = evaluation.required_consecutive_frames;
  return lite;
}

PlanningTrajectoryType StageTrajectoryType(
    const TL::planning::PartitionOutput& partition_output,
    const TL::planning::SpeedOptimizerOutput* speed_output,
    const StageFinishEvaluation* finish_evaluation) {
  StageFinishOutputContractLite finish_lite;
  const StageFinishOutputContractLite* finish_lite_ptr = nullptr;
  if (finish_evaluation != nullptr) {
    finish_lite = BuildStageFinishOutputContractLite(*finish_evaluation);
    finish_lite_ptr = &finish_lite;
  }
  return StageTrajectoryType(partition_output, speed_output,
                             finish_lite_ptr);
}

}  // namespace valet_parking
