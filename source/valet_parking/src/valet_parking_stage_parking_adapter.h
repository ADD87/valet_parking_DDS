#pragma once

#include "valet_parking_c_api.h"

#include <memory>
#include <mutex>
#include <string>

class PlanningTrajectory;
class SelectedSlot;

namespace valet_parking {

class ValetParkingStageParkingAdapter final {
 public:
  explicit ValetParkingStageParkingAdapter(const valet_parking_config_t& config);
  ~ValetParkingStageParkingAdapter();

  bool Process(const SelectedSlot& input_sample,
               PlanningTrajectory* output_sample,
               std::string* status_reason);

  int UpdateVehicleState(const valet_parking_vehicle_state_t& vehicle_state);
  int ClearVehicleState();
  int UpdateObstacles(const valet_parking_obstacle_t* obstacles,
                      uint32_t obstacle_count);
  int ClearObstacles();

 private:
  struct RuntimeContext;

  valet_parking_config_t config_{};
  std::unique_ptr<RuntimeContext> runtime_;
  std::mutex runtime_mutex_;
};

}  // namespace valet_parking
