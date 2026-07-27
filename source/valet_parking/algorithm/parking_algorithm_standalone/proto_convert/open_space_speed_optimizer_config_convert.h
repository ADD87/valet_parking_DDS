/**
 * @file open_space_speed_optimizer_config_convert.h
 * @brief 独立编译改造：将 OpenSpaceSpeedOptimizerConfig proto 转换为 C++ 结构体
 * @description 替代 planning/proto/open_space_task_config.pb.h 中的
 *              OpenSpaceSpeedOptimizerConfig 消息
 *              所有字段采用 proto 定义的默认值
 * @date 2025-07-11
 */

#pragma once

#include <cstdint>
#include <string>

namespace TL {
namespace planning {

/**
 * @brief 开放空间速度优化器配置（独立编译版）
 * 原 Proto: message OpenSpaceSpeedOptimizerConfig (open_space_task_config.proto)
 */
struct OpenSpaceSpeedOptimizerConfig {

  // ========== 嵌套结构体 ==========

  /**
   * @brief 碰撞缓冲区配置
   * 原 Proto: message CollisionBufferInfo
   */
  struct CollisionBufferInfo {
    // 纵向缓冲
    double front_longitudinal_buffer = 0.4;
    double rear_longitudinal_buffer = 0.05;
    double front_longitudinal_buffer_lidar = 0.25;
    double rear_longitudinal_buffer_lidar = 0.05;
    double front_longitudinal_buffer_low_fs = 0.25;
    double rear_longitudinal_buffer_low_fs = 0.05;
    double front_longitudinal_buffer_outside_wheelmask = 0.30;
    double rear_longitudinal_buffer_outside_wheelmask = 0.30;

    // 横向缓冲
    double left_lateral_buffer = 0.1;
    double right_lateral_buffer = 0.1;
    double left_lateral_buffer_bigger = 0.20;
    double right_lateral_buffer_bigger = 0.20;
    double left_lateral_buffer_middle = 0.15;
    double right_lateral_buffer_middle = 0.15;
    double left_lateral_buffer_low_fs = 0.05;
    double right_lateral_buffer_low_fs = 0.15;
    double left_lateral_buffer_outside_wheelmask = 0.30;
    double right_lateral_buffer_outside_wheelmask = 0.30;

    CollisionBufferInfo() = default;
  };

  /**
   * @brief 速度边界信息
   * 原 Proto: message SpeedBoundInfo
   */
  struct SpeedBoundInfo {
    double max_sample_speed = 0.5;
    double min_sample_speed = 0.3;
    double max_sample_acc = 0.2;
    double min_sample_acc = 0.1;
    double max_sample_dec = -0.2;
    double min_sample_dec = -0.1;

    SpeedBoundInfo() = default;
  };

  /**
   * @brief 场景速度边界信息
   * 原 Proto: message ScenarioSpeedBoundInfo
   */
  struct ScenarioSpeedBoundInfo {
    SpeedBoundInfo forward_info;
    SpeedBoundInfo reverse_info;

    ScenarioSpeedBoundInfo() = default;
  };

  // ========== 预测参数 ==========
  double prediction_time_period = 5.0;
  double prediction_unit_t = 0.2;

  // ========== 碰撞缓冲区（按场景/方向） ==========
  CollisionBufferInfo vertical_forward_collision_buffer;
  CollisionBufferInfo vertical_backward_collision_buffer;
  CollisionBufferInfo parallel_forward_collision_buffer;
  CollisionBufferInfo parallel_backward_collision_buffer;
  bool is_consider_control_diff = true;
  int32_t bigger_buffer_safe_min_count = 30;

  // ========== 静态障碍物碰撞缓冲 ==========
  double collision_buffer_for_static_pedestrian = 0.5;
  double collision_buffer_for_static_vehicle = 0.3;
  double collision_buffer_for_other = 0.15;
  double longitudinal_filter_distance = 0.5;
  double lateral_filter_distance = 0.8;
  double path_collision_check_buffer = 0.15;
  double path_collision_risk_max_count = 3;
  double lateral_collision_stop_buffer = 0.1;
  double path_collision_risk_max_distance = 1.0;
  double path_collision_risk_max_distance_for_narrow_spot = 1.0;
  double path_collision_risk_max_distance_for_nns_adjust = 1.0;

  // ========== 移动障碍物碰撞缓冲 ==========
  double apa_moving_obstacle_front_buffer = 0.6;
  double apa_moving_obstacle_rear_buffer = 0.6;
  double apa_moving_obstacle_left_right_buffer = 0.6;

  // ========== HMI交互 ==========
  double restore_dis_buffer = 0.3;
  bool enable_wait_for_replan = false;
  double max_wait_time_for_replan = 5.0;
  double wait_obstacle_min_count = 30;
  double min_wait_obstacle_state_time = 0.3;
  double max_wait_time_for_replan_rpa_direct = 1.0;
  double min_wait_replan_state_time = 0.3;

  // ========== ST采样参数 ==========
  ScenarioSpeedBoundInfo rpa_direct_speed_bound_info;
  ScenarioSpeedBoundInfo rpa_speed_bound_info;
  ScenarioSpeedBoundInfo apa_speed_bound_info;
  ScenarioSpeedBoundInfo nns_adjust_speed_bound_info;
  double tarjectory_unit_t = 0.1;       // 注：原proto拼写错误，保持一致
  double speed_limit_unit_s = 0.2;
  double sample_unit_acc = 0.02;
  double sample_unit_max_v = 0.1;
  uint64_t sample_thread_size = 4;
  double efficiency_cost = 10;
  double acc_cost = 1;
  double jerk_cost = 10;
  double over_speed_cost_max = 100;
  double over_speed_cost_min = 10;
  double diff_cost = 10;
  uint32_t speed_limit_smooth_window_size = 3;

  // ========== 轮挡 ==========
  double wheel_mask_stop_accuracy = 0.1;
  double wheel_mask_to_wheel_base_distance = 0.25;

  // ========== 速度限制过滤缓冲 ==========
  double rear_fillter_buffer = 0.15;    // 注：原proto拼写错误(fillter)，保持一致
  double front_fillter_buffer = 0.15;

  // ========== 从 FLAGS 迁移的配置参数 ==========
  // 原 FLAGS_enable_record_debug
  bool enable_record_debug = false;
  // 原 FLAGS_publish_trajectory_points_number
  int publish_trajectory_points_number = 200;
  // 原 FLAGS_speed_upper_bound
  double speed_upper_bound = 2.0;

  OpenSpaceSpeedOptimizerConfig() = default;

  /**
   * @brief 获取默认配置实例
   */
  static OpenSpaceSpeedOptimizerConfig GetDefault() {
    return OpenSpaceSpeedOptimizerConfig();
  }
};

}  // namespace planning
}  // namespace TL
