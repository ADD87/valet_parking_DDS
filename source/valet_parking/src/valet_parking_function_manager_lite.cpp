#include "valet_parking_function_manager_lite.h"

#include <cmath>
#include <sstream>

namespace valet_parking {

namespace {

const char* BoolText(bool value) {
  return value ? "true" : "false";
}

std::string DirectFinishStateName(
    const DirectFinishEvaluation& evaluation) {
  if (evaluation.ready_to_finish) {
    return "READY";
  }
  return evaluation.ready_condition ? "HOLDING" : "WAITING";
}

}  // namespace

std::string ParkingCommandModeToString(ParkingCommandMode mode) {
  switch (mode) {
    case ParkingCommandMode::PARKING_COMMAND_PARKING_IN:
      return "PARKING_IN";
    case ParkingCommandMode::PARKING_COMMAND_DIRECT_FORWARD:
      return "DIRECT_FORWARD";
    case ParkingCommandMode::PARKING_COMMAND_DIRECT_BACKWARD:
      return "DIRECT_BACKWARD";
    case ParkingCommandMode::PARKING_COMMAND_PAUSE:
      return "PAUSE";
    case ParkingCommandMode::PARKING_COMMAND_BRAKE:
      return "BRAKE";
    case ParkingCommandMode::PARKING_COMMAND_FINISH:
      return "FINISH";
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_LEFT:
      return "PARKING_OUT_LEFT";
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_RIGHT:
      return "PARKING_OUT_RIGHT";
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_FRONT:
      return "PARKING_OUT_FRONT";
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_BACK:
      return "PARKING_OUT_BACK";
    case ParkingCommandMode::PARKING_COMMAND_NONE:
      return "NONE";
  }
  return "UNKNOWN";
}

bool IsDirectCommandMode(ParkingCommandMode mode) {
  return mode == ParkingCommandMode::PARKING_COMMAND_DIRECT_FORWARD ||
         mode == ParkingCommandMode::PARKING_COMMAND_DIRECT_BACKWARD;
}

std::string DirectReleasedActionName(ParkingCommandMode mode) {
  return mode == ParkingCommandMode::PARKING_COMMAND_DIRECT_BACKWARD
             ? "DIRECT_BACKWARD_RELEASED"
             : "DIRECT_FORWARD_RELEASED";
}

DirectFinishEvaluation BuildDirectFinishEvaluation(
    double vehicle_speed_mps,
    bool command_active,
    double standstill_threshold_mps) {
  DirectFinishEvaluation evaluation;
  evaluation.command_active = command_active;
  evaluation.command_inactive = !command_active;
  evaluation.vehicle_standstill =
      std::fabs(vehicle_speed_mps) <= standstill_threshold_mps;
  evaluation.ready_condition =
      evaluation.command_inactive && evaluation.vehicle_standstill;
  evaluation.ready_to_finish = evaluation.ready_condition;
  return evaluation;
}

void AppendDirectFinishContract(
    const DirectFinishEvaluation& evaluation,
    std::ostringstream* stream) {
  if (stream == nullptr) {
    return;
  }
  *stream << ", finish_condition=direct_command_inactive_and_standstill"
          << ", direct_command_active=" << BoolText(evaluation.command_active)
          << ", direct_command_inactive="
          << BoolText(evaluation.command_inactive)
          << ", direct_vehicle_standstill="
          << BoolText(evaluation.vehicle_standstill)
          << ", direct_finish_ready="
          << BoolText(evaluation.ready_to_finish)
          << ", direct_stage_finish_state="
          << DirectFinishStateName(evaluation);
}

std::string ProjectSysCommandName(ParkingCommandMode mode) {
  switch (mode) {
    case ParkingCommandMode::PARKING_COMMAND_PARKING_IN:
    case ParkingCommandMode::PARKING_COMMAND_NONE:
      return "PARKINCONTROL";
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_LEFT:
      return "LEFTPARKOUTCONTROL";
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_RIGHT:
      return "RIGHTPARKOUTCONTROL";
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_FRONT:
      return "FRONTPARKOUTCONTROL";
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_BACK:
      return "BACKPARKOUTCONTROL";
    case ParkingCommandMode::PARKING_COMMAND_DIRECT_FORWARD:
      return "FORWARDCONTROL";
    case ParkingCommandMode::PARKING_COMMAND_DIRECT_BACKWARD:
      return "BACKWARDCONTROL";
    case ParkingCommandMode::PARKING_COMMAND_PAUSE:
      return "STOPPARKINROUTE";
    case ParkingCommandMode::PARKING_COMMAND_BRAKE:
      return "BRAKECONTROL";
    case ParkingCommandMode::PARKING_COMMAND_FINISH:
      return "PARKINGFINISH";
  }
  return "PARKINNONE";
}

std::string ProjectSysRunStateName(ParkingCommandMode mode,
                                   bool has_path_history,
                                   bool reset_history) {
  switch (mode) {
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_LEFT:
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_RIGHT:
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_FRONT:
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_BACK:
      return "PARKOUT";
    case ParkingCommandMode::PARKING_COMMAND_DIRECT_FORWARD:
    case ParkingCommandMode::PARKING_COMMAND_DIRECT_BACKWARD:
      return "PARKING";
    case ParkingCommandMode::PARKING_COMMAND_PAUSE:
      return "PAUSE";
    case ParkingCommandMode::PARKING_COMMAND_BRAKE:
      return "STRAIGHTBRAKE";
    case ParkingCommandMode::PARKING_COMMAND_FINISH:
      return "QUIT";
    case ParkingCommandMode::PARKING_COMMAND_PARKING_IN:
    case ParkingCommandMode::PARKING_COMMAND_NONE:
      return (!has_path_history || reset_history) ? "PARKSTART" : "PARKING";
  }
  return "STOP";
}

std::string ProjectSysWarningInfoName(ParkingCommandMode mode) {
  switch (mode) {
    case ParkingCommandMode::PARKING_COMMAND_PAUSE:
      return "SYSTEM_PAUSE_0x0B";
    case ParkingCommandMode::PARKING_COMMAND_NONE:
    case ParkingCommandMode::PARKING_COMMAND_PARKING_IN:
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_LEFT:
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_RIGHT:
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_FRONT:
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_BACK:
    case ParkingCommandMode::PARKING_COMMAND_DIRECT_FORWARD:
    case ParkingCommandMode::PARKING_COMMAND_DIRECT_BACKWARD:
    case ParkingCommandMode::PARKING_COMMAND_BRAKE:
    case ParkingCommandMode::PARKING_COMMAND_FINISH:
      return "NO_WARNING";
  }
  return "NO_WARNING";
}

std::string ProjectParkingTypeName(ParkingCommandMode mode) {
  switch (mode) {
    case ParkingCommandMode::PARKING_COMMAND_PARKING_IN:
    case ParkingCommandMode::PARKING_COMMAND_NONE:
      return "PARKING_IN";
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_LEFT:
      return "PARKING_OUT_LEFT";
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_RIGHT:
      return "PARKING_OUT_RIGHT";
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_FRONT:
      return "PARKING_OUT_FRONT";
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_BACK:
      return "PARKING_OUT_BACK";
    case ParkingCommandMode::PARKING_COMMAND_DIRECT_FORWARD:
      return "DIRECT_FORWARD";
    case ParkingCommandMode::PARKING_COMMAND_DIRECT_BACKWARD:
      return "DIRECT_BACKWARD";
    case ParkingCommandMode::PARKING_COMMAND_PAUSE:
      return "UNCHANGED";
    case ParkingCommandMode::PARKING_COMMAND_BRAKE:
    case ParkingCommandMode::PARKING_COMMAND_FINISH:
      return "NOSTATE";
  }
  return "NOSTATE";
}

FunctionManagerProjection BuildFunctionManagerProjection(
    const ParkingCommand* command,
    bool has_path_history) {
  FunctionManagerProjection projection;
  const bool has_command = command != nullptr && command->is_valid();
  const ParkingCommandMode mode =
      has_command ? command->mode()
                  : ParkingCommandMode::PARKING_COMMAND_NONE;
  projection.source = has_command ? "parking_command" : "selected_slot";
  projection.parking_command =
      has_command ? ParkingCommandModeToString(mode) : "NONE";
  projection.reset_history = has_command && command->reset_history();
  projection.sys_command = ProjectSysCommandName(mode);
  projection.sys_run_state =
      ProjectSysRunStateName(mode, has_path_history,
                             projection.reset_history);
  projection.sys_warning_info = ProjectSysWarningInfoName(mode);
  projection.parking_type = ProjectParkingTypeName(mode);
  return projection;
}

FunctionManagerProjection BuildReleasedDirectFunctionProjection(
    ParkingCommandMode previous_direct_mode) {
  FunctionManagerProjection projection;
  projection.source = "cleared_direct_command";
  projection.parking_command = "NONE";
  projection.sys_command = ProjectSysCommandName(previous_direct_mode);
  projection.sys_run_state = "QUIT";
  projection.sys_warning_info = "NO_WARNING";
  projection.parking_type = ProjectParkingTypeName(previous_direct_mode);
  projection.reset_history = false;
  return projection;
}

FunctionManagerProjection BuildStageFinishHoldFunctionProjection() {
  FunctionManagerProjection projection;
  projection.source = "stage_finish_latched";
  projection.parking_command = "NONE";
  projection.sys_command = "PARKINGFINISH";
  projection.sys_run_state = "QUIT";
  projection.sys_warning_info = "NO_WARNING";
  projection.parking_type = "PARKING_IN";
  projection.reset_history = false;
  return projection;
}

StageFunctionManagerProjectionContractLite
BuildFunctionManagerProjectionContractLite(
    const FunctionManagerProjection& projection) {
  StageFunctionManagerProjectionContractLite lite;
  lite.source = projection.source;
  lite.parking_command = projection.parking_command;
  lite.sys_mode = projection.sys_mode;
  lite.sys_command = projection.sys_command;
  lite.sys_run_state = projection.sys_run_state;
  lite.sys_warning_info = projection.sys_warning_info;
  lite.parking_type = projection.parking_type;
  lite.reset_history = projection.reset_history;
  return lite;
}

StageFunctionManagerLite BuildStageFunctionManagerLite(
    const FunctionManagerProjection& projection) {
  StageFunctionManagerLite function_manager;
  function_manager.source = projection.source;
  function_manager.sys_command = projection.sys_command;
  function_manager.sys_run_state = projection.sys_run_state;
  function_manager.parking_type = projection.parking_type;
  function_manager.command = projection.parking_command;
  function_manager.reset_history = projection.reset_history;
  return function_manager;
}

void AppendFunctionManagerProjectionContract(
    const FunctionManagerProjection& projection,
    std::ostringstream* stream) {
  AppendFunctionManagerProjectionContract(
      BuildFunctionManagerProjectionContractLite(projection), stream);
}

std::string BuildFunctionManagerProjectionReason(
    const FunctionManagerProjection& projection) {
  return BuildFunctionManagerProjectionReason(
      BuildFunctionManagerProjectionContractLite(projection));
}

}  // namespace valet_parking
