#pragma once

#include "valet_parking_stage_process_lite.h"

#include <cstddef>
#include <cstdint>
#include <string>

namespace TL {
namespace planning {
struct OpenSpacePathOutput;
struct PartitionOutput;
struct SpeedOptimizerOutput;
}  // namespace planning
}  // namespace TL

namespace valet_parking {

enum class StageFacadeBranchLite {
  kNormalOpenSpace,
  kDirectOpenSpace,
  kStageControlOverride,
  kStageFinishHold,
};

struct StageFacadeInputLite {
  StageFacadeBranchLite branch{StageFacadeBranchLite::kNormalOpenSpace};
  bool finish_requested{false};

  StageFrameLite frame;
  StageOpenSpaceInfoLite open_space_info;
  StagePlanningContextLite planning_context;
  StageFunctionManagerLite function_manager;
  StageHmiStateLite hmi_state;
  StageCollisionResultLite collision;
  StageWheelMaskLite wheel_mask;
};

struct StageFinishEvaluationLite {
  bool ready_condition{false};
  bool ready_to_finish{false};
  uint32_t consecutive_ready_frames{0U};
  uint32_t required_consecutive_frames{0U};
};

std::size_t CountPathProviderPoints(
    const TL::planning::OpenSpacePathOutput& path_output);

StageProcessContextLite BuildStageProcessContextLite(
    const StageFacadeInputLite& input);

void MarkStageEarlyInputFallback(const std::string& fallback_event,
                                 StageProcessContextLite* context);
void MarkStageFallback(const std::string& fallback_event,
                       StageProcessContextLite* context);
void MarkStageSelectedParkingLot(uint32_t parking_seq,
                                 StageProcessContextLite* context);
void MarkStageRoiOutput(uint32_t parking_seq,
                        StageProcessContextLite* context);
void MarkStagePathProviderOutput(
    const TL::planning::OpenSpacePathOutput& path_output,
    StageProcessContextLite* context);
void MarkStagePathPartitionOutput(
    const TL::planning::PartitionOutput& partition_output,
    StageProcessContextLite* context);
void MarkStageStraightPathOutput(
    const TL::planning::PartitionOutput& partition_output,
    StageProcessContextLite* context);
void MarkStageSpeedOutput(
    const TL::planning::SpeedOptimizerOutput& speed_output,
    StageProcessContextLite* context);
void MarkStageFinishEvaluation(const StageFinishEvaluationLite& evaluation,
                               StageProcessContextLite* context);
void MarkStageDirectRelease(bool ready_to_finish,
                            const StageFunctionManagerLite& function_manager,
                            StageProcessContextLite* context);
void MarkStageFinishHold(const StageFunctionManagerLite& function_manager,
                         StageProcessContextLite* context);

}  // namespace valet_parking
