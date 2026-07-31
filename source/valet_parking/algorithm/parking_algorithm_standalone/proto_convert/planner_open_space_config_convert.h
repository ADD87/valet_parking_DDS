/******************************************************************************
 * Planner Open Space Config Proto to C++ Conversion Header
 *
 * Description: Convert planning/proto/planner_open_space_config.proto to C++ structures
 * Original Proto: planning/proto/planner_open_space_config.proto
 * Priority: P0 (Core open space planning configuration)
 *
 * Date: 2025-12-19
 * Note: This file replaces Protobuf dependencies for standalone compilation.
 *       Configuration for Hybrid A*, NLP smoother, and distance approach.
 *****************************************************************************/

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace TL {
namespace planning {

// ========================================
// Enumerations
// ========================================

/**
 * @brief Dual variable warm-up solver mode
 */
enum class DualWarmUpMode {
  IPOPT = 0,      ///< Use IPOPT solver
  IPOPTQP = 1,    ///< Use IPOPT with QP
  OSQP = 2,       ///< Use OSQP solver
  DEBUG = 3,      ///< Debug mode
  SLACKQP = 4,    ///< QP with slack variables
};

/**
 * @brief Distance approach optimization mode
 */
enum class DistanceApproachMode {
  DISTANCE_APPROACH_IPOPT = 0,                   ///< Standard IPOPT
  DISTANCE_APPROACH_IPOPT_CUDA = 1,              ///< IPOPT with CUDA acceleration
  DISTANCE_APPROACH_IPOPT_FIXED_TS = 2,          ///< IPOPT with fixed time step
  DISTANCE_APPROACH_IPOPT_FIXED_DUAL = 3,        ///< IPOPT with fixed dual variables
  DISTANCE_APPROACH_IPOPT_RELAX_END = 4,         ///< IPOPT with relaxed end constraints
  DISTANCE_APPROACH_IPOPT_RELAX_END_SLACK = 5,   ///< IPOPT with relaxed end + slack
};

// ========================================
// Configuration Structures
// ========================================

/**
 * @brief IPOPT solver configuration
 */
struct IpoptConfig {
  int32_t ipopt_print_level = 0;                           ///< Print verbosity level
  int32_t mumps_mem_percent = 2000;                        ///< MUMPS memory percentage
  double mumps_pivtol = 1e-6;                              ///< MUMPS pivot tolerance
  int32_t ipopt_max_iter = 100;                            ///< Maximum iterations
  double ipopt_tol = 1e-8;                                 ///< Convergence tolerance
  double ipopt_acceptable_constr_viol_tol = 1e-1;          ///< Acceptable constraint violation
  double ipopt_min_hessian_perturbation = 1e-12;           ///< Minimum Hessian perturbation
  double ipopt_jacobian_regularization_value = 1e-7;       ///< Jacobian regularization
  std::string ipopt_print_timing_statistics = "yes";       ///< Print timing statistics
  std::string ipopt_alpha_for_y = "min";                   ///< Alpha for y strategy
  std::string ipopt_recalc_y = "no";                       ///< Recalculate y option
  double ipopt_mu_init = 0.1;                              ///< Initial barrier parameter
};

/**
 * @brief OSQP solver configuration
 */
struct OSQPConfig {
  double alpha = 1.0;          ///< Relaxation parameter
  double eps_abs = 1e-3;       ///< Absolute tolerance
  double eps_rel = 1e-3;       ///< Relative tolerance
  int32_t max_iter = 10000;    ///< Maximum iterations
  bool polish = true;          ///< Enable solution polishing
  bool osqp_debug_log = false; ///< Enable debug logging
};

/**
 * @brief FEM position deviation smoother configuration
 */
struct FemPosDeviationSmootherConfig {
  double weight_fem_pos_deviation = 1.0e10;            ///< Weight for FEM position deviation
  double weight_ref_deviation = 1.0;                   ///< Weight for reference deviation
  double weight_path_length = 1.0;                     ///< Weight for path length
  bool apply_curvature_constraint = false;             ///< Apply curvature constraints
  double weight_curvature_constraint_slack_var = 1.0e2; ///< Weight for curvature slack
  double curvature_constraint = 0.2;                   ///< Maximum curvature constraint
  bool use_sqp = false;                                ///< Use SQP solver
  double sqp_ftol = 1e-4;                              ///< SQP function tolerance
  double sqp_ctol = 1e-3;                              ///< SQP constraint tolerance
  int32_t sqp_pen_max_iter = 10;                       ///< SQP penalty max iterations
  int32_t sqp_sub_max_iter = 100;                      ///< SQP subproblem max iterations

  // OSQP settings
  int32_t max_iter = 500;                              ///< OSQP max iterations
  double time_limit = 0.0;                             ///< Time limit (0 = no limit)
  bool verbose = false;                                ///< Verbose output
  bool scaled_termination = true;                      ///< Scaled termination
  bool warm_start = true;                              ///< Enable warm start

  // IPOPT settings
  int32_t print_level = 0;                             ///< IPOPT print level
  int32_t max_num_of_iterations = 500;                 ///< IPOPT max iterations
  int32_t acceptable_num_of_iterations = 15;           ///< IPOPT acceptable iterations
  double tol = 1e-8;                                   ///< IPOPT tolerance
  double acceptable_tol = 1e-1;                        ///< IPOPT acceptable tolerance
};

/**
 * @brief Piecewise jerk speed optimizer configuration (simplified)
 */
struct PiecewiseJerkSpeedOptimizerConfig {
  double acc_weight = 1.0;        ///< Acceleration weight
  double jerk_weight = 1.0;       ///< Jerk weight
  double max_acc = 3.0;           ///< Maximum acceleration (m/s²)
  double max_jerk = 4.0;          ///< Maximum jerk (m/s³)
  double delta_t = 0.1;           ///< Time step (seconds)
};

/**
 * @brief Warm start configuration for Hybrid A*
 */
struct WarmStartConfig {
  // Hybrid A* grid parameters
  double xy_grid_resolution = 0.2;                     ///< XY grid resolution (m)
  double phi_grid_resolution = 0.05;                   ///< Heading grid resolution (rad)
  uint64_t next_node_num = 10;                         ///< Number of next nodes to explore
  double step_size = 0.5;                              ///< Step size for node expansion (m)

  // Hybrid A* penalties
  double path_forward_penalty = 0.0;                   ///< Forward driving penalty
  double path_back_penalty = 0.0;                      ///< Backward driving penalty
  double path_gear_switch_penalty = 10.0;              ///< Gear switching penalty
  double path_steer_penalty = 100.0;                   ///< Steering angle penalty
  double path_steer_change_penalty = 10.0;             ///< Steering change penalty
  double steer_margin_penalty = 3.0;                   ///< Steering margin penalty

  // Horizontal parking parameters
  double horizontal_xy_grid_resolution = 0.1;          ///< Horizontal XY resolution (m)
  double horizontal_phi_grid_resolution = 0.02;        ///< Horizontal heading resolution (rad)
  double horizontal_step_size = 0.2;                   ///< Horizontal step size (m)
  double min_one_direction_length = 0.2;               ///< Min one-direction length (m)

  // Grid A* for heuristic
  double grid_a_star_xy_resolution = 0.1;              ///< Grid A* resolution (m)
  double node_radius = 0.5;                            ///< Node radius for collision (m)
  PiecewiseJerkSpeedOptimizerConfig s_curve_config;    ///< Speed profile configuration

  // Exploration time limits
  double max_exploration_time = 10.0;                  ///< Max exploration time (s)
  double accept_exploration_time = 5.0;                ///< Acceptable exploration time (s)
  double dead_end_scenario_max_exploration_time = 15.0; ///< Dead-end scenario max time (s)

  // Geometry path parameters
  double extra_distance_for_geometry_path = 0.05;      ///< Extra distance for geometry path (m)
  double extra_distance_for_rs = 0.1;                  ///< Extra distance for Reeds-Shepp (m)

  // Obstacle handling
  double obstacle_filter_distance = 0.5;               ///< Obstacle filtering distance (m)
  double obstacle_distance_penalty = 1.0;              ///< Obstacle distance penalty
  double path_length_penalty = 1.0;                    ///< Path length penalty

  // Steering angle margins
  double max_steer_angle_margin = 1.570796326;         ///< Max steer angle margin (~90°)
  double vertical_spot_steer_angle_margin = 1.570796326; ///< Vertical spot margin (~90°)
  double lateral_spot_steer_angle_margin = 2.3561945;  ///< Lateral spot margin (~135°)
  double geometry_planner_steer_angle_margin = 0.785;  ///< Geometry planner margin (~45°)

  // Target and cutoff parameters
  double lateral_distance_to_target = 1.0;             ///< Lateral distance to target (m)
  double max_y_cut_off = 0.0;                          ///< Max Y cutoff (m)
  double horizontal_max_y_cut_off = 0.0;               ///< Horizontal max Y cutoff (m)
  double dead_end_scenario_max_y_cut_off = -1.0;       ///< Dead-end max Y cutoff (m)

  // Additional penalties
  double end_pose_diff_penalty = 0.0;                  ///< End pose difference penalty
  double reference_line_bias_penalty = 1.0;            ///< Reference line bias penalty
  double path_gear_switch_penalty_nns_adjust = 20.0;   ///< Gear switch penalty adjustment
};

/**
 * @brief Dual variable warm start configuration
 */
struct DualVariableWarmStartConfig {
  double weight_d = 1.0;                 ///< Weight for dual variable
  IpoptConfig ipopt_config;              ///< IPOPT solver configuration
  DualWarmUpMode qp_format = DualWarmUpMode::IPOPT; ///< QP solver format
  double min_safety_distance = 0.0;      ///< Minimum safety distance (m)
  bool debug_osqp = false;               ///< Enable OSQP debug mode
  double beta = 1.0;                     ///< Beta parameter
  OSQPConfig osqp_config;                ///< OSQP solver configuration
};

/**
 * @brief Distance approach optimization configuration
 */
struct DistanceApproachConfig {
  // Weight parameters
  double weight_steer = 1.0;             ///< Steering weight
  double weight_a = 1.0;                 ///< Acceleration weight
  double weight_steer_rate = 1.0;        ///< Steering rate weight
  double weight_a_rate = 1.0;            ///< Acceleration rate weight
  double weight_x = 1.0;                 ///< X position weight
  double weight_y = 1.0;                 ///< Y position weight
  double weight_phi = 1.0;               ///< Heading weight
  double weight_v = 1.0;                 ///< Velocity weight
  double weight_steer_stitching = 1.0;   ///< Steering stitching weight
  double weight_a_stitching = 1.0;       ///< Acceleration stitching weight
  double weight_first_order_time = 1.0;  ///< First-order time weight
  double weight_second_order_time = 1.0; ///< Second-order time weight

  // Constraints
  double min_safety_distance = 0.0;      ///< Minimum safety distance (m)
  double max_speed_forward = 3.0;        ///< Max forward speed (m/s)
  double max_speed_reverse = 2.0;        ///< Max reverse speed (m/s)
  double max_acceleration_forward = 2.0; ///< Max forward acceleration (m/s²)
  double max_acceleration_reverse = 2.0; ///< Max reverse acceleration (m/s²)

  // Time sampling
  double min_time_sample_scaling = 0.1;  ///< Min time sample scaling
  double max_time_sample_scaling = 10.0; ///< Max time sample scaling
  bool use_fix_time = false;             ///< Use fixed time horizon

  // Solver configuration
  IpoptConfig ipopt_config;              ///< IPOPT configuration

  // Flags
  bool enable_constraint_check = false;  ///< Enable constraint checking
  bool enable_hand_derivative = false;   ///< Enable hand-derived derivatives
  bool enable_derivative_check = false;  ///< Enable derivative checking
  bool enable_initial_final_check = false; ///< Enable initial/final state check
  bool enable_jacobian_ad = false;       ///< Enable Jacobian automatic differentiation
  bool enable_check_initial_state = false; ///< Enable initial state check

  // Mode
  DistanceApproachMode distance_approach_mode = DistanceApproachMode::DISTANCE_APPROACH_IPOPT;

  // Additional weights
  double weight_end_state = 0.0;         ///< End state weight
  double weight_slack = 0.0;             ///< Slack variable weight
};

/**
 * @brief Iterative anchoring smoother configuration
 */
struct IterativeAnchoringConfig {
  double interpolated_delta_s = 0.1;     ///< Interpolated delta s (m)
  int32_t reanchoring_trails_num = 50;   ///< Number of reanchoring trials
  double reanchoring_pos_stddev = 0.25;  ///< Position standard deviation (m)
  double reanchoring_length_stddev = 1.0; ///< Length standard deviation (m)

  bool estimate_bound = false;           ///< Estimate bound automatically
  double default_bound = 2.0;            ///< Default bound value (m)
  double vehicle_shortest_dimension = 1.04; ///< Vehicle shortest dimension (m)

  FemPosDeviationSmootherConfig fem_pos_deviation_smoother_config; ///< FEM smoother config
  double collision_decrease_ratio = 0.9; ///< Collision decrease ratio

  // Speed limits
  double max_forward_v = 2.0;            ///< Max forward velocity (m/s)
  double max_reverse_v = 2.0;            ///< Max reverse velocity (m/s)
  double max_forward_acc = 3.0;          ///< Max forward acceleration (m/s²)
  double max_reverse_acc = 2.0;          ///< Max reverse acceleration (m/s²)
  double max_acc_jerk = 4.0;             ///< Max acceleration jerk (m/s³)
  double delta_t = 0.2;                  ///< Time step (s)

  PiecewiseJerkSpeedOptimizerConfig s_curve_config; ///< Speed profile config

  // Direct move parameters
  double max_direct_move_forward_v = 2.0; ///< Max direct move forward velocity (m/s)
  double max_direct_move_reverse_v = 2.0; ///< Max direct move reverse velocity (m/s)

  // Smoothing parameters
  int32_t smooth_extend_size = 3;        ///< Smooth extend size
  double min_smooth_path_length = 0.20;  ///< Minimum smooth path length (m)
};

/**
 * @brief NLP path smoother configuration
 */
struct NlpPathSmootherConfig {
  bool enable_smoother = true;          ///< Enable NLP smoother
  int32_t max_iter_num = 100;            ///< Maximum iterations
  int32_t print_level = 0;               ///< Print level
  double max_cpu_time = 1.0;             ///< Max CPU time (s)
  double tol = 1e-4;                     ///< Tolerance
  double acceptable_tol = 1e-2;          ///< Acceptable tolerance
  int32_t acceptable_iter = 0.15;          ///< Acceptable iterations

  double interpolation_s = 0.1;          ///< Interpolation step (m)
  int32_t min_point_num = 8;            ///< Minimum point number

  // Weights
  double bias_weight = 0.5;              ///< Bias weight
  double kappa_weight = 10;             ///< Curvature weight
  double dkappa_weight = 20;            ///< Curvature derivative weight

  double output_path_ds = 0.1;           ///< Output path delta s (m)
  double lon_collision_buffer = 0.15;     ///< Longitudinal collision buffer (m)
  double lat_moving_buffer = 0.1;        ///< Lateral moving buffer (m)

  bool enable_automatic_differentiation = false; ///< Enable automatic differentiation
  uint64_t max_path_partition_size = 12;  ///< Max path partition size
};

/**
 * @brief Top-level planner open space configuration
 */
struct PlannerOpenSpaceConfig {
  DualVariableWarmStartConfig dual_variable_warm_start_config; ///< Dual variable warm start
  DistanceApproachConfig distance_approach_config;             ///< Distance approach config
  IterativeAnchoringConfig iterative_anchoring_smoother_config; ///< Iterative anchoring config

  float delta_t = 1.0;                            ///< Time step (s)
  bool enable_check_parallel_trajectory = false;  ///< Enable parallel trajectory check
  bool enable_linear_interpolation = false;       ///< Enable linear interpolation
};

}  // namespace planning
}  // namespace TL
