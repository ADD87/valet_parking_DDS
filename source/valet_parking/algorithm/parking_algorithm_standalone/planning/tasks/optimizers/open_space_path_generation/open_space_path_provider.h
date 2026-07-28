#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "common/status/status.h"
#include "planning/open_space/open_space_thread/open_space_thread_manager.h"
#include "planning/open_space/vehicle_param.h"
#include "planning/open_space/hybrid_a_star_config.h"
#include "proto_convert/open_space_types_convert.h"
#include "proto_convert/planning_internal_convert.h"

namespace TL {
namespace planning {

struct OpenSpacePathProviderConfig {
  HybridAStarConfig hybrid_config;
  VehicleParam vehicle_param;
  std::size_t search_thread_num{4U};
  double target_plan_timeout_s{8.5};
};

struct OpenSpacePathProviderDiagnostics {
  bool threaded{false};
  bool target_plan_submitted{false};
  bool target_output_ready{false};
  bool target_used_candidate_result{false};
  bool target_generated_in_target_thread{false};
  bool target_timed_out{false};
  double wait_time_s{0.0};
  std::vector<int> thread_path_ids;
  std::string provider_status{"OFF"};
};

class OpenSpacePathProvider {
 public:
  explicit OpenSpacePathProvider(const OpenSpacePathProviderConfig& config);
  ~OpenSpacePathProvider();

  OpenSpacePathProvider(const OpenSpacePathProvider&) = delete;
  OpenSpacePathProvider& operator=(const OpenSpacePathProvider&) = delete;

  TL::common::Status Reset();
  void PrePlan(const std::vector<OpenSpacePathInput>& candidate_inputs);
  TL::common::Status Plan(const OpenSpacePathInput& target_input,
                          OpenSpacePathOutput* output,
                          OpenSpacePathProviderDiagnostics* diagnostics);

 private:
  OpenSpacePathProviderConfig config_;
  std::unique_ptr<OpenSpaceThreadManager> thread_manager_;
};

}  // namespace planning
}  // namespace TL
