/******************************************************************************
 * Parking Space Information for Hybrid A* Planning
 *
 * Description: Simplified parking space and obstacle representation
 * Original: planning/common/open_space_info.h
 * Date: 2025-12-18
 *
 * Note: Lightweight alternative using Box2d for collision detection
 *****************************************************************************/

#pragma once

#include <vector>
#include <algorithm>
#include "common/math/box2d.h"
#include "common/math/vec2d.h"
#include "common/math/polygon2d.h"

namespace TL {
namespace planning {

/**
 * @brief 泊车障碍物表示
 *
 * 使用Box2d简化障碍物表示，替代复杂的多边形+线性约束
 */
struct ParkingObstacle {
  /// 障碍物包围盒
  common::math::Box2d box;

  /// 是否为边界障碍物（如停车场围墙）
  bool is_boundary = false;

  /// 是否为静态障碍物
  bool is_static = true;

  /// 障碍物ID（用于调试）
  int id = -1;

  ParkingObstacle() = default;

  ParkingObstacle(const common::math::Vec2d& center,
                  double heading,
                  double length,
                  double width)
      : box(center, heading, length, width) {}

  /**
   * @brief 检查与另一个Box是否碰撞
   */
  bool HasOverlapWith(const common::math::Box2d& other) const {
    return box.HasOverlap(other);
  }
};

/**
 * @brief 泊车空间信息
 *
 * 包含车位信息、障碍物列表和搜索边界
 * 替代Apollo的OpenSpaceInfo，大幅简化
 */
class ParkingSpaceInfo {
 public:
  ParkingSpaceInfo() = default;

  // ========== 车位信息 ==========

  /**
   * @brief 设置目标车位
   * @param vertices 车位4个顶点（逆时针顺序）
   * @param heading 车位朝向（弧度）
   */
  void SetParkingSpot(const std::vector<common::math::Vec2d>& vertices,
                      double heading) {
    if (vertices.size() != 4) {
      std::cerr << "ERROR: Parking spot must have 4 vertices" << std::endl;
      return;
    }
    parking_spot_vertices_ = vertices;
    parking_spot_heading_ = heading;

    // 计算车位中心点
    parking_spot_center_.set_x(0.0);
    parking_spot_center_.set_y(0.0);
    for (const auto& v : vertices) {
      parking_spot_center_ = parking_spot_center_ + v;
    }
    parking_spot_center_ = parking_spot_center_ / 4.0;

    // 计算车位尺寸
    double len1 = vertices[0].DistanceTo(vertices[1]);
    double len2 = vertices[1].DistanceTo(vertices[2]);
    parking_spot_length_ = std::max(len1, len2);
    parking_spot_width_ = std::min(len1, len2);
  }

  /**
   * @brief 简化设置：矩形车位
   * @param center 车位中心
   * @param heading 车位朝向
   * @param length 车位长度
   * @param width 车位宽度
   */
  void SetRectangularParkingSpot(const common::math::Vec2d& center,
                                 double heading,
                                 double length,
                                 double width) {
    parking_spot_center_ = center;
    parking_spot_heading_ = heading;
    parking_spot_length_ = length;
    parking_spot_width_ = width;

    // 生成4个顶点
    double cos_h = std::cos(heading);
    double sin_h = std::sin(heading);
    double half_len = length / 2.0;
    double half_wid = width / 2.0;

    parking_spot_vertices_.clear();
    parking_spot_vertices_.reserve(4);

    // 前左
    parking_spot_vertices_.emplace_back(
        center.x() + half_len * cos_h - half_wid * sin_h,
        center.y() + half_len * sin_h + half_wid * cos_h);
    // 前右
    parking_spot_vertices_.emplace_back(
        center.x() + half_len * cos_h + half_wid * sin_h,
        center.y() + half_len * sin_h - half_wid * cos_h);
    // 后右
    parking_spot_vertices_.emplace_back(
        center.x() - half_len * cos_h + half_wid * sin_h,
        center.y() - half_len * sin_h - half_wid * cos_h);
    // 后左
    parking_spot_vertices_.emplace_back(
        center.x() - half_len * cos_h - half_wid * sin_h,
        center.y() - half_len * sin_h + half_wid * cos_h);
  }

  // ========== 障碍物管理 ==========

  /**
   * @brief 添加障碍物
   */
  void AddObstacle(const ParkingObstacle& obstacle) {
    obstacles_.push_back(obstacle);
  }

  /**
   * @brief 添加矩形障碍物
   */
  void AddRectangleObstacle(const common::math::Vec2d& center,
                            double heading,
                            double length,
                            double width,
                            bool is_boundary = false) {
    ParkingObstacle obs;
    obs.box = common::math::Box2d(center, heading, length, width);
    obs.is_boundary = is_boundary;
    obs.id = static_cast<int>(obstacles_.size());
    obstacles_.push_back(obs);
  }

  /**
   * @brief 清空所有障碍物
   */
  void ClearObstacles() {
    obstacles_.clear();
  }

  // ========== 搜索边界 ==========

  /**
   * @brief 设置XY搜索边界
   * @param bounds [x_min, x_max, y_min, y_max]
   */
  void SetXYBounds(const std::vector<double>& bounds) {
    if (bounds.size() != 4) {
      std::cerr << "ERROR: XY bounds must have 4 values" << std::endl;
      return;
    }
    x_min_ = bounds[0];
    x_max_ = bounds[1];
    y_min_ = bounds[2];
    y_max_ = bounds[3];
  }

  void SetXYBounds(double x_min, double x_max, double y_min, double y_max) {
    x_min_ = x_min;
    x_max_ = x_max;
    y_min_ = y_min;
    y_max_ = y_max;
  }

  // ========== 碰撞检测 ==========

  /**
   * @brief 检查车辆Box是否与障碍物碰撞
   * @param vehicle_box 车辆包围盒
   * @return true if collision
   */
  bool IsInCollision(const common::math::Box2d& vehicle_box) const {
    // 检查与所有障碍物的碰撞
    for (const auto& obs : obstacles_) {
      if (obs.HasOverlapWith(vehicle_box)) {
        return true;  // 碰撞
      }
    }
    return false;
  }

  /**
   * @brief 检查点是否在搜索边界内
   * @param x X坐标
   * @param y Y坐标
   * @return true if within bounds
   */
  bool IsWithinBounds(double x, double y) const {
    return (x >= x_min_ && x <= x_max_ && y >= y_min_ && y <= y_max_);
  }

  /**
   * @brief 检查车辆Box是否越界
   * @param vehicle_box 车辆包围盒
   * @return true if out of bounds
   */
  bool IsOutOfBounds(const common::math::Box2d& vehicle_box) const {
    // 获取车辆4个角点
    std::vector<common::math::Vec2d> corners;
    vehicle_box.GetAllCorners(&corners);

    // 任何一个角点越界即判定为越界
    for (const auto& corner : corners) {
      if (!IsWithinBounds(corner.x(), corner.y())) {
        return true;
      }
    }
    return false;
  }

  // ========== Getters ==========

  const std::vector<common::math::Vec2d>& parking_spot_vertices() const {
    return parking_spot_vertices_;
  }

  const common::math::Vec2d& parking_spot_center() const {
    return parking_spot_center_;
  }

  double parking_spot_heading() const {
    return parking_spot_heading_;
  }

  double parking_spot_length() const {
    return parking_spot_length_;
  }

  double parking_spot_width() const {
    return parking_spot_width_;
  }

  const std::vector<ParkingObstacle>& obstacles() const {
    return obstacles_;
  }

  size_t obstacle_count() const {
    return obstacles_.size();
  }

  double x_min() const { return x_min_; }
  double x_max() const { return x_max_; }
  double y_min() const { return y_min_; }
  double y_max() const { return y_max_; }

  // ========== 调试与可视化 ==========

  /**
   * @brief 打印泊车空间信息
   */
  void Print() const {
    std::cout << "===== Parking Space Info =====" << std::endl;
    std::cout << "Parking Spot:" << std::endl;
    std::cout << "  Center: (" << parking_spot_center_.x() << ", "
              << parking_spot_center_.y() << ")" << std::endl;
    std::cout << "  Heading: " << parking_spot_heading_ << " rad" << std::endl;
    std::cout << "  Size: " << parking_spot_length_ << "m x "
              << parking_spot_width_ << "m" << std::endl;
    std::cout << "Obstacles: " << obstacles_.size() << std::endl;
    std::cout << "Bounds: [" << x_min_ << ", " << x_max_ << "] x ["
              << y_min_ << ", " << y_max_ << "]" << std::endl;
    std::cout << "==============================" << std::endl;
  }

  /**
   * @brief 验证数据完整性
   */
  bool Validate() const {
    if (parking_spot_vertices_.size() != 4) {
      std::cerr << "ERROR: Invalid parking spot vertices" << std::endl;
      return false;
    }
    if (parking_spot_length_ <= 0.0 || parking_spot_width_ <= 0.0) {
      std::cerr << "ERROR: Invalid parking spot dimensions" << std::endl;
      return false;
    }
    if (x_min_ >= x_max_ || y_min_ >= y_max_) {
      std::cerr << "ERROR: Invalid XY bounds" << std::endl;
      return false;
    }
    return true;
  }

 private:
  // 车位信息
  std::vector<common::math::Vec2d> parking_spot_vertices_;  // 4个顶点
  common::math::Vec2d parking_spot_center_;                 // 中心点
  double parking_spot_heading_ = 0.0;                       // 朝向（弧度）
  double parking_spot_length_ = 5.0;                        // 长度（米）
  double parking_spot_width_ = 2.5;                         // 宽度（米）

  // 障碍物列表
  std::vector<ParkingObstacle> obstacles_;

  // 搜索边界
  double x_min_ = -50.0;
  double x_max_ = 50.0;
  double y_min_ = -50.0;
  double y_max_ = 50.0;
};

}  // namespace planning
}  // namespace TL
