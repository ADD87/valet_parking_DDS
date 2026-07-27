/**
 * @file open_space_path_partition_config_convert.h
 * @brief 独立编译改造：将 OpenSpacePathPartitionConfig proto 转换为 C++ 结构体
 * @description 用于替代 proto/planning/open_space_task_config.pb.h 中的 OpenSpacePathPartitionConfig
 */

#pragma once

#include <cstdint>
#include <string>

namespace TL {
namespace planning {

/**
 * @brief OpenSpacePathPartitionConfig - 路径分段配置
 * 原 Proto: message OpenSpacePathPartitionConfig (open_space_task_config.proto)
 *
 * 默认值均来自 proto 定义中的 [default = xxx] 或 Apollo 配置文件
 */
struct OpenSpacePathPartitionConfig {
  // ========== 换挡轨迹参数 ==========
  double gear_shift_max_t = 3.0;
  double gear_shift_unit_t = 0.02;
  double gear_shift_period_duration = 1.0;
  uint64_t interpolated_pieces_num = 10;
  uint64_t initial_gear_check_horizon = 20;

  // ========== 搜索范围参数 ==========
  // heading_search_range: 过滤航向角差异过大的点
  double heading_search_range = 0.79;  // ~45度
  // heading_track_range: 过滤车辆航向与到路径点方向差异过大的点
  double heading_track_range = 1.05;   // ~60度
  double distance_search_range = 1.0e-6;

  // ========== IOU 匹配参数 ==========
  double heading_offset_to_midpoint = 0.1;
  double lateral_offset_to_midpoint = 0.1;
  double longitudinal_offset_to_midpoint = 0.1;
  double vehicle_box_iou_threshold_to_midpoint = 0.95;
  double linear_velocity_threshold_on_ego = 0.2;

  // ========== 粗匹配参数 ==========
  double rough_heading_offset_to_midpoint = 0.2;
  double rough_lateral_offset_to_midpoint = 0.2;
  double rough_longitudinal_offset_to_midpoint = 0.2;

  // ========== 启动响应时间 ==========
  double warm_start_response_time_threshold = 3.5;
  double cold_start_response_time_threshold = 0.5;
  double point_match_iou_threshold = 0.5;

  // ========== 终点判断阈值 ==========
  double is_earily_finish_distance_threshold = 1.0;
  double is_earily_finish_theta_threshold = 0.027;
  double is_near_destination_distance_threshold = 0.2;
  double is_near_destination_theta_threshold = 0.05;

  // ========== 超时/等待参数 ==========
  uint32_t destination_long_time_count = 100;
  uint32_t parkout_destination_long_time_count = 100;
  double early_blocked_replan_time_threshold = 5.0;
  double yaw_error_replan_time_threshold = 3.0;

  // ========== 精确到达参数 ==========
  double is_precisely_arrive_theta_threshold = 0.05;
  double reach_ideal_pose_threshold = 0.5;
  double lon_distance_to_wheel_mask_threshold = 0.1;

  // ========== 收敛/发散趋势参数 ==========
  double convergence_trend_yaw_error_threshold = 0.0262;
  double divergent_trend_yaw_error_threshold = 0.0088;

  // ========== 后视镜折叠参数 ==========
  double fold_mirror_depth_threshold = 1.0;

  // ========== MVP 扩展：独立编译新增参数 ==========
  bool enable_record_debug = false;  // 替代 FLAGS_enable_record_debug

  /**
   * @brief 打印配置信息
   */
  void Print() const {
    // 使用标准输出打印关键配置
  }

  /**
   * @brief 获取默认泊车配置
   */
  static OpenSpacePathPartitionConfig GetDefault() {
    return OpenSpacePathPartitionConfig{};
  }
};

}  // namespace planning
}  // namespace TL
