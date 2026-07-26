#pragma once

#include "valet_parking_c_api.h"

#include <memory>
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

 private:
  struct RuntimeContext;

  valet_parking_config_t config_{};
  std::unique_ptr<RuntimeContext> runtime_;
};

}  // namespace valet_parking
