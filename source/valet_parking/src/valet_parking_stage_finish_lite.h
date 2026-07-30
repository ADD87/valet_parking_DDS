#pragma once

#include "valet_parking_stage_contract_lite.h"
#include "valet_parking_stage_facade_lite.h"

#include <cstdint>

namespace valet_parking {

struct StageFinishRuntimeState {
  void Reset() {
    consecutive_ready_frames = 0U;
    last_ready_condition = false;
    last_ready_to_finish = false;
  }

  uint32_t consecutive_ready_frames{0U};
  bool last_ready_condition{false};
  bool last_ready_to_finish{false};
};

struct StageFinishEvaluation {
  bool destination_reached{false};
  bool vehicle_standstill{false};
  bool ready_condition{false};
  bool ready_to_finish{false};
  uint32_t consecutive_ready_frames{0U};
  uint32_t required_consecutive_frames{0U};
};

StageFinishEvaluation UpdateStageFinishEvaluation(
    const TL::planning::PartitionOutput& partition_output,
    double vehicle_speed_mps,
    double standstill_threshold_mps,
    uint32_t required_consecutive_frames,
    StageFinishRuntimeState* finish_state);

StageFinishEvaluationLite BuildStageFinishEvaluationLite(
    const StageFinishEvaluation& evaluation);
StageFinishOutputContractLite BuildStageFinishOutputContractLite(
    const StageFinishEvaluation& evaluation);

PlanningTrajectoryType StageTrajectoryType(
    const TL::planning::PartitionOutput& partition_output,
    const TL::planning::SpeedOptimizerOutput* speed_output,
    const StageFinishEvaluation* finish_evaluation);

}  // namespace valet_parking
