/******************************************************************************
 * Vehicle Parameters for Parking Algorithm
 *
 * Description: Simplified vehicle geometric parameters
 * Original: proto/common/vehicle_config.proto
 * Date: 2025-12-18
 *
 * Note: Hard-coded typical vehicle dimensions for parking scenarios
 *****************************************************************************/

#pragma once

#include <iostream>
#include <cmath>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace TL {
namespace planning {

/**
 * @brief 方向盘转速限制分段表
 *
 * 映射车速到方向盘最大允许转速，用于曲率变化限速
 * 数据来源：vehicle_param.pb.txt 中的 steer_wheel_speed_segment
 */
struct SteerWheelSpeedSegment {
  std::vector<double> vehicle_speed_segment;              ///< 车速分段 (m/s)
  std::vector<double> steering_wheel_speed_limit_segment;  ///< 方向盘转速限制 (deg/s)

  bool is_valid() const {
    return !vehicle_speed_segment.empty() &&
           vehicle_speed_segment.size() == steering_wheel_speed_limit_segment.size();
  }
};

/**
 * @brief 车辆几何参数
 *
 * 定义车辆的几何尺寸，用于碰撞检测和路径规划
 * 默认值基于典型轿车尺寸
 */
struct VehicleParam {
  // ========== 基本尺寸 ==========
  // 以下默认值来自 common/data/vehicle_param/ep30/vehicle_param.pb.txt

  /// 车辆总长度（米）
  double length = 4.486;

  /// 车辆总宽度（米）
  double width = 1.86;

  /// 车辆高度（米） - 泊车规划通常不需要，但保留以备用
  double height = 1.628;

  // ========== 轴距和悬长 ==========

  /// 前后轴距离（米）
  double wheel_base = 2.77;

  /// 前悬长度（前轴到车头距离，米）
  double front_overhang = 0.84;  // 由 front_edge_to_center - wheel_base 计算得出

  /// 后悬长度（后轴到车尾距离，米）
  double rear_overhang = 0.876;

  // ========== 中心点偏移 ==========

  /// 车辆中心点到前边缘距离（米）
  /// 注意：中心点通常定义在后轴中点
  double front_edge_to_center = 3.61;

  /// 车辆中心点到后边缘距离（米）
  double back_edge_to_center = 0.876;

  /// 车辆中心点到左边缘距离（米）
  double left_edge_to_center = 0.93;

  /// 车辆中心点到右边缘距离（米）
  double right_edge_to_center = 0.93;

  // ========== 转向参数 ==========

  /// 方向盘最大转角（弧度）
  /// EP30实际：方向盘最大转角8.378弧度，前轮最大转角 = 8.378 / 16.67 = 0.5026弧度（≈28.8°）
  /// 注意：此值为方向盘转角，使用时需除以 steer_ratio 得到前轮转角
  double max_steer_angle = 8.378;  // EP30 方向盘最大转角，确保前轮转角≈28.8°，转弯半径≈4.8m

  /// 方向盘最大转角速率（弧度/秒）
  /// EP30实际：方向盘转角速率8.55211弧度/s，前轮转角速率 = 8.55211 / 16.67 = 0.5129弧度/s
  /// 注意：此值为方向盘转角速率，使用时需除以 steer_ratio 得到前轮转角速率
  double max_steer_angle_rate = 8.55211;  // EP30 方向盘转角速率

  /// 方向盘转角到车轮转角的比例（无量纲）
  /// steer_ratio = 方向盘转角 / 车轮转角
  double steer_ratio = 16.67;  // EP30 实际转向比

  // ========== 运动参数 ==========

  /// 最大速度（米/秒）- 泊车场景低速
  double max_velocity = 2.0;  // 7.2 km/h

  /// 最大加速度（米/秒²）
  double max_acceleration = 1.0;

  /// 最大减速度（米/秒²）
  double max_deceleration = 2.5;

  // ========== 安全边界 ==========

  /// 碰撞检测安全缓冲区（米）
  /// 在车辆轮廓外增加的安全距离
  double safety_buffer = 0.1;

  // ========== 方向盘转速限制 ==========

  /// 方向盘转速限制分段表（用于 GetDkappaSpeedLimit）
  SteerWheelSpeedSegment steer_wheel_speed_segment;

  /// 检查方向盘转速限制分段表是否有效
  bool has_steer_wheel_speed_segment() const {
    return steer_wheel_speed_segment.is_valid();
  }

  // ========== 构造函数 ==========
  VehicleParam() = default;

  /**
   * @brief 根据车辆中心计算四个角点的位置
   * @param center_x 中心点X坐标
   * @param center_y 中心点Y坐标
   * @param heading 车辆航向角（弧度）
   * @param corners 输出四个角点坐标 [FL, FR, RR, RL]
   */
  void GetCorners(double center_x, double center_y, double heading,
                  std::vector<std::pair<double, double>>* corners) const {
    if (corners == nullptr) return;
    corners->clear();
    corners->reserve(4);

    double cos_h = std::cos(heading);
    double sin_h = std::sin(heading);

    // 前左角点 (Front Left)
    double fl_x = center_x + front_edge_to_center * cos_h - left_edge_to_center * sin_h;
    double fl_y = center_y + front_edge_to_center * sin_h + left_edge_to_center * cos_h;
    corners->emplace_back(fl_x, fl_y);

    // 前右角点 (Front Right)
    double fr_x = center_x + front_edge_to_center * cos_h + right_edge_to_center * sin_h;
    double fr_y = center_y + front_edge_to_center * sin_h - right_edge_to_center * cos_h;
    corners->emplace_back(fr_x, fr_y);

    // 后右角点 (Rear Right)
    double rr_x = center_x - back_edge_to_center * cos_h + right_edge_to_center * sin_h;
    double rr_y = center_y - back_edge_to_center * sin_h - right_edge_to_center * cos_h;
    corners->emplace_back(rr_x, rr_y);

    // 后左角点 (Rear Left)
    double rl_x = center_x - back_edge_to_center * cos_h - left_edge_to_center * sin_h;
    double rl_y = center_y - back_edge_to_center * sin_h + left_edge_to_center * cos_h;
    corners->emplace_back(rl_x, rl_y);
  }

  /**
   * @brief 计算最小转弯半径
   * @return 最小转弯半径（米）
   * @note max_steer_angle 是方向盘转角，需要除以 steer_ratio 得到前轮转角
   */
  double GetMinTurningRadius() const {
    // 前轮转角 = 方向盘转角 / 转向比
    double front_wheel_angle = max_steer_angle / steer_ratio;
    if (std::abs(front_wheel_angle) < 1e-6) {
      return 1e9;  // 避免除以零
    }
    return wheel_base / std::tan(front_wheel_angle);
  }

  /**
   * @brief 验证参数合理性
   * @return true if valid
   */
  bool Validate() const {
    if (length <= 0.0 || length > 10.0) return false;  // 车长0-10米
    if (width <= 0.0 || width > 3.0) return false;     // 车宽0-3米
    if (wheel_base <= 0.0 || wheel_base > length) return false;
    // max_steer_angle是方向盘转角，可达到8.378 rad (约480°)
    if (max_steer_angle <= 0.0 || max_steer_angle > 10.0) return false;

    // 验证尺寸一致性
    double computed_length = front_edge_to_center + back_edge_to_center;
    if (std::abs(computed_length - length) > 0.1) return false;

    double computed_width = left_edge_to_center + right_edge_to_center;
    if (std::abs(computed_width - width) > 0.1) return false;

    return true;
  }

  /**
   * @brief 打印车辆参数信息
   */
  void Print() const {
    std::cout << "===== Vehicle Parameters =====" << std::endl;
    std::cout << "Dimensions: L=" << length << "m, W=" << width << "m" << std::endl;
    std::cout << "Wheelbase: " << wheel_base << "m" << std::endl;
    std::cout << "Steering Wheel Max Angle: " << max_steer_angle << " rad ("
              << (max_steer_angle * 180.0 / M_PI) << " deg)" << std::endl;
    double front_wheel_max_angle = max_steer_angle / steer_ratio;
    std::cout << "Front Wheel Max Angle: " << front_wheel_max_angle << " rad ("
              << (front_wheel_max_angle * 180.0 / M_PI) << " deg)" << std::endl;
    std::cout << "Steer Ratio: " << steer_ratio << std::endl;
    std::cout << "Min Turning Radius: " << GetMinTurningRadius() << "m" << std::endl;
    std::cout << "==============================" << std::endl;
  }
};

/**
 * @brief 从配置文件加载车辆参数（EP30）
 * @return EP30车型的车辆参数
 * @note 参数来源：common/data/vehicle_param/ep30/vehicle_param.pb.txt
 */
inline VehicleParam LoadEP30VehicleParam() {
  VehicleParam param;
  // EP30 实际参数（从 vehicle_param.pb.txt 提取）
  param.length = 4.486;
  param.width = 1.86;
  param.height = 1.628;
  param.wheel_base = 2.77;
  param.front_overhang = 0.84;  // front_edge_to_center - wheel_base = 3.61 - 2.77
  param.rear_overhang = 0.876;
  param.front_edge_to_center = 3.61;
  param.back_edge_to_center = 0.876;
  param.left_edge_to_center = 0.93;
  param.right_edge_to_center = 0.93;
  // 转向参数：pb.txt中的8.378是方向盘最大转角（弧度），需除以转向比得到前轮转角
  // 前轮最大转角 = 8.378 / 16.67 = 0.5026 弧度（≈28.8°），转弯半径≈4.8m
  // EP30 转向参数（来自 vehicle_param.pb.txt）
  // 注意：max_steer_angle 和 max_steer_angle_rate 存储的是方向盘转角
  param.max_steer_angle = 8.378;          // 方向盘最大转角（rad）
  param.max_steer_angle_rate = 8.55211;   // 方向盘最大转角速率（rad/s）
  param.steer_ratio = 16.67;              // 转向比（方向盘转角/前轮转角）
  param.max_velocity = 2.0;  // 泊车场景限速
  param.max_acceleration = 1.0;
  param.max_deceleration = 2.5;
  param.safety_buffer = 0.1;

  // 方向盘转速限制分段表（来自 EP40 车型配置，EP30 参数近似相同）
  // 数据源：common/data/vehicle_param/ep40/vehicle_param.pb.txt
  param.steer_wheel_speed_segment.vehicle_speed_segment = {
    0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 1.2, 1.5,
    5.55556, 8.33333, 11.1111, 13.8889, 16.6667, 22.2222, 27.7778, 30.5556
  };
  param.steer_wheel_speed_segment.steering_wheel_speed_limit_segment = {
    460.0, 400.0, 350.0, 320.0, 320.0, 300.0, 280.0, 280.0, 260.0,
    200.0, 70.0, 50.0, 30.0, 20.0, 12.0, 8.0, 7.0
  };

  return param;
}

/**
 * @brief 获取默认车辆参数（EP30）
 * @deprecated 请使用 LoadEP30VehicleParam()
 */
inline VehicleParam GetDefaultVehicleParam() {
  return LoadEP30VehicleParam();  // 统一使用 EP30 参数
}

/**
 * @brief 获取小型车参数
 */
inline VehicleParam GetSmallVehicleParam() {
  VehicleParam param;
  param.length = 4.2;
  param.width = 1.7;
  param.wheel_base = 2.5;
  param.front_edge_to_center = 3.3;
  param.back_edge_to_center = 0.9;
  param.left_edge_to_center = 0.85;
  param.right_edge_to_center = 0.85;
  return param;
}

/**
 * @brief 获取大型SUV参数
 */
inline VehicleParam GetLargeVehicleParam() {
  VehicleParam param;
  param.length = 5.2;
  param.width = 2.0;
  param.wheel_base = 3.0;
  param.front_edge_to_center = 4.1;
  param.back_edge_to_center = 1.1;
  param.left_edge_to_center = 1.0;
  param.right_edge_to_center = 1.0;
  param.max_steer_angle = 7.5;  // SUV方向盘转角通常更小（约450°）
  param.steer_ratio = 18.0;     // SUV转向比通常更大
  return param;
}

}  // namespace planning
}  // namespace TL
