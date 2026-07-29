#pragma once

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <string>

namespace valet_parking {

struct StageFrameLite {
  uint64_t seq{0U};
  std::string frame_id{"selected_slot"};
  uint64_t data_stamp_ms{0U};
  bool selected_slot_valid{false};
  uint32_t selected_slot_count{0U};
  std::size_t parking_lot_count{0U};
  uint32_t selected_parking_seq{0U};
  bool vehicle_state_valid{false};
  std::string vehicle_state_source{"config_fake_vehicle"};
  std::size_t obstacle_count{0U};
};

struct StageOpenSpaceInfoLite {
  bool open_space_trajectory{true};
  std::string parking_type{"PARKING_IN"};
  uint32_t target_parking_seq{0U};
  bool roi_decider_output_ready{false};
  bool path_provider_output_ready{false};
  bool path_partition_output_ready{false};
  bool speed_optimizer_output_ready{false};
  bool straight_path_output_ready{false};
  bool destination_reached{false};
  std::size_t chosen_path_points{0U};
};

struct StagePlanningContextLite {
  bool path_history_available{false};
  int path_id{-1};
  bool speed_frame_available{false};
  bool stage_exit_requested{false};
  bool direct_command_active{false};
  uint64_t processed_frames{0U};
  std::string fallback_event{"none"};
};

struct StageFunctionManagerLite {
  std::string source{"selected_slot"};
  std::string sys_command{"PARKINCONTROL"};
  std::string sys_run_state{"PARKSTART"};
  std::string parking_type{"PARKING_IN"};
  std::string command{"NONE"};
  bool reset_history{false};
};

struct StageHmiStateLite {
  std::string state{"NO_STAGE"};
  std::string source{"stub_default"};
};

struct StageCollisionResultLite {
  std::string contract{"geometry_precheck_only"};
  bool collision_risk{false};
  std::string source{"roi_and_external_segments"};
};

struct StageWheelMaskLite {
  std::string contract{"stub_invalid"};
  bool valid{false};
  std::string source{"none"};
};

struct StageProcessInputLite {
  std::string record_type{"normal_open_space"};
  std::string branch{"normal_open_space"};
  std::string task_entry{"ROI_DECIDER"};
  std::string task_chain{"ROI_DECIDER>PATH_PROVIDER>PATH_PARTITION>SPEED_OPTIMIZER"};
  bool execute_task_on_open_space{true};
  bool is_ready_to_finish_stage_called{true};
  bool ready_to_finish{false};
  bool finish_scenario_intent{false};

  StageFrameLite frame;
  StageOpenSpaceInfoLite open_space_info;
  StagePlanningContextLite planning_context;
  StageFunctionManagerLite function_manager;
  StageHmiStateLite hmi_state;
  StageCollisionResultLite collision;
  StageWheelMaskLite wheel_mask;
};

struct StageProcessContextLite {
  std::string record_type{"normal_open_space"};
  std::string branch{"normal_open_space"};
  std::string stage_name{"PARKING"};
  std::string next_stage{"PARKING"};
  std::string task_entry{"ROI_DECIDER"};
  std::string task_chain{"ROI_DECIDER>PATH_PROVIDER>PATH_PARTITION>SPEED_OPTIMIZER"};
  bool process_called{false};
  bool set_parking_type_called{false};
  bool execute_task_on_open_space_called{false};
  bool is_ready_to_finish_stage_called{false};
  bool finish_scenario_projected{false};

  StageFrameLite frame;
  StageOpenSpaceInfoLite open_space_info;
  StagePlanningContextLite planning_context;
  StageFunctionManagerLite function_manager;
  StageHmiStateLite hmi_state;
  StageCollisionResultLite collision;
  StageWheelMaskLite wheel_mask;
};

class ValetParkingStageParkingStageLite final {
 public:
  StageProcessContextLite Process(const StageProcessInputLite& input) const;

 private:
  void SetParkingType(const StageProcessInputLite& input,
                      StageProcessContextLite* context) const;
  void ExecuteTaskOnOpenSpace(const StageProcessInputLite& input,
                              StageProcessContextLite* context) const;
  void IsReadyToFinishStage(const StageProcessInputLite& input,
                            StageProcessContextLite* context) const;
  void FinishScenario(const StageProcessInputLite& input,
                      StageProcessContextLite* context) const;
};

void AppendStageProcessContract(const StageProcessContextLite& context,
                                std::ostringstream* stream);

}  // namespace valet_parking
