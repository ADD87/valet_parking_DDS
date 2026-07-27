/******************************************************************************
 * Simplified VehicleConfigHelper implementation for standalone version
 *****************************************************************************/

// 独立编译改造：必须先包含pnc_point_convert.h定义struct PathPoint
// 避免pnc_types.h中的class PathPoint版本被使用（ODR问题）
#include "proto_convert/pnc_point_convert.h"
#include "common/configs/vehicle_config_helper.h"
#include "common/math/polygon2d.h"
#include "common/math/vec2d.h"
#include <cmath>

namespace TL {
namespace common {

VehicleConfig VehicleConfigHelper::vehicle_config_;
bool VehicleConfigHelper::is_init_ = false;

void VehicleConfigHelper::Init() {
  // Initialize with EP30 default parameters
  vehicle_config_.vehicle_param_ = TL::planning::LoadEP30VehicleParam();
  is_init_ = true;
}

void VehicleConfigHelper::Init(const std::string& config_file) {
  // For standalone, ignore config file and use hardcoded values
  Init();
}

void VehicleConfigHelper::Init(const VehicleConfig& vehicle_params) {
  vehicle_config_ = vehicle_params;
  is_init_ = true;
}

const VehicleConfig& VehicleConfigHelper::GetConfig() {
  if (!is_init_) {
    Init();
  }
  return vehicle_config_;
}

double VehicleConfigHelper::MinSafeTurnRadius() {
  const auto& param = GetConfig().vehicle_param();
  double lat_edge_to_center =
      std::max(param.left_edge_to_center, param.right_edge_to_center);
  double lon_edge_to_center =
      std::max(param.front_edge_to_center, param.back_edge_to_center);
  // planning::VehicleParam没有min_turn_radius成员，使用GetMinTurningRadius()计算
  double min_turn_radius = param.GetMinTurningRadius();
  return std::sqrt((lat_edge_to_center + min_turn_radius) *
                       (lat_edge_to_center + min_turn_radius) +
                   lon_edge_to_center * lon_edge_to_center);
}

common::math::Box2d VehicleConfigHelper::GetBoundingBox(
    const common::PathPoint& path_point) {
  const auto& vehicle_param = GetConfig().vehicle_param();
  double diff_truecenter_and_pointX = (vehicle_param.front_edge_to_center -
                                       vehicle_param.back_edge_to_center) /
                                      2.0;
  common::math::Vec2d true_center(
      path_point.x +
          diff_truecenter_and_pointX * std::cos(path_point.theta),
      path_point.y +
          diff_truecenter_and_pointX * std::sin(path_point.theta));
  common::math::Box2d box_2d(true_center, path_point.theta,
                             vehicle_param.length, vehicle_param.width);
  return box_2d;
}

common::math::Polygon2d VehicleConfigHelper::GetPolygon2dWithBuffer(
    double x, double y, double theta, double extra_length,
    double extra_width) {
  const auto& vehicle_param = GetConfig().vehicle_param();
  double diff_truecenter_and_pointX = (vehicle_param.front_edge_to_center -
                                       vehicle_param.back_edge_to_center) /
                                      2.0;
  common::math::Vec2d true_center(
      x + diff_truecenter_and_pointX * std::cos(theta),
      y + diff_truecenter_and_pointX * std::sin(theta));
  common::math::Box2d box_2d(true_center, theta,
                             vehicle_param.length + extra_length,
                             vehicle_param.width + extra_width);
  return common::math::Polygon2d(box_2d);
}

common::math::Polygon2d VehicleConfigHelper::GetPolygon2dWithBuffer(
    double x, double y, double theta) {
  return GetPolygon2dWithBuffer(x, y, theta, 0.0, 0.0);
}

// 独立编译改造：添加缺失的工具函数

double VehicleConfigHelper::GetRearToCenter() {
  const auto& p = GetConfig().vehicle_param();
  return (p.front_edge_to_center - p.back_edge_to_center) / 2.0;
}

double VehicleConfigHelper::GetDiagonal() {
  const auto& p = GetConfig().vehicle_param();
  return std::hypot(p.length, p.width);
}

common::math::Polygon2d VehicleConfigHelper::GetPolygon2dWithBuffer(
    double x, double y, double theta,
    double front_buffer, double rear_buffer,
    double left_buffer, double right_buffer) {
  const auto& vp = GetConfig().vehicle_param();
  double cos_t = std::cos(theta);
  double sin_t = std::sin(theta);
  double front = vp.front_edge_to_center + front_buffer;
  double rear = -(vp.back_edge_to_center + rear_buffer);
  double left = vp.left_edge_to_center + left_buffer;
  double right_d = -(vp.right_edge_to_center + right_buffer);
  std::vector<common::math::Vec2d> pts;
  pts.reserve(4);
  // FL, FR, RR, RL (逆时针)
  pts.emplace_back(x + front * cos_t - left * sin_t,
                   y + front * sin_t + left * cos_t);
  pts.emplace_back(x + front * cos_t - right_d * sin_t,
                   y + front * sin_t + right_d * cos_t);
  pts.emplace_back(x + rear * cos_t - right_d * sin_t,
                   y + rear * sin_t + right_d * cos_t);
  pts.emplace_back(x + rear * cos_t - left * sin_t,
                   y + rear * sin_t + left * cos_t);
  return common::math::Polygon2d(pts);
}

// 独立编译改造：GetBoundingBox(x, y, heading) 3参数重载
common::math::Box2d VehicleConfigHelper::GetBoundingBox(
    const double x, const double y, const double heading) {
  common::PathPoint pp;
  pp.x = x;
  pp.y = y;
  pp.theta = heading;
  return GetBoundingBox(pp);
}

// 独立编译改造：GetMirrorCirclesWithBuffer — 左右后视镜碰撞检测圆
// 原始实现返回2个圆（左镜/右镜），path_handle.cc 要求 circles.size() == 2
// EP30后视镜参数近似：mirror_to_center ≈ 0.5m（镜面中心到后轴纵向偏移），
// mirror_radius ≈ 0.15m（镜面突出半径）
std::vector<std::pair<common::math::Vec2d, double>>
VehicleConfigHelper::GetMirrorCirclesWithBuffer(
    double x, double y, double heading) {
  const auto& vp = GetConfig().vehicle_param();
  double cos_h = std::cos(heading);
  double sin_h = std::sin(heading);

  // 后视镜纵向位置：大约在A柱附近，距后轴 ~1.0m
  constexpr double kMirrorToCenter = 0.5;
  // 后视镜突出半径
  constexpr double kMirrorRadius = 0.15;
  // 后视镜和车身半宽的横向偏移
  double half_width = vp.width / 2.0;

  // RFUToENU: 左镜 (lateral = -half_width, longitudinal = mirror_to_center)
  // 右镜 (lateral = +half_width, longitudinal = mirror_to_center)
  // 注意：RFU坐标系中 lateral正为右，所以左为负
  double left_x = x + kMirrorToCenter * cos_h - (-half_width) * sin_h;
  double left_y = y + kMirrorToCenter * sin_h + (-half_width) * cos_h;
  double right_x = x + kMirrorToCenter * cos_h - half_width * sin_h;
  double right_y = y + kMirrorToCenter * sin_h + half_width * cos_h;

  std::vector<std::pair<common::math::Vec2d, double>> circles;
  circles.reserve(2);
  // 左镜
  circles.emplace_back(common::math::Vec2d(left_x, left_y), kMirrorRadius);
  // 右镜
  circles.emplace_back(common::math::Vec2d(right_x, right_y), kMirrorRadius);
  return circles;
}

// 独立编译改造：GetAllRadarPos — 返回空（泊车场景不使用雷达位置）
std::vector<common::math::Vec2d> VehicleConfigHelper::GetAllRadarPos(
    const double x, const double y, const double heading) {
  return {};
}

}  // namespace common
}  // namespace TL
