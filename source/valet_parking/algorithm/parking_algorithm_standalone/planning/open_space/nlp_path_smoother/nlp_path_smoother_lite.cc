#include "planning/open_space/nlp_path_smoother/nlp_path_smoother_lite.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#if defined(TL_ENABLE_NLP_SMOOTHER)
#include "coin/IpIpoptApplication.hpp"
#include "coin/IpTNLP.hpp"
#include "coin/IpTypes.hpp"
#include "coin/IpSmartPtr.hpp"
#endif

namespace MAGNA {
namespace planning {

namespace {
constexpr double kLargeBound = 1.0e19;
constexpr double kMinPointDistance = 1.0e-3;
constexpr double kMinKappa = 1.0e-4;

double NormalizeAngle(double angle) {
  while (angle > M_PI) {
    angle -= 2.0 * M_PI;
  }
  while (angle < -M_PI) {
    angle += 2.0 * M_PI;
  }
  return angle;
}

bool IsFinitePath(const std::vector<PathGearPair>& paths) {
  for (const auto& path_pair : paths) {
    if (path_pair.first.size() < 2U) {
      return false;
    }
    for (const auto& point : path_pair.first) {
      if (!std::isfinite(point.x) || !std::isfinite(point.y) ||
          !std::isfinite(point.theta) || !std::isfinite(point.kappa)) {
        return false;
      }
    }
  }
  return !paths.empty();
}

void SetFallback(NlpPathSmootherLiteDiagnostics* diagnostics,
                 const std::string& reason) {
  if (diagnostics == nullptr) {
    return;
  }
  diagnostics->success = false;
  diagnostics->fallback_reason = reason;
}

#if defined(TL_ENABLE_NLP_SMOOTHER)
class LiteNlpModel final : public Ipopt::TNLP {
 public:
  LiteNlpModel(std::vector<PathGearPair> paths,
               const NlpPathSmootherLiteConfig& config,
               const VehicleParam& vehicle_param,
               double x_min, double x_max, double y_min, double y_max)
      : paths_(std::move(paths)),
        config_(config),
        vehicle_param_(vehicle_param),
        point_count_(CountPoints(paths_)),
        max_kappa_(ComputeMaxKappa(vehicle_param_)),
        x_min_(x_min),
        x_max_(x_max),
        y_min_(y_min),
        y_max_(y_max) {
    BuildFlattenedPath();
  }

  bool get_nlp_info(Ipopt::Index& n, Ipopt::Index& m,
                    Ipopt::Index& nnz_jac_g, Ipopt::Index& nnz_h_lag,
                    IndexStyleEnum& index_style) override {
    n = 4 * static_cast<Ipopt::Index>(point_count_);
    m = 2 * static_cast<Ipopt::Index>(point_count_ - paths_.size()) +
        3 * static_cast<Ipopt::Index>(paths_.size() - 1U);
    nnz_jac_g = 11 * static_cast<Ipopt::Index>(point_count_ - paths_.size()) +
                6 * static_cast<Ipopt::Index>(paths_.size() - 1U);
    nnz_h_lag = 0;
    index_style = TNLP::C_STYLE;
    return true;
  }

  bool get_bounds_info(Ipopt::Index n, Ipopt::Number* x_l,
                       Ipopt::Number* x_u, Ipopt::Index m,
                       Ipopt::Number* g_l, Ipopt::Number* g_u) override {
    const Ipopt::Index total = static_cast<Ipopt::Index>(point_count_);
    const Ipopt::Index motion_edge_count = static_cast<Ipopt::Index>(
      point_count_ - paths_.size());
    const Ipopt::Index partition_seam_count = static_cast<Ipopt::Index>(
      paths_.size() - 1U);
    if (n != 4 * total ||
      m != 2 * motion_edge_count + 3 * partition_seam_count) {
      return false;
    }
    for (Ipopt::Index i = 0; i < total; ++i) {
      x_l[i] = x_min_;
      x_u[i] = x_max_;
      x_l[total + i] = y_min_;
      x_u[total + i] = y_max_;
      x_l[2 * total + i] = -kLargeBound;
      x_u[2 * total + i] = kLargeBound;
      x_l[3 * total + i] = -max_kappa_;
      x_u[3 * total + i] = max_kappa_;
    }
    const auto& first = paths_.front().first.front();
    const auto& last = paths_.back().first.back();
    x_l[0] = x_u[0] = first.x;
    x_l[total] = x_u[total] = first.y;
    x_l[2 * total] = x_u[2 * total] = first.theta;
    x_l[total - 1] = x_u[total - 1] = last.x;
    x_l[2 * total - 1] = x_u[2 * total - 1] = last.y;
    x_l[3 * total - 1] = x_u[3 * total - 1] = last.theta;
    for (Ipopt::Index i = 0; i < m; ++i) {
      g_l[i] = 0.0;
      g_u[i] = 0.0;
    }
    return true;
  }

  bool get_starting_point(Ipopt::Index n, bool init_x, Ipopt::Number* x,
                          bool init_z, Ipopt::Number* z_l,
                          Ipopt::Number* z_u, Ipopt::Index m,
                          bool init_lambda, Ipopt::Number* lambda) override {
    (void)init_z;
    (void)z_l;
    (void)z_u;
    (void)m;
    (void)init_lambda;
    (void)lambda;
    if (!init_x || n != 4 * static_cast<Ipopt::Index>(point_count_)) {
      return false;
    }
    for (std::size_t i = 0; i < point_count_; ++i) {
      x[i] = reference_[i].x;
      x[point_count_ + i] = reference_[i].y;
      x[2U * point_count_ + i] = reference_[i].theta;
      x[3U * point_count_ + i] =
          std::max(-max_kappa_, std::min(max_kappa_, reference_[i].kappa));
    }
    return true;
  }

  bool eval_f(Ipopt::Index n, const Ipopt::Number* x, bool new_x,
              Ipopt::Number& obj_value) override {
    (void)new_x;
    const std::size_t total = point_count_;
    if (n != 4 * static_cast<Ipopt::Index>(total)) {
      return false;
    }
    obj_value = 0.0;
    for (std::size_t i = 1U; i < total; ++i) {
      const double dx = x[i] - reference_[i].x;
      const double dy = x[total + i] - reference_[i].y;
      obj_value += config_.bias_weight * (dx * dx + dy * dy);
    }
    for (std::size_t i = 0U; i < total; ++i) {
      obj_value += config_.kappa_weight *
                   x[3U * total + i] * x[3U * total + i];
      if (i > 0U) {
        const double dk = x[3U * total + i] - x[3U * total + i - 1U];
        obj_value += config_.dkappa_weight * dk * dk;
      }
    }
    return true;
  }

  bool eval_grad_f(Ipopt::Index n, const Ipopt::Number* x, bool new_x,
                   Ipopt::Number* grad_f) override {
    (void)new_x;
    std::fill(grad_f, grad_f + n, 0.0);
    const std::size_t total = point_count_;
    for (std::size_t i = 1U; i < total; ++i) {
      grad_f[i] += 2.0 * config_.bias_weight * (x[i] - reference_[i].x);
      grad_f[total + i] +=
          2.0 * config_.bias_weight * (x[total + i] - reference_[i].y);
    }
    for (std::size_t i = 0U; i < total; ++i) {
      grad_f[3U * total + i] +=
          2.0 * config_.kappa_weight * x[3U * total + i];
      if (i > 0U) {
        const double dk = x[3U * total + i] - x[3U * total + i - 1U];
        grad_f[3U * total + i - 1U] -=
            2.0 * config_.dkappa_weight * dk;
        grad_f[3U * total + i] += 2.0 * config_.dkappa_weight * dk;
      }
    }
    return true;
  }

  bool eval_g(Ipopt::Index n, const Ipopt::Number* x, bool new_x,
              Ipopt::Index m, Ipopt::Number* g) override {
    (void)n;
    (void)new_x;
    (void)m;
    const std::size_t total = point_count_;
    std::size_t row = 0U;
    std::size_t global = 0U;
    for (const auto& path_pair : paths_) {
      const bool forward = path_pair.second == soc::GearPosition::GEAR_DRIVE;
      const double direction = forward ? 1.0 : -1.0;
      const std::size_t segment_start = global;
      for (std::size_t j = 1U; j < path_pair.first.size(); ++j) {
        const std::size_t current = segment_start + j;
        const std::size_t previous = current - 1U;
        const double dx = x[current] - x[previous];
        const double dy = x[total + current] - x[total + previous];
        const double theta = x[2U * total + previous];
        g[row++] = dx * std::sin(theta) - dy * std::cos(theta);
        g[row++] = direction * std::hypot(dx, dy) *
                       x[3U * total + previous] -
                   (x[2U * total + current] - theta);
      }
      global += path_pair.first.size();
    }
    global = paths_.front().first.size() - 1U;
    for (std::size_t i = 1U; i < paths_.size(); ++i) {
      const std::size_t next = global + 1U;
      g[row++] = x[global] - x[next];
      g[row++] = x[total + global] - x[total + next];
      g[row++] = x[2U * total + global] - x[2U * total + next];
      global += paths_[i].first.size();
    }
    return row == static_cast<std::size_t>(m);
  }

  bool eval_jac_g(Ipopt::Index n, const Ipopt::Number* x, bool new_x,
                  Ipopt::Index m, Ipopt::Index nele_jac, Ipopt::Index* i_row,
                  Ipopt::Index* j_col, Ipopt::Number* values) override {
    (void)n;
    (void)x;
    (void)new_x;
    (void)m;
    if (values == nullptr) {
      std::size_t row = 0U;
      std::size_t global = 0U;
      std::size_t entry = 0U;
      const std::size_t total = point_count_;
      for (const auto& path_pair : paths_) {
        const std::size_t segment_start = global;
        for (std::size_t j = 1U; j < path_pair.first.size(); ++j) {
          const std::size_t current = segment_start + j;
          const std::size_t previous = current - 1U;
          for (std::size_t column : {previous, current, total + previous,
                                     total + current, 2U * total + previous}) {
            i_row[entry] = static_cast<Ipopt::Index>(row);
            j_col[entry++] = static_cast<Ipopt::Index>(column);
          }
          for (std::size_t column : {previous, current, total + previous,
                                     total + current, 2U * total + previous,
                                     3U * total + previous}) {
            i_row[entry] = static_cast<Ipopt::Index>(row + 1U);
            j_col[entry++] = static_cast<Ipopt::Index>(column);
          }
          row += 2U;
        }
        global += path_pair.first.size();
      }
      global = paths_.front().first.size() - 1U;
      for (std::size_t i = 1U; i < paths_.size(); ++i) {
        const std::size_t next = global + 1U;
        for (std::size_t column : {global, next}) {
          i_row[entry] = static_cast<Ipopt::Index>(row);
          j_col[entry++] = static_cast<Ipopt::Index>(column);
        }
        ++row;
        for (std::size_t column : {total + global, total + next}) {
          i_row[entry] = static_cast<Ipopt::Index>(row);
          j_col[entry++] = static_cast<Ipopt::Index>(column);
        }
        ++row;
        for (std::size_t column : {2U * total + global,
                                   2U * total + next}) {
          i_row[entry] = static_cast<Ipopt::Index>(row);
          j_col[entry++] = static_cast<Ipopt::Index>(column);
        }
        ++row;
        global += paths_[i].first.size();
      }
      return static_cast<Ipopt::Index>(entry) == nele_jac;
    }
    std::fill(values, values + nele_jac, 0.0);
    // Ipopt accepts a zero Hessian and this model supplies the exact gradient.
    // The Jacobian values are populated by the same ordered sparsity pattern.
    std::size_t global = 0U;
    std::size_t entry = 0U;
    const std::size_t total = point_count_;
    for (const auto& path_pair : paths_) {
      const bool forward = path_pair.second == soc::GearPosition::GEAR_DRIVE;
      const double direction = forward ? 1.0 : -1.0;
      const std::size_t segment_start = global;
      for (std::size_t j = 1U; j < path_pair.first.size(); ++j) {
        const std::size_t current = segment_start + j;
        const std::size_t previous = current - 1U;
        const double dx = x[current] - x[previous];
        const double dy = x[total + current] - x[total + previous];
        const double theta = x[2U * total + previous];
        values[entry++] = -std::sin(theta);
        values[entry++] = std::sin(theta);
        values[entry++] = std::cos(theta);
        values[entry++] = -std::cos(theta);
        values[entry++] = dx * std::cos(theta) + dy * std::sin(theta);
        const double distance = std::max(kMinPointDistance, std::hypot(dx, dy));
        values[entry++] = -direction * dx / distance *
                          x[3U * total + previous];
        values[entry++] = direction * dx / distance *
                          x[3U * total + previous];
        values[entry++] = -direction * dy / distance *
                          x[3U * total + previous];
        values[entry++] = direction * dy / distance *
                          x[3U * total + previous];
        values[entry++] = 1.0;
        values[entry++] = direction * distance;
      }
      global += path_pair.first.size();
    }
    global = paths_.front().first.size() - 1U;
    for (std::size_t i = 1U; i < paths_.size(); ++i) {
      values[entry++] = 1.0;
      values[entry++] = -1.0;
      values[entry++] = 1.0;
      values[entry++] = -1.0;
      values[entry++] = 1.0;
      values[entry++] = -1.0;
      global += paths_[i].first.size();
    }
    return static_cast<Ipopt::Index>(entry) == nele_jac;
  }

  bool eval_h(Ipopt::Index n, const Ipopt::Number* x, bool new_x,
              Ipopt::Number obj_factor, Ipopt::Index m,
              const Ipopt::Number* lambda, bool new_lambda,
              Ipopt::Index nele_hess, Ipopt::Index* i_row,
              Ipopt::Index* j_col, Ipopt::Number* values) override {
    (void)n;
    (void)x;
    (void)new_x;
    (void)obj_factor;
    (void)m;
    (void)lambda;
    (void)new_lambda;
    (void)nele_hess;
    (void)i_row;
    (void)j_col;
    (void)values;
    return true;
  }

  void finalize_solution(Ipopt::SolverReturn status, Ipopt::Index n,
                         const Ipopt::Number* x, const Ipopt::Number* z_l,
                         const Ipopt::Number* z_u, Ipopt::Index m,
                         const Ipopt::Number* g, const Ipopt::Number* lambda,
                         Ipopt::Number obj_value,
                         const Ipopt::IpoptData* ip_data,
                         Ipopt::IpoptCalculatedQuantities* ip_cq) override {
    (void)z_l;
    (void)z_u;
    (void)m;
    (void)g;
    (void)lambda;
    (void)obj_value;
    (void)ip_data;
    (void)ip_cq;
    status_ = status;
    if (x != nullptr && n == 4 * static_cast<Ipopt::Index>(point_count_)) {
      optimum_.assign(x, x + n);
    }
  }

  Ipopt::SolverReturn status() const { return status_; }
  const std::vector<double>& optimum() const { return optimum_; }
  std::size_t point_count() const { return point_count_; }

 private:
  static std::size_t CountPoints(const std::vector<PathGearPair>& paths) {
    std::size_t count = 0U;
    for (const auto& path_pair : paths) {
      count += path_pair.first.size();
    }
    return count;
  }

  static double ComputeMaxKappa(const VehicleParam& vehicle_param) {
    const double wheel_angle = vehicle_param.max_steer_angle /
                               std::max(1.0, vehicle_param.steer_ratio);
    return std::max(kMinKappa, std::fabs(std::tan(wheel_angle) /
                                         vehicle_param.wheel_base));
  }

  void BuildFlattenedPath() {
    reference_.reserve(point_count_);
    for (const auto& path_pair : paths_) {
      for (const auto& point : path_pair.first) {
        reference_.push_back(point);
      }
    }
  }

  std::vector<PathGearPair> paths_;
  NlpPathSmootherLiteConfig config_;
  VehicleParam vehicle_param_;
  std::size_t point_count_{0U};
  double max_kappa_{1.0};
  double x_min_{0.0};
  double x_max_{0.0};
  double y_min_{0.0};
  double y_max_{0.0};
  std::vector<common::PathPoint> reference_;
  Ipopt::SolverReturn status_{Ipopt::UNASSIGNED};
  std::vector<double> optimum_;
};

bool IsSuccessful(Ipopt::SolverReturn status) {
  return status == Ipopt::SUCCESS || status == Ipopt::STOP_AT_ACCEPTABLE_POINT ||
         status == Ipopt::FEASIBLE_POINT_FOUND;
}
#endif
}  // namespace

NlpPathSmootherLite::NlpPathSmootherLite(
    const NlpPathSmootherLiteConfig& config, const VehicleParam& vehicle_param)
    : config_(config), vehicle_param_(vehicle_param) {}

NlpPathSmootherLite::~NlpPathSmootherLite() = default;

bool NlpPathSmootherLite::Smooth(
    const OpenSpacePathInput& input, OpenSpacePathOutput* output,
    NlpPathSmootherLiteDiagnostics* diagnostics) const {
  if (diagnostics != nullptr) {
    *diagnostics = NlpPathSmootherLiteDiagnostics();
    diagnostics->attempted = config_.enabled;
  }
  if (output == nullptr || !config_.enabled) {
    SetFallback(diagnostics, output == nullptr ? "null_output" : "disabled");
    return false;
  }
  if (!IsFinitePath(output->partitioned_path)) {
    SetFallback(diagnostics, "invalid_coarse_path");
    return false;
  }
  std::size_t point_count = 0U;
  for (const auto& path_pair : output->partitioned_path) {
    point_count += path_pair.first.size();
  }
  if (diagnostics != nullptr) {
    diagnostics->point_count = point_count;
  }
  if (point_count < 4U || input.xy_bounds.size() < 4U) {
    SetFallback(diagnostics, "insufficient_points_or_bounds");
    return false;
  }

#if !defined(TL_ENABLE_NLP_SMOOTHER)
  SetFallback(diagnostics, "solver_runtime_not_compiled");
  return false;
#else
  const auto start_time = std::chrono::steady_clock::now();
  double x_min = input.xy_bounds[0];
  double x_max = input.xy_bounds[1];
  double y_min = input.xy_bounds[2];
  double y_max = input.xy_bounds[3];
  if (input.xy_bounds_is_local) {
    const double c = std::cos(input.rotate_angle);
    const double s = std::sin(input.rotate_angle);
    const double local_x[2] = {x_min, x_max};
    const double local_y[2] = {y_min, y_max};
    x_min = y_min = std::numeric_limits<double>::max();
    x_max = y_max = -std::numeric_limits<double>::max();
    for (double lx : local_x) {
      for (double ly : local_y) {
        const double gx = lx * c - ly * s + input.translate_origin.x();
        const double gy = lx * s + ly * c + input.translate_origin.y();
        x_min = std::min(x_min, gx);
        x_max = std::max(x_max, gx);
        y_min = std::min(y_min, gy);
        y_max = std::max(y_max, gy);
      }
    }
  }
  x_min += config_.lateral_buffer;
  x_max -= config_.lateral_buffer;
  y_min += config_.lateral_buffer;
  y_max -= config_.lateral_buffer;
  if (!(x_min < x_max && y_min < y_max)) {
    SetFallback(diagnostics, "invalid_solver_bounds");
    return false;
  }

  auto model = Ipopt::SmartPtr<LiteNlpModel>(new LiteNlpModel(
      output->partitioned_path, config_, vehicle_param_, x_min, x_max, y_min,
      y_max));
  if (model->point_count() < 4U) {
    SetFallback(diagnostics, "insufficient_solver_points");
    return false;
  }
  auto application = Ipopt::SmartPtr<Ipopt::IpoptApplication>(
      IpoptApplicationFactory());
  application->Options()->SetIntegerValue("print_level", 0);
  application->Options()->SetIntegerValue("max_iter", config_.max_iterations);
  application->Options()->SetNumericValue("max_cpu_time", config_.max_cpu_time_s);
  application->Options()->SetStringValue("hessian_approximation", "limited-memory");
  application->Options()->SetNumericValue("tol", 1.0e-5);
  if (application->Initialize() != Ipopt::Solve_Succeeded) {
    SetFallback(diagnostics, "solver_initialize_failed");
    return false;
  }
  const Ipopt::ApplicationReturnStatus status = application->OptimizeTNLP(model);
  const auto elapsed = std::chrono::steady_clock::now() - start_time;
  if (diagnostics != nullptr) {
    diagnostics->solve_time_s =
        std::chrono::duration<double>(elapsed).count();
    diagnostics->solver_status = std::to_string(static_cast<int>(status));
  }
  if (!IsSuccessful(model->status()) || model->optimum().size() != 4U * point_count) {
    SetFallback(diagnostics, "solver_failed");
    return false;
  }

  const auto& optimum = model->optimum();
  std::size_t index = 0U;
  std::vector<PathGearPair> smoothed;
  smoothed.reserve(output->partitioned_path.size());
  for (const auto& path_pair : output->partitioned_path) {
    DiscretizedPath path;
    path.reserve(path_pair.first.size());
    for (std::size_t i = 0U; i < path_pair.first.size(); ++i, ++index) {
      common::PathPoint point = path_pair.first[i];
      point.x = optimum[index];
      point.y = optimum[point_count + index];
      point.theta = NormalizeAngle(optimum[2U * point_count + index]);
      point.kappa = optimum[3U * point_count + index];
      if (!std::isfinite(point.x) || !std::isfinite(point.y) ||
          !std::isfinite(point.theta) || !std::isfinite(point.kappa)) {
        SetFallback(diagnostics, "non_finite_solver_output");
        return false;
      }
      if (path.empty()) {
        point.s = 0.0;
      } else {
        point.s = path.back().s +
                  std::hypot(point.x - path.back().x, point.y - path.back().y);
      }
      path.push_back(point);
    }
    smoothed.emplace_back(std::move(path), path_pair.second);
  }
  const auto& original_start = output->partitioned_path.front().first.front();
  const auto& smoothed_start = smoothed.front().first.front();
  const auto& original_end = output->partitioned_path.back().first.back();
  const auto& smoothed_end = smoothed.back().first.back();
  if (!IsFinitePath(smoothed) ||
      std::hypot(smoothed_start.x - original_start.x,
                 smoothed_start.y - original_start.y) > 1.0e-5 ||
      std::hypot(smoothed_end.x - original_end.x,
                 smoothed_end.y - original_end.y) > 1.0e-5 ||
      std::fabs(NormalizeAngle(smoothed_end.theta - original_end.theta)) >
          1.0e-4) {
    SetFallback(diagnostics, "endpoint_validation_failed");
    return false;
  }
  output->partitioned_path = std::move(smoothed);
  output->has_smoothed = true;
  if (diagnostics != nullptr) {
    diagnostics->success = true;
    diagnostics->fallback_reason.clear();
  }
  return true;
#endif
}

}  // namespace planning
}  // namespace MAGNA