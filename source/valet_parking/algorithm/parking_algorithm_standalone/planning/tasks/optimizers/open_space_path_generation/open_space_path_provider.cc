#include "planning/tasks/optimizers/open_space_path_generation/open_space_path_provider.h"

#include <chrono>
#include <thread>

#include "proto_convert/error_code_convert.h"

namespace TL {
namespace planning {

namespace {
constexpr double kPollIntervalS = 0.01;

double NowSeconds() {
  return std::chrono::duration<double>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}

void FillProviderDiagnostics(
    const OpenSpaceThreadManagerDiagnostics& manager_diagnostics,
    const std::string& provider_status,
    OpenSpacePathProviderDiagnostics* diagnostics) {
  if (diagnostics == nullptr) {
    return;
  }
  diagnostics->threaded = true;
  diagnostics->target_plan_submitted =
      manager_diagnostics.target_plan_submitted;
  diagnostics->target_output_ready = manager_diagnostics.target_output_ready;
  diagnostics->target_used_candidate_result =
      manager_diagnostics.target_used_candidate_result;
  diagnostics->target_generated_in_target_thread =
      manager_diagnostics.target_generated_in_smooth_thread;
  diagnostics->target_timed_out = manager_diagnostics.target_timed_out;
  diagnostics->target_cancel_requested =
      manager_diagnostics.target_cancel_requested;
  diagnostics->wait_time_s = manager_diagnostics.wait_time_s;
  diagnostics->thread_path_ids = manager_diagnostics.thread_path_ids;
  diagnostics->provider_status = provider_status;
}

}  // namespace

OpenSpacePathProvider::OpenSpacePathProvider(
    const OpenSpacePathProviderConfig& config)
    : config_(config) {
  Reset();
}

OpenSpacePathProvider::~OpenSpacePathProvider() = default;

TL::common::Status OpenSpacePathProvider::Reset() {
  OpenSpaceThreadManagerConfig thread_config;
  thread_config.hybrid_config = config_.hybrid_config;
  thread_config.vehicle_param = config_.vehicle_param;
  thread_config.search_thread_num = config_.search_thread_num;
  thread_config.max_wait_time_s = config_.target_plan_timeout_s;
  thread_manager_ = std::make_unique<OpenSpaceThreadManager>(thread_config);
  return TL::common::Status::OK();
}

void OpenSpacePathProvider::PrePlan(
    const std::vector<OpenSpacePathInput>& candidate_inputs) {
  if (thread_manager_ != nullptr) {
    thread_manager_->PrePlan(candidate_inputs);
  }
}

TL::common::Status OpenSpacePathProvider::Plan(
    const OpenSpacePathInput& target_input,
    OpenSpacePathOutput* output,
    OpenSpacePathProviderDiagnostics* diagnostics) {
  if (output == nullptr) {
    return TL::common::Status(
        TL::common::ErrorCode::CORE_PLANNING_SOLVEFAIL_ERROR,
        "OpenSpacePathProvider output is null");
  }
  if (thread_manager_ == nullptr) {
    const TL::common::Status status = Reset();
    if (!status.ok()) {
      return status;
    }
  }

  output->Reset();
  OpenSpaceThreadManagerDiagnostics manager_diagnostics;
  const uint64_t target_plan_id = thread_manager_->TargetPlan(target_input);
  const double start_time = NowSeconds();
  while (NowSeconds() - start_time <= config_.target_plan_timeout_s) {
    manager_diagnostics.wait_time_s = NowSeconds() - start_time;
    if (thread_manager_->PollTargetOutput(target_plan_id, output, nullptr,
                                          &manager_diagnostics)) {
      FillProviderDiagnostics(manager_diagnostics, "TARGET_READY",
                              diagnostics);
      return TL::common::Status::OK();
    }
    std::this_thread::sleep_for(
        std::chrono::duration<double>(kPollIntervalS));
  }

  manager_diagnostics.target_timed_out = true;
  manager_diagnostics.wait_time_s = NowSeconds() - start_time;
  manager_diagnostics.target_cancel_requested =
      thread_manager_->CancelTargetPlan(target_plan_id);
  manager_diagnostics.thread_path_ids = thread_manager_->GetThreadPathIds();
  FillProviderDiagnostics(manager_diagnostics, "TARGET_TIMEOUT", diagnostics);
  return TL::common::Status(
      TL::common::ErrorCode::CORE_PLANNING_SOLVEFAIL_ERROR,
      "OpenSpacePathProvider target plan timeout");
}

}  // namespace planning
}  // namespace TL
