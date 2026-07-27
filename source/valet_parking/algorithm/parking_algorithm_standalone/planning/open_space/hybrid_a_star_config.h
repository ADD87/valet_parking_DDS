/******************************************************************************
 * Hybrid A* Configuration for Parking Algorithm
 *
 * Description: Simplified configuration parameters for Hybrid A* search
 * Original: planning/proto/planner_open_space_config.proto
 * Date: 2025-12-18
 *
 * Note: Hard-coded parameters optimized for parking scenarios
 *****************************************************************************/

#pragma once

#include <iostream>
#include <cmath>

namespace TL {
namespace planning {

/**
 * @brief Hybrid A* Configuration Parameters
 *
 * 简化的Hybrid A*配置参数，针对泊车场景优化
 */
struct HybridAStarConfig {
  // ========== Grid Search Parameters ==========

  /// XY平面网格分辨率（米）
  /// 典型值: 0.1-0.3米，越小搜索越精细但计算量越大
  double xy_grid_resolution = 0.2;  // proto默认值

  /// 航向角网格分辨率（弧度）
  /// 典型值: 0.05-0.1弧度（约3-6度）
  double phi_grid_resolution = 0.05;  // proto默认值 ~2.86度

  // 注意：车辆参数（wheel_base, max_steer_angle等）应该从VehicleParam获取
  // 不在此配置中重复定义

  // ========== A* Search Parameters ==========

  /// 每个节点扩展时的子节点数量
  /// 典型值: 10-20，越大路径越多样但计算量越大
  int next_node_num = 10;  // proto默认值

  /// 扩展步长（米）
  /// 值越小，路径越平滑，碰撞检测越精确，但计算量越大
  /// 推荐范围：0.1-0.5
  double step_size = 0.5;  // proto默认值

  /// 前进方向惩罚系数（相对于倒车）
  /// proto: path_forward_penalty
  double traj_forward_penalty = 0.0;  // proto默认值

  /// 倒车方向惩罚系数
  /// proto: path_back_penalty
  double traj_back_penalty = 0.0;  // proto默认值

  /// 转向惩罚系数
  /// proto: path_steer_penalty
  /// 惩罚大转向角，促进路径平滑
  double traj_steer_penalty = 100.0;  // proto默认值

  /// 转向变化惩罚系数
  /// proto: path_steer_change_penalty
  /// 惩罚频繁转向变化，提高舒适性
  double traj_steer_change_penalty = 10.0;  // proto默认值

  /// 路径长度惩罚系数
  /// proto: path_length_penalty
  double path_length_penalty = 1.0;  // proto默认值

  /// 换挡惩罚（前进/倒车切换）
  /// proto: path_gear_switch_penalty
  double path_gear_switch_penalty = 10.0;  // proto默认值

  /// 终点姿态差异惩罚
  /// proto: end_pose_diff_penalty
  double end_pose_diff_penalty = 0.0;  // proto默认值

  /// 转向角度裕量惩罚
  /// proto: steer_margin_penalty
  double steer_margin_penalty = 3.0;  // proto默认值

  /// 最小单向行驶长度
  /// proto: min_one_direction_length
  double min_one_direction_length = 0.2;  // proto默认值

  /// 最大转向角裕度（弧度）
  /// proto: max_steer_angle_margin
  double max_steer_angle_margin = 1.570796326;  // proto默认值 (π/2，约90°)

  /// 垂直车位转向角裕度（弧度）
  /// proto: vertical_spot_steer_angle_margin
  double vertical_spot_steer_angle_margin = 1.570796326;  // proto默认值 (π/2，约90°)

  /// Y方向最大裁剪偏移（默认场景，米）
  /// proto: max_y_cut_off
  double max_y_cut_off = 0.0;  // proto默认值

  /// 死胡同场景Y方向最大裁剪偏移（米）
  /// proto: dead_end_scenario_max_y_cut_off
  double dead_end_scenario_max_y_cut_off = -1.0;  // proto默认值

  /// 横向车位转向角裕度（弧度）
  /// proto: lateral_spot_steer_angle_margin
  double lateral_spot_steer_angle_margin = 2.3561945;  // proto默认值 (约135°)

  /// 几何规划器转向角裕度（弧度）
  /// proto: geometry_planner_steer_angle_margin
  double geometry_planner_steer_angle_margin = 0.785;  // proto默认值 (π/4，约45°)

  /// 到目标的横向距离（米）
  /// proto: lateral_distance_to_target
  double lateral_distance_to_target = 1.0;  // proto默认值

  /// 横向车位步长（米）
  /// proto: horizontal_step_size
  double horizontal_step_size = 0.2;  // proto默认值

  /// 横向车位XY网格分辨率（米）
  /// proto: horizontal_xy_grid_resolution
  double horizontal_xy_grid_resolution = 0.1;  // proto默认值

  /// 横向车位Phi网格分辨率（弧度）
  /// proto: horizontal_phi_grid_resolution
  double horizontal_phi_grid_resolution = 0.02;  // proto默认值

  /// 横向车位Y方向最大裁剪偏移（米）
  /// proto: horizontal_max_y_cut_off
  double horizontal_max_y_cut_off = 0.0;  // proto默认值

  // ========== 新增字段（Phase 4补充） ==========

  /// NNS调整场景的换挡惩罚
  /// proto: path_gear_switch_penalty_nns_adjust
  double path_gear_switch_penalty_nns_adjust = 20.0;  // proto默认值

  /// 障碍物过滤距离（米）
  /// proto: obstacle_filter_distance
  double obstacle_filter_distance = 0.2;  // proto默认值0.5

  /// 障碍物过滤放大系数
  /// 用于计算 extra_filter_distance = fmax(obstacle_filter_distance, amplification_ratio * obstacle_radius)
  /// 参考: common/math/math_utils.cc GetMinDistance2ObstaclesSegments
  double amplification_ratio = 1.2;  // 默认值

  /// 最小安全距离（米）
  /// 车辆轮廓到障碍物的最小允许距离
  double min_safe_distance = 0.5;  // 默认值

  /// 目标区域容差 - X方向（米）
  /// 停车时允许的横向偏差
  double tolerance_x = 0.3;  // 默认值

  /// 目标区域容差 - Y方向（米）
  /// 停车时允许的纵向偏差
  double tolerance_y = 0.3;  // 默认值

  /// 目标区域容差 - 角度（弧度）
  /// 停车时允许的角度偏差
  double tolerance_angle = 0.3;  // 默认值（约17度）

  /// 泊车最大速度（m/s）
  /// 泊车过程中的速度上限
  double max_parking_speed = 2.0;  // 默认值

  /// 路径前进惩罚（与traj_forward_penalty一致）
  /// proto: path_forward_penalty
  double path_forward_penalty = 0.0;  // proto默认值

  /// 路径倒车惩罚（与traj_back_penalty一致）
  /// proto: path_back_penalty
  double path_back_penalty = 0.0;  // proto默认值

  /// 障碍物距离惩罚系数
  /// proto: obstacle_distance_penalty
  double obstacle_distance_penalty = 1.0;  // proto默认值

  /// 路径转向变化惩罚（与traj_steer_change_penalty一致）
  /// proto: path_steer_change_penalty
  double path_steer_change_penalty = 10.0;  // proto默认值

  /// 参考线偏移惩罚
  /// proto: reference_line_bias_penalty
  double reference_line_bias_penalty = 1.0;  // proto默认值

  /// 几何路径额外距离（米）
  /// proto: extra_distance_for_geometry_path
  double extra_distance_for_geometry_path = 0.05;  // proto默认值

  /// Reed-Shepp曲线额外距离（米）
  /// proto: extra_distance_for_rs
  double extra_distance_for_rs = 0.1;  // proto默认值

  /// 死胡同场景最大探索时间（秒）
  /// proto: dead_end_scenario_max_exploration_time
  double dead_end_scenario_max_exploration_time = 15.0;  // proto默认值

  /// 最大探索时间（秒）
  /// proto: max_exploration_time
  double max_exploration_time = 10.0;  // proto默认值

  /// 可接受探索时间（秒）
  /// proto: accept_exploration_time
  double accept_exploration_time = 5.0;  // proto默认值

  /// 横向RS转向角裕度（弧度）
  /// 注意：此字段不在proto中，为代码中使用的自定义字段
  /// 根据代码逻辑，应与 max_steer_angle_margin 或 vertical_spot_steer_angle_margin 保持一致
  double lateral_rs_steer_angle_margin = 1.570796326;  // π/2 (约90°)，与其他margin字段一致

  // ========== Search Termination Parameters ==========

  /// 目标容差：XY位置（米）
  double goal_xy_tolerance = 0.5;

  /// 目标容差：航向角（弧度）
  double goal_phi_tolerance = 0.1;  // ~5.7度

  /// 最大搜索迭代次数
  /// 防止无解情况下无限循环
  int max_iterations = 50000;

  /// 最大搜索时间（秒）
  double max_search_time = 25.0;  // 增加到 25 秒，允许更多探索

  // ========== 构造函数（默认值已在声明中设置） ==========
  HybridAStarConfig() = default;

  /**
   * @brief 验证配置参数合理性
   * @return true if valid, false otherwise
   */
  bool Validate() const {
    if (xy_grid_resolution <= 0.0 || xy_grid_resolution > 1.0) return false;
    if (phi_grid_resolution <= 0.0 || phi_grid_resolution > 0.2) return false;
    // 注意：wheel_base和max_steer_angle已移至VehicleParam，不在此校验
    if (step_size <= 0.0 || step_size > 2.0) return false;
    if (next_node_num <= 0 || next_node_num > 50) return false;
    if (max_iterations <= 0) return false;
    if (max_search_time <= 0.0) return false;
    return true;
  }

  /**
   * @brief 打印配置信息（用于调试）
   */
  void Print() const {
    std::cout << "===== Hybrid A* Configuration =====" << std::endl;
    std::cout << "Grid Resolution: xy=" << xy_grid_resolution
              << "m, phi=" << phi_grid_resolution << "rad" << std::endl;
    std::cout << "Search: nodes=" << next_node_num
              << ", step=" << step_size << "m" << std::endl;
    std::cout << "Limits: max_iter=" << max_iterations
              << ", max_time=" << max_search_time << "s" << std::endl;
    std::cout << "Note: Vehicle params from VehicleParam" << std::endl;
    std::cout << "===================================" << std::endl;
  }
};

/**
 * @brief 获取泊车场景的默认配置
 * @return 优化的泊车配置
 */
inline HybridAStarConfig GetParkingDefaultConfig() {
  HybridAStarConfig config;
  // 所有参数已在结构体中设置默认值
  return config;
}

/**
 * @brief 获取高精度泊车配置（更细腻但更慢）
 * @return 高精度配置
 */
inline HybridAStarConfig GetParkingHighPrecisionConfig() {
  HybridAStarConfig config;
  config.xy_grid_resolution = 0.1;     // 更细的网格
  config.phi_grid_resolution = 0.03;   // 更细的航向
  config.next_node_num = 15;           // 更多扩展节点
  config.step_size = 0.05;             // 更小步长（从0.3m改为0.05m）
  config.max_iterations = 100000;      // 更多迭代
  return config;
}

/**
 * @brief 获取快速泊车配置（粗糙但快速）
 * @return 快速配置
 */
inline HybridAStarConfig GetParkingFastConfig() {
  HybridAStarConfig config;
  config.xy_grid_resolution = 0.3;     // 粗网格
  config.phi_grid_resolution = 0.1;    // 粗航向
  config.next_node_num = 8;            // 少节点
  config.step_size = 0.3;              // 步长（从0.8m改为0.3m，仍需保证安全）
  config.max_iterations = 20000;       // 少迭代
  return config;
}

/**
 * @brief 获取大横向偏移场景配置（适应0-4米横向偏移 + 小障碍物0.1m）
 * @return 大偏移场景优化配置
 *
 * 策略：
 * - 使用适中的网格分辨率（平衡精度与速度）
 * - 增加搜索时间和节点扩展数以应对复杂路径
 * - 适度步长保证碰撞检测精度
 * - 小碰撞缓冲区以通过狭窄空间
 */
inline HybridAStarConfig GetParkingLargeOffsetConfig() {
  HybridAStarConfig config;

  // 网格分辨率：适中，平衡精度与速度
  config.xy_grid_resolution = 0.2;         // 20cm网格
  config.phi_grid_resolution = 0.05;       // ~2.86度

  // 搜索参数：增强探索能力
  config.step_size = 0.15;                 // 15cm步长（碰撞检测精度）
  config.next_node_num = 20;               // 增加扩展方向
  config.max_iterations = 50000;          // 增加迭代次数
  config.accept_exploration_time = 20.0;   // 20秒可接受时间
  config.max_exploration_time = 30.0;      // 30秒最大时间

  // 代价函数：适度惩罚，允许复杂路径
  config.traj_steer_penalty = 0.15;        // 减小转向惩罚
  config.traj_back_penalty = 1.5;          // 适度倒车惩罚
  config.traj_steer_change_penalty = 0.5;  // 转向变化惩罚

  return config;
}

}  // namespace planning
}  // namespace TL
