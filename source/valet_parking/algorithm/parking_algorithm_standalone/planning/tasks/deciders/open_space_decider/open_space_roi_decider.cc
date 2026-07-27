/******************************************************************************
 * Copyright 2019 The TL Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

/**
 * @file open_space_roi_decider.cc
 * @brief ROI决策器 - 独立编译改造版
 *
 * 独立编译改造（doc 130 八大改造规律）：
 *   1. Proto→convert headers
 *   2. getter去括号 → 直接成员访问 (vehicle_params_.width() → vehicle_params_.width)
 *   3. setter→赋值 (set_x(v) → x = v)
 *   4. 注入→直接 (injector->xxx → 移除)
 *   5. 去重依赖 (移除 Frame/DependencyInjector/Decider)
 *   6. absl→std
 *   7. 日志适配 (AERROR→LOG(ERROR), AINFO→LOG(INFO), ADEBUG→VLOG(2))
 *   8. 本地实现 (ENUToRFU 等辅助函数)
 *
 * 原始代码: open_space_decider_Original/open_space_roi_decider.cc (2905行)
 * 改造后: 保留核心泊入(PARKING_IN)逻辑，移除Framework依赖
 **/

#include "planning/tasks/deciders/open_space_decider/open_space_roi_decider.h"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstddef>
#include <limits>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "common/configs/vehicle_config_helper.h"
#include "common/math/box2d.h"
#include "common/math/line_segment2d.h"
#include "common/math/math_utils.h"
#include "common/math/polygon2d.h"
#include "common/math/vec2d.h"

// 独立编译改造：移除以下原始头文件
// #include "common/configs/config_gflags.h"
// #include "planning/common/planning_context.h"
// #include "planning/common/planning_gflags.h"
// P2改造：终点微调模块（doc 159）
#include "planning/tasks/deciders/open_space_decider/open_space_fine_tuning.h"
// #include "planning/tasks/deciders/open_space_decider/open_space_obstacle.h"
// #include "planning/tasks/task.h"
// #include "proto/xxx.pb.h"

namespace TL {
namespace planning {

using common::math::Box2d;
using common::math::LineSegment2d;
using common::math::NormalizeAngle;
using common::math::Polygon2d;
using common::math::Vec2d;

namespace {
constexpr double kEpsilon = 1.0e-3;
// 独立编译改造：kADCBoxEps 原定义在 open_space_obstacle.h
constexpr double kADCBoxEps = 0.001;
// FSP 感知区域半径（自车坐标系 x[-20,20] y[-20,20]）
constexpr double kFspSensorRange = 20.0;

double AverageEdgeLength(const Vec2d& a, const Vec2d& b,
                         const Vec2d& c, const Vec2d& d) {
  return 0.5 * (a.DistanceTo(b) + c.DistanceTo(d));
}

Vec2d ToLocalPoint(const Vec2d& point, const Vec2d& origin_point,
                   double origin_heading) {
  Vec2d local_point = point;
  local_point -= origin_point;
  local_point.SelfRotate(-origin_heading);
  return local_point;
}

void ExpandRoiBoundaryToIncludePoint(const Vec2d& local_point,
                                     const double padding,
                                     const double clip_x_min,
                                     const double clip_x_max,
                                     const double clip_y_min,
                                     const double clip_y_max,
                                     std::vector<double>* roi_xy_boundary) {
  if (roi_xy_boundary == nullptr || roi_xy_boundary->size() < 4) {
    return;
  }

  (*roi_xy_boundary)[0] = std::max(
      clip_x_min, std::min((*roi_xy_boundary)[0], local_point.x() - padding));
  (*roi_xy_boundary)[1] = std::min(
      clip_x_max, std::max((*roi_xy_boundary)[1], local_point.x() + padding));
  (*roi_xy_boundary)[2] = std::max(
      clip_y_min, std::min((*roi_xy_boundary)[2], local_point.y() - padding));
  (*roi_xy_boundary)[3] = std::min(
      clip_y_max, std::max((*roi_xy_boundary)[3], local_point.y() + padding));
}

double ComputeOpeningFacingScore(const ParkingLotVertexType& vertices,
                                 const Vec2d& vehicle_pos) {
  const Vec2d opening_mid = (vertices[0] + vertices[3]) * 0.5;
  const Vec2d rear_mid = (vertices[1] + vertices[2]) * 0.5;
  Vec2d inward_dir = rear_mid - opening_mid;
  const double inward_len = inward_dir.Length();
  if (inward_len < kEpsilon) {
    return std::numeric_limits<double>::infinity();
  }
  inward_dir /= inward_len;

  const Vec2d slot_center = (opening_mid + rear_mid) * 0.5;
  const double opening_projection =
      (vehicle_pos - opening_mid).InnerProd(inward_dir);
  const double center_projection =
      (vehicle_pos - slot_center).InnerProd(inward_dir);
  return opening_projection + 0.35 * center_projection;
}

double ComputeOpeningMidDistance(const ParkingLotVertexType& vertices,
                                 const Vec2d& vehicle_pos) {
  return (((vertices[0] + vertices[3]) * 0.5) - vehicle_pos).Length();
}

bool ShouldPreferOpeningCandidate(const ParkingLotVertexType& current,
                                  const ParkingLotVertexType& candidate,
                                  const Vec2d& vehicle_pos,
                                  const char* current_name,
                                  const char* candidate_name) {
  const double current_score = ComputeOpeningFacingScore(current, vehicle_pos);
  const double candidate_score =
      ComputeOpeningFacingScore(candidate, vehicle_pos);
  constexpr double kScoreSwitchMargin = 0.15;
  if (candidate_score + kScoreSwitchMargin < current_score) {
    LOG(INFO) << "[PARKING] 开口候选切换(" << current_name << " -> "
              << candidate_name << "): score=" << current_score << " -> "
              << candidate_score;
    return true;
  }
  if (current_score + kScoreSwitchMargin < candidate_score) {
    return false;
  }

  const double current_dist = ComputeOpeningMidDistance(current, vehicle_pos);
  const double candidate_dist = ComputeOpeningMidDistance(candidate, vehicle_pos);
  constexpr double kDistanceSwitchMargin = 0.05;
  if (candidate_dist + kDistanceSwitchMargin < current_dist) {
    LOG(INFO) << "[PARKING] 开口候选切换(" << current_name << " -> "
              << candidate_name << "): dist=" << current_dist << " -> "
              << candidate_dist;
    return true;
  }
  return false;
}

void NormalizeVerticesForSlotGeometry(
    const TL::perception::ParkingLotOut& parking_lot,
    ParkingLotVertexType* vertices,
    const Vec2d& vehicle_pos) {
  if (vertices == nullptr) {
    return;
  }

  if (parking_lot.type != TL::perception::ParkingLotOut::VERTICAL &&
      parking_lot.type != TL::perception::ParkingLotOut::OBLIQUE) {
    return;
  }

  // pair_a: v[0]→v[1], v[3]→v[2] 方向的平均边长
  // pair_b: v[0]→v[3], v[1]→v[2] 方向的平均边长
  const double pair_a_len = AverageEdgeLength(
      vertices->at(0), vertices->at(1), vertices->at(3), vertices->at(2));
  const double pair_b_len = AverageEdgeLength(
      vertices->at(0), vertices->at(3), vertices->at(1), vertices->at(2));

  // pair_a_is_width = true 表示 pair_a 实际是宽度方向(短边)，需要交换轴
  bool pair_a_is_width = false;
  if (parking_lot.hmi_width > kEpsilon && parking_lot.hmi_depth > kEpsilon) {
    const double keep_cost = std::fabs(pair_a_len - parking_lot.hmi_depth) +
                             std::fabs(pair_b_len - parking_lot.hmi_width);
    const double swap_cost = std::fabs(pair_a_len - parking_lot.hmi_width) +
                             std::fabs(pair_b_len - parking_lot.hmi_depth);
    pair_a_is_width = swap_cost + kEpsilon < keep_cost;
  } else {
    pair_a_is_width = pair_a_len + kEpsilon < pair_b_len;
  }

  if (!pair_a_is_width) {
    // 轴向正确，但仍需确认 top_edge (v[0]→v[3]) 是否真的是开口。
    // 这里直接比较“开口法向是否朝向自车”，比仅看边中心距离更稳。
    const ParkingLotVertexType keep_vertices = *vertices;
    ParkingLotVertexType flipped_vertices;
    flipped_vertices[0] = keep_vertices[2];
    flipped_vertices[1] = keep_vertices[3];
    flipped_vertices[2] = keep_vertices[0];
    flipped_vertices[3] = keep_vertices[1];
    if (ShouldPreferOpeningCandidate(
            keep_vertices, flipped_vertices, vehicle_pos, "top_edge", "bottom_edge")) {
      *vertices = flipped_vertices;
      LOG(INFO) << "[PARKING] 车位角点按开口朝向翻转 (bottom_edge 朝向自车)";
    }
    return;
  }

  // 需要交换轴：pair_a (v[0]→v[1], v[3]→v[2]) 是宽度方向(短边)
  // ROI 期望: top_edge = v[0]→v[3] 为宽度(开口)方向
  //           left_edge = v[0]→v[1] 为深度方向
  // 需要将一条短边映射到 top_edge 位置，并比较哪一个候选的开口法向真正朝向自车。

  const ParkingLotVertexType orig = *vertices;
  ParkingLotVertexType candidate_01;
  candidate_01[0] = orig[0];  // left_top → new left_top (开口左)
  candidate_01[1] = orig[3];  // right_top → new left_down (深度方向)
  candidate_01[2] = orig[2];  // right_down → new right_down (深度方向)
  candidate_01[3] = orig[1];  // left_down → new right_top (开口右)

  ParkingLotVertexType candidate_32;
  candidate_32[0] = orig[3];  // right_top → new left_top (开口左)
  candidate_32[1] = orig[0];  // left_top → new left_down (深度方向)
  candidate_32[2] = orig[1];  // left_down → new right_down (深度方向)
  candidate_32[3] = orig[2];  // right_down → new right_top (开口右)

  const bool use_32 = ShouldPreferOpeningCandidate(
      candidate_01, candidate_32, vehicle_pos, "v0v1", "v3v2");
  *vertices = use_32 ? candidate_32 : candidate_01;

  LOG(INFO) << "[PARKING] 车位角点轴向交换+开口朝向修正: pair_a=" << pair_a_len
            << "(宽) pair_b=" << pair_b_len << "(深)"
            << " hmi_w=" << parking_lot.hmi_width
            << " hmi_d=" << parking_lot.hmi_depth
            << " opening_edge=" << (use_32 ? "v3v2" : "v0v1");
}

double ComputePlannerTargetHeadingForSlot(const ParkingLotVertexType& vertices,
                                          const bool is_parking_inwards) {
  const double parking_spot_heading = (vertices[1] - vertices[0]).Angle();
  return is_parking_inwards ? parking_spot_heading
                            : NormalizeAngle(parking_spot_heading + M_PI);
}

ParkingLotVertexType BuildMirroredOpeningVertices(
    const ParkingLotVertexType& vertices) {
  ParkingLotVertexType mirrored = vertices;
  mirrored[0] = vertices[1];
  mirrored[1] = vertices[0];
  mirrored[2] = vertices[3];
  mirrored[3] = vertices[2];
  return mirrored;
}

void StabilizeSlotOpeningForLocalGoal(const RoiDeciderConfig& config,
                                      ParkingLotVertexType* vertices) {
  if (vertices == nullptr) {
    return;
  }

    const double current_target_theta = ComputePlannerTargetHeadingForSlot(
      *vertices, config.avp_enable_parking_inwards);
  const ParkingLotVertexType mirrored = BuildMirroredOpeningVertices(*vertices);
    const double mirrored_target_theta = ComputePlannerTargetHeadingForSlot(
      mirrored, config.avp_enable_parking_inwards);

  constexpr double kLocalThetaFlipThreshold = M_PI_2;
  constexpr double kSwitchMargin = 0.35;
    if (std::abs(current_target_theta) > kLocalThetaFlipThreshold &&
      std::abs(mirrored_target_theta) + kSwitchMargin <
        std::abs(current_target_theta)) {
    LOG(INFO) << "[PARKING] 稳定车位开口朝向: target_theta="
          << current_target_theta * 180.0 / M_PI << "° -> "
          << mirrored_target_theta * 180.0 / M_PI << "°";
    *vertices = mirrored;
  }
}
}  // namespace

// ============================================================
// 辅助函数：ENU→车辆坐标变换（独立编译改造：替代 common::math::ENUToRFU）
// 原始代码中 ENUToRFU 定义在 Apollo 框架中，本项目未提供
// ============================================================
static std::pair<double, double> ENUToVRF(
    double x_enu, double y_enu, double ref_x, double ref_y, double ref_heading) {
  double dx = x_enu - ref_x;
  double dy = y_enu - ref_y;
  double cos_h = std::cos(ref_heading);
  double sin_h = std::sin(ref_heading);
  // 旋转到车体坐标系 (x=前方, y=左方)
  double x_vrf = dx * cos_h + dy * sin_h;
  double y_vrf = -dx * sin_h + dy * cos_h;
  return {x_vrf, y_vrf};
}

// ============================================================
// 辅助函数：Sutherland-Hodgman 多边形裁剪（针对轴对齐矩形 AABB）
// 将 ENU 坐标系中的多边形顶点列表裁剪到 [xmin,xmax]×[ymin,ymax]
// 若多边形完全在范围外则返回空列表
// ============================================================
static std::vector<Vec2d> ClipPolygonToAabb(
    std::vector<Vec2d> poly,
    double xmin, double xmax, double ymin, double ymax) {
  if (poly.empty()) return poly;

  // Sutherland-Hodgman 单边裁剪
  // axis: 0=x轴方向, 1=y轴方向; val: 边界值; keep_pos: true保留>=val侧
  auto clip_edge = [](std::vector<Vec2d> pts, int axis,
                      double val, bool keep_pos) -> std::vector<Vec2d> {
    std::vector<Vec2d> out;
    const int n = static_cast<int>(pts.size());
    for (int i = 0; i < n; ++i) {
      const Vec2d& cur = pts[i];
      const Vec2d& prv = pts[(i + n - 1) % n];
      double c = (axis == 0) ? cur.x() : cur.y();
      double p = (axis == 0) ? prv.x() : prv.y();
      bool c_in = keep_pos ? (c >= val) : (c <= val);
      bool p_in = keep_pos ? (p >= val) : (p <= val);
      if (c_in) {
        if (!p_in && std::abs(c - p) > 1e-12) {
          double t = (val - p) / (c - p);
          if (axis == 0) out.emplace_back(val, prv.y() + t * (cur.y() - prv.y()));
          else           out.emplace_back(prv.x() + t * (cur.x() - prv.x()), val);
        }
        out.push_back(cur);
      } else if (p_in && std::abs(c - p) > 1e-12) {
        double t = (val - p) / (c - p);
        if (axis == 0) out.emplace_back(val, prv.y() + t * (cur.y() - prv.y()));
        else           out.emplace_back(prv.x() + t * (cur.x() - prv.x()), val);
      }
    }
    return out;
  };

  poly = clip_edge(poly, 0, xmin, true);   // x >= xmin
  poly = clip_edge(poly, 0, xmax, false);  // x <= xmax
  poly = clip_edge(poly, 1, ymin, true);   // y >= ymin
  poly = clip_edge(poly, 1, ymax, false);  // y <= ymax
  return poly;
}

// ============================================================
// 构造函数
// 独立编译改造：替代 OpenSpaceRoiDecider(TaskConfig, DependencyInjector)
// 原始: L80-94
// ============================================================
OpenSpaceRoiDecider::OpenSpaceRoiDecider(
    const VehicleParam& vehicle_params,
    const RoiDeciderConfig& config)
    : vehicle_params_(vehicle_params),
      config_(config) {
  road_width_ = config_.open_space_lane_width;
  // P1改造：初始化障碍物处理模块（doc 159）
  open_space_obstacle_ =
      std::make_shared<OpenSpaceObstacle>(vehicle_params_, obstacle_config_);
}

// ============================================================
// Process: 独立编译入口函数
// 独立编译改造：替代原始 Process(Frame*) (L160-273)
// 对应: roi_decider_lite.cpp::RunRoiDecider 的逻辑
// ============================================================
int OpenSpaceRoiDecider::Process(
    const TL::perception::ParkingLotOut& parking_lot,
    const TL::common::VehicleState& vehicle_state,
    RoiDeciderOutput* output) {
  if (nullptr == output) {
    LOG(ERROR) << "[PARKING] ROI Decider: output is nullptr";
    return -1;
  }

  // 初始化成员变量
  vehicle_state_ = vehicle_state;
  // 独立编译改造：init_adc_point_ 直接从 vehicle_state 初始化
  init_adc_point_.x = vehicle_state_.x;
  init_adc_point_.y = vehicle_state_.y;
  init_adc_point_.theta = vehicle_state_.heading;

  // 提取车位类型和4角点
  TL::perception::ParkingLotOut::ParkType park_type = parking_lot.type;
  ParkingLotVertexType vertices = ExtractVertices(
      parking_lot, Vec2d(vehicle_state_.x, vehicle_state_.y));

  if (park_type == TL::perception::ParkingLotOut::VERTICAL ||
      park_type == TL::perception::ParkingLotOut::OBLIQUE) {
    StabilizeSlotOpeningForLocalGoal(config_, &vertices);
  }

  LOG(INFO) << "[PARKING] ROI Decider: 车位类型=" << static_cast<int>(park_type);
  for (int i = 0; i < 4; ++i) {
    LOG(INFO) << "[PARKING]   角点[" << i << "]: (" << vertices[i].x()
              << ", " << vertices[i].y() << ")";
  }

  // Step 1: 车位校验
  // 原始: CheckReceiveParkinglot (L99-157)
  ParkLotStatus lot_status = NORMAL;
  CheckReceiveParkinglot(park_type, vertices, &lot_status);
  output->lot_status = lot_status;
  if (lot_status != NORMAL) {
    LOG(ERROR) << "[PARKING] 车位校验失败: status=" << lot_status;
    return -2;
  }
  LOG(INFO) << "[PARKING] 车位校验通过";

  // Step 2: 判断车位方位（左/右侧）
  // 原始: IsParkLotInRightSide (L1420-1429)
  bool is_right_side = IsParkLotInRightSide(vertices[0], vertices[3]);
  output->is_right_side = is_right_side;
  LOG(INFO) << "[PARKING] 车位方位: " << (is_right_side ? "右侧" : "左侧");

  // Step 3: 设置坐标原点
  // 原始: SetOrigin (inline in .h)
  SetOrigin(vertices, &output->origin_point, &output->origin_heading);
  LOG(INFO) << "[PARKING] 坐标原点: (" << output->origin_point.x() << ", "
            << output->origin_point.y() << "), heading="
            << output->origin_heading * 180.0 / M_PI << "°";

  // Step 4: 计算终点位姿
  // 原始: InitEndPoseBaseSlot → SetParkingSpotEndPose (L1431-1615)
  // 独立编译改造：仅泊入(PARKING_IN)场景
  output->is_parking_inwards = config_.avp_enable_parking_inwards;
  std::tuple<bool, Vec2d, Vec2d> wheel_mask = {false, Vec2d(), Vec2d()};
  SetParkingSpotEndPose(park_type, vertices, is_right_side,
                        wheel_mask, &output->end_pose,
                        &output->is_parking_inwards);
  LOG(INFO) << "[PARKING] 终点位姿: (" << output->end_pose.x << ", "
            << output->end_pose.y << "), theta="
            << output->end_pose.theta * 180.0 / M_PI << "°"
            << (output->is_parking_inwards ? " (车头入库)" : " (倒车入库)");

  // Step 5: 计算ROI边界
  // 原始: GetParkingBoundary (L2173-2218)
  std::vector<LineSegment2d> inner_roi_boundary;
  std::vector<LineSegment2d> outer_roi_boundary;

  // 可视化：先填充 vertices_roi_order 和 VRF Y 値
  output->vertices_roi_order.clear();
  for (const auto& v : vertices)
    output->vertices_roi_order.push_back({v.x(), v.y()});
  {
    // vertices[0]=left_top/TOP_LEFT, vertices[3]=right_top/TOP_RIGHT
    auto lt_vrf = ENUToVRF(vertices[0].x(), vertices[0].y(),
                           init_adc_point_.x, init_adc_point_.y, init_adc_point_.theta);
    auto rt_vrf = ENUToVRF(vertices[3].x(), vertices[3].y(),
                           init_adc_point_.x, init_adc_point_.y, init_adc_point_.theta);
    output->left_top_vrf_y  = lt_vrf.second;
    output->right_top_vrf_y = rt_vrf.second;
  }

  GetParkingBoundary(output->origin_point, output->origin_heading,
                     park_type, vertices, output->end_pose,
                     &inner_roi_boundary, &outer_roi_boundary,
                     &output->xy_bounds, output);
  LOG(INFO) << "[PARKING] ROI边界(局部坐标): ["
            << output->xy_bounds[0] << ", " << output->xy_bounds[1] << "] x ["
            << output->xy_bounds[2] << ", " << output->xy_bounds[3] << "]";

  // 存储 inner_roi_boundary（ENU），供 P1 AddVirtualObs 传入
  output->inner_roi_boundary_enu = inner_roi_boundary;
  // 将 outer_roi_boundary 存入 output（供 P1改造版追加到 obs_segments\uff09
  // outer_roi = inner_roi 外扩 0.5m，车身距 outer_roi >= kMinSafeDistance(0.5m)
  // 等价于车身内收在 inner_roi 内部（允许靠近但不越过 inner_roi 边界）
  output->roi_wall_segments.clear();
  for (const auto& seg : outer_roi_boundary)
    output->roi_wall_segments.push_back({seg, 0.0});

  // Step 6: 车辆在ROI内检查
  // 原始: IsVehicleInRoi (L2024-2041)
  bool in_roi = IsVehicleInRoi(output->origin_point, output->origin_heading,
                               output->xy_bounds);
  if (!in_roi) {
    LOG(WARNING) << "[PARKING] 车辆在ROI外，但继续执行";
  }

  // Step 7: 计算目标区域
  // 原始: CaculateDestRegion → CalculateParkingInRegion (L2486-2593)
  CaculateDestRegion(output->end_pose, park_type, vertices,
                     &output->dest_region);
  LOG(INFO) << "[PARKING] 目标区域角度范围: ["
            << std::get<1>(output->dest_region) * 180.0 / M_PI << "°, "
            << std::get<2>(output->dest_region) * 180.0 / M_PI << "°]";

  // Step 8: 场景类型决策
  // 原始: ParkingScenarioTypeDecision (L2665-2793)
  ParkingScenarioTypeDecision(is_right_side, park_type,
                              &output->scenario_type);
  LOG(INFO) << "[PARKING] 场景类型: "
            << static_cast<int>(output->scenario_type);

  LOG(INFO) << "[PARKING] ROI Decider 完成";
  return 0;
}

// ============================================================
// Process: P1改造版 - 带障碍物输入的重载
// 独立编译改造（P1，doc 159）：
//   调用基础 Process() → 初始化 OpenSpaceObstacle →
//   LoadObs() 过滤 → AddVirtualObs() 添加虚拟障碍
// ============================================================
int OpenSpaceRoiDecider::Process(
    const TL::perception::ParkingLotOut& parking_lot,
    const TL::common::VehicleState& vehicle_state,
    const ObstacleInput& obstacle_input,
    RoiDeciderOutput* output) {
  if (nullptr == output) {
    LOG(ERROR) << "[OBS_FILTER] Process(ObstacleInput): null output";
    return -1;
  }

  // Step 1: 调用基础 Process() 完成 ROI/终点/边界等核心计算
  int ret = Process(parking_lot, vehicle_state, output);
  if (ret != 0) {
    LOG(ERROR) << "[OBS_FILTER] 基础 Process() 失败: " << ret;
    return ret;
  }

  // Step 2: 提取车位角点（AddVirtualObs 及后续步骤需要）
  // 注：inner_roi_boundary 已通过 output->inner_roi_boundary_enu 传递给 AddVirtualObs
  ParkingLotVertexType vertices = ExtractVertices(
      parking_lot, Vec2d(vehicle_state.x, vehicle_state.y));

  // Step 3: 当前车辆位姿（init 和 cur 在泊入时相同）
  common::PathPoint cur_adc_pose;
  cur_adc_pose.x = vehicle_state.x;
  cur_adc_pose.y = vehicle_state.y;
  cur_adc_pose.theta = vehicle_state.heading;

  // Step 4: 初始化障碍物模块（从外部 ObstacleInput 加载）
  // 注：open_space_obstacle_ 在构造函数中已创建，此处直接 Init 清空并重载
  auto init_status = open_space_obstacle_->Init(obstacle_input, cur_adc_pose);
  if (!init_status.ok()) {
    LOG(ERROR) << "[OBS_FILTER] OpenSpaceObstacle::Init 失败: "
               << init_status.error_message();
    return -3;
  }

  // Step 5: 构造 ParkLotInfo
  ParkLotInfo park_lot_info;
  park_lot_info.vertices = vertices;
  park_lot_info.is_right_side = output->is_right_side;
  park_lot_info.park_type = parking_lot.type;
  // 独立编译改造（Bug修复）：is_narrow_spot 从车位宽度几何计算
  // 原始：origin_park_lot.park_size() == perception::ParkingLotOut::NARROW
  // 独立版：slot_width < vehicle_width + 0.6m（总安全余量 0.6m，即每侧 0.3m）
  {
    const double slot_width = (vertices[0] - vertices[3]).Length();  // left_top→right_top
    constexpr double kNarrowSpotMargin = 0.6;  // 安全余量（m）
    park_lot_info.is_narrow_spot =
        (slot_width < vehicle_params_.width + kNarrowSpotMargin) &&
        (parking_lot.type == TL::perception::ParkingLotOut::VERTICAL ||
         parking_lot.type == TL::perception::ParkingLotOut::OBLIQUE);
    LOG(INFO) << "[OBS_FILTER] slot_width=" << slot_width
              << "m, veh_width=" << vehicle_params_.width
              << "m, is_narrow_spot=" << park_lot_info.is_narrow_spot;
  }
  park_lot_info.is_high_quality_triggered = false;
  park_lot_info.status = output->lot_status;

  // Step 6: LoadObs — 从 input_obs_ 取预计算线段，应用 ROI 过滤
  ObsFilterMap obs_filter_map;  // P1简化：暂用空过滤（后续接入 SetObsFilterStrategy）
  std::vector<std::pair<LineSegment2d, double>> linked_obs, high_curb_obs, low_fs_obs;
  auto load_status = open_space_obstacle_->LoadObs(
      vertices, obs_filter_map,
      &output->obs_segments, &linked_obs, &high_curb_obs, &low_fs_obs);
  if (!load_status.ok()) {
    LOG(ERROR) << "[OBS_FILTER] LoadObs 失败: " << load_status.error_message();
    return -4;
  }
  output->linked_obs_segments = linked_obs;
  output->high_curb_obs_segments = high_curb_obs;

  // Step 7: AddVirtualObs — 在车位两侧添加纵向虚拟墙
  // 对齐原始代码：传入 inner_roi_boundary（ENU，N-1条非闭合边）作为虚拟障碍物
  // inner_roi_boundary 不含通道入口边（TransRoiAndSetBoundary 只生成 N-1 条边）
  // AddVirtualObs 内部会过滤掉与车辆重叠的线段，再以 buffer=epsilon 加入 obs_segments
  // 注意：此修改会导致右侧车位因通道侧壁约束过严而规划失败，左侧车位正常
  common::math::Box2d wheel_mask_box;  // 默认Box（侧方泊入用，is_consider_wheel_mask=false时不访问）
  open_space_obstacle_->AddVirtualObs(
      parking_lot.type, park_lot_info,
      true,                      // is_parking_in（当前仅支持泊入）
      output->is_parking_inwards,
      output->end_pose,
      output->inner_roi_boundary_enu,
      cur_adc_pose,              // init_pose（泊入时 init 同 cur）
      cur_adc_pose,              // cur_pose
      false,                     // is_consider_wheel_mask（P1暂不启用）
      wheel_mask_box,
      park_lot_info.is_narrow_spot,
      &output->obs_segments);
  output->virtual_obs_segments = open_space_obstacle_->virtual_obs();

  LOG(INFO) << "[OBS_FILTER] P1完成: obs_segments=" << output->obs_segments.size()
            << " (含虚拟障碍+inner_roi_boundary), roi_wall=" << output->roi_wall_segments.size() << "条";

  // ============================================================
  // Step 8: P2改造 - OpenSpaceFineTuning 终点微调（doc 159）
  // 独立编译改造：
  //   无 AVPStatus::ParkingType → is_parking_in=true（当前仅支持注入）
  //   无 OpenSpacePathInfo → 拆解参数传入
  //   无 injector_ → has_end_replaned_ 恒 false—小调凃无影响
  // ============================================================
  {
    if (open_space_fine_tuning_ == nullptr) {
      open_space_fine_tuning_ =
          std::make_shared<OpenSpaceFineTuning>(vehicle_params_, fine_tuning_config_);
    }

    common::PathPoint fine_tuned = output->end_pose;
    const bool is_parking_in = true;  // P2简化：当前只支持注入
    const bool is_entered_lateral_slot_domain = false;  // 默认：没有进入侧方车位域
    const bool is_consider_wheel_mask = false;          // P2简化：不启用轮罩遮罩
    common::math::LineSegment2d empty_ref_curb;        // 空参考边缘（注入时不需要）

    auto ft_status = open_space_fine_tuning_->Process(
        is_entered_lateral_slot_domain,
        is_consider_wheel_mask,
        is_parking_in,
        park_lot_info,
        output->obs_segments,
        output->origin_point,
        output->origin_heading,
        output->is_parking_inwards,
        output->linked_obs_segments,
        output->high_curb_obs_segments,
        cur_adc_pose,
        empty_ref_curb,
        &fine_tuned);

    if (ft_status.ok()) {
      output->fine_tuned_end_pose = fine_tuned;
      output->has_fine_tuned = true;
      LOG(INFO) << "[FINE_TUNE] P2微调完成: ("
                << fine_tuned.x << ", " << fine_tuned.y
                << ") theta=" << fine_tuned.theta * 180.0 / M_PI << "\xc2\xb0"
                << "  lat_tune=" << open_space_fine_tuning_->lat_fine_tune_dis()
                << " lon_tune=" << open_space_fine_tuning_->lon_fine_tune_dis();
    } else {
      LOG(WARNING) << "[FINE_TUNE] P2微调失败 (非致命), 使用原始终点: "
                   << ft_status.error_message();
      output->has_fine_tuned = false;
    }
  }

  // ============================================================
  // Step 9: P3改造 - 场景难度评估 + 泊出终点计算（doc 159）
  // 独立编译改造：
  //   ScenarioDifficultyDecision → 基于 is_narrow_spot 的简化版（无 FreeSpace）
  //   CaculateParkingOutTarget → 无 reference_line，使用终点位姿几何反向
  // ============================================================
  {
    // Step 9a: 场景难度评估（基于 park_lot_info.is_narrow_spot）
    output->scenario_difficulty = open_space_obstacle_->ScenarioDifficultyDecision(
        true,         // is_parking_in（当前仅支持泊入）
        park_lot_info);

    // Step 9b: 泊出终点（简化版：沿泊入终点反方向 + 固定偏移）
    // 原始 CaculateParkingOutTarget 依赖 reference_line::SLToXY（无法独立使用）
    // 简化策略：泊出目标 = 沿泊入终点朝向的反方向行驶 kParkOutOffset 米处
    // 实际场景中，泊出起点 = 当前车辆位置，此处仅计算参考终点供规划器使用
    // Bug修复：先取 base_pose，再从 base_pose.theta 计算 out_heading（逻辑一致性）
    constexpr double kParkOutOffset = 5.0;   // 泊出目标距终点的纵向偏移(m)
    const common::PathPoint& base_pose =
        output->has_fine_tuned ? output->fine_tuned_end_pose : output->end_pose;
    const double out_heading = common::math::NormalizeAngle(base_pose.theta + M_PI);
    output->parking_out_end_pose.x = base_pose.x + std::cos(out_heading) * kParkOutOffset;
    output->parking_out_end_pose.y = base_pose.y + std::sin(out_heading) * kParkOutOffset;
    output->parking_out_end_pose.theta = out_heading;

    LOG(INFO) << "[P3] 场景难度=" << output->scenario_difficulty
              << (output->scenario_difficulty == 0 ? " (正常)" : " (窄车位)")
              << " | 泊出终点: (" << output->parking_out_end_pose.x
              << ", " << output->parking_out_end_pose.y
              << ") heading=" << out_heading * 180.0 / M_PI << "\xc2\xb0";
  }

  // ============================================================
  // Step 10: P4改造 - 车位内FreeSpace几何检测（doc 159）
  // 独立编译改造：
  //   移除 has_valid_history_path_ / is_high_quality_triggered（依赖 frame_history）
  //   移除 FreeSpaceOutArray 依赖
  //   仅基于车辆到车位入口的距离判断是否进入车位内域
  // 原始: IsSlotInnerFsValid() L1033（依赖 has_valid_history_path_ + FS）
  // ============================================================
  {
    output->is_slot_inner_fs_valid = IsSlotInnerFsGeometric(
        vertices, output->is_parking_inwards);
    LOG(INFO) << "[P4] is_slot_inner_fs_valid=" << output->is_slot_inner_fs_valid
              << (output->is_slot_inner_fs_valid ? " (车辆已进入车位域)" : " (车辆在车位外)");
  }

  return 0;
}

// ============================================================
// ExtractVertices: ParkingLotOut → 4角点 Vec2d 数组
// 独立编译改造：与 roi_decider_lite.cpp 相同实现
// ============================================================
ParkingLotVertexType OpenSpaceRoiDecider::ExtractVertices(
    const TL::perception::ParkingLotOut& parking_lot,
    const Vec2d& vehicle_pos) {
  ParkingLotVertexType vertices;
  // 按 PSPoint::Position 枚举查找
  // 原始 ROI Decider 要求: [0]=left_top, [1]=left_down, [2]=right_down, [3]=right_top
  if (parking_lot.pts_vrf.size() >= 4) {
    Vec2d left_top, left_down, right_down, right_top;
    for (const auto& pt : parking_lot.pts_vrf) {
      Vec2d v(pt.point.x, pt.point.y);
      switch (pt.position) {
        case TL::perception::PSPoint::TOP_LEFT:
          left_top = v;
          break;
        case TL::perception::PSPoint::BOTTOM_LEFT:
          left_down = v;
          break;
        case TL::perception::PSPoint::BOTTOM_RIGHT:
          right_down = v;
          break;
        case TL::perception::PSPoint::TOP_RIGHT:
          right_top = v;
          break;
        default:
          break;
      }
    }
    vertices[0] = left_top;
    vertices[1] = left_down;
    vertices[2] = right_down;
    vertices[3] = right_top;
    NormalizeVerticesForSlotGeometry(parking_lot, &vertices, vehicle_pos);
  } else {
    LOG(ERROR) << "[PARKING] 车位角点数量不足: " << parking_lot.pts_vrf.size();
  }
  return vertices;
}

// ============================================================
// CheckReceiveParkinglot: 车位合法性校验
// 原始: L99-157
// 独立编译改造：
//   - config_.open_space_roi_decider_config().xxx() → config_.xxx
//   - vehicle_params_.width() → vehicle_params_.width
//   - AERROR → LOG(ERROR)
//   - CrossProd(Vec2d, Vec2d, Vec2d) → CrossProd(dx1, dy1, dx2, dy2)
// ============================================================
void OpenSpaceRoiDecider::CheckReceiveParkinglot(
    const perception::ParkingLotOut::ParkType& park_type,
    const ParkingLotVertexType& parking_spot_enu,
    ParkLotStatus* const park_lot_status_ptr) {
  if (nullptr == park_lot_status_ptr) {
    return;
  }
  // 凸多边形检查
  if (!Polygon2d::IsConvexPolygon(
          {parking_spot_enu.at(0), parking_spot_enu.at(1),
           parking_spot_enu.at(2), parking_spot_enu.at(3)})) {
    LOG(ERROR) << "[PARKING] 车位非凸多边形！";
    *park_lot_status_ptr = NONCONVEX;
    return;
  }
  // 共线检查
  int spot_vertice_num = static_cast<int>(parking_spot_enu.size());
  for (int i = 0; i < spot_vertice_num; ++i) {
    int prev_idx = (i - 1 + spot_vertice_num) % spot_vertice_num;
    int next_idx = (i + 1) % spot_vertice_num;
    // 独立编译改造：CrossProd(Vec2d, Vec2d, Vec2d) → 4参数形式
    // 原始: CrossProd(parking_spot_enu[i], parking_spot_enu[prev_idx],
    //                  parking_spot_enu[next_idx])
    // 等价于: (i - prev) × (next - prev)
    double dx1 = parking_spot_enu.at(i).x() - parking_spot_enu.at(prev_idx).x();
    double dy1 = parking_spot_enu.at(i).y() - parking_spot_enu.at(prev_idx).y();
    double dx2 = parking_spot_enu.at(next_idx).x() - parking_spot_enu.at(prev_idx).x();
    double dy2 = parking_spot_enu.at(next_idx).y() - parking_spot_enu.at(prev_idx).y();
    if (std::fabs(common::math::CrossProd(dx1, dy1, dx2, dy2)) < kEpsilon) {
      LOG(ERROR) << "[PARKING] 车位角点共线！";
      *park_lot_status_ptr = SMALL;
      return;
    }
  }

  // 尺寸检查
  const auto& left_top = parking_spot_enu.at(0);
  const auto& left_down = parking_spot_enu.at(1);
  const auto& right_down = parking_spot_enu.at(2);
  const auto& right_top = parking_spot_enu.at(3);
  // 独立编译改造：vehicle_params_.width() → vehicle_params_.width
  double min_width = vehicle_params_.width;
  double top_to_down_dis =
      0.5 * (left_top.DistanceTo(left_down) + right_top.DistanceTo(right_down));
  double left_to_right_dis =
      0.5 * (left_top.DistanceTo(right_top) + left_down.DistanceTo(right_down));

  if (park_type == TL::perception::ParkingLotOut::VERTICAL ||
      park_type == TL::perception::ParkingLotOut::OBLIQUE) {
    // 独立编译改造：config_.open_space_roi_decider_config().xxx() → config_.xxx
    min_width += config_.vertical_min_parklot_lateral_buffer;
    if (top_to_down_dis < vehicle_params_.length +
                              config_.vertical_slot_longitudinal_buffer ||
        min_width - left_to_right_dis > kEpsilon) {
      LOG(ERROR) << "[PARKING] 垂直/斜向车位尺寸过小！depth="
                 << top_to_down_dis << " width=" << left_to_right_dis;
      *park_lot_status_ptr = SMALL;
    }
  } else if (park_type == TL::perception::ParkingLotOut::LATERAL) {
    min_width += config_.lateral_min_parklot_lateral_buffer;
    if (top_to_down_dis < min_width ||
        left_to_right_dis < vehicle_params_.length +
                                config_.lateral_slot_longitudinal_buffer) {
      LOG(ERROR) << "[PARKING] 侧方车位尺寸过小！";
      *park_lot_status_ptr = SMALL;
    }
  }
}

// ============================================================
// IsParkLotInRightSide: 判断车位在车辆右侧
// 原始: L1420-1429
// 独立编译改造：
//   - common::math::ENUToRFU → 本地辅助函数 ENUToVRF
//   - init_adc_point_.x() → init_adc_point_.x
// ============================================================
bool OpenSpaceRoiDecider::IsParkLotInRightSide(
    const Vec2d& left_top_enu, const Vec2d& right_top_enu) {
  // 独立编译改造：使用本地ENUToVRF替代原始ENUToRFU
  auto left_top_vrf = ENUToVRF(
      left_top_enu.x(), left_top_enu.y(),
      init_adc_point_.x, init_adc_point_.y, init_adc_point_.theta);
  auto right_top_vrf = ENUToVRF(
      right_top_enu.x(), right_top_enu.y(),
      init_adc_point_.x, init_adc_point_.y, init_adc_point_.theta);
  return right_top_vrf.second > left_top_vrf.second;
}

// ============================================================
// SetParkingSpotEndPose: 泊入端点调度
// 原始: L1577-1615
// 独立编译改造：
//   - Frame* 参数移除
//   - AINFO → LOG(INFO)
//   - ADEBUG → VLOG(2)
// ============================================================
void OpenSpaceRoiDecider::SetParkingSpotEndPose(
    const perception::ParkingLotOut::ParkType& park_type,
    const ParkingLotVertexType& vertices, const bool is_right_side,
    const std::tuple<bool, Vec2d, Vec2d>& wheel_mask,
    common::PathPoint* const end_pose_ptr, bool* const is_parking_inwards) {
  // 独立编译改造：移除 UNUSED(frame)
  Vec2d stop_left;
  Vec2d stop_right;
  const auto& left_top = vertices[0];
  const auto& left_down = vertices[1];
  const auto& right_down = vertices[2];
  const auto& right_top = vertices[3];

  if (std::get<0>(wheel_mask)) {
    VLOG(2) << " AVP -- AVP:consider wheel mask scenario";
    stop_left = std::get<1>(wheel_mask);
    stop_right = std::get<2>(wheel_mask);
  }
  LOG(INFO) << "[PARKING] park_type " << static_cast<int>(park_type);

  switch (park_type) {
    case TL::perception::ParkingLotOut::VERTICAL:
    case TL::perception::ParkingLotOut::OBLIQUE: {
      LOG(INFO) << "[PARKING] SetNonLateralSlotEndPose";
      SetNonLateralSlotEndPose(park_type, left_top, left_down, right_top,
                               right_down, stop_left, stop_right, is_right_side,
                               end_pose_ptr, is_parking_inwards);
      break;
    }
    case TL::perception::ParkingLotOut::LATERAL: {
      LOG(INFO) << "[PARKING] SetLateralSlotEndPose";
      SetLateralSlotEndPose(is_right_side, left_top, left_down, right_top,
                            right_down, stop_left, stop_right, end_pose_ptr);
      break;
    }
    default:
      break;
  }
}

// ============================================================
// SetNonLateralSlotEndPose: 垂直/斜向终点计算
// 原始: L1617-1700
// 独立编译改造：
//   - FLAGS_avp_enable_parking_inwards → config_.avp_enable_parking_inwards
//   - end_pose_ptr->set_x(v) → end_pose_ptr->x = v
//   - config_.open_space_roi_decider_config().xxx() → config_.xxx
//   - vehicle_params_.front_edge_to_center() → vehicle_params_.front_edge_to_center
// ============================================================
void OpenSpaceRoiDecider::SetNonLateralSlotEndPose(
    const perception::ParkingLotOut::ParkType& park_type,
    const Vec2d& left_top, const Vec2d& left_down,
    const Vec2d& right_top, const Vec2d& right_down,
    const Vec2d& /*stop_left*/, const Vec2d& /*stop_right*/,
    const bool is_right_side, common::PathPoint* const end_pose_ptr,
    bool* const is_parking_inwards_ptr) {

  // 斜向车位投影处理（与原始逻辑完全一致）
  Vec2d projected_left_top = left_top;
  Vec2d projected_left_down = left_down;
  Vec2d projected_right_top = right_top;
  Vec2d projected_right_down = right_down;

  // 独立编译改造：config_.open_space_roi_decider_config() → config_
  double parking_depth_buffer = config_.vertical_parking_depth_buffer;

  // 默认泊车方向 = 倒车入库
  if (park_type == TL::perception::ParkingLotOut::OBLIQUE) {
    parking_depth_buffer = config_.oblique_parking_depth_buffer;
    // 计算斜向角度余弦/正弦
    Vec2d down_top_unit_vector = left_top - left_down;
    down_top_unit_vector.Normalize();
    Vec2d left_right_unit_vector = right_down - left_down;
    left_right_unit_vector.Normalize();
    const double projected_length =
        left_right_unit_vector.InnerProd(down_top_unit_vector) *
        (right_down - left_down).Length();
    if (projected_length > 0) {
      projected_left_down =
          projected_left_down + down_top_unit_vector * projected_length;
      projected_right_top =
          projected_right_top - down_top_unit_vector * projected_length;
    } else {
      projected_left_top =
          projected_left_top + down_top_unit_vector * projected_length;
      projected_right_down =
          projected_right_down - down_top_unit_vector * projected_length;
    }
    *is_parking_inwards_ptr =
        is_right_side
            ? left_right_unit_vector.InnerProd(down_top_unit_vector) < 0
            : left_right_unit_vector.InnerProd(down_top_unit_vector) > 0;
    // 独立编译改造：FLAGS_avp_enable_parking_inwards → config_
    *is_parking_inwards_ptr &= config_.avp_enable_parking_inwards;
  }

  // 垂直车位：手动设置 is_parking_inwards
  if (park_type == TL::perception::ParkingLotOut::VERTICAL) {
    // 独立编译改造：FLAGS_avp_enable_parking_inwards → config_
    *is_parking_inwards_ptr = config_.avp_enable_parking_inwards;
  }

  // 计算终点位姿
  double end_x = 0.0;
  double end_y = 0.0;
  double end_phi = 0.0;

  // Step 1: 设置终点朝向
  const double parking_spot_heading =
      (projected_left_down - projected_left_top).Angle();
  end_phi = *is_parking_inwards_ptr
                ? parking_spot_heading
                : NormalizeAngle(parking_spot_heading + M_PI);

  // Step 2: 设置终点 x, y
  const double top_to_down_distance =
      (projected_left_top - projected_left_down).Length();
  const Vec2d top_middle_point =
      (projected_left_top + projected_right_top) * 0.5;
  const Vec2d down_middle_point =
      (projected_left_down + projected_right_down) * 0.5;
  double dis_move = 0.0;

  if (*is_parking_inwards_ptr) {
    // 车头入库
    // 独立编译改造：vehicle_params_.front_edge_to_center() → .front_edge_to_center
    dis_move = (std::max(3.0 * top_to_down_distance / 4.0,
                         vehicle_params_.front_edge_to_center) +
                parking_depth_buffer);
    end_x = down_middle_point.x() - dis_move * cos(parking_spot_heading);
    end_y = down_middle_point.y() - dis_move * sin(parking_spot_heading);
  } else {
    // 倒车入库
    dis_move = vehicle_params_.front_edge_to_center + parking_depth_buffer;
    end_x = top_middle_point.x() + dis_move * cos(parking_spot_heading);
    end_y = top_middle_point.y() + dis_move * sin(parking_spot_heading);
  }

  // 独立编译改造：set_x(v) → x = v
  end_pose_ptr->x = end_x;
  end_pose_ptr->y = end_y;
  end_pose_ptr->theta = end_phi;
}

// ============================================================
// SetLateralSlotEndPose: 侧方车位终点计算
// 原始: L1702-1766
// 独立编译改造：
//   - set_x/set_y/set_theta → 直接赋值
//   - vehicle_params_.xxx() → vehicle_params_.xxx
//   - config_.open_space_roi_decider_config().xxx() → config_.xxx
//   - 移除 frame_ 相关 wheel_mask 逻辑（独立编译不支持）
// ============================================================
void OpenSpaceRoiDecider::SetLateralSlotEndPose(
    const bool is_right_side, const Vec2d& left_top,
    const Vec2d& /*left_down*/, const Vec2d& right_top,
    const Vec2d& right_down, const Vec2d& stop_left,
    const Vec2d& stop_right,
    common::PathPoint* const end_pose_ptr) {
  (void)stop_left;
  (void)stop_right;

  double lt_rt_angle = (right_top - left_top).Angle();
  double rt_rd_angle = (right_down - right_top).Angle();
  double end_phi = is_right_side
                       ? lt_rt_angle
                       : NormalizeAngle(lt_rt_angle + M_PI);

  // 独立编译改造：vehicle_params_.length() → vehicle_params_.length
  double rear_axis_to_center =
      vehicle_params_.length * 0.5 - vehicle_params_.back_edge_to_center;

  // 独立编译改造：config_.open_space_roi_decider_config().lateral_parking_depth_buffer()
  // → 简化为固定值（原始配置结构中未定义该字段，使用垂直深度缓冲作为替代）
  double lateral_parking_depth_buffer = 0.2;
  double park_depth =
      ((lateral_parking_depth_buffer +
        vehicle_params_.left_edge_to_center) /
       std::fabs(std::sin(rt_rd_angle - lt_rt_angle)));

  Vec2d top_edge_center = (left_top + right_top) * 0.5;
  Vec2d end_position =
      top_edge_center +
      Vec2d::CreateUnitVec2d(end_phi + M_PI) * rear_axis_to_center +
      Vec2d::CreateUnitVec2d(rt_rd_angle) * park_depth;

  // 独立编译改造：移除 frame_->open_space_info().is_consider_wheel_mask() 分支
  // wheel_mask 功能在独立编译版本中不可用

  // 独立编译改造：set_x → 直接赋值
  end_pose_ptr->x = end_position.x();
  end_pose_ptr->y = end_position.y();
  end_pose_ptr->theta = end_phi;
}

// ============================================================
// CalculateInnerRoi: 内部ROI多边形计算
// 原始: L1831-1946
// 独立编译改造：
//   - vehicle_state_.x() → vehicle_state_.x
//   - config_.open_space_roi_decider_config().xxx() → config_.xxx
//   - vehicle_params_.width() → vehicle_params_.width
//   - parking_type_ 成员 → 固定 PARKING_IN
//   - AERROR → LOG(ERROR)
// ============================================================
void OpenSpaceRoiDecider::CalculateInnerRoi(
    const perception::ParkingLotOut::ParkType& park_type,
    const ParkingLotVertexType& spot_vertices,
    const bool is_use_larger_roi,
    std::vector<Vec2d>* const inner_roi_vertex) {

  auto left_top = spot_vertices[0];
  auto left_down = spot_vertices[1];
  auto right_down = spot_vertices[2];
  auto right_top = spot_vertices[3];

  // 独立编译改造：adc_point.set_x(vehicle_state_.x()) → 直接赋值
  common::PathPoint adc_point;
  adc_point.x = vehicle_state_.x;
  adc_point.y = vehicle_state_.y;
  adc_point.theta = vehicle_state_.heading;

  const double adc_height =
      CalculateAdcHeight(adc_point,
                         LineSegment2d(left_down, right_down)) -
      left_top.DistanceTo(left_down);
  static constexpr double kDefaultAcAroundRange = 2.5;

  road_width_ = fmax(road_width_, (adc_height + kDefaultAcAroundRange) / 2);

  const auto top_edge = LineSegment2d(left_top, right_top);
  auto left_edge = LineSegment2d(left_top, left_down);
  auto right_edge = LineSegment2d(right_top, right_down);

  // 独立编译改造：config_.open_space_roi_decider_config() → config_
  double start_s = config_.roi_longitudinal_range_start - top_edge.length() / 2;
  double end_s = config_.roi_longitudinal_range_end - top_edge.length() / 2;

  // 独立编译改造：parking_type_ == PARKING_IN → 固定为 true
  const bool is_park_in = true;

  if (is_use_larger_roi) {
    // LIDAR模式ROI扩展（原始逻辑保留）
    VLOG(2) << " sensor config is with lidar, use larger roi boundary";
    LineSegment2d roi_top(left_top, right_top);
    const double slot_height = roi_top.DistanceTo(left_down);
    roi_top.Translate(
        config_.lateral_expand_bottom_buffer + slot_height,
        roi_top.heading() - M_PI_2);
    roi_top.Translate(start_s, roi_top.heading() + M_PI);
    inner_roi_vertex->reserve(4);
    inner_roi_vertex->push_back(roi_top.start());
    roi_top.Extend(start_s + end_s);
    inner_roi_vertex->push_back(roi_top.end());
    roi_top.Translate(2 * road_width_ + slot_height +
                          config_.lateral_expand_bottom_buffer,
                      roi_top.heading() + M_PI_2);
    inner_roi_vertex->push_back(roi_top.end());
    inner_roi_vertex->push_back(roi_top.start());
    return;
  }

  // 常规ROI计算
  std::vector<Vec2d> common_vec2d = {left_top, left_down, right_down,
                                     right_top};
  double lat_edge_expand_buffer = 0.0;

  switch (park_type) {
    case TL::perception::ParkingLotOut::LATERAL: {
      lat_edge_expand_buffer =
          vehicle_params_.length +
          config_.lateral_expand_buffer - top_edge.length();
      left_edge.Extend(config_.lateral_expand_bottom_buffer);
      right_edge.Extend(config_.lateral_expand_bottom_buffer);
      break;
    }
    case TL::perception::ParkingLotOut::VERTICAL:
    case TL::perception::ParkingLotOut::OBLIQUE: {
      const double project_ratio =
          fabs(sin(left_edge.heading() - top_edge.heading()));
      if (project_ratio <= kEpsilon) {
        LOG(ERROR) << "[PARKING] park slot is not legal";
        return;
      }
      double lat_expand_buffer =
          is_park_in ? config_.vertical_expand_buffer
                     : config_.park_out_vertical_expand_buffer;
      lat_edge_expand_buffer =
          (vehicle_params_.width + lat_expand_buffer) / project_ratio -
          top_edge.length();
      break;
    }
    default: {
      lat_edge_expand_buffer =
          vehicle_params_.width +
          config_.vertical_expand_buffer - top_edge.length();
      break;
    }
  }

  constexpr double kMinExpandBuffer = 0.1;
  if (lat_edge_expand_buffer > kMinExpandBuffer) {
    left_edge.Translate(0.5 * lat_edge_expand_buffer,
                        top_edge.heading() + M_PI);
    right_edge.Translate(0.5 * lat_edge_expand_buffer, top_edge.heading());
    common_vec2d[0] = left_edge.start();
    common_vec2d[1] = left_edge.end();
    common_vec2d[2] = right_edge.end();
    common_vec2d[3] = right_edge.start();
  }

  // 构造8顶点凹多边形
  inner_roi_vertex->reserve(8);
  LineSegment2d roi_lane(left_top, right_top);
  roi_lane.Translate(start_s, roi_lane.heading() + M_PI);
  inner_roi_vertex->push_back(roi_lane.start());

  // 插入车位附近4个顶点（凹口）
  inner_roi_vertex->insert(inner_roi_vertex->end(), common_vec2d.begin(),
                           common_vec2d.end());

  // 插入通道另一侧3个顶点
  roi_lane.Extend(start_s + end_s);
  inner_roi_vertex->push_back(roi_lane.end());
  roi_lane.Translate(2 * road_width_, roi_lane.heading() + M_PI_2);
  inner_roi_vertex->push_back(roi_lane.end());
  inner_roi_vertex->push_back(roi_lane.start());
}

// ============================================================
// CalculateAdcHeight: 车辆包围盒到底边投影高度
// 原始: L1948-1965
// ============================================================
double OpenSpaceRoiDecider::CalculateAdcHeight(
    const common::PathPoint& adc_point,
    const LineSegment2d& left_to_right_bottom) {
  const auto adc_box = common::VehicleConfigHelper::GetBoundingBox(adc_point);
  double height = 0.0;
  double point_dist = 0.0;
  // 独立编译改造：GetAllCorners() 需要输出参数
  std::vector<Vec2d> corners;
  adc_box.GetAllCorners(&corners);
  for (const auto& point : corners) {
    point_dist = fabs(left_to_right_bottom.ProductOntoUnit(point));
    height = fmax(point_dist, height);
  }
  VLOG(2) << "[PARKING] adc height is: " << height;
  return height;
}

// ============================================================
// CalculateAdcDis2ParkBottom: 车辆到车位底边最小距离
// 原始: L1979-1993
// ============================================================
double OpenSpaceRoiDecider::CalculateAdcDis2ParkBottom(
    const common::PathPoint& adc_point,
    const LineSegment2d& left_to_right_bottom) {
  double bottom_dist = 100.0;
  double point_dist = 0.0;
  const auto adc_box = common::VehicleConfigHelper::GetBoundingBox(adc_point);
  // 独立编译改造：GetAllCorners() 需要输出参数
  std::vector<Vec2d> corners;
  adc_box.GetAllCorners(&corners);
  for (const auto& point : corners) {
    point_dist = fabs(left_to_right_bottom.ProductOntoUnit(point));
    bottom_dist = fmin(point_dist, bottom_dist);
  }
  VLOG(2) << "[PARKING] adc dist to park bottom is: " << bottom_dist;
  return bottom_dist;
}

// ============================================================
// IsUseLargerRoi: 是否使用LIDAR扩大ROI
// 原始: L1967-1977
// 独立编译改造：移除 sensor_config_state_ 依赖
//              独立编译版本默认不使用LIDAR扩大ROI
// ============================================================
bool OpenSpaceRoiDecider::IsUseLargerRoi(
    const ParkingLotVertexType& spot_vertices) {
  // 独立编译改造：无传感器配置，默认不使用LIDAR ROI
  if (!config_.enable_adapt_lidar_sensor_roi) {
    return false;
  }
  // 若配置启用，则检查车辆距离
  static constexpr double kEnableOuterFsAdcHeight = 1.0;
  const auto adc_point = Vec2d(vehicle_state_.x, vehicle_state_.y);
  return LineSegment2d(spot_vertices.at(0), spot_vertices.at(3))
             .ProductOntoUnit(adc_point) > kEnableOuterFsAdcHeight;
}

// ============================================================
// CalculateOuterRoi: 外扩ROI (0.5m)
// 原始: L1991-2021 (static)
// 无需改造：不依赖任何Framework
// ============================================================
void OpenSpaceRoiDecider::CalculateOuterRoi(
    const std::vector<Vec2d>& inner_roi_vertex,
    std::vector<Vec2d>* const outer_roi_vertex) {
  outer_roi_vertex->assign(inner_roi_vertex.begin(), inner_roi_vertex.end());
  int roi_edge_num = static_cast<int>(outer_roi_vertex->size());
  if (roi_edge_num < 2) {
    return;
  }
  const double slack_distance = 0.5;
  for (int i = 0; i < roi_edge_num; ++i) {
    int pre_idx = (i - 1 + roi_edge_num) % roi_edge_num;
    int next_idx = (i + 1) % roi_edge_num;
    Vec2d vec1 =
        slack_distance *
        Vec2d::CreateUnitVec2d(
            (inner_roi_vertex.at(i) - inner_roi_vertex.at(pre_idx)).Angle());
    Vec2d vec2 =
        slack_distance *
        Vec2d::CreateUnitVec2d(
            (inner_roi_vertex.at(next_idx) - inner_roi_vertex.at(i)).Angle());
    Vec2d vec_merge = vec1 + vec2;
    vec_merge.SelfRotate(-M_PI_2);
    double length =
        fabs(slack_distance / sin(vec_merge.Angle() - vec1.Angle()));
    Vec2d vec = inner_roi_vertex.at(i) +
                length * Vec2d::CreateUnitVec2d(vec_merge.Angle());
    outer_roi_vertex->at(i) = vec;
  }
}

// ============================================================
// IsVehicleInRoi: 车辆是否在ROI内
// 原始: L2024-2041
// 独立编译改造：
//   - vehicle_state_.x() → vehicle_state_.x
//   - AERROR/AINFO → LOG()
// ============================================================
bool OpenSpaceRoiDecider::IsVehicleInRoi(
    const Vec2d& origin_point, double origin_heading,
    const std::vector<double>& roi_xy_boundary) {
  // 独立编译改造：vehicle_state_.x() → vehicle_state_.x
  auto vehicle_xy = Vec2d(vehicle_state_.x, vehicle_state_.y);
  vehicle_xy -= origin_point;
  vehicle_xy.SelfRotate(-origin_heading);

  LOG(INFO) << "[PARKING] vehicle_xy x " << vehicle_xy.x()
            << " y " << vehicle_xy.y();
  LOG(INFO) << "[PARKING] roi_xy_boundary " << roi_xy_boundary[0] << " "
            << roi_xy_boundary[1] << " " << roi_xy_boundary[2] << " "
            << roi_xy_boundary[3];

  if (vehicle_xy.x() < roi_xy_boundary[0] ||
      vehicle_xy.x() > roi_xy_boundary[1] ||
      vehicle_xy.y() < roi_xy_boundary[2] ||
      vehicle_xy.y() > roi_xy_boundary[3]) {
    LOG(ERROR) << "[PARKING] vehicle outside of xy boundary of parking ROI";
    return false;
  }
  return true;
}

// ============================================================
// GetParkingBoundary: ROI边界编排
// 原始: L2170-2218
// 独立编译改造：移除 Frame* 参数, 保留完整 RoiBoundarySlack
// ============================================================
void OpenSpaceRoiDecider::GetParkingBoundary(
    const Vec2d& origin_point, double origin_heading,
    const perception::ParkingLotOut::ParkType& park_type,
    const ParkingLotVertexType& vertices,
    const common::PathPoint& end_pose_enu,
    std::vector<LineSegment2d>* inner_roi_boundary,
    std::vector<LineSegment2d>* outer_roi_boundary,
    std::vector<double>* roi_xy_boundary,
    RoiDeciderOutput* vis_output) {
  // 独立编译改造：UNUSED(frame) 移除
  std::vector<Vec2d> inner_roi_vertex;
  std::vector<Vec2d> outer_roi_vertex;

  is_use_larger_roi_ = is_use_larger_roi_ || IsUseLargerRoi(vertices);

  // Step 1: 计算内部ROI
  CalculateInnerRoi(park_type, vertices, is_use_larger_roi_, &inner_roi_vertex);

  // 可视化：捕获 pre-slack 状态（ENU）——在裁剪前记录，反映原始ROI形状
  if (vis_output) {
    vis_output->inner_roi_pre_slack_enu.clear();
    for (const auto& p : inner_roi_vertex)
      vis_output->inner_roi_pre_slack_enu.push_back({p.x(), p.y()});
  }

  // 预计算 FSP 感知范围的 ENU AABB
  // FSP 覆盖自车坐标系 x/y ∈ [-kFspSensorRange, kFspSensorRange]
  // VRF 角点 (±range, ±range) → ENU: ex = vx + a*cos(θ) - b*sin(θ)
  const double fsp_cos_h = std::cos(init_adc_point_.theta);
  const double fsp_sin_h = std::sin(init_adc_point_.theta);
  double fsp_xmin =  1e9, fsp_xmax = -1e9;
  double fsp_ymin =  1e9, fsp_ymax = -1e9;
  for (int ai = -1; ai <= 1; ai += 2) {
    for (int bi = -1; bi <= 1; bi += 2) {
      double ex = init_adc_point_.x + ai * kFspSensorRange * fsp_cos_h
                                    - bi * kFspSensorRange * fsp_sin_h;
      double ey = init_adc_point_.y + ai * kFspSensorRange * fsp_sin_h
                                    + bi * kFspSensorRange * fsp_cos_h;
      fsp_xmin = std::min(fsp_xmin, ex); fsp_xmax = std::max(fsp_xmax, ex);
      fsp_ymin = std::min(fsp_ymin, ey); fsp_ymax = std::max(fsp_ymax, ey);
    }
  }
  if (vis_output) {
    vis_output->fsp_aabb_enu = {
        {fsp_xmin, fsp_ymin},
        {fsp_xmax, fsp_ymin},
        {fsp_xmax, fsp_ymax},
        {fsp_xmin, fsp_ymax},
    };
  }
  LOG(INFO) << "[PARKING] FSP感知范围(ENU): x[" << fsp_xmin << "," << fsp_xmax
            << "] y[" << fsp_ymin << "," << fsp_ymax << "]";

  // Step 2: ROI边界松弛（与原始代码一致，roi_decider_lite 中跳过了此步）
  RoiBoundarySlack(end_pose_enu, is_use_larger_roi_, &inner_roi_vertex);

  // 裁剪内ROI顶点到 FSP 感知范围（ENU），防止规划超出感知区域
  inner_roi_vertex = ClipPolygonToAabb(inner_roi_vertex,
                                       fsp_xmin, fsp_xmax, fsp_ymin, fsp_ymax);

  // 可视化：捕获裁剪后的 post-slack inner ROI（ENU）
  if (vis_output) {
    vis_output->inner_roi_post_slack_enu.clear();
    for (const auto& p : inner_roi_vertex)
      vis_output->inner_roi_post_slack_enu.push_back({p.x(), p.y()});
  }

  // Step 3: 计算外扩ROI
  CalculateOuterRoi(inner_roi_vertex, &outer_roi_vertex);

  // 裁剪外ROI顶点到 FSP 感知范围（ENU）
  outer_roi_vertex = ClipPolygonToAabb(outer_roi_vertex,
                                       fsp_xmin, fsp_xmax, fsp_ymin, fsp_ymax);

  // 可视化：捕获裁剪后的 outer ROI（ENU）
  if (vis_output) {
    vis_output->outer_roi_enu.clear();
    for (const auto& p : outer_roi_vertex)
      vis_output->outer_roi_enu.push_back({p.x(), p.y()});
  }

  // Step 4: 坐标变换 + AABB
  TransRoiAndSetBoundary(origin_point, origin_heading, &inner_roi_vertex,
                         &outer_roi_vertex, inner_roi_boundary,
                         outer_roi_boundary, roi_xy_boundary);

  // 将 AABB roi_xy_boundary 进一步限制到局部坐标系中的 FSP 范围
  // 把 FSP 角点(ENU) → 局部坐标系（减原点+旋转-origin_heading），取 AABB 后与 roi_xy_boundary 求交
  {
    const double cos_o = std::cos(-origin_heading);
    const double sin_o = std::sin(-origin_heading);
    double fsp_lx_min =  1e9, fsp_lx_max = -1e9;
    double fsp_ly_min =  1e9, fsp_ly_max = -1e9;
    for (int ai = -1; ai <= 1; ai += 2) {
      for (int bi = -1; bi <= 1; bi += 2) {
        double ex = init_adc_point_.x + ai * kFspSensorRange * fsp_cos_h
                                      - bi * kFspSensorRange * fsp_sin_h;
        double ey = init_adc_point_.y + ai * kFspSensorRange * fsp_sin_h
                                      + bi * kFspSensorRange * fsp_cos_h;
        double dx = ex - origin_point.x();
        double dy = ey - origin_point.y();
        double lx = dx * cos_o - dy * sin_o;
        double ly = dx * sin_o + dy * cos_o;
        fsp_lx_min = std::min(fsp_lx_min, lx); fsp_lx_max = std::max(fsp_lx_max, lx);
        fsp_ly_min = std::min(fsp_ly_min, ly); fsp_ly_max = std::max(fsp_ly_max, ly);
      }
    }
    (*roi_xy_boundary)[0] = std::max((*roi_xy_boundary)[0], fsp_lx_min);
    (*roi_xy_boundary)[1] = std::min((*roi_xy_boundary)[1], fsp_lx_max);
    (*roi_xy_boundary)[2] = std::max((*roi_xy_boundary)[2], fsp_ly_min);
    (*roi_xy_boundary)[3] = std::min((*roi_xy_boundary)[3], fsp_ly_max);
    LOG(INFO) << "[PARKING] FSP裁剪后ROI边界(局部坐标): ["
              << (*roi_xy_boundary)[0] << ", " << (*roi_xy_boundary)[1] << "] x ["
              << (*roi_xy_boundary)[2] << ", " << (*roi_xy_boundary)[3] << "]";

    // FSP 裁剪后，继续保证局部边界至少覆盖当前自车和终点位姿，
    // 避免局部 AABB 在入口侧被裁得过紧，导致 vehicle_xy 落在 ROI 外。
    const double roi_guard_padding =
      std::max(config_.roi_edge_buffer, vehicle_params_.width * 0.5) + 0.2;
    const Vec2d vehicle_local =
      ToLocalPoint(Vec2d(vehicle_state_.x, vehicle_state_.y),
             origin_point, origin_heading);
    const Vec2d end_pose_local =
      ToLocalPoint(Vec2d(end_pose_enu.x, end_pose_enu.y),
             origin_point, origin_heading);

    const std::vector<double> roi_before_expand = *roi_xy_boundary;
    ExpandRoiBoundaryToIncludePoint(vehicle_local, roi_guard_padding,
                    fsp_lx_min, fsp_lx_max,
                    fsp_ly_min, fsp_ly_max,
                    roi_xy_boundary);
    ExpandRoiBoundaryToIncludePoint(end_pose_local, 0.5,
                    fsp_lx_min, fsp_lx_max,
                    fsp_ly_min, fsp_ly_max,
                    roi_xy_boundary);

    if (roi_before_expand != *roi_xy_boundary) {
      LOG(INFO) << "[PARKING] ROI边界补偿后(局部坐标): ["
          << (*roi_xy_boundary)[0] << ", " << (*roi_xy_boundary)[1]
          << "] x [" << (*roi_xy_boundary)[2] << ", "
          << (*roi_xy_boundary)[3] << "]"
          << " vehicle_local=(" << vehicle_local.x() << ", "
          << vehicle_local.y() << ") end_local=(" << end_pose_local.x()
          << ", " << end_pose_local.y() << ")";
    }
  }

  // 可视化：捕获局部坐标系状态（TransRoi后，inner_roi_vertex已变换）
  if (vis_output) {
    vis_output->inner_roi_local.clear();
    for (const auto& p : inner_roi_vertex)
      vis_output->inner_roi_local.push_back({p.x(), p.y()});
  }
}

// ============================================================
// TransRoiAndSetBoundary: 坐标变换 + AABB边界
// 原始: L2194-2218 (static)
// 无需改造：不依赖任何Framework
// ============================================================
void OpenSpaceRoiDecider::TransRoiAndSetBoundary(
    const Vec2d& origin_point, const double origin_heading,
    std::vector<Vec2d>* const inner_roi_vertex,
    std::vector<Vec2d>* const outer_roi_vertex,
    std::vector<LineSegment2d>* const inner_roi_boundary,
    std::vector<LineSegment2d>* const outer_roi_boundary,
    std::vector<double>* const roi_xy_boundary) {
  // 构建ENU坐标系下的边界线段
  for (size_t i = 0; i + 1 < inner_roi_vertex->size(); ++i) {
    inner_roi_boundary->push_back(
        {inner_roi_vertex->at(i), inner_roi_vertex->at(i + 1)});
  }
  for (size_t i = 0; i + 1 < outer_roi_vertex->size(); ++i) {
    outer_roi_boundary->push_back(
        {outer_roi_vertex->at(i), outer_roi_vertex->at(i + 1)});
  }

  // 坐标变换：ENU → 以车位左上角为原点的局部坐标系
  for (auto& point : *inner_roi_vertex) {
    point -= origin_point;
    point.SelfRotate(-origin_heading);
  }

  // 计算AABB
  auto xminmax = std::minmax_element(
      inner_roi_vertex->begin(), inner_roi_vertex->end(),
      [](const Vec2d& a, const Vec2d& b) { return a.x() < b.x(); });
  auto yminmax = std::minmax_element(
      inner_roi_vertex->begin(), inner_roi_vertex->end(),
      [](const Vec2d& a, const Vec2d& b) { return a.y() < b.y(); });
  *roi_xy_boundary = {xminmax.first->x(), xminmax.second->x(),
                      yminmax.first->y(), yminmax.second->y()};
}

// ============================================================
// RoiBoundarySlack: ROI边界松弛
// 原始: L2220-2297
// 独立编译改造：
//   - GetBoundingBox(pose, lat, lon) → GetBoundingBox(pose) (kADCBoxEps 可忽略)
//   - parking_type_ 成员 → 固定 PARKING_IN
//   - config_.open_space_roi_decider_config() → config_
//   - adc_pose.set_x() → 直接赋值
// ============================================================
void OpenSpaceRoiDecider::RoiBoundarySlack(
    const common::PathPoint& end_pose_enu, const bool is_use_larger_roi,
    std::vector<Vec2d>* const inner_roi_vertex) {
  if (nullptr == inner_roi_vertex || is_use_larger_roi) {
    VLOG(2) << "[PARKING] Skip roi slack";
    return;
  }

  // 独立编译改造：config_ 直接访问
  const double default_slack_distance = config_.roi_slack_distance;

  // 独立编译改造：adc_pose.set_x → 直接赋值
  common::PathPoint adc_pose;
  adc_pose.x = vehicle_state_.x;
  adc_pose.y = vehicle_state_.y;
  adc_pose.theta = vehicle_state_.heading;

  // 独立编译改造：GetBoundingBox(pose, kADCBoxEps, kADCBoxEps) 不存在
  // 使用 GetBoundingBox(pose) 替代 (kADCBoxEps=0.001 可忽略)
  auto adc_box = common::VehicleConfigHelper::GetBoundingBox(adc_pose);
  auto end_pose_box = common::VehicleConfigHelper::GetBoundingBox(end_pose_enu);

  // 独立编译改造：parking_type_ == PARKING_IN 固定为 true
  adc_box.LateralExtend(config_.roi_edge_buffer);

  if (last_slack_dist_vec_.size() != inner_roi_vertex->size()) {
    last_slack_dist_vec_.clear();
    last_slack_dist_vec_.resize(inner_roi_vertex->size());
  }

  for (size_t i = 1; i < inner_roi_vertex->size(); ++i) {
    LineSegment2d roi_lane(inner_roi_vertex->at(i - 1),
                           inner_roi_vertex->at(i));
    LineSegment2d pre_roi_lane{
        inner_roi_vertex->at((i - 2 + inner_roi_vertex->size()) %
                             inner_roi_vertex->size()),
        inner_roi_vertex->at((i - 1 + inner_roi_vertex->size()) %
                             inner_roi_vertex->size())};
    pre_roi_lane.Translate(default_slack_distance,
                           pre_roi_lane.heading() + M_PI);
    pre_roi_lane.Extend(2 * default_slack_distance);
    LineSegment2d next_roi_lane{
        inner_roi_vertex->at((i + inner_roi_vertex->size()) %
                             inner_roi_vertex->size()),
        inner_roi_vertex->at((i + 1 + inner_roi_vertex->size()) %
                             inner_roi_vertex->size())};
    next_roi_lane.Translate(default_slack_distance,
                            next_roi_lane.heading() + M_PI);
    next_roi_lane.Extend(2 * default_slack_distance);

    size_t pre_idx =
        (i - 1 + inner_roi_vertex->size()) % inner_roi_vertex->size();
    size_t next_idx = (i + 1) % inner_roi_vertex->size();
    Vec2d vec1 = Vec2d::CreateUnitVec2d(
        (inner_roi_vertex->at(i) - inner_roi_vertex->at(pre_idx)).Angle());
    Vec2d vec2 = Vec2d::CreateUnitVec2d(
        (inner_roi_vertex->at(next_idx) - inner_roi_vertex->at(i)).Angle());
    double angle_bias = vec1.CrossProd(vec2) > 0 ? M_PI : 0;

    roi_lane.Translate(last_slack_dist_vec_.at(i), angle_bias + vec2.Angle());

    if (adc_box.HasOverlap(roi_lane) || end_pose_box.HasOverlap(roi_lane)) {
      VLOG(2) << "[PARKING] slack roi boundary has collision with current pose "
              << adc_box.HasOverlap(roi_lane);
      VLOG(2) << "[PARKING] slack roi boundary has collision with end pose "
              << end_pose_box.HasOverlap(roi_lane);
      LOG(INFO) << "[PARKING] slack roi boundary has collision, edge " << i;
      roi_lane.Translate(default_slack_distance, angle_bias + vec2.Angle());
      last_slack_dist_vec_.at(i) += default_slack_distance;
    }

    Vec2d slack_start_point = roi_lane.start();
    Vec2d slack_end_point = roi_lane.end();
    if (i < inner_roi_vertex->size() - 1) {
      roi_lane.GetIntersect(pre_roi_lane, &slack_start_point);
      roi_lane.GetIntersect(next_roi_lane, &slack_end_point);
    }
    inner_roi_vertex->at(i - 1) = slack_start_point;
    inner_roi_vertex->at(i) = slack_end_point;
  }
}

// ============================================================
// CaculateDestRegion: 目标区域计算（调度函数）
// 原始: L2486-2570
// 独立编译改造：仅保留 PARKING_IN 分支，移除 Frame*
// ============================================================
void OpenSpaceRoiDecider::CaculateDestRegion(
    const common::PathPoint& end_pose_enu,
    const perception::ParkingLotOut::ParkType& park_type,
    const ParkingLotVertexType& vertices,
    DestRegionWithAng* const dest_region_with_angle) {
  if (nullptr == dest_region_with_angle) {
    LOG(ERROR) << "[PARKING] dest_region_with_angle is nullptr";
    return;
  }
  // 独立编译改造：parking_type_ 固定为 PARKING_IN
  // 原始代码有 PARKING_OUT_LEFT/RIGHT/NNS/FRONT/BACK 等分支，此处全部省略
  CalculateParkingInRegion(park_type, end_pose_enu, dest_region_with_angle);
}

// ============================================================
// CalculateParkingInRegion: 泊入目标区域计算
// 原始: L2573-2593
// 独立编译改造：
//   - config_.open_space_roi_decider_config().park_in_xxx_end_pose_relax() → config_
//   - end_pose_enu.x() → end_pose_enu.x
//   - end_pose_enu.theta() → end_pose_enu.theta
// ============================================================
void OpenSpaceRoiDecider::CalculateParkingInRegion(
    const perception::ParkingLotOut::ParkType& park_type,
    const common::PathPoint& end_pose_enu,
    DestRegionWithAng* const dest_region_with_angle) {
  const bool is_lat_spot =
      (park_type == TL::perception::ParkingLotOut::LATERAL);

  // 独立编译改造：config_.xxx_end_pose_relax().vertical/horizontal/angle()
  // → config_ 结构体直接成员
  double end_pose_relax_box_length = 0.0;
  double end_pose_relax_box_width = 0.0;
  double theta_relax = 0.0;

  if (is_lat_spot) {
    end_pose_relax_box_length = config_.park_in_lateral_relax_vertical;
    end_pose_relax_box_width = config_.park_in_lateral_relax_horizontal;
    theta_relax = config_.park_in_lateral_relax_angle;
  } else {
    end_pose_relax_box_length = config_.park_in_vertical_relax_vertical;
    end_pose_relax_box_width = config_.park_in_vertical_relax_horizontal;
    theta_relax = config_.park_in_vertical_relax_angle;
  }

  // 独立编译改造：end_pose_enu.x() → end_pose_enu.x
  std::get<0>(*dest_region_with_angle) =
      Polygon2d(Box2d(
          Vec2d(end_pose_enu.x, end_pose_enu.y), end_pose_enu.theta,
          end_pose_relax_box_length, end_pose_relax_box_width));
  std::get<1>(*dest_region_with_angle) = end_pose_enu.theta - theta_relax;
  std::get<2>(*dest_region_with_angle) = end_pose_enu.theta + theta_relax;
}

// ============================================================
// ParkingScenarioTypeDecision: 场景类型决策
// 原始: L2665-2793
// 独立编译改造：
//   - parking_type_ 成员 → 固定 PARKING_IN（仅保留泊入分支）
//   - 移除 PARKING_OUT_LEFT/RIGHT/FRONT/BACK/TEST_CONTROL_MODE 分支
// ============================================================
void OpenSpaceRoiDecider::ParkingScenarioTypeDecision(
    const bool is_right_side,
    const perception::ParkingLotOut::ParkType& park_type,
    ParkingScenarioType* const parking_scenario_type_ptr) {
  if (nullptr == parking_scenario_type_ptr) {
    return;
  }
  *parking_scenario_type_ptr = ParkingScenarioType::DEFAULT_TYPE;

  // 独立编译改造：parking_type_ 固定为 PARKING_IN
  // 原始代码有完整的 switch(parking_type_) 分支，此处仅保留泊入
  switch (park_type) {
    case TL::perception::ParkingLotOut::VERTICAL: {
      *parking_scenario_type_ptr =
          is_right_side ? ParkingScenarioType::RIGHT_VERTICAL_PARKING_IN
                        : ParkingScenarioType::LEFT_VERTICAL_PARKING_IN;
      break;
    }
    case TL::perception::ParkingLotOut::OBLIQUE: {
      *parking_scenario_type_ptr =
          is_right_side ? ParkingScenarioType::RIGHT_OBLIQUE_PARKING_IN
                        : ParkingScenarioType::LEFT_OBLIQUE_PARKING_IN;
      break;
    }
    case TL::perception::ParkingLotOut::LATERAL: {
      *parking_scenario_type_ptr =
          is_right_side ? ParkingScenarioType::RIGHT_LATERAL_PARKING_IN
                        : ParkingScenarioType::LEFT_LATERAL_PARKING_IN;
      break;
    }
    default: {
      *parking_scenario_type_ptr = ParkingScenarioType::DEFAULT_TYPE;
      break;
    }
  }
}

// ============================================================
// 以下原始方法在独立编译版中移除（Framework依赖太重）：
// - Process(Frame*) → 被上面的独立 Process() 替代
// - Reset() → 构造函数直接初始化
// - RecordDebugInfo(Frame*) → 无调试信息录制
// - UpdateWheelMaskToOpenSpace(Frame*) → 无 wheel mask
// - SetOpenSpacePathInfo(Frame*) → 无 OpenSpacePathInfo
// - UpdateTargetPathInfo(Frame*) → 无 path info
// - SetOpenSpacePathInfoBasedOnParklot() → 被 Process() 替代
// - SetOpenSpacePathInfoForTestMode() → 无测试模式
// - SetOpenSpacePathInfoForNNSAdjust() → 无 NNS
// - CutOffTracePath() → 无 trace path
// - UpdateReplanInfo(Frame*) → 无 replan
// - UpdateSpeedBumpInfo(Frame*) → 无 speed bump
// - SetObsFilterStrategy() → 无障碍物过滤
// - IsSlotInnerFsValid() → 无自由空间
// - VerticalParkInFilter/LateralParkInFilter() → 无过滤
// - VerticalParkOutFilter() → 无泊出
// - GetParkingSpots(Frame*) → 由外部提供
// - InputValidCheck() → 简化为 CheckReceiveParkinglot
// - TransParkLotsToOpenSpace() → 被 Process() 替代
// - CaculateParkingOutTarget() → 无泊出
// - InitEndPoseBaseSlot(Frame*) → 简化为 SetParkingSpotEndPose
// - GetStaticBoundary() → 无静态边界
// - GetDynamicBoundary() → 无动态边界
// - IsValidTracePath() → 无 trace path
// - ChoosePartitionTracePath() → 无 trace path
// - GetParkingReplanStatus() → 无 replan
// - GetNNSAdjustReplanStatus() → 无 NNS
// - HasNNSAdjustTraceReplan() → 无 NNS
// - EndPoseThresholdManager() → 无阈值管理
// - CalculateParkingOutRegion(Frame*) → 无泊出
// - SensorStateDecider() → 无传感器状态
// - GetPreEndPose() → 无历史位姿（P4: 通过 SetLastEndPose/GetLastEndPose 外部维护）
// - IsTargetPoseDiffLargerThanThreshold() → 无阈值检查
// - TopKParkingLots() → 无多车位排序
// - SetLateralSlotEndPose (frame_ 相关部分) → 已简化
// ============================================================

// ============================================================
// IsSlotInnerFsGeometric: 车位内FreeSpace几何检测（P4独立版）
// 独立编译改造：替代 IsSlotInnerFsValid() (L1033)
// 原始依赖：has_valid_history_path_ + is_high_quality_triggered + FreeSpaceOutArray
// 独立版：仅基于车辆到车位入口中点的欧氏距离检测
// ============================================================
bool OpenSpaceRoiDecider::IsSlotInnerFsGeometric(
    const ParkingLotVertexType& vertices, bool is_parking_inwards) const {
  // 车位入口中点 = vertices[0] (left_top) 和 vertices[3] (right_top) 的中点
  // （垂直泊车：top = 靠近道路端）
  const auto slot_entrance = 0.5 * (vertices.at(0) + vertices.at(3));

  // 当前车辆后轴中心
  const Vec2d rear_center(vehicle_state_.x, vehicle_state_.y);
  // 车辆前轴中心 = 后轴中心 + wheel_base * 朝向向量
  const Vec2d front_center =
      rear_center +
      vehicle_params_.wheel_base *
          Vec2d::CreateUnitVec2d(vehicle_state_.heading);

  // 选择参考点：车头入库用前轴，倒车入库用后轴
  const Vec2d& reference_point = is_parking_inwards ? front_center : rear_center;
  const double dist_to_entrance = (reference_point - slot_entrance).Length();

  LOG(INFO) << "[P4] dist_to_slot_entrance=" << dist_to_entrance
            << "m (threshold=" << config_.use_slot_inner_fs_radius << "m)";

  return dist_to_entrance < config_.use_slot_inner_fs_radius;
}

}  // namespace planning
}  // namespace TL
