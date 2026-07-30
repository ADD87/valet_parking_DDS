#pragma once

#include "valet_parking_stage_contract_lite.h"
#include "valet_parking_topics.h"

#include <iosfwd>
#include <string>

namespace valet_parking {

struct DirectFinishEvaluation {
  bool command_active{true};
  bool command_inactive{false};
  bool vehicle_standstill{false};
  bool ready_condition{false};
  bool ready_to_finish{false};
};

struct FunctionManagerProjection {
  std::string source{"selected_slot"};
  std::string parking_command{"NONE"};
  std::string sys_mode{"RPA"};
  std::string sys_command{"PARKINCONTROL"};
  std::string sys_run_state{"PARKSTART"};
  std::string sys_warning_info{"NO_WARNING"};
  std::string parking_type{"PARKING_IN"};
  bool reset_history{false};
};

std::string ParkingCommandModeToString(ParkingCommandMode mode);
bool IsDirectCommandMode(ParkingCommandMode mode);
std::string DirectReleasedActionName(ParkingCommandMode mode);

DirectFinishEvaluation BuildDirectFinishEvaluation(
    double vehicle_speed_mps,
    bool command_active,
    double standstill_threshold_mps);
void AppendDirectFinishContract(const DirectFinishEvaluation& evaluation,
                                std::ostringstream* stream);

std::string ProjectSysCommandName(ParkingCommandMode mode);
std::string ProjectSysRunStateName(ParkingCommandMode mode,
                                   bool has_path_history,
                                   bool reset_history);
std::string ProjectSysWarningInfoName(ParkingCommandMode mode);
std::string ProjectParkingTypeName(ParkingCommandMode mode);

FunctionManagerProjection BuildFunctionManagerProjection(
    const ParkingCommand* command,
    bool has_path_history);
FunctionManagerProjection BuildReleasedDirectFunctionProjection(
    ParkingCommandMode previous_direct_mode);
FunctionManagerProjection BuildStageFinishHoldFunctionProjection();

StageFunctionManagerProjectionContractLite
BuildFunctionManagerProjectionContractLite(
    const FunctionManagerProjection& projection);
StageFunctionManagerLite BuildStageFunctionManagerLite(
    const FunctionManagerProjection& projection);
void AppendFunctionManagerProjectionContract(
    const FunctionManagerProjection& projection,
    std::ostringstream* stream);
std::string BuildFunctionManagerProjectionReason(
    const FunctionManagerProjection& projection);

}  // namespace valet_parking
