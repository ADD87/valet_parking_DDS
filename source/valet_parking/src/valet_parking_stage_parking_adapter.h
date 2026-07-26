#pragma once

#include "valet_parking_c_api.h"

#include <string>

class PlanningTrajectory;
class SelectedSlot;

namespace valet_parking {

class ValetParkingStageParkingAdapter final {
 public:
  explicit ValetParkingStageParkingAdapter(const valet_parking_config_t& config);

  bool Process(const SelectedSlot& input_sample,
               PlanningTrajectory* output_sample,
               std::string* status_reason) const;

 private:
  valet_parking_config_t config_{};
};

}  // namespace valet_parking
