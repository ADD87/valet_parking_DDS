#pragma once

#include <string>

#include "planning/open_space/vehicle_param.h"
#include "proto_convert/open_space_types_convert.h"

namespace MAGNA {
namespace planning {

struct NlpPathSmootherLiteConfig {
  bool enabled{false};
  int max_iterations{80};
  double max_cpu_time_s{0.25};
  double bias_weight{1.0};
  double kappa_weight{0.02};
  double dkappa_weight{0.25};
  double lateral_buffer{0.05};
};

struct NlpPathSmootherLiteDiagnostics {
  bool attempted{false};
  bool success{false};
  std::size_t point_count{0U};
  double solve_time_s{0.0};
  std::string solver_status;
  std::string fallback_reason;
};

// A protobuf-free parking-stage adapter for the TempAPA NLP path smoother.
// The adapter deliberately keeps the current coarse path as the fallback and
// only replaces it after the solver result passes finite-value and endpoint
// checks.
class NlpPathSmootherLite final {
 public:
  NlpPathSmootherLite(const NlpPathSmootherLiteConfig& config,
                      const VehicleParam& vehicle_param);
  ~NlpPathSmootherLite();

  NlpPathSmootherLite(const NlpPathSmootherLite&) = delete;
  NlpPathSmootherLite& operator=(const NlpPathSmootherLite&) = delete;

  bool Smooth(const OpenSpacePathInput& input, OpenSpacePathOutput* output,
              NlpPathSmootherLiteDiagnostics* diagnostics) const;

 private:
  NlpPathSmootherLiteConfig config_;
  VehicleParam vehicle_param_;
};

}  // namespace planning
}  // namespace MAGNA