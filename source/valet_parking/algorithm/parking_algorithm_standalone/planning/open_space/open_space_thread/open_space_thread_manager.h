#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

#include "common/file/log.h"
#include "planning/tasks/optimizers/open_space_path_generation/open_space_path_generator.h"
#include "proto_convert/planning_internal_convert.h"

namespace TL {
namespace planning {

struct OpenSpaceThreadManagerConfig {
  HybridAStarConfig hybrid_config;
  VehicleParam vehicle_param;
  std::size_t search_thread_num{4U};
  double max_wait_time_s{8.5};
};

struct OpenSpaceThreadManagerDiagnostics {
  bool target_plan_submitted{false};
  bool target_output_ready{false};
  bool target_used_candidate_result{false};
  bool target_generated_in_smooth_thread{false};
  bool target_timed_out{false};
  bool target_cancel_requested{false};
  bool search_plan_disabled{false};
  double wait_time_s{0.0};
  std::vector<int> thread_path_ids;
};

class OpenSpaceThreadManager {
 public:
  explicit OpenSpaceThreadManager(const OpenSpaceThreadManagerConfig& config);
  ~OpenSpaceThreadManager();

  OpenSpaceThreadManager(const OpenSpaceThreadManager&) = delete;
  OpenSpaceThreadManager& operator=(const OpenSpaceThreadManager&) = delete;

  void Reset();
  void PrePlan(const std::vector<OpenSpacePathInput>& candidate_inputs);
  uint64_t TargetPlan(const OpenSpacePathInput& target_input);
  bool CancelTargetPlan(uint64_t target_plan_id);
  bool PollTargetOutput(uint64_t target_plan_id,
                        OpenSpacePathOutput* output,
                        planning_internal::OpenSpaceDebug* debug,
                        OpenSpaceThreadManagerDiagnostics* diagnostics);
  std::vector<int> GetThreadPathIds() const;

 private:
  struct SearchWorker {
    explicit SearchWorker(int worker_id) : id(worker_id) {}

    int id{-1};
    mutable std::mutex mutex;
    std::condition_variable cv;
    std::thread thread;
    std::atomic<bool> stop{false};
    bool has_task{false};
    bool running{false};
    bool finished{true};
    int path_id{-1};
    std::shared_ptr<std::atomic<bool>> early_stop;
    OpenSpacePathInput input;
    OpenSpacePathOutput output;
    planning_internal::OpenSpaceDebug debug;
  };

  struct TargetWorker {
    mutable std::mutex mutex;
    std::condition_variable cv;
    std::thread thread;
    std::atomic<bool> stop{false};
    bool has_task{false};
    bool running{false};
    bool finished{true};
    uint64_t plan_id{0U};
    std::shared_ptr<std::atomic<bool>> early_stop;
    OpenSpacePathInput input;
    OpenSpacePathOutput output;
    planning_internal::OpenSpaceDebug debug;
    bool used_candidate_result{false};
    bool generated_in_target_thread{false};
    bool cancel_requested{false};
  };

  void StartThreads();
  void StopThreads();
  void SearchLoop(SearchWorker* worker);
  void TargetLoop();
  bool TryGetCandidateOutputLocked(int path_id,
                                   OpenSpacePathOutput* output,
                                   planning_internal::OpenSpaceDebug* debug);
  static double NowSeconds();

  OpenSpaceThreadManagerConfig config_;
  std::vector<std::unique_ptr<SearchWorker>> search_workers_;
  TargetWorker target_worker_;
  mutable std::mutex manager_mutex_;
  uint64_t next_target_plan_id_{0U};
  std::atomic<bool> search_plan_disabled_{false};
};

}  // namespace planning
}  // namespace TL
