#include "planning/open_space/open_space_thread/open_space_thread_manager.h"

#include <algorithm>

namespace TL {
namespace planning {

OpenSpaceThreadManager::OpenSpaceThreadManager(
    const OpenSpaceThreadManagerConfig& config)
    : config_(config), target_worker_() {
  StartThreads();
}

OpenSpaceThreadManager::~OpenSpaceThreadManager() {
  StopThreads();
}

void OpenSpaceThreadManager::StartThreads() {
  target_worker_.stop.store(false);
  const std::size_t thread_num =
      std::max<std::size_t>(1U, config_.search_thread_num);
  search_workers_.reserve(thread_num);
  for (std::size_t i = 0U; i < thread_num; ++i) {
    search_workers_.emplace_back(
        std::make_unique<SearchWorker>(static_cast<int>(i)));
  }
  for (auto& worker : search_workers_) {
    worker->thread = std::thread(&OpenSpaceThreadManager::SearchLoop, this,
                                 worker.get());
  }
  target_worker_.thread =
      std::thread(&OpenSpaceThreadManager::TargetLoop, this);
}

void OpenSpaceThreadManager::Reset() {
  StopThreads();
  search_workers_.clear();
  {
    std::lock_guard<std::mutex> lock(target_worker_.mutex);
    target_worker_.stop.store(false);
    target_worker_.has_task = false;
    target_worker_.running = false;
    target_worker_.finished = true;
    target_worker_.plan_id = 0U;
    target_worker_.early_stop.reset();
    target_worker_.input = OpenSpacePathInput();
    target_worker_.output.Reset();
    target_worker_.debug = planning_internal::OpenSpaceDebug();
    target_worker_.used_candidate_result = false;
    target_worker_.generated_in_target_thread = false;
    target_worker_.cancel_requested = false;
  }
  {
    std::lock_guard<std::mutex> manager_lock(manager_mutex_);
    next_target_plan_id_ = 0U;
    search_plan_disabled_.store(false);
  }

  StartThreads();
}

void OpenSpaceThreadManager::PrePlan(
    const std::vector<OpenSpacePathInput>& candidate_inputs) {
  std::lock_guard<std::mutex> manager_lock(manager_mutex_);
  if (search_plan_disabled_.load()) {
    return;
  }

  for (const OpenSpacePathInput& input : candidate_inputs) {
    bool duplicate = false;
    for (const auto& existing_worker : search_workers_) {
      std::lock_guard<std::mutex> lock(existing_worker->mutex);
      if (existing_worker->path_id == input.path_id &&
          (existing_worker->has_task || existing_worker->running ||
           existing_worker->finished)) {
        duplicate = true;
        break;
      }
    }
    if (duplicate) {
      continue;
    }

    SearchWorker* selected_worker = nullptr;
    for (const auto& worker : search_workers_) {
      std::lock_guard<std::mutex> lock(worker->mutex);
      if (!worker->has_task && !worker->running && worker->path_id < 0) {
        selected_worker = worker.get();
        break;
      }
    }
    if (selected_worker == nullptr) {
      for (const auto& worker : search_workers_) {
        std::lock_guard<std::mutex> lock(worker->mutex);
        if (!worker->has_task && !worker->running) {
          selected_worker = worker.get();
          break;
        }
      }
    }
    if (selected_worker == nullptr) {
      continue;
    }

    std::lock_guard<std::mutex> lock(selected_worker->mutex);
    selected_worker->input = input;
    selected_worker->path_id = input.path_id;
    selected_worker->early_stop = std::make_shared<std::atomic<bool>>(false);
    selected_worker->output.Reset();
    selected_worker->debug = planning_internal::OpenSpaceDebug();
    selected_worker->has_task = true;
    selected_worker->finished = false;
    selected_worker->cv.notify_all();
  }
}

uint64_t OpenSpaceThreadManager::TargetPlan(
    const OpenSpacePathInput& target_input) {
  {
    std::lock_guard<std::mutex> manager_lock(manager_mutex_);
    search_plan_disabled_.store(true);
  }
  std::lock_guard<std::mutex> target_lock(target_worker_.mutex);
  if (target_worker_.early_stop != nullptr) {
    target_worker_.early_stop->store(true);
  }
  ++next_target_plan_id_;
  target_worker_.plan_id = next_target_plan_id_;
  target_worker_.input = target_input;
  target_worker_.early_stop = std::make_shared<std::atomic<bool>>(false);
  target_worker_.output.Reset();
  target_worker_.debug = planning_internal::OpenSpaceDebug();
  target_worker_.used_candidate_result = false;
  target_worker_.generated_in_target_thread = false;
  target_worker_.cancel_requested = false;
  target_worker_.has_task = true;
  target_worker_.finished = false;
  target_worker_.cv.notify_all();
  return next_target_plan_id_;
}

bool OpenSpaceThreadManager::CancelTargetPlan(uint64_t target_plan_id) {
  std::lock_guard<std::mutex> target_lock(target_worker_.mutex);
  if (target_worker_.plan_id != target_plan_id || target_worker_.finished) {
    return false;
  }
  target_worker_.cancel_requested = true;
  target_worker_.has_task = false;
  if (target_worker_.early_stop != nullptr) {
    target_worker_.early_stop->store(true);
  }
  search_plan_disabled_.store(false);
  target_worker_.cv.notify_all();
  return true;
}

bool OpenSpaceThreadManager::PollTargetOutput(
    uint64_t target_plan_id,
    OpenSpacePathOutput* output,
    planning_internal::OpenSpaceDebug* debug,
    OpenSpaceThreadManagerDiagnostics* diagnostics) {
  if (output == nullptr) {
    return false;
  }
  const std::vector<int> thread_path_ids = GetThreadPathIds();
  std::lock_guard<std::mutex> lock(target_worker_.mutex);
  if (diagnostics != nullptr) {
    diagnostics->target_plan_submitted = target_worker_.plan_id > 0U;
    diagnostics->target_output_ready =
        target_worker_.plan_id == target_plan_id && target_worker_.finished;
    diagnostics->target_used_candidate_result =
        target_worker_.used_candidate_result;
    diagnostics->target_generated_in_smooth_thread =
        target_worker_.generated_in_target_thread;
    diagnostics->target_cancel_requested = target_worker_.cancel_requested;
    diagnostics->search_plan_disabled = search_plan_disabled_.load();
    diagnostics->thread_path_ids = thread_path_ids;
  }
  if (target_worker_.plan_id != target_plan_id || !target_worker_.finished) {
    return false;
  }
  *output = target_worker_.output;
  if (debug != nullptr) {
    *debug = target_worker_.debug;
  }
  return true;
}

std::vector<int> OpenSpaceThreadManager::GetThreadPathIds() const {
  std::vector<int> path_ids;
  path_ids.reserve(search_workers_.size());
  for (const auto& worker : search_workers_) {
    std::lock_guard<std::mutex> lock(worker->mutex);
    path_ids.push_back(worker->path_id);
  }
  return path_ids;
}

void OpenSpaceThreadManager::StopThreads() {
  std::size_t joined_search_threads = 0U;
  bool joined_target_thread = false;
  for (auto& worker : search_workers_) {
    {
      std::lock_guard<std::mutex> lock(worker->mutex);
      worker->stop.store(true);
      worker->has_task = false;
      if (worker->early_stop != nullptr) {
        worker->early_stop->store(true);
      }
    }
    worker->cv.notify_all();
  }
  {
    std::lock_guard<std::mutex> lock(target_worker_.mutex);
    target_worker_.stop.store(true);
    target_worker_.has_task = false;
    target_worker_.cancel_requested = true;
    if (target_worker_.early_stop != nullptr) {
      target_worker_.early_stop->store(true);
    }
  }
  target_worker_.cv.notify_all();
  for (auto& worker : search_workers_) {
    if (worker->thread.joinable()) {
      worker->thread.join();
      ++joined_search_threads;
    }
  }
  if (target_worker_.thread.joinable()) {
    target_worker_.thread.join();
    joined_target_thread = true;
  }
  search_plan_disabled_.store(false);
  if (joined_search_threads > 0U || joined_target_thread) {
    AINFO << "OpenSpaceThreadManager stopped, search_threads="
          << joined_search_threads << ", target_thread_joined="
          << (joined_target_thread ? "true" : "false");
  }
}

void OpenSpaceThreadManager::SearchLoop(SearchWorker* worker) {
  if (worker == nullptr) {
    return;
  }
  while (!worker->stop.load()) {
    OpenSpacePathInput input;
    std::shared_ptr<std::atomic<bool>> early_stop;
    {
      std::unique_lock<std::mutex> lock(worker->mutex);
      worker->cv.wait(lock, [&]() {
        return worker->stop.load() || worker->has_task;
      });
      if (worker->stop.load()) {
        break;
      }
      input = worker->input;
      early_stop = worker->early_stop;
      if (early_stop == nullptr) {
        early_stop = std::make_shared<std::atomic<bool>>(false);
        worker->early_stop = early_stop;
      }
      worker->has_task = false;
      worker->running = true;
      worker->finished = false;
    }

    OpenSpacePathOutput output;
    OpenSpacePathGenerator generator(config_.hybrid_config,
                                     config_.vehicle_param);
    generator.Plan(*early_stop, input, &output);

    {
      std::lock_guard<std::mutex> lock(worker->mutex);
      worker->output = std::move(output);
      worker->debug = planning_internal::OpenSpaceDebug();
      worker->running = false;
      worker->finished = true;
      if (worker->early_stop == early_stop) {
        worker->early_stop.reset();
      }
    }
    ADEBUG << "OpenSpaceThreadManager search thread " << worker->id
           << " finished path_id=" << input.path_id;
  }
}

void OpenSpaceThreadManager::TargetLoop() {
  while (!target_worker_.stop.load()) {
    OpenSpacePathInput input;
    uint64_t plan_id = 0U;
    std::shared_ptr<std::atomic<bool>> early_stop;
    {
      std::unique_lock<std::mutex> lock(target_worker_.mutex);
      target_worker_.cv.wait(lock, [&]() {
        return target_worker_.stop.load() || target_worker_.has_task;
      });
      if (target_worker_.stop.load()) {
        break;
      }
      input = target_worker_.input;
      plan_id = target_worker_.plan_id;
      early_stop = target_worker_.early_stop;
      if (early_stop == nullptr) {
        early_stop = std::make_shared<std::atomic<bool>>(false);
        target_worker_.early_stop = early_stop;
      }
      target_worker_.has_task = false;
      target_worker_.running = true;
      target_worker_.finished = false;
    }

    OpenSpacePathOutput output;
    planning_internal::OpenSpaceDebug debug;
    bool used_candidate = false;
    if (TryGetCandidateOutputLocked(input.path_id, &output, &debug) &&
        output.error_msg.empty() && !output.partitioned_path.empty()) {
      used_candidate = true;
    }

    bool generated_in_target = false;
    if (!used_candidate) {
      OpenSpacePathGenerator generator(config_.hybrid_config,
                                       config_.vehicle_param);
      generator.Plan(*early_stop, input, &output);
      debug = planning_internal::OpenSpaceDebug();
      generated_in_target = true;
    }

    {
      std::lock_guard<std::mutex> lock(target_worker_.mutex);
      if (target_worker_.plan_id == plan_id && !early_stop->load()) {
        target_worker_.output = std::move(output);
        target_worker_.debug = std::move(debug);
        target_worker_.used_candidate_result = used_candidate;
        target_worker_.generated_in_target_thread = generated_in_target;
        target_worker_.running = false;
        target_worker_.finished = true;
        target_worker_.cancel_requested = false;
        if (target_worker_.early_stop == early_stop) {
          target_worker_.early_stop.reset();
        }
      } else if (target_worker_.plan_id == plan_id) {
        target_worker_.output.Reset();
        target_worker_.output.error_msg = "target plan cancelled";
        target_worker_.debug = planning_internal::OpenSpaceDebug();
        target_worker_.used_candidate_result = used_candidate;
        target_worker_.generated_in_target_thread = generated_in_target;
        target_worker_.running = false;
        target_worker_.finished = true;
        target_worker_.cancel_requested = true;
        if (target_worker_.early_stop == early_stop) {
          target_worker_.early_stop.reset();
        }
      }
    }
    search_plan_disabled_.store(false);
    ADEBUG << "OpenSpaceThreadManager target thread finished path_id="
           << input.path_id << ", used_candidate="
           << (used_candidate ? "true" : "false")
           << ", cancelled=" << (early_stop->load() ? "true" : "false");
  }
}

bool OpenSpaceThreadManager::TryGetCandidateOutputLocked(
    int path_id,
    OpenSpacePathOutput* output,
    planning_internal::OpenSpaceDebug* debug) {
  if (path_id < 0 || output == nullptr) {
    return false;
  }
  for (const auto& worker : search_workers_) {
    std::lock_guard<std::mutex> lock(worker->mutex);
    if (worker->path_id == path_id && worker->finished && !worker->running) {
      *output = worker->output;
      if (debug != nullptr) {
        *debug = worker->debug;
      }
      return true;
    }
  }
  return false;
}

double OpenSpaceThreadManager::NowSeconds() {
  return std::chrono::duration<double>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}

}  // namespace planning
}  // namespace TL
