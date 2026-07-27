/******************************************************************************

核心算法函数。主要覆盖：
✅ 搜索算法核心：Plan函数（A*主循环、节点扩展、Reed-Shepp连接）
✅ 代价计算系统：PathCost（6项惩罚）、CalculateNodeCost（启发式+路径）
✅ RS曲线模块：解析式扩展、路径检查、代价评估
✅ 路径提取：GetResult（节点回溯、路径重建）
✅ 节点生成：Next_node_generator（运动基元生成）

核心函数（约 800 行代码）：
✅ InitParam() - 初始化参数（转向角序列生成）
✅ AnalyticExpansion() - 解析式扩展（Reed-Shepp直接连接）
✅ CaculateRsPathSteerMargion() - RS路径转向裕量计算
✅ EvaluateRsPathCost() - RS路径代价评估
✅ RSPCheck() - RS路径有效性检查
✅ RSShiftTimes() - RS路径换挡次数统计
✅ LoadRSPinCS() - RS路径加载到配置空间
✅ Next_node_generator() - 下一节点生成器
✅ ⭐ CalculateNodeCost() - 节点代价计算（路径+启发式）
✅ ⭐ PathCost() - 分段路径代价（6项惩罚）
✅ ⭐ GetReferenceLineCost() - 参考线偏离代价
✅ ⭐ GetResult() - 路径结果提取（回溯）
✅ ⭐ CalculateParkingPrefinishCondition() - 泊车完成条件判断
✅ ⭐ Plan() - 主搜索函数（Hybrid A*核心算法，~300行）
              开始
                ↓
              1-6步：初始化参数、清空容器、设置边界
                ↓
              7步：生成扩展路径（避障）
                ↓
              8-12步：重新初始化、验证起终点
                ↓
              13步：【核心循环】
                │
                ├─ 13.1：检查提前停止
                ├─ 13.2：检查超时
                ├─ 13.3：取出代价最小节点
                ├─ 13.4：尝试 RS 直连 → 成功则退出
                ├─ 13.5：标记已探索
                └─ 13.6：扩展相邻节点 → 加入待探索集合
                │    │
                │    └─ 循环直到找到路径或队列为空
                ↓
              14步：检查结果（从候选中选最优）
                ↓
              15步：记录 RS 连接点
                ↓
              16步：回溯提取完整路径
                ↓
              17步：输出统计信息
                ↓
              结束（返回 true/false）

              开放集（open_set_）


              🎯 关键概念解释
                      开放集（open_set_）
                      存储待探索的节点（还没访问但已发现）
                      类比：导航中的"备选路线列表"

                      优先队列（open_pq_）
                      按代价从小到大排序的开放集
                      类比：优先选择"代价最小"的路线

                      闭集（close_set_）
                      存储已探索的节点（已访问且不会再访问）
                      类比：导航中的"已走过的路"

                      终点队列（end_node_pq_）
                      存储满足目标条件的候选节点
                      类比：多个可接受的停车位置，选最优的
              ⚡ 算法特点
                      A 贪心策略*：每次选代价最小的节点扩展

                      Reed-Shepp 加速：每步尝试直接连接到终点

                      动态调整：超时后增加探索时间/范围

                      避障优先：先生成扩展路径避开初始碰撞

                      双向规划：支持从起点或终点开始规划


 *****************************************************************************/

/*
 * @file
 */
#include "planning/open_space/coarse_path_generator/hybrid_a_star.h"
#include <algorithm>
#include <array>
#include <atomic>  // 独立编译改造：atomic_early_stop_flag需要
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

#include "common/math/double_type.h"
#include "common/math/math_utils.h"
#include "common/math/polygon2d.h"
#include "common/math/vec2d.h"
#include "common/util/util.h"
#include <chrono>  // 替代 common/time/time.h
#include "common/file/log.h"
#include "planning/common/planning_gflags.h"
#include "proto_convert/pnc_point_convert.h"
#include "proto_convert/planning_internal_convert.h"  // 独立编译改造：PathUpdateStatus
#include "proto_convert/math_util_convert.h"  // 独立编译改造：BoundedValue, ComparedToZero

// 使用轻量级依赖
#include "planning/open_space/hybrid_a_star_config.h"
#include "planning/open_space/vehicle_param.h"
#include "planning/open_space/parking_space_info.h"
#include "common/file/log.h"  // 统一日志宏定义

// ========== 命名空间开始 ==========
// 就像给代码分门别类放进不同的文件夹
namespace TL {
namespace planning {

// 【说明】：停车方向和空间结构的枚举类型现在定义在 proto_convert/open_space_types_convert.h
// 这里不需要重复定义

// ========== 时钟工具命名空间 ==========
// 【用最简单的话解释】：提供获取当前时间的工具函数
namespace Clock {
  // 获取当前时间（以秒为单位）
  inline double NowInSeconds() {
    auto now = std::chrono::high_resolution_clock::now();  // 获取当前高精度时间点
    auto duration = now.time_since_epoch();                // 计算从1970年1月1日到现在的时长
    return std::chrono::duration<double>(duration).count(); // 转换为秒数
  }
}

// ========== 匿名命名空间：常量定义 ==========
// 这些常量只在本文件内使用，就像私有变量
namespace {
constexpr double kEpsilon = 1.0e-3;                        // 极小值，用于数值计算防止除零
constexpr double kDefaultIgnorableDist = 100;              // 默认可忽略距离（米）
constexpr double kExtensionExtraDist = 0.3;                // 路径延伸额外距离（米）
constexpr double kDisableOverTimeExtraDistHeight = -1.0;   // 禁用超时时的额外距离标记

// ========== 辅助函数：估算Beta角（前轮转角）==========
// 将方向盘转角转换为前轮转角
// @param steer_angle: 方向盘转角（弧度）
// @param vehicle_param: 车辆参数
// @return 前轮转角（弧度）
double EstimateBetaAngle(const double steer_angle, const VehicleParam& vehicle_param) {
  static constexpr double kMinSteerRatio = 1e-6;

  // 获取转向比（固定值）
  const double steer_ratio = std::max(vehicle_param.steer_ratio, kMinSteerRatio);

  // Beta角（前轮转角） = 方向盘转向角 / 转向比
  return steer_angle / steer_ratio;
}

}  // namespace

// ========== 构造函数 ==========


HybridAStar::HybridAStar(const HybridAStarConfig& config, const VehicleParam& vehicle_param)
    : PathGenerator(config, vehicle_param),  // 调用父类构造函数
      step_size_(config.step_size) {          // 设置搜索步长（每步前进的距离）
  // 创建 Reed-Shepp 路径生成器
  // Reed-Shepp 是一种生成最短路径的算法，由直线和圆弧组成
  reed_shepp_generator_ =
      std::make_unique<ReedShepp>(vehicle_param_, warm_start_config_);
}

// ========== 函数：初始化参数 ==========

// 计算车辆的转弯能力和搜索策略
void HybridAStar::InitParam() {
  // ========== 第1步：计算方向盘最大转向角裕度 ==========
  // 注意：vehicle_param_.max_steer_angle 是方向盘转角（非前轮转角）
  // margin 参数也是针对方向盘转角的裕度
  const double max_steer_angle_margin =
      path_search_strategy_.is_nns_adjust_senario  // 如果是 NNS 调整场景
          ? 0.0                                     // 不留余量
          : warm_start_config_.max_steer_angle_margin;  // 否则使用配置的余量（方向盘角度裕度）

  // ========== 第2步：计算 Reed-Shepp 和探索用的方向盘转角 ==========
  // 从方向盘最大转角中减去裕度，得到实际使用的方向盘转角
  const double max_rs_angle =
      vehicle_param_.max_steer_angle -  // 方向盘最大转角
      (path_search_strategy_.space_structure == SpaceStructure::LAT_PARK_LOT  // 如果是横向泊车
           ? lateral_rs_steer_angle_margin_   // 使用横向泊车的余量
           : max_steer_angle_margin);         // 否则使用普通余量

  // 计算搜索过程中使用的方向盘转角
  const double max_exploration_angle =
      vehicle_param_.max_steer_angle - max_steer_angle_margin;

  // ========== 第3步：通过 EstimateBetaAngle 将方向盘转角转换为前轮转角 ==========
  // EstimateBetaAngle 函数内部会除以 steer_ratio 进行转换
  const double max_rs_beta_angle =
      EstimateBetaAngle(max_rs_angle, vehicle_param_);
  const double max_exploration_beta_angle =
      EstimateBetaAngle(max_exploration_angle, vehicle_param_);

  // ========== 第4步：计算转弯半径 ==========
  // 使用前轮转角（beta角）计算转弯半径
  // 转弯半径公式：R = 轴距 / tan(前轮转角)

  // 计算转弯半径
  double rs_radius_value = vehicle_param_.wheel_base / (fabs(tan(max_rs_beta_angle)) + kEpsilon);
  double explore_radius_value = vehicle_param_.wheel_base / (fabs(tan(max_exploration_beta_angle)) + kEpsilon);

  // 打印转弯半径信息
  ADEBUG << "rs radius: " << rs_radius_value;
  ADEBUG << "explore radius: " << explore_radius_value;

  // ========== 第5步：更新 Reed-Shepp 生成器配置 ==========
  reed_shepp_generator_->UpdateConfig(max_rs_beta_angle);

  // ========== 第6步：生成转向角序列 ==========
  // 清空之前的转向角序列
  steer_seq_.clear();
  next_node_num_ = 0;

  static constexpr double kEpsilonPhi = 0.0001;  // 极小值，防止除零
  if (phi_grid_resolution_ > kEpsilonPhi) {
    // 计算每步最大角度变化
    double max_delta_phi = step_size_ / vehicle_param_.wheel_base *
                           std::tan(max_exploration_beta_angle);

    // 根据角度网格分辨率计算需要多少个角度步长
    auto size_phi =
        static_cast<size_t>(std::ceil(max_delta_phi / phi_grid_resolution_));
    static constexpr size_t kTwo = 2.0;
    size_t size_steer = kTwo * size_phi + 1;  // 总的转向角数量：左+右+直行

    // 安全检查：防止异常大的vector分配
    static constexpr size_t kMaxSteerSeqSize = 1000;  // 最大转向角序列长度
    if (size_steer > kMaxSteerSeqSize || size_phi > kMaxSteerSeqSize / 2) {
      AERROR << "[InitParam错误] 计算的转向角序列过大: size_steer=" << size_steer
             << ", size_phi=" << size_phi;
      AERROR << "  参数: phi_grid_resolution=" << phi_grid_resolution_
             << ", step_size=" << step_size_
             << ", max_delta_phi=" << max_delta_phi;
      AERROR << "  建议: 增大phi_grid_resolution或减小step_size";
      // 使用安全的默认值
      size_phi = (warm_start_config_.next_node_num - 1) / kTwo;
      size_steer = kTwo * size_phi + 1;
      max_delta_phi = phi_grid_resolution_ * size_phi;
      AERROR << "  回退到安全值: size_phi=" << size_phi << ", size_steer=" << size_steer;
    }

    // 分配内存空间
    steer_seq_.resize(size_steer);
    steer_seq_[0] = 0;  // 第一个是直行（转向角=0）

    // 生成对称的转向角序列（正负交替）
    for (size_t i = 1; i <= size_phi; i++) {
      double angle = std::atan(max_delta_phi * static_cast<double>(i) /
                               static_cast<double>(size_phi) *
                               vehicle_param_.wheel_base / step_size_);
      // 安全边界检查
      if (kTwo * i - 1 < size_steer && kTwo * i < size_steer) {
        steer_seq_.at(kTwo * i - 1) = angle;      // 正角度（左转）
        steer_seq_.at(kTwo * i) = -1 * angle;     // 负角度（右转）
      } else {
        AERROR << "[InitParam错误] 索引越界: i=" << i << ", size_steer=" << size_steer;
        break;
      }
    }
    next_node_num_ = size_steer * kTwo;  // 总节点数=转向角数量 * 2（前进+后退）
  }

  // ========== 第5步：计算最大曲率 ==========
  // 曲率 = tan(转向角) / 轴距，表示路径的弯曲程度
  double max_kappa =
      fabs(std::tan(max_rs_beta_angle) / vehicle_param_.wheel_base);
  // 设置 Reed-Shepp 路径的步长（与 reeds_shepp_path.cc 中的设计一致）
  rs_step_size_ = fmax(step_size_ * max_kappa * 0.2, 0.01);
}

// ========== 函数：分析式扩展 ==========
// 【用最简单的话解释】：
// 尝试直接连接当前位置到目标位置，就像"走捷径"，如果能直接到达就不需要继续搜索了
bool HybridAStar::AnalyticExpansion(
    const std::shared_ptr<Node3d>& current_node,  // 当前节点
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        obstacles_segments_vec) {  // 障碍物列表

  // ========== 第1步：检查是否满足提前结束条件 ==========
  // 如果当前节点距离目标很近，可以提前结束搜索
  if (CalculateParkingPrefinishCondition(current_node)) {
    std::cout << "[AnalyticExpansion] finish with search conditions 触发!" << std::endl;
    ADEBUG << "finish with search conditions";

    const auto cost_bucket = [](double cost) {
      if (cost < 20.0) {
        return 0U;
      } else if (cost < 40.0) {
        return 1U;
      } else if (cost < 60.0) {
        return 2U;
      } else if (cost < 80.0) {
        return 3U;
      } else if (cost < 100.0) {
        return 4U;
      } else if (cost < 140.0) {
        return 5U;
      } else if (cost < 200.0) {
        return 6U;
      }
      return 7U;
    };

    if (end_node_pq_.empty()) {
      // 第一次找到符合条件的节点，直接加入终点队列
      end_node_pq_.push(current_node);
      std::cout << "  首个节点加入 end_node_pq_" << std::endl;
    } else {
      // 创建新节点并计算到终点的代价
      std::shared_ptr<Node3d> node = std::make_shared<Node3d>(
          current_node->GetX(), current_node->GetY(), current_node->GetPhi(),
          xy_bounds_, xy_grid_resolution_, phi_grid_resolution_);
      node->SetPre(current_node->GetPreNode());

      // 计算到终点的距离
      double cur_to_end_dis =
          std::sqrt(pow(node->GetX() - end_node_->GetX(), 2) +
                    pow(node->GetY() - end_node_->GetY(), 2));

      // 计算角度差
      double ang_diff =
          fabs(common::math::AngleDiff(node->GetPhi(), end_node_->GetPhi()));

      // 将角度转换为距离代价的辅助函数
      const auto norm_angle_to_dis = [](double angle) {
        // 比例表示 1 弧度 / 1 米
        static constexpr double kNormRatio = 10.0;
        return fabs(angle) / M_PI * kNormRatio;
      };

      // 计算总路径代价 = 已走路径 + 到终点距离惩罚 + 角度差惩罚
      node->SetPathCost(current_node->GetPathCost() +
                        warm_start_config_.end_pose_diff_penalty *
                            (cur_to_end_dis + norm_angle_to_dis(ang_diff)));

      std::cout << "  节点代价: PathCost=" << node->GetPathCost()
                << ", 到终点距离=" << cur_to_end_dis
                << ", 角度差=" << (ang_diff * 180.0 / M_PI) << "°" << std::endl;
      end_node_pq_.push(std::move(node));
    }
    std::cout << "  当前 end_node_pq_ 大小: " << end_node_pq_.size() << std::endl;
    return true;
  }

  // ========== 第2步：检查终点姿态是否有效 ==========
  if (!is_valid_end_pose_) {
    return false;
  }

  // ========== 第3步：尝试生成 Reed-Shepp 路径 ==========
  // Reed-Shepp 是一种最短路径算法，尝试直接连接当前点到终点
  std::shared_ptr<ReedSheppPath> reeds_shepp_to_check =
      std::make_shared<ReedSheppPath>();
  if (!reed_shepp_generator_->ShortestRSP(current_node, end_node_,
                                          reeds_shepp_to_check)) {
    return false;
  }

  // ========== 第4步：检查 RS 路径是否合法 ==========
  // 验证 RS 路径是否会碰撞障碍物
  if (!RSPCheck(current_node, reeds_shepp_to_check, obstacles_segments_vec)) {
    return false;
  }

  ADEBUG << "RStypes  =  " << reeds_shepp_to_check->segs_lengths.size();

  // ========== 第5步：加载 RS 路径到闭集 ==========
  // 将整个 RS 路径作为节点加入到已搜索集合中
  auto node = LoadRSPinCS(reeds_shepp_to_check, current_node);

  // ========== 第6步：计算路径代价 ==========
  // 检查是否需要考虑起点曲率代价
  const bool check_start_point_kappa_cost =
      path_search_strategy_.enable_init_kappa_cost &&
      (!path_search_strategy_.is_plan_from_start ||
       (path_search_strategy_.is_plan_from_start &&
        *current_node == *start_node_));

  // 计算 RS 路径的转向裕度
  const double steer_margin =
      check_start_point_kappa_cost
          ? CaculateRsPathSteerMargion(reeds_shepp_to_check)
          : 0.0;

  // 设置节点的总路径代价
  node->SetPathCost(current_node->GetPathCost() +
                    EvaluateRsPathCost(reeds_shepp_to_check, current_node) +
                    warm_start_config_.steer_margin_penalty * steer_margin);
  end_node_pq_.push(std::move(node));
  return false;
}

// ========== 函数：计算 RS 路径转向裕度 ==========
// 【用最简单的话解释】：
// 计算 Reed-Shepp 路径末端的转向角度与当前转向角度的差距
double HybridAStar::CaculateRsPathSteerMargion(
    const std::shared_ptr<ReedSheppPath>& reeds_shepp_path) {
  const auto rs_path_size = reeds_shepp_path->x.size();
  double steer_margin = 0.0;

  if (rs_path_size > 2) {
    // 计算路径末端两点之间的角度变化
    double dtheta = common::math::NormalizeAngle(
        reeds_shepp_path->phi.at(rs_path_size - 1) -
        reeds_shepp_path->phi.at(rs_path_size - 2));

    // 计算路径末端两点之间的距离
    double ds = std::hypot(reeds_shepp_path->x.at(rs_path_size - 1) -
                               reeds_shepp_path->x.at(rs_path_size - 2),
                           reeds_shepp_path->y.at(rs_path_size - 1) -
                               reeds_shepp_path->y.at(rs_path_size - 2));

    // 根据角度变化和距离计算曲率
    double kappa =
        reeds_shepp_path->gear.back() ? dtheta / ds : -1 * dtheta / ds;

    // 将曲率转换为转向角
    double steer_angle = TransKappaToSteering(kappa);

    // 计算转向裕度（当前转向角与目标转向角的差）
    steer_margin = fabs(steer_angle - cur_steer_angle_);
  }
  ADEBUG << "steer_margin is " << steer_margin;
  return steer_margin;
}

// ========== 函数：评估 RS 路径代价 ==========
// 【用最简单的话解释】：
// 计算 Reed-Shepp 路径的总代价，包括路径长度、换挡次数等
double HybridAStar::EvaluateRsPathCost(
    const std::shared_ptr<ReedSheppPath>& reeds_shepp_path,
    const std::shared_ptr<Node3d>& node_ptr) {
  int gear_shift_cnt = 0;  // 换挡次数
  std::vector<double> rs_seg_length;  // 各段路径长度

  // 计算换挡次数和各段长度
  RSShiftTimes(node_ptr, reeds_shepp_path, &gear_shift_cnt, &rs_seg_length);

  // 根据场景选择不同的换挡惩罚系数
  double path_gear_switch_penalty =
      path_search_strategy_.is_nns_adjust_senario
          ? warm_start_config_.path_gear_switch_penalty_nns_adjust
          : warm_start_config_.path_gear_switch_penalty;

  // 计算基础代价 = 总长度 + 换挡惩罚
  double rs_cost = reeds_shepp_path->total_length +
                   gear_shift_cnt * path_gear_switch_penalty;

  // 累加每段路径的长度代价
  for (const auto seg_length : rs_seg_length) {
    rs_cost += CalculatePathLengthCost(seg_length);
  }

  // 如果不是 NNS 调整场景，直接返回
  if (!path_search_strategy_.is_nns_adjust_senario) {
    return rs_cost;
  }

  // NNS 场景下，额外计算参考线代价
  for (int i = 0; i < reeds_shepp_path->x.size(); ++i) {
    rs_cost += GetReferenceLineCost(reeds_shepp_path->x[i],
                                    reeds_shepp_path->y[i], rs_step_size_,
                                    path_search_strategy_.reference_line);
  }
  return rs_cost;
}

// ========== 函数：RS 路径检查 ==========
// 【用最简单的话解释】：
// 检查 Reed-Shepp 路径是否合法，包括路径方向、换挡次数、碰撞检测等
bool HybridAStar::RSPCheck(
    const std::shared_ptr<Node3d>& current_node,
    const std::shared_ptr<ReedSheppPath>& reeds_shepp_to_end,
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        obstacles_segments_vec) {
  // 检查路径是否为空
  if (reeds_shepp_to_end->x.empty()) {
    return false;
  }

  // ========== 第1步：检查路径方向约束 ==========
  if (forced_path_direction_ != 0) {
    // 检查初始路径方向（从起点规划）
    if (path_search_strategy_.is_plan_from_start &&
        *current_node == *start_node_) {
      int init_gear = reeds_shepp_to_end->gear.front() ? 1 : -1;  // 1=前进，-1=后退
      if (init_gear * forced_path_direction_ < 0) {
        ADEBUG << "gear not satisfied init_gear: " << init_gear;
        return false;  // 方向不符合要求
      }
    }

    // 检查初始路径方向（从终点反向规划）
    if (!path_search_strategy_.is_plan_from_start) {
      int init_gear = reeds_shepp_to_end->gear.back() ? -1 : 1;  // 反向规划时符号相反
      if (init_gear * forced_path_direction_ < 0) {
        ADEBUG << "gear not satisfied init_gear: " << init_gear;
        return false;
      }
    }
  }

  // ========== 第2步：检查换挡次数约束 ==========
  // 在 NNS 调整场景下，确保只使用无换挡路径
  if (path_search_strategy_.is_nns_adjust_senario) {
    int gear_shift_cnt = 0;
    RSShiftTimes(current_node, reeds_shepp_to_end, &gear_shift_cnt);
    if (gear_shift_cnt > 0) {
      return false;  // 有换挡，不符合要求
    }
  }

  // ========== 第3步：碰撞检测 ==========
  // 创建节点对象并检查是否与障碍物碰撞
  std::shared_ptr<Node3d> node = std::make_shared<Node3d>(
      reeds_shepp_to_end->x, reeds_shepp_to_end->y, reeds_shepp_to_end->phi,
      xy_bounds_, xy_grid_resolution_, phi_grid_resolution_);
  return ValidityCheck(node, obstacles_segments_vec);  // 返回碰撞检测结果
}

// ========== 函数：计算 RS 路径换挡次数 ==========
// 【用最简单的话解释】：
// 统计 Reed-Shepp 路径中换挡的次数（前进↔后退切换），并记录每段的长度
void HybridAStar::RSShiftTimes(
    const std::shared_ptr<Node3d>& current_node,
    const std::shared_ptr<ReedSheppPath>& reeds_shepp_to_end,
    int* const rs_shift_times, std::vector<double>* const rs_seg_length) {
  if (nullptr == rs_shift_times) {
    return;
  }

  *rs_shift_times = 0;  // 换挡次数初始化为0
  std::vector<double> seg_lengths;  // 各段长度
  bool prev_gear = true;  // 上一个档位状态
  double seg_length = current_node->GetNearestGearShiftLength();  // 当前段长度

  // 遍历路径上的每个点，检测换挡
  for (size_t i = 0; i < reeds_shepp_to_end->gear.size(); ++i) {
    if (i == 0) {
      // 检查第一个点是否与当前节点方向不同
      if (nullptr != current_node && nullptr != current_node->GetPreNode() &&
          current_node->GetDirec() != reeds_shepp_to_end->gear[0]) {
        ++(*rs_shift_times);  // 换挡次数+1
        seg_length = 0.0;
      }
    } else if (prev_gear != reeds_shepp_to_end->gear[i]) {
      // 检测到换挡（前进↔后退切换）
      seg_lengths.emplace_back(seg_length);  // 保存上一段长度
      seg_length = 0.0;  // 重置当前段长度
      ++(*rs_shift_times);  // 换挡次数+1
    } else {
      // 未换挡，累加当前段长度
      seg_length +=
          std::hypot(reeds_shepp_to_end->x[i] - reeds_shepp_to_end->x[i - 1],
                     reeds_shepp_to_end->y[i] - reeds_shepp_to_end->y[i - 1]);
    }
    prev_gear = reeds_shepp_to_end->gear[i];  // 更新上一个档位状态
  }

  seg_lengths.emplace_back(seg_length);  // 保存最后一段长度
  if (nullptr != rs_seg_length) {
    rs_seg_length->assign(seg_lengths.begin(), seg_lengths.end());  // 返回各段长度
  }
}

// ========== 函数：加载 RS 路径到闭集 ==========
// 【用最简单的话解释】：
// 将 Reed-Shepp 路径的终点作为节点加入到已搜索集合（闭集）中
std::shared_ptr<Node3d> HybridAStar::LoadRSPinCS(
    const std::shared_ptr<ReedSheppPath>& reeds_shepp_to_end,
    const std::shared_ptr<Node3d>& current_node) {
  // 创建终点节点
  std::shared_ptr<Node3d> end_node = std::make_shared<Node3d>(
      reeds_shepp_to_end->x, reeds_shepp_to_end->y, reeds_shepp_to_end->phi,
      xy_bounds_, xy_grid_resolution_, phi_grid_resolution_);

  // 设置父节点（用于回溯路径）
  end_node->SetPre(current_node);

  // 将节点加入闭集（已搜索集合）
  close_set_.emplace(end_node->GetIndex(), end_node);

  return end_node;
}

// ========== 函数：生成下一个节点 ==========
// 【用最简单的话解释】：
// 从当前节点出发，尝试不同的转向角和方向（前进/后退），生成一个新的可能节点
std::shared_ptr<Node3d> HybridAStar::Next_node_generator(
    const std::shared_ptr<Node3d>& current_node, size_t next_node_index,
    const bool is_collison_free_exploration) {
  // TODO(lsy): 在 NNS 调整场景中裁剪大曲率变化节点
  double steering = 0.0;       // 转向角
  double traveled_distance = 0.0;  // 行驶距离

  // ========== 第1步：判断是前进还是后退 ==========
  if (static_cast<double>(next_node_index) <
      static_cast<double>(next_node_num_) / 2) {
    // 前半部分索引对应前进节点

    // 检查是否需要裁剪前进扩展
    if (!current_node->GetDirec() &&  // 当前节点是后退方向
        current_node->GetNearestGearShiftLength() > 0 &&
        (current_node->GetNearestGearShiftLength() <
             warm_start_config_.min_one_direction_length ||  // 单方向长度太短
         is_collison_free_exploration)) {  // 或是无碰撞探索
      ADEBUG << "forward expansion cutted off once, current node length: "
             << current_node->GetNearestGearShiftLength();
      ++nextgen_forward_length_cutoff_;
      return nullptr;  // 不生成前进节点
    }

    // 检查初始路径搜索策略是否允许前进
    if (forced_path_direction_ == -1 &&  // 强制后退
        path_search_strategy_.is_plan_from_start &&
        *current_node == *start_node_) {
      AINFO << "forward expansion cutted off by initial path search strategy";
      ++nextgen_forward_init_cutoff_;
      return nullptr;
    }

    steering = steer_seq_[next_node_index];  // 获取转向角

    // 根据场景和动作裁剪前进节点生成
    if (!is_collison_free_exploration &&
        ForwardSteerActionCutoff(current_node, steering)) {
      ++nextgen_forward_action_cutoff_;
      return nullptr;
    }

    traveled_distance = step_size_;  // 前进距离为正

  } else {
    // 后半部分索引对应后退节点

    // TODO(lsy): 某些 NNS 调整场景会启用后退节点
    if (path_search_strategy_.is_nns_adjust_senario) {
      ++nextgen_backward_nns_cutoff_;
      return nullptr;  // NNS 场景下不生成后退节点
    }

    // 检查是否需要裁剪后退扩展
    if (current_node->GetDirec() &&  // 当前节点是前进方向
        current_node->GetNearestGearShiftLength() > 0 &&
        (current_node->GetNearestGearShiftLength() <
             warm_start_config_.min_one_direction_length ||
         is_collison_free_exploration)) {
      ADEBUG << "Backward expansion cutted off once, current node length: "
             << current_node->GetNearestGearShiftLength();
      ++nextgen_backward_length_cutoff_;
      return nullptr;
    }

    // 检查初始路径搜索策略是否允许后退
    if (forced_path_direction_ == 1 &&  // 强制前进
        path_search_strategy_.is_plan_from_start &&
        *current_node == *start_node_) {
      AINFO << "Backward expansion cutted off by initial path search strategy";
      ++nextgen_backward_init_cutoff_;
      return nullptr;
    }

    size_t index = next_node_index - next_node_num_ / 2;
    steering = steer_seq_[index];  // 获取转向角

    // 根据场景和动作裁剪后退节点生成
    if (!is_collison_free_exploration &&
        BackwardSteerActionCutoff(current_node, steering)) {
      ++nextgen_backward_action_cutoff_;
      return nullptr;
    }

    traveled_distance = -step_size_;  // 后退距离为负
  }

  // ========== 第2步：生成运动曲线 ==========
  // 使用上述运动原语生成一条驱动汽车到不同网格的曲线
  double arc = std::sqrt(2) * xy_grid_resolution_;  // 弧长
  std::vector<double> intermediate_x;
  std::vector<double> intermediate_y;
  std::vector<double> intermediate_phi;
  double last_x = current_node->GetX();
  double last_y = current_node->GetY();
  double last_phi = current_node->GetPhi();

  // 添加起点
  intermediate_x.push_back(last_x);
  intermediate_y.push_back(last_y);
  intermediate_phi.push_back(last_phi);

  // 沿弧长生成中间点
  for (size_t i = 0; i <= static_cast<int>(arc / step_size_); ++i) {
    const double next_x = last_x + traveled_distance * std::cos(last_phi);
    const double next_y = last_y + traveled_distance * std::sin(last_phi);
    const double next_phi = common::math::NormalizeAngle(
        last_phi +
        traveled_distance / vehicle_param_.wheel_base * std::tan(steering));

    intermediate_x.push_back(next_x);
    intermediate_y.push_back(next_y);
    intermediate_phi.push_back(next_phi);

    last_x = next_x;
    last_y = next_y;
    last_phi = next_phi;
  }

  // ========== 第3步：检查边界 ==========
  // 检查车辆是否超出 XY 边界
  double x = intermediate_x.back();
  double y = intermediate_y.back();
  bool out_of_bounds = false;

  if (x > xy_bounds_[1]) {
    double exceed = x - xy_bounds_[1];
    ++nextgen_bounds_x_upper_cutoff_;
    max_x_upper_exceed_ = std::max(max_x_upper_exceed_, exceed);
    if (nextgen_bounds_x_upper_cutoff_ <= 3) {
      ADEBUG << "[剪枝] x超上界: x=" << x << " > " << xy_bounds_[1]
             << ", 超出=" << exceed << "m";
    }
    out_of_bounds = true;
  }
  if (x < xy_bounds_[0]) {
    double exceed = xy_bounds_[0] - x;
    ++nextgen_bounds_x_lower_cutoff_;
    max_x_lower_exceed_ = std::max(max_x_lower_exceed_, exceed);
    if (nextgen_bounds_x_lower_cutoff_ <= 3) {
      ADEBUG << "[剪枝] x超下界: x=" << x << " < " << xy_bounds_[0]
             << ", 超出=" << exceed << "m";
    }
    out_of_bounds = true;
  }
  if (y > xy_bounds_[3]) {
    double exceed = y - xy_bounds_[3];
    ++nextgen_bounds_y_upper_cutoff_;
    max_y_upper_exceed_ = std::max(max_y_upper_exceed_, exceed);
    if (nextgen_bounds_y_upper_cutoff_ <= 3) {
      ADEBUG << "[剪枝] y超上界: y=" << y << " > " << xy_bounds_[3]
             << ", 超出=" << exceed << "m";
    }
    out_of_bounds = true;
  }
  if (y < xy_bounds_[2]) {
    double exceed = xy_bounds_[2] - y;
    ++nextgen_bounds_y_lower_cutoff_;
    max_y_lower_exceed_ = std::max(max_y_lower_exceed_, exceed);
    if (nextgen_bounds_y_lower_cutoff_ <= 3) {
      ADEBUG << "[剪枝] y超下界: y=" << y << " < " << xy_bounds_[2]
             << ", 超出=" << exceed << "m";
    }
    out_of_bounds = true;
  }

  if (out_of_bounds) {
    return nullptr;  // 超出边界，返回空指针
  }

  // ========== 第4步：创建新节点 ==========
  // 用生成的轨迹创建新节点
  std::shared_ptr<Node3d> next_node = std::make_shared<Node3d>(
      intermediate_x, intermediate_y, intermediate_phi, xy_bounds_,
      xy_grid_resolution_, phi_grid_resolution_);

  // 设置节点关系和属性
  next_node->SetPre(current_node);  // 设置父节点
  next_node->SetDirec(traveled_distance > 0.0);  // 设置方向（true=前进，false=后退）
  next_node->SetSteer(steering);  // 设置转向角

  // 处理换挡情况
  if (current_node->GetDirec() != next_node->GetDirec()) {
    // 发生换挡，重置当前节点的换挡距离
    current_node->SetNearestGearShiftLength(0);
    next_node->SetNearestGearShiftLength(step_size_);
  } else {
    // 未换挡，累加距离
    next_node->SetNearestGearShiftLength(
        current_node->GetNearestGearShiftLength() + step_size_);
  }

  return next_node;
}

// ========== 函数：计算节点代价 ==========
// 【用最简单的话解释】：
// 计算从当前节点到新节点的代价，包括路径代价和启发式代价
void HybridAStar::CalculateNodeCost(const std::shared_ptr<Node3d>& current_node,
                                    const std::shared_ptr<Node3d>& next_node,
                                    const double distance_to_obstalce,
                                    const bool is_collison_free_exploration) {
  // 如果是无碰撞探索模式
  if (is_collison_free_exploration) {
    // 代价 = 障碍物过滤距离 - 实际到障碍物距离（越接近障碍物代价越大）
    next_node->SetPathCost(warm_start_config_.obstacle_filter_distance -
                           distance_to_obstalce);
    next_node->SetHeuCost(0.0);  // 启发式代价为0
    return;
  }

  // 正常模式下，计算路径代价
  next_node->SetPathCost(
      current_node->GetPathCost() +  // 累加之前的路径代价
      PathCost(current_node, next_node, distance_to_obstalce));  // 加上当前步的代价

  // 计算启发式代价（估计到终点的代价）
  // 基础方法：使用欧几里得距离估计
  double optimal_path_cost =
      std::sqrt(2.0) * std::hypot(next_node->GetX() - end_node_->GetX(),
                                  next_node->GetY() - end_node_->GetY());

  constexpr double kGain = 1.0;
  common::SLPoint next_node_sl;  // Frenet坐标系下的点（沿参考线的纵向s和横向l）
  common::SLPoint end_node_sl;

  // 如果是NNS调整场景且有参考线，使用参考线距离代替欧氏距离
  if (path_search_strategy_.is_nns_adjust_senario &&
      !path_search_strategy_.reference_line.reference_points().empty() &&
      path_search_strategy_.reference_line.XYToSL(
          common::math::Vec2d(next_node->GetX(), next_node->GetY()), &next_node_sl) &&
      path_search_strategy_.reference_line.XYToSL(
          common::math::Vec2d(end_node_->GetX(), end_node_->GetY()), &end_node_sl)) {
    // 使用沿参考线的纵向距离（s坐标差）
    optimal_path_cost = kGain * (end_node_sl.s - next_node_sl.s);
  }

  // 定义角度代价计算函数（Lambda表达式）
  auto angle_cost = [](double from_angle, double to_angle) {
    // 归一化角度差到[0,1]范围
    double diff_angle_norm =
        fabs(common::math::AngleDiff(from_angle, to_angle)) / M_PI;
    // 使用指数函数惩罚大角度差
    return 5.0 * (exp(diff_angle_norm) - 1.0);
  };

  // 累加角度代价（车辆朝向与目标朝向的差异）
  optimal_path_cost += angle_cost(next_node->GetPhi(), end_node_->GetPhi());

  // 设置节点的启发式代价
  next_node->SetHeuCost(optimal_path_cost);
}

// ========== 函数：计算路径代价 ==========
// 【用最简单的话解释】：
// 评估从当前节点到下一个节点的路径代价，包括行驶方向、换挡、转向、障碍物等多种因素
double HybridAStar::PathCost(const std::shared_ptr<Node3d>& current_node,
                             const std::shared_ptr<Node3d>& next_node,
                             const double distance_to_obstalce) const {
  double piecewise_cost = 0.0;  // 分段代价

  // ========== 第1项：行驶方向惩罚 ==========
  // 前进和后退有不同的惩罚系数（一般后退代价更大）
  if (next_node->GetDirec()) {
    // 前进方向
    piecewise_cost += static_cast<double>(next_node->GetStepSize() - 1) *
                      step_size_ * warm_start_config_.path_forward_penalty;
  } else {
    // 后退方向
    piecewise_cost += static_cast<double>(next_node->GetStepSize() - 1) *
                      step_size_ * warm_start_config_.path_back_penalty;
  }

  // ========== 第2项：换挡惩罚 ==========
  // 如果方向改变（前进↔后退），增加换挡惩罚
  if (current_node->GetDirec() != next_node->GetDirec()) {
    piecewise_cost +=
        path_search_strategy_.is_nns_adjust_senario
            ? warm_start_config_.path_gear_switch_penalty_nns_adjust
            : warm_start_config_.path_gear_switch_penalty;
  }

  // ========== 第3项：障碍物距离惩罚 ==========
  // 离障碍物越近，代价越大（在过滤距离内）
  if (distance_to_obstalce < warm_start_config_.obstacle_filter_distance &&
      warm_start_config_.obstacle_filter_distance > kEpsilon) {
    piecewise_cost +=
        (warm_start_config_.obstacle_filter_distance - distance_to_obstalce) /
        warm_start_config_.obstacle_filter_distance * step_size_ *
        warm_start_config_.obstacle_distance_penalty;
  }

  // ========== 第4项：转向变化惩罚 ==========
  // 转向角变化越大（方向盘打得越猛），代价越大
  piecewise_cost += warm_start_config_.path_steer_change_penalty *
                    std::fabs(common::math::AngleDiff(
                        next_node->GetSteer(), current_node->GetSteer()));

  // ========== 第5项：路径长度惩罚 ==========
  // 根据单方向行驶长度计算代价（鼓励少换挡）
  piecewise_cost +=
      next_node->GetDirec() == current_node->GetDirec()
          ? CalculatePathLengthCost(next_node->GetNearestGearShiftLength()) -
                CalculatePathLengthCost(
                    current_node->GetNearestGearShiftLength())  // 增量代价
          : CalculatePathLengthCost(next_node->GetNearestGearShiftLength());  // 完整代价

  // ========== 第6项：参考线偏离惩罚 ==========
  // NNS场景下，偏离参考线会增加代价
  if (path_search_strategy_.is_nns_adjust_senario) {
    piecewise_cost +=
        GetReferenceLineCost(next_node->GetX(), next_node->GetY(), step_size_,
                             path_search_strategy_.reference_line);
  }

  return piecewise_cost;
}

// ========== 函数：计算参考线代价 ==========
// 【用最简单的话解释】：
// 计算节点偏离参考线的代价，偏离越远代价越大（使用指数函数）
double HybridAStar::GetReferenceLineCost(
    const double x, const double y, const double step_size,
    const ReferenceLine& reference_line) const {
  // 如果没有参考线，返回0
  if (reference_line.reference_points().empty()) {
    return 0.0;
  }

  // 定义横向偏离的阈值
  constexpr double kNormalLateral = 0.5;  // 正常横向偏离距离（米）
  constexpr double kMinLateral = 0.2;     // 最小横向偏离（小于此值不惩罚）
  constexpr double kGain = 3.0;           // 指数增长系数

  common::SLPoint current_node_sl;  // Frenet坐标系下的点
  const auto current_node = common::math::Vec2d(x, y);

  // 将XY坐标转换为SL坐标（沿参考线的纵向s和横向l）
  if (!reference_line.XYToSL(current_node, &current_node_sl) ||
      std::fabs(current_node_sl.l) < kMinLateral) {
    return 0.0;  // 转换失败或偏离太小，不惩罚
  }

  // 使用指数函数计算代价（偏离越远，代价增长越快）
  return warm_start_config_.reference_line_bias_penalty * step_size *
         exp(kGain * (std::fabs(current_node_sl.l) - kNormalLateral) /
             kNormalLateral);
}

// ========== 函数：提取路径结果 ==========
// 【用最简单的话解释】：
// 从终点沿着父节点指针回溯到起点，提取完整路径（类似倒带回放）
bool HybridAStar::GetResult(PathGeneratorResult* const result,
                            const bool is_collison_free_exploration_path) {
  // 检查输入参数是否有效
  if (result == nullptr) {
    AERROR << "GetResult input check fails";
    return false;
  }

  // 从终点节点开始回溯
  std::shared_ptr<Node3d> current_node = final_node_;
  ADEBUG << "final_node: " << final_node_->GetX() << ", " << final_node_->GetY()
         << ", " << final_node_->GetPhi();

  // 定义三个数组保存路径的x, y, 朝向角
  std::vector<double> hybrid_a_x;
  std::vector<double> hybrid_a_y;
  std::vector<double> hybrid_a_phi;

  // ========== 主循环：回溯路径 ==========
  // 沿着父节点指针向前回溯，直到起点（父节点为空）
  while (current_node->GetPreNode() != nullptr) {
    // 获取当前节点的状态序列（每个节点可能包含多个点）
    std::vector<double> x = current_node->GetXs();
    std::vector<double> y = current_node->GetYs();
    std::vector<double> phi = current_node->GetPhis();

    // ========== 数据有效性检查 ==========
    // 检查数据是否为空
    if (x.empty() || y.empty() || phi.empty()) {
      AERROR << "result size check failed";
      return false;
    }
    // 检查三个数组长度是否一致
    if (x.size() != y.size() || x.size() != phi.size()) {
      AERROR << "states sizes are not equal";
      return false;
    }

    // ========== 反转数组 ==========
    // 因为存储顺序是从父节点到当前节点，需要反转
    std::reverse(x.begin(), x.end());
    std::reverse(y.begin(), y.end());
    std::reverse(phi.begin(), phi.end());

    // 删除最后一个点（避免与父节点重复）
    if (x.size() > 1) {
      x.pop_back();
      y.pop_back();
      phi.pop_back();
    }

    // ========== 合并到总路径 ==========
    // 将当前节点的点添加到总路径末尾
    hybrid_a_x.insert(hybrid_a_x.end(), x.begin(), x.end());
    hybrid_a_y.insert(hybrid_a_y.end(), y.begin(), y.end());
    hybrid_a_phi.insert(hybrid_a_phi.end(), phi.begin(), phi.end());

    // 移动到父节点，继续回溯
    current_node = current_node->GetPreNode();
  }

  // ========== 添加起点 ==========
  // 最后添加起点的状态
  hybrid_a_x.push_back(current_node->GetX());
  hybrid_a_y.push_back(current_node->GetY());
  hybrid_a_phi.push_back(current_node->GetPhi());

  // ========== 特殊处理：移除第一个点 ==========
  // 对于无碰撞探索路径，移除第一个点
  if (is_collison_free_exploration_path) {
    auto iter = hybrid_a_x.begin();
    hybrid_a_x.erase(iter);
    iter = hybrid_a_y.begin();
    hybrid_a_y.erase(iter);
    iter = hybrid_a_phi.begin();
    hybrid_a_phi.erase(iter);
  }

  // ========== 路径方向处理 ==========
  // 如果从起点规划或是探索路径，需要反转路径
  // （因为回溯是从终点到起点，需要改成从起点到终点）
  if (path_search_strategy_.is_plan_from_start ||
      is_collison_free_exploration_path) {
    std::reverse(hybrid_a_x.begin(), hybrid_a_x.end());
    std::reverse(hybrid_a_y.begin(), hybrid_a_y.end());
    std::reverse(hybrid_a_phi.begin(), hybrid_a_phi.end());
  }

  // ========== 起点修正 ==========
  // 如果从终点规划（非探索路径），强制第一个点为当前车辆位置
  if (!path_search_strategy_.is_plan_from_start &&
      !is_collison_free_exploration_path) {
    // force adc pose as path start point
    hybrid_a_x[0] = end_node_->GetX();
    hybrid_a_y[0] = end_node_->GetY();
    hybrid_a_phi[0] = end_node_->GetPhi();
  }

  // ========== 合并到结果 ==========
  // 将路径添加到输出结果中
  ADEBUG << " origin result size: " << result->x.size();
  (*result).x.insert((*result).x.end(), hybrid_a_x.begin(), hybrid_a_x.end());
  (*result).y.insert((*result).y.end(), hybrid_a_y.begin(), hybrid_a_y.end());
  (*result).phi.insert((*result).phi.end(), hybrid_a_phi.begin(),
                       hybrid_a_phi.end());
  ADEBUG << "new result size: " << result->x.size();

  // ========== 最终验证 ==========
  // 确保三个数组长度一致
  if (result->x.size() != result->y.size() ||
      result->x.size() != result->phi.size()) {
    AERROR << "state sizes not equal, "
           << "result->x.size(): " << result->x.size() << "result->y.size()"
           << result->y.size() << "result->phi.size()" << result->phi.size();
    return false;
  }
  return true;
}

// ========== 函数：计算泊车预完成条件 ==========
// 【用最简单的话解释】：
// 判断当前节点是否到达目标区域（位置+角度都满足要求）
bool HybridAStar::CalculateParkingPrefinishCondition(
    const std::shared_ptr<Node3d>& current_node) {
  // ========== 轨迹调整场景检查 ==========
  // 如果是轨迹调整模式，检查是否接近参考轨迹
  if (path_search_strategy_.trace_adjust_search_strategy.is_trace_adjust) {
    common::SLPoint current_node_sl;
    // 将当前节点转换为SL坐标
    if (path_search_strategy_.trace_adjust_search_strategy.trace_path.XYToSL(
            current_node->GetX(), current_node->GetY(), &current_node_sl)) {
      // 找到参考轨迹上最近的匹配点
      const auto matched_point =
          path_search_strategy_.trace_adjust_search_strategy.trace_path
              .Evaluate(current_node_sl.s);

      // 计算横向偏离和朝向角偏离
      const double diff_l = std::fabs(current_node_sl.l);  // 横向偏离
      const double diff_theta = std::fabs(common::math::AngleDiff(
          matched_point.theta, current_node->GetPhi()));  // 角度偏离

      // 如果横向和角度偏离都小于阈值，认为到达目标
      if (diff_l < path_search_strategy_.trace_adjust_search_strategy
                       .finish_l_threshold &&
          diff_theta < path_search_strategy_.trace_adjust_search_strategy
                           .finish_theta_threshold) {
        return true;
      }
    }
  }

  // ========== 普通泊车场景检查 ==========
  bool direction_check = true;  // 方向检查标志
  const auto& polygon = std::get<0>(dest_region_with_angle_);  // 目标区域多边形
  const double from_angle = std::get<1>(dest_region_with_angle_);  // 最小角度
  const double to_angle = std::get<2>(dest_region_with_angle_);    // 最大角度

  // 检查当前节点是否在目标区域内（位置+角度）
  bool angle_in_range = common::math::AngleInRange(current_node->GetPhi(), from_angle, to_angle);
  bool point_in_polygon = polygon.IsPointIn({current_node->GetX(), current_node->GetY()});
  bool reach_dest =
      (polygon.num_points() > 2 && angle_in_range && point_in_polygon);

  // 统计失败原因
  prefinish_check_count_++;
  if (angle_in_range) {
    prefinish_angle_in_range_count_++;
  } else {
    prefinish_angle_fail_count_++;
    double angle_diff = std::min(
        std::fabs(common::math::AngleDiff(current_node->GetPhi(), from_angle)),
        std::fabs(common::math::AngleDiff(current_node->GetPhi(), to_angle)));
    if (angle_diff < min_angle_diff_) {
      min_angle_diff_ = angle_diff;
    }
  }

  if (point_in_polygon) {
    prefinish_point_in_polygon_count_++;
  } else {
    prefinish_position_fail_count_++;
    // 计算到目标区域中心的距离
    double center_x = 0, center_y = 0;
    for (const auto& pt : polygon.points()) {
      center_x += pt.x();
      center_y += pt.y();
    }
    center_x /= polygon.num_points();
    center_y /= polygon.num_points();
    double dist = std::sqrt(pow(current_node->GetX() - center_x, 2) +
                           pow(current_node->GetY() - center_y, 2));
    if (dist < min_dist_to_target_) {
      min_dist_to_target_ = dist;
    }
  }

  // ========== 方向检查 ==========
  // 如果指定了强制路径方向，检查初始档位是否匹配
  if (reach_dest && forced_path_direction_ != 0 &&
      !path_search_strategy_.is_plan_from_start) {
    const int init_gear = current_node->GetDirec() ? -1 : 1;  // 1=前进, -1=后退
    // need check init direction
    direction_check = init_gear * forced_path_direction_ > 0;  // 同号表示方向一致
    if (!direction_check) {
      AERROR << "direction_check failed " << init_gear;
    }
  }

  return reach_dest && direction_check;
}

// ========== 函数：路径规划主函数（Hybrid A*核心） ==========
// 【用最简单的话解释】：
// 这是整个Hybrid A*算法的核心函数，从起点搜索到终点，生成泊车路径
// 类似于迷宫寻路，但要考虑车辆运动学约束（不能原地转向）
bool HybridAStar::Plan(
    const std::atomic<bool>& atomic_early_stop_flag,  // 提前停止标志
    const common::PathPoint& start_point,   // 起点（车辆当前位置）
    const common::PathPoint& end_point,     // 终点（泊车目标位置）
    const std::vector<double>& xy_bounds,   // 搜索边界[x_min, x_max, y_min, y_max]
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        obstacles_segments_vec,  // 障碍物线段+膨胀距离
    const DestRegionWithAng& dest_region_with_angle,  // 目标区域+角度范围
    const PathSearchStrategy& path_search_strategy,   // 搜索策略
    PathGeneratorResult* const result) {  // 输出：路径结果

  // ========== 第1步：输入参数检查 ==========
  // xy_bounds必须是4个元素（x_min, x_max, y_min, y_max）
  if (xy_bounds.size() != 4 || result == nullptr) {
    AERROR << "hybrid a star input check fails";
    return false;
  }

  // ========== 第2步：初始化搜索参数 ==========
  // 加载搜索策略和强制路径方向
  path_search_strategy_ = path_search_strategy;
  forced_path_direction_ = path_search_strategy_.init_path_direction;
  ADEBUG << path_search_strategy_.DebugString();

  // ========== 第3步：清空容器并重置数据 ==========
  // 清空上次搜索的残留数据
  open_set_.clear();   // 待探索节点集合
  close_set_.clear();  // 已探索节点集合
  result->reset();     // 重置结果
  bool has_search_extension_path = false;  // 是否有扩展路径

  // 设置目标区域（仅在从起点规划时使用）
  if (path_search_strategy_.is_plan_from_start) {
    dest_region_with_angle_ = dest_region_with_angle;
  } else {
    dest_region_with_angle_ = DestRegionWithAng();  // 从终点规划时清空
  }

  // 清空优先队列和终点节点
  open_pq_ = decltype(open_pq_)();  // 重新构造空队列
  final_node_ = nullptr;            // 最终节点
  is_valid_end_pose_ = true;        // 终点姿态是否有效

  // ========== 第4步：加载搜索边界 ==========
  // 设置搜索区域的边界
  xy_bounds_ = xy_bounds;

  // ========== 第5步：处理障碍物（为Reed-Shepp路径增加额外膨胀） ==========
  // Reed-Shepp路径需要更大的安全距离
  static constexpr double kEpsilon = 1e-3;
  auto obstacles_segments_vec_rs = obstacles_segments_vec;
  for (auto& obstacles_segments : obstacles_segments_vec_rs) {
    obstacles_segments.second +=
        obstacles_segments.second <= kEpsilon
            ? 0
            : warm_start_config_.extra_distance_for_rs;
  }

  // ========== 第6步：决定节点生成参数 ==========
  // 根据起点和终点位置，选择合适的步长和分辨率
  NodeGenerationParameterDecision(start_point, end_point);

  // ========== 第7步：生成局部扩展路径（避障探索） ==========
  // 如果起点附近有碰撞，先探索一段无碰撞路径
  common::PathPoint intermediate_point = start_point;

  if (!GenerateLocalExtensionPath(
          start_point, obstacles_segments_vec, &forced_path_direction_, result,
          &intermediate_point, &has_search_extension_path)) {
    AERROR << "generate collision free local Path fails";
    return false;
  }
  ADEBUG << "intermediate_point: " << intermediate_point.x << ", "
         << intermediate_point.y << "," << intermediate_point.theta;
  ADEBUG << "forced_path_direction: " << forced_path_direction_;

  // ========== 第8步：重新初始化搜索容器 ==========
  // 清空之前的探索数据，准备正式搜索
  open_set_.clear();
  close_set_.clear();
  final_node_ = nullptr;
  open_pq_ = decltype(open_pq_)();
  end_node_pq_ = decltype(end_node_pq_)();

  // ========== 第9步：生成起点和终点节点 ==========
  // 根据规划方向决定起点和终点
  start_node_ =
      path_search_strategy_.is_plan_from_start
          ? std::make_shared<Node3d>(intermediate_point.x,
                                     intermediate_point.y,
                                     intermediate_point.theta, xy_bounds_,
                                     xy_grid_resolution_, phi_grid_resolution_)
          : std::make_shared<Node3d>(end_point.x, end_point.y,
                                     end_point.theta, xy_bounds_,
                                     xy_grid_resolution_, phi_grid_resolution_);

  end_node_ = path_search_strategy_.is_plan_from_start
                  ? std::make_shared<Node3d>(
                        end_point.x, end_point.y, end_point.theta,
                        xy_bounds_, xy_grid_resolution_, phi_grid_resolution_)
                  : std::make_shared<Node3d>(
                        intermediate_point.x, intermediate_point.y,
                        intermediate_point.theta, xy_bounds_,
                        xy_grid_resolution_, phi_grid_resolution_);

  // ========== 第10步：处理轨迹调整场景 ==========
  // 如果是轨迹调整模式，沿参考线设置目标点
  if (path_search_strategy_.trace_adjust_search_strategy.is_trace_adjust) {
    xy_bounds_ = path_search_strategy.trace_adjust_search_strategy.xy_bounds;
    common::SLPoint start_node_sl;
    if (!path_search_strategy_.trace_adjust_search_strategy.trace_path.XYToSL(
            start_point.x, start_point.y, &start_node_sl)) {
      return false;
    }
    const auto target_s = std::min(
        start_node_sl.s +
            path_search_strategy_.trace_adjust_search_strategy.target_s,
        path_search_strategy.trace_adjust_search_strategy.trace_path.Length());
    const auto trace_adjust_target =
        path_search_strategy_.trace_adjust_search_strategy.trace_path.Evaluate(
            target_s);
    end_node_ = std::make_shared<Node3d>(
        trace_adjust_target.x, trace_adjust_target.y,
        trace_adjust_target.theta, xy_bounds_, xy_grid_resolution_,
        phi_grid_resolution_);
  }

  // ========== 第11步：验证起点和终点 ==========
  // 检查起点是否与障碍物碰撞
  if (!ValidityCheck(start_node_, obstacles_segments_vec)) {
    AERROR << "[规划失败] 起点与障碍物碰撞";
    AERROR << "  起点位置: (" << start_node_->GetX() << ", "
           << start_node_->GetY() << "), 角度: "
           << start_node_->GetPhi() * 180.0 / M_PI << "°";
    AERROR << "  障碍物数量: " << obstacles_segments_vec.size();
    AERROR << "  建议: 调整起点位置或移除附近障碍物";
    result->reset();
    return false;
  }

  // 检查终点是否与障碍物碰撞
  if (!ValidityCheck(end_node_, obstacles_segments_vec_rs)) {
    // 检查是否有有效的目标区域
    const bool has_valid_target_region =
        path_search_strategy_.is_plan_from_start &&
        std::get<0>(dest_region_with_angle).num_points() > 2;

    // 计算终点到最近障碍物的距离 (使用精确的Polygon-Segment距离)
    auto min_distance = common::math::GetMinDistance2ObstaclesSegments(
        end_node_->GetX(), end_node_->GetY(), end_node_->GetPhi(),
        obstacles_segments_vec, warm_start_config_.obstacle_filter_distance);
    is_valid_end_pose_ =
        common::math::double_type::ComparedToZero(min_distance) > 0;

    // 如果没有有效目标区域且终点无效，返回失败
    if (!has_valid_target_region && !is_valid_end_pose_) {
      AERROR << "[规划失败] 终点与障碍物碰撞";
      AERROR << "  终点位置: (" << end_node_->GetX() << ", "
             << end_node_->GetY() << "), 角度: "
             << end_node_->GetPhi() * 180.0 / M_PI << "°";
      AERROR << "  最近障碍物距离: " << min_distance << "m";
      AERROR << "  建议: 调整终点位置或移除附近障碍物";
      result->reset();
      return false;
    }

    // 动态调整Reed-Shepp路径的安全缓冲区
    obstacles_segments_vec_rs = obstacles_segments_vec;
    static constexpr double kMinBuffer = 0.25;  // 增加到 0.25m 以满足 NLP 约束
    const double rs_buffer = common::util::BoundedValue(
        0.0, warm_start_config_.extra_distance_for_rs,
        min_distance - kMinBuffer);
    for (auto& obstacles_segments : obstacles_segments_vec_rs) {
      obstacles_segments.second +=
          obstacles_segments.second <= kEpsilon ? 0 : rs_buffer;
    }
    ADEBUG << " RS buffer : " << rs_buffer;
    AINFO << (is_valid_end_pose_
                  ? "modified rs buffer to acclerate search "
                  : "end node is invaild, it will skip reeds shepp connecting");
  }
  // ========== 第12步：初始化开放集和优先队列 ==========
  // 将起点加入开放集，开始搜索
  start_node_->SetPathCost(
      CalculatePathLengthCost(start_node_->GetNearestGearShiftLength()));
  open_set_.emplace(start_node_->GetIndex(), start_node_);
  open_pq_.emplace(start_node_->GetIndex(), start_node_->GetCost());

  // 初始化探索信息（用于调试）
  auto expansion_info = planning_internal::ExpansionInfo(intermediate_point);

  // ========== 第13步：Hybrid A*主搜索循环开始 ==========
  size_t explored_node_num = 0;  // 已探索节点数
  double astar_start_time = Clock::NowInSeconds();
  double heuristic_time = 0.0;   // 启发式计算时间
  double rs_time = 0.0;          // Reed-Shepp连接尝试时间

  // 记录Reed-Shepp连接点（用于结果输出）
  result->rs_connect_point.x = end_node_->GetX();
  result->rs_connect_point.y = end_node_->GetY();
  result->rs_connect_point.theta = end_node_->GetPhi();

  // 添加详细调试信息
  size_t collision_fail_count = 0;  // 碰撞检测失败计数
  size_t closed_skip_count = 0;     // 已探索节点跳过计数
  size_t next_gen_fail_count = 0;   // 节点生成失败次数
  size_t open_set_skip_count = 0;   // 已在开放集中的节点跳过次数
  size_t expansion_attempt_count = 0;  // 扩展尝试次数（运动基元总数）
  size_t validity_success_count = 0;   // 通过碰撞检测次数

  // 节点生成pruning计数器初始化
  nextgen_forward_length_cutoff_ = 0;
  nextgen_forward_init_cutoff_ = 0;
  nextgen_forward_action_cutoff_ = 0;
  nextgen_backward_length_cutoff_ = 0;
  nextgen_backward_init_cutoff_ = 0;
  nextgen_backward_action_cutoff_ = 0;
  nextgen_backward_nns_cutoff_ = 0;

  // 边界越界剪枝计数器初始化
  nextgen_bounds_x_upper_cutoff_ = 0;
  nextgen_bounds_x_lower_cutoff_ = 0;
  nextgen_bounds_y_upper_cutoff_ = 0;
  nextgen_bounds_y_lower_cutoff_ = 0;
  max_x_upper_exceed_ = 0.0;
  max_x_lower_exceed_ = 0.0;
  max_y_upper_exceed_ = 0.0;
  max_y_lower_exceed_ = 0.0;

  // 终点条件检查计数器初始化
  prefinish_check_count_ = 0;
  prefinish_angle_in_range_count_ = 0;
  prefinish_point_in_polygon_count_ = 0;
  prefinish_angle_fail_count_ = 0;
  prefinish_position_fail_count_ = 0;
  min_dist_to_target_ = std::numeric_limits<double>::max();
  min_angle_diff_ = std::numeric_limits<double>::max();

  // 碰撞和扩展统计初始化
  collision_detail_print_count_ = 0;
  forward_expansion_attempt_ = 0;
  forward_expansion_success_ = 0;
  backward_expansion_attempt_ = 0;
  backward_expansion_success_ = 0;

  // 根据场景类型设置最大探索时间
  const double max_explore_time =
      path_search_strategy_.is_dead_end_scenario ||
              path_search_strategy_.is_narrow_passage_scenario
          ? warm_start_config_.dead_end_scenario_max_exploration_time
          : warm_start_config_.max_exploration_time;

  std::cout << "\n[Hybrid A*] ========== 搜索开始 ==========" << std::endl;
  std::cout << "[Hybrid A*] 起点: (" << start_node_->GetX() << ", " << start_node_->GetY()
            << "), 角度: " << (start_node_->GetPhi() * 180.0 / M_PI) << "°" << std::endl;
  std::cout << "[Hybrid A*] 终点: (" << end_node_->GetX() << ", " << end_node_->GetY()
            << "), 角度: " << (end_node_->GetPhi() * 180.0 / M_PI) << "°" << std::endl;
  std::cout << "[Hybrid A*] 障碍物数量: " << obstacles_segments_vec.size() << std::endl;

  // 打印车辆尺寸参数
  std::cout << "\n[车辆参数]" << std::endl;
  std::cout << "  尺寸: 长=" << vehicle_param_.length << "m, 宽=" << vehicle_param_.width
            << "m, 高=" << vehicle_param_.height << "m" << std::endl;
  std::cout << "  轴距: " << vehicle_param_.wheel_base << "m" << std::endl;
  std::cout << "  悬长: 前=" << vehicle_param_.front_overhang << "m, 后="
            << vehicle_param_.rear_overhang << "m" << std::endl;
  std::cout << "  中心偏移: 前边缘=" << vehicle_param_.front_edge_to_center
            << "m, 后边缘=" << vehicle_param_.back_edge_to_center << "m" << std::endl;
  std::cout << "  安全缓冲: " << vehicle_param_.safety_buffer << "m" << std::endl;

  // 打印转向性能限制
  double front_wheel_max_angle = vehicle_param_.max_steer_angle / vehicle_param_.steer_ratio;
  double min_turn_radius = vehicle_param_.GetMinTurningRadius();
  std::cout << "\n[转向性能限制]" << std::endl;
  std::cout << "  方向盘最大转角: " << vehicle_param_.max_steer_angle << " rad ("
            << (vehicle_param_.max_steer_angle * 180.0 / M_PI) << "°)" << std::endl;
  std::cout << "  前轮最大转角: " << front_wheel_max_angle << " rad ("
            << (front_wheel_max_angle * 180.0 / M_PI) << "°)" << std::endl;
  std::cout << "  转向比: " << vehicle_param_.steer_ratio << std::endl;
  std::cout << "  最小转弯半径: " << min_turn_radius << "m" << std::endl;
  std::cout << "  最大转角速率(方向盘): " << vehicle_param_.max_steer_angle_rate
            << " rad/s (" << (vehicle_param_.max_steer_angle_rate * 180.0 / M_PI) << "°/s)" << std::endl;
  std::cout << "  最大转角速率(前轮): " << (vehicle_param_.max_steer_angle_rate / vehicle_param_.steer_ratio)
            << " rad/s (" << (vehicle_param_.max_steer_angle_rate / vehicle_param_.steer_ratio * 180.0 / M_PI) << "°/s)" << std::endl;

  // 打印运动性能限制
  std::cout << "\n[运动性能限制]" << std::endl;
  std::cout << "  最大速度: " << vehicle_param_.max_velocity << " m/s ("
            << (vehicle_param_.max_velocity * 3.6) << " km/h)" << std::endl;
  std::cout << "  最大加速度: " << vehicle_param_.max_acceleration << " m/s²" << std::endl;
  std::cout << "  最大减速度: " << vehicle_param_.max_deceleration << " m/s²" << std::endl;

  // 打印搜索配置参数
  std::cout << "\n[搜索配置参数]" << std::endl;
  std::cout << "  网格分辨率: xy=" << warm_start_config_.xy_grid_resolution
            << "m, phi=" << warm_start_config_.phi_grid_resolution << " rad ("
            << (warm_start_config_.phi_grid_resolution * 180.0 / M_PI) << "°)" << std::endl;
  std::cout << "  扩展步长: " << warm_start_config_.step_size << "m" << std::endl;
  std::cout << "  扩展节点数: " << warm_start_config_.next_node_num << std::endl;
  std::cout << "  最大迭代次数: " << warm_start_config_.max_iterations << std::endl;
  std::cout << "  最大搜索时间: " << max_explore_time << "s" << std::endl;

  // 打印代价惩罚系数
  std::cout << "\n[代价惩罚系数]" << std::endl;
  std::cout << "  前进惩罚: " << warm_start_config_.traj_forward_penalty << std::endl;
  std::cout << "  倒车惩罚: " << warm_start_config_.traj_back_penalty << std::endl;
  std::cout << "  转向惩罚: " << warm_start_config_.traj_steer_penalty << std::endl;
  std::cout << "  转向变化惩罚: " << warm_start_config_.traj_steer_change_penalty << std::endl;
  std::cout << "  换挡惩罚: " << warm_start_config_.path_gear_switch_penalty << std::endl;
  std::cout << "  路径长度惩罚: " << warm_start_config_.path_length_penalty << std::endl;
  std::cout << "  转向裕度惩罚: " << warm_start_config_.steer_margin_penalty << std::endl;
  std::cout << "  障碍物距离惩罚: " << warm_start_config_.obstacle_distance_penalty << std::endl;

  // 打印安全距离限制
  std::cout << "\n[安全距离限制]" << std::endl;
  std::cout << "  障碍物过滤距离: " << warm_start_config_.obstacle_filter_distance << "m" << std::endl;
  std::cout << "  RS曲线额外距离: " << warm_start_config_.extra_distance_for_rs << "m" << std::endl;
  std::cout << "  几何路径额外距离: " << warm_start_config_.extra_distance_for_geometry_path << "m" << std::endl;

  // 打印搜索边界
  std::cout << "\n[搜索边界]" << std::endl;
  std::cout << "  X范围: [" << xy_bounds[0] << ", " << xy_bounds[1] << "]" << std::endl;
  std::cout << "  Y范围: [" << xy_bounds[2] << ", " << xy_bounds[3] << "]" << std::endl;

  std::cout << "[Hybrid A*] ====================================\n" << std::endl;

  std::vector<double> angle_vec;
  static constexpr double kExplorationStepTime = 1.0;
  static constexpr int kMaxStepNum = 4;
  double iter_num = 0;

  const auto distance_bucket = [](double dist) {
    if (dist < 1.0) {
      return 0U;
    } else if (dist < 2.0) {
      return 1U;
    } else if (dist < 3.0) {
      return 2U;
    } else if (dist < 5.0) {
      return 3U;
    } else if (dist < 8.0) {
      return 4U;
    } else if (dist < 12.0) {
      return 5U;
    } else if (dist < 20.0) {
      return 6U;
    }
    return 7U;
  };

  const auto dump_prefinish_stats = [this]() {
    std::cout << "\n[终点条件统计]" << std::endl;
    std::cout << "  检查次数: " << prefinish_check_count_ << std::endl;
    std::cout << "  角度满足次数: " << prefinish_angle_in_range_count_
              << " (" << (prefinish_check_count_ > 0 ? 100.0 * prefinish_angle_in_range_count_ / prefinish_check_count_ : 0.0) << "%)" << std::endl;
    std::cout << "  角度不满足次数: " << prefinish_angle_fail_count_
              << " (" << (prefinish_check_count_ > 0 ? 100.0 * prefinish_angle_fail_count_ / prefinish_check_count_ : 0.0) << "%)" << std::endl;
    std::cout << "  位置满足次数: " << prefinish_point_in_polygon_count_
              << " (" << (prefinish_check_count_ > 0 ? 100.0 * prefinish_point_in_polygon_count_ / prefinish_check_count_ : 0.0) << "%)" << std::endl;
    std::cout << "  位置不满足次数: " << prefinish_position_fail_count_
              << " (" << (prefinish_check_count_ > 0 ? 100.0 * prefinish_position_fail_count_ / prefinish_check_count_ : 0.0) << "%)" << std::endl;
    std::cout << "  最近目标区域距离: " << (min_dist_to_target_ < 1e6 ? min_dist_to_target_ : 99.0) << "m" << std::endl;
    std::cout << "  最小角度差: " << (min_angle_diff_ < 1e6 ? min_angle_diff_ * 180.0 / M_PI : 999.0) << "°" << std::endl;
  };

  std::cout << "[Hybrid A*] ROI范围: X[" << xy_bounds[0] << ", " << xy_bounds[1]
            << "], Y[" << xy_bounds[2] << ", " << xy_bounds[3] << "]" << std::endl;
  std::cout << "[Hybrid A*] max_y_cut_off=" << max_y_cut_off_
            << ", cut_off_strategy=" << path_search_strategy_.cut_off_strategy << std::endl;
  std::cout << "[Hybrid A*] max_explore_time=" << max_explore_time
            << "s, accept_exploration_time=" << warm_start_config_.accept_exploration_time
            << "s" << std::endl;
  std::cout << "[Hybrid A*] step_size=" << step_size_
            << ", rs_step_size=" << rs_step_size_
            << ", next_node_num=" << next_node_num_ << std::endl;

// LCOV_EXCL_START
#ifndef ISMDC
  if (FLAGS_enable_one_shoot_log) {
    std::cout << "[HybridAStar] Extension environment: obstacles_segments=" << obstacles_segments_vec.size() << std::endl;
  }
#endif
  // LCOV_EXCL_STOP

  double search_time = 0.0;

  // ========== 主循环：A*搜索 ==========
  // 循环弹出代价最小的节点，直到找到路径或队列为空
  while (!open_pq_.empty()) {
    // 检查是否需要提前停止
    if (atomic_early_stop_flag.load()) {
      break;
    }

    // 检查搜索时间是否超时
    search_time = Clock::NowInSeconds() - astar_start_time;
    if (search_time >
        max_explore_time + kExplorationStepTime * explored_overtime_times_) {
      if (explored_overtime_times_ < kMaxStepNum) {
        explored_overtime_times_++;
      }
      AERROR << "[规划失败] 搜索时间超时";
      AERROR << "  搜索时间: " << search_time << "s";
      AERROR << "  最大允许时间: " << (max_explore_time + kExplorationStepTime * explored_overtime_times_) << "s";
      AERROR << "  已探索节点: " << close_set_.size();
      AERROR << "  待探索节点: " << open_set_.size();
      AERROR << "  候选终点数: " << end_node_pq_.size();
      AERROR << "  碰撞失败次数: " << collision_fail_count;
      AERROR << "  已探索跳过次数: " << closed_skip_count;
      AERROR << "  节点生成失败次数: " << next_gen_fail_count;
      AERROR << "  开放集重复跳过: " << open_set_skip_count;
      AERROR << "  扩展尝试总数: " << expansion_attempt_count;
      AERROR << "  碰撞通过次数: " << validity_success_count;
      AERROR << "  建议: 增加max_exploration_time或简化障碍物环境";

      // 打印当前节点信息（随机采样，因为unordered_map无序）
      if (close_set_.size() > 0) {
        auto it = close_set_.begin();
        std::cout << "[DEBUG] 已探索节点采样: (" << it->second->GetX() << ", "
                  << it->second->GetY() << "), 角度: "
                  << (it->second->GetPhi() * 180.0 / M_PI) << "°" << std::endl;
        double dist_to_goal = std::sqrt(
            pow(it->second->GetX() - end_node_->GetX(), 2) +
            pow(it->second->GetY() - end_node_->GetY(), 2));
        std::cout << "[DEBUG] 距离目标点: " << dist_to_goal << "m" << std::endl;
      }

      dump_prefinish_stats();

      // 打印节点生成pruning统计
      size_t total_pruning = nextgen_forward_length_cutoff_ + nextgen_forward_init_cutoff_ +
                             nextgen_forward_action_cutoff_ + nextgen_backward_length_cutoff_ +
                             nextgen_backward_init_cutoff_ + nextgen_backward_action_cutoff_ +
                             nextgen_backward_nns_cutoff_ + nextgen_bounds_x_upper_cutoff_ +
                             nextgen_bounds_x_lower_cutoff_ + nextgen_bounds_y_upper_cutoff_ +
                             nextgen_bounds_y_lower_cutoff_;

      std::cout << "\n[节点生成Pruning统计]" << std::endl;
      std::cout << "  总剪枝次数: " << total_pruning << std::endl;

      std::cout << "  前进方向被剪枝总数: " << (nextgen_forward_length_cutoff_ + nextgen_forward_init_cutoff_ + nextgen_forward_action_cutoff_)
                << " (" << (total_pruning > 0 ? (nextgen_forward_length_cutoff_ + nextgen_forward_init_cutoff_ + nextgen_forward_action_cutoff_) * 100.0 / total_pruning : 0) << "%)" << std::endl;
      std::cout << "    - 长度约束: " << nextgen_forward_length_cutoff_ << std::endl;
      std::cout << "    - 初始化失败: " << nextgen_forward_init_cutoff_ << std::endl;
      std::cout << "    - 动作约束: " << nextgen_forward_action_cutoff_ << std::endl;

      std::cout << "  后退方向被剪枝总数: " << (nextgen_backward_length_cutoff_ + nextgen_backward_init_cutoff_ + nextgen_backward_action_cutoff_ + nextgen_backward_nns_cutoff_)
                << " (" << (total_pruning > 0 ? (nextgen_backward_length_cutoff_ + nextgen_backward_init_cutoff_ + nextgen_backward_action_cutoff_ + nextgen_backward_nns_cutoff_) * 100.0 / total_pruning : 0) << "%)" << std::endl;
      std::cout << "    - 长度约束: " << nextgen_backward_length_cutoff_ << std::endl;
      std::cout << "    - 初始化失败: " << nextgen_backward_init_cutoff_ << std::endl;
      std::cout << "    - 动作约束: " << nextgen_backward_action_cutoff_ << std::endl;
      std::cout << "    - 无最近邻: " << nextgen_backward_nns_cutoff_ << std::endl;

      size_t total_bounds = nextgen_bounds_x_upper_cutoff_ + nextgen_bounds_x_lower_cutoff_ +
                            nextgen_bounds_y_upper_cutoff_ + nextgen_bounds_y_lower_cutoff_;
      std::cout << "  边界越界剪枝总数: " << total_bounds
                << " (" << (total_pruning > 0 ? total_bounds * 100.0 / total_pruning : 0) << "%)" << std::endl;
      std::cout << "    - x超上界: " << nextgen_bounds_x_upper_cutoff_
                << " (" << (total_bounds > 0 ? nextgen_bounds_x_upper_cutoff_ * 100.0 / total_bounds : 0) << "%)"
                << ", 最大超出=" << max_x_upper_exceed_ << "m" << std::endl;
      std::cout << "    - x超下界: " << nextgen_bounds_x_lower_cutoff_
                << " (" << (total_bounds > 0 ? nextgen_bounds_x_lower_cutoff_ * 100.0 / total_bounds : 0) << "%)"
                << ", 最大超出=" << max_x_lower_exceed_ << "m" << std::endl;
      std::cout << "    - y超上界: " << nextgen_bounds_y_upper_cutoff_
                << " (" << (total_bounds > 0 ? nextgen_bounds_y_upper_cutoff_ * 100.0 / total_bounds : 0) << "%)"
                << ", 最大超出=" << max_y_upper_exceed_ << "m" << std::endl;
      std::cout << "    - y超下界: " << nextgen_bounds_y_lower_cutoff_
                << " (" << (total_bounds > 0 ? nextgen_bounds_y_lower_cutoff_ * 100.0 / total_bounds : 0) << "%)"
                << ", 最大超出=" << max_y_lower_exceed_ << "m" << std::endl;

      // 打印扩展方向统计
      std::cout << "\n[节点扩展方向统计]" << std::endl;
      std::cout << "  前进方向: 尝试=" << forward_expansion_attempt_
                << ", 成功=" << forward_expansion_success_
                << " (" << (forward_expansion_attempt_ > 0 ? forward_expansion_success_ * 100.0 / forward_expansion_attempt_ : 0) << "%)" << std::endl;
      std::cout << "  后退方向: 尝试=" << backward_expansion_attempt_
                << ", 成功=" << backward_expansion_success_
                << " (" << (backward_expansion_attempt_ > 0 ? backward_expansion_success_ * 100.0 / backward_expansion_attempt_ : 0) << "%)" << std::endl;
      std::cout << "  总体成功率: "
                << (expansion_attempt_count > 0 ? (forward_expansion_success_ + backward_expansion_success_) * 100.0 / expansion_attempt_count : 0)
                << "%" << std::endl;

      // 打印碰撞失败统计
      std::cout << "\n[碰撞失败原因统计]" << std::endl;
      std::cout << "  总碰撞检测: " << collision_stats_.total_collision_checks << "次" << std::endl;
      std::cout << "  总碰撞失败: " << collision_stats_.total_collision_failures << "次"
                << " (" << (collision_stats_.total_collision_checks > 0 ?
                    collision_stats_.total_collision_failures * 100.0 / collision_stats_.total_collision_checks : 0)
                << "%)" << std::endl;

      if (collision_stats_.total_collision_failures > 0) {
        std::cout << "\n  失败原因分布:" << std::endl;
        std::cout << "    - 安全距离不足: " << collision_stats_.safety_distance_violation
                  << "次 (" << (collision_stats_.safety_distance_violation * 100.0 / collision_stats_.total_collision_failures) << "%)" << std::endl;
        std::cout << "    - 边界越界: " << collision_stats_.boundary_violation
                  << "次 (" << (collision_stats_.boundary_violation * 100.0 / collision_stats_.total_collision_failures) << "%)" << std::endl;
        std::cout << "    - 转向角超限: " << collision_stats_.steer_angle_violation
                  << "次 (" << (collision_stats_.steer_angle_violation * 100.0 / collision_stats_.total_collision_failures) << "%)" << std::endl;
        std::cout << "    - 曲率超限: " << collision_stats_.curvature_violation
                  << "次 (" << (collision_stats_.curvature_violation * 100.0 / collision_stats_.total_collision_failures) << "%)" << std::endl;
        std::cout << "    - 车身碰撞: " << collision_stats_.vehicle_collision
                  << "次 (" << (collision_stats_.vehicle_collision * 100.0 / collision_stats_.total_collision_failures) << "%)" << std::endl;

        std::cout << "\n  方向分布:" << std::endl;
        std::cout << "    - 前进方向碰撞: " << collision_stats_.forward_collision
                  << "次 (" << (collision_stats_.forward_collision * 100.0 / collision_stats_.total_collision_failures) << "%)" << std::endl;
        std::cout << "    - 后退方向碰撞: " << collision_stats_.backward_collision
                  << "次 (" << (collision_stats_.backward_collision * 100.0 / collision_stats_.total_collision_failures) << "%)" << std::endl;

        if (collision_stats_.distance_sample_count > 0) {
          double avg_distance = collision_stats_.sum_obstacle_distance / collision_stats_.distance_sample_count;
          std::cout << "\n  障碍物距离统计:" << std::endl;
          std::cout << "    - 最小距离: " << collision_stats_.min_obstacle_distance << "m" << std::endl;
          std::cout << "    - 平均距离: " << avg_distance << "m" << std::endl;
          std::cout << "    - 采样数量: " << collision_stats_.distance_sample_count << std::endl;
        }

        if (!collision_stats_.obstacle_hit_count.empty()) {
          std::cout << "\n  最常碰撞障碍物 (Top 5):" << std::endl;
          std::vector<std::pair<size_t, size_t>> hit_vec(
              collision_stats_.obstacle_hit_count.begin(),
              collision_stats_.obstacle_hit_count.end());
          std::sort(hit_vec.begin(), hit_vec.end(),
                    [](const auto& a, const auto& b) { return a.second > b.second; });
          for (size_t i = 0; i < std::min(size_t(5), hit_vec.size()); ++i) {
            std::cout << "    - 障碍物#" << hit_vec[i].first << ": "
                      << hit_vec[i].second << "次碰撞 ("
                      << (hit_vec[i].second * 100.0 / collision_stats_.total_collision_failures) << "%)" << std::endl;
          }
        }

        // 打印迭代阶段统计（每10000次为一阶段）
        std::cout << "\n  迭代阶段碰撞统计 (每10000次迭代):" << std::endl;
        bool has_stage_data = false;
        for (size_t stage = 0; stage < 10 && stage < collision_stats_.stage_collisions.size(); ++stage) {
          if (collision_stats_.stage_collisions[stage] > 0) {
            has_stage_data = true;
            std::cout << "    - 第" << (stage * 10000) << "-" << ((stage + 1) * 10000)
                      << "次: " << collision_stats_.stage_collisions[stage] << "次碰撞" << std::endl;
          }
        }
        if (!has_stage_data) {
          std::cout << "    (无阶段数据)" << std::endl;
        }
      }

      result->reset();
      return false;
    }

    // 如果超过可接受时间且有候选结果，提前返回
    if (search_time > warm_start_config_.accept_exploration_time &&
        !end_node_pq_.empty()) {
      AINFO << "[Hybrid A*] 达到可接受搜索时间，提前结束";
      AINFO << "  搜索时间: " << search_time << "s";
      AINFO << "  候选终点数: " << end_node_pq_.size();
      AINFO << "  已探索节点: " << close_set_.size();
      AINFO << "  待探索节点: " << open_set_.size();
      break;
    }

    // ========== 取出代价最小的节点 ==========
    // 从优先队列中取出代价最小的节点（贪心策略）
    const std::string current_id = open_pq_.top().first;
    open_pq_.pop();
    const std::shared_ptr<Node3d>& current_node = open_set_[current_id];

    // LCOV_EXCL_START
#ifndef ISMDC
    if (FLAGS_enable_one_shoot_log) {
      std::cout << "[HybridAStar] Iter " << iter_num << ": current_id=" << current_id
                << ", cost=" << current_node->GetCost()
                << ", steer=" << current_node->GetSteer() << std::endl;
    }
#endif
    // LCOV_EXCL_STOP

    // ========== 尝试直接连接到终点（解析式扩展） ==========
    // 尝试用Reed-Shepp曲线直接连接当前节点到终点
    const double rs_start_time = Clock::NowInSeconds();
    if (AnalyticExpansion(current_node, obstacles_segments_vec_rs)) {
      break;  // 成功连接，搜索结束
    }

    const double rs_end_time = Clock::NowInSeconds();
    rs_time += rs_end_time - rs_start_time;

    // 将当前节点加入已探索集合
    close_set_.emplace(current_node->GetIndex(), current_node);

    // ========== 扩展相邻节点 ==========
    double distance_to_obstalce = kDefaultIgnorableDist;
    size_t valid_expansion = 0;  // 本轮有效扩展计数

    for (size_t i = 0; i < next_node_num_; ++i) {
      if (atomic_early_stop_flag.load()) {
        break;
      }

      expansion_attempt_count++;

      // 生成下一个候选节点（前进/后退 + 不同转向角）
      const auto& next_node = Next_node_generator(current_node, i);

      // 统计扩展方向（基于i的值判断前进/后退）
      bool is_forward = (i < next_node_num_ / 2);
      if (is_forward) {
        forward_expansion_attempt_++;
      } else {
        backward_expansion_attempt_++;
      }

      if (next_node == nullptr) {
        next_gen_fail_count++;
        continue;
      }

      // 跳过已探索的节点
      if (close_set_.find(next_node->GetIndex()) != close_set_.end()) {
        closed_skip_count++;
        continue;
      }

      // 如果是新节点，检查碰撞并计算代价
      if (open_set_.find(next_node->GetIndex()) == open_set_.end()) {
        double distance_to_obstacle = 0.0;
        bool is_valid = ValidityCheck(next_node, obstacles_segments_vec,
                           &distance_to_obstacle);

        // 更新统计信息
        collision_stats_.total_collision_checks++;
        size_t current_stage = close_set_.size() / 10000;
        if (current_stage < collision_stats_.stage_collisions.size()) {
          if (!is_valid) {
            collision_stats_.stage_collisions[current_stage]++;
          }
        }

        if (!is_valid) {
          collision_fail_count++;
          collision_stats_.total_collision_failures++;

          // 方向统计
          bool is_forward = next_node->GetDirec();
          if (is_forward) {
            collision_stats_.forward_collision++;
          } else {
            collision_stats_.backward_collision++;
          }

          // 分析失败原因
          std::vector<std::string> failure_reasons;
          double x = next_node->GetX();
          double y = next_node->GetY();
          double phi = next_node->GetPhi();
          double steer = next_node->GetSteer();

          // 1. 边界检查
          bool boundary_fail = false;
          if (x > xy_bounds_[1] || x < xy_bounds_[0] ||
              y > xy_bounds_[3] || y < xy_bounds_[2]) {
            collision_stats_.boundary_violation++;
            boundary_fail = true;
            failure_reasons.push_back("边界越界");
          }

          // 2. 安全距离检查
          const double kMinSafeDistance = warm_start_config_.obstacle_filter_distance;
          if (distance_to_obstacle < kMinSafeDistance && !boundary_fail) {
            collision_stats_.safety_distance_violation++;
            collision_stats_.sum_obstacle_distance += distance_to_obstacle;
            collision_stats_.distance_sample_count++;
            if (distance_to_obstacle < collision_stats_.min_obstacle_distance) {
              collision_stats_.min_obstacle_distance = distance_to_obstacle;
            }
            // 采样保存最近的距离
            if (collision_stats_.recent_distances.size() < 1000) {
              collision_stats_.recent_distances.push_back(distance_to_obstacle);
            }

            char dist_buf[100];
            snprintf(dist_buf, sizeof(dist_buf), "安全距离不足(%.6fm < %.1fm)",
                     distance_to_obstacle, kMinSafeDistance);
            failure_reasons.push_back(dist_buf);

            // 使用精确的车辆轮廓到障碍物距离计算（使用现有函数）
            // 注意：这里使用 obstacle_filter_distance 参数过滤远距离障碍物
            double precise_distance = common::math::GetMinDistance2ObstaclesSegments(
                x, y, phi, obstacles_segments_vec,
                warm_start_config_.obstacle_filter_distance);

            // 找到最近的障碍物ID（遍历所有障碍物，找到距离最小的）
            size_t nearest_obstacle_id = 0;
            double min_obs_dist = std::numeric_limits<double>::max();
            for (size_t obs_id = 0; obs_id < obstacles_segments_vec.size(); ++obs_id) {
              const auto& obs_box = obstacles_segments_vec[obs_id].first;
              TL::common::math::Vec2d vehicle_pos(x, y);
              TL::common::math::Vec2d obs_center = obs_box.center();
              double center_dist = vehicle_pos.DistanceTo(obs_center);
              if (center_dist < min_obs_dist) {
                min_obs_dist = center_dist;
                nearest_obstacle_id = obs_id;
              }
            }
            collision_stats_.obstacle_hit_count[nearest_obstacle_id]++;
          }

          // 3. 转向角检查
          double front_wheel_angle = steer / vehicle_param_.steer_ratio;
          double max_front_wheel_angle = vehicle_param_.max_steer_angle / vehicle_param_.steer_ratio;
          if (std::abs(front_wheel_angle) > max_front_wheel_angle) {
            collision_stats_.steer_angle_violation++;
            failure_reasons.push_back("转向角超限(" +
                std::to_string(front_wheel_angle * 180.0 / M_PI) + "° > " +
                std::to_string(max_front_wheel_angle * 180.0 / M_PI) + "°)");
          }

          // 4. 曲率/转弯半径检查
          if (std::abs(front_wheel_angle) > 1e-6) {
            double turn_radius = vehicle_param_.wheel_base / std::tan(std::abs(front_wheel_angle));
            double min_turn_radius = vehicle_param_.GetMinTurningRadius();
            if (turn_radius < min_turn_radius * 0.9) {  // 留10%裕度
              collision_stats_.curvature_violation++;
              failure_reasons.push_back("转弯半径过小(" +
                  std::to_string(turn_radius) + "m < " +
                  std::to_string(min_turn_radius) + "m)");
            }
          }

          // 5. 如果没有其他原因，归类为车身直接碰撞
          if (failure_reasons.empty()) {
            collision_stats_.vehicle_collision++;
            failure_reasons.push_back("车身碰撞");
          }

          // 采样打印（每10次打印1次，最多100次）
          if (collision_stats_.sample_print_count < 100 &&
              collision_fail_count % 10 == 1) {
            collision_stats_.sample_print_count++;
            std::cout << "[碰撞失败 #" << collision_fail_count << "] "
                      << "位置:(" << x << "," << y << "), "
                      << "角度:" << (phi * 180.0 / M_PI) << "°, "
                      << "方向:" << (is_forward ? "前进" : "后退") << ", "
                      << "迭代:" << close_set_.size() << std::endl;
            std::cout << "  失败原因: ";
            for (size_t i = 0; i < failure_reasons.size(); ++i) {
              std::cout << failure_reasons[i];
              if (i < failure_reasons.size() - 1) std::cout << ", ";
            }
            std::cout << std::endl;
          }

          continue;  // 碰撞检查失败，跳过
        }

        valid_expansion++;
        validity_success_count++;
        explored_node_num++;

        // 记录成功扩展的方向
        if (is_forward) {
          forward_expansion_success_++;
        } else {
          backward_expansion_success_++;
        }

        // 计算节点代价（路径代价+启发式代价）
        const double start_time = Clock::NowInSeconds();
        CalculateNodeCost(current_node, next_node, distance_to_obstalce);
        const double end_time = Clock::NowInSeconds();
        heuristic_time += end_time - start_time;

        // 将新节点加入开放集和优先队列
        open_set_.emplace(next_node->GetIndex(), next_node);
        open_pq_.emplace(next_node->GetIndex(), next_node->GetCost());

        // LCOV_EXCL_START
#ifndef ISMDC
        if (FLAGS_enable_one_shoot_log) {
          std::cout << "[HybridAStar] Iter " << iter_num << ": extend from " << current_id
                    << " to " << next_node->GetIndex() << ", cost=" << next_node->GetCost() << std::endl;
        }
#endif
        // LCOV_EXCL_STOP
      } else {
        open_set_skip_count++;
      }
    }

    // 每1000次迭代打印一次进度
    iter_num = iter_num + 1;
    if (static_cast<int>(iter_num) % 1000 == 0) {
      std::cout << "[Hybrid A*] 迭代 " << iter_num << ": "
                << "已探索=" << close_set_.size()
                << ", 待探索=" << open_set_.size()
                << ", 候选终点=" << end_node_pq_.size()
                << ", 碰撞失败=" << collision_fail_count
                << ", 已探索跳过=" << closed_skip_count
                << ", 节点生成失败=" << next_gen_fail_count
                << ", 耗时=" << (Clock::NowInSeconds() - astar_start_time) << "s"
                << std::endl;

      // 打印当前节点与目标的距离
      double dist_to_goal = std::sqrt(
          pow(current_node->GetX() - end_node_->GetX(), 2) +
          pow(current_node->GetY() - end_node_->GetY(), 2));
      std::cout << "[Hybrid A*] 当前节点: (" << current_node->GetX() << ", "
                << current_node->GetY() << "), 距目标: " << dist_to_goal << "m" << std::endl;
    }
  }  // while循环结束

  // ========== 第14步：检查搜索结果 ==========
  ADEBUG << "end_node_pq_ " << end_node_pq_.size();

  // 如果没有最终节点但有候选节点队列，从队列中选择最优节点
  if (nullptr == final_node_ && !end_node_pq_.empty()) {
    std::cout << "\n[Final Node Selection] 从 end_node_pq_ 选择最终节点" << std::endl;
    std::cout << "  end_node_pq_ 候选节点数: " << end_node_pq_.size() << std::endl;

    // 显示选择的最佳节点信息（队列顶部=代价最小）
    auto top_node = end_node_pq_.top();
    std::cout << "  选择的最佳节点 (top): "
              << "位置=(" << top_node->GetX() << ", " << top_node->GetY()
              << "), 角度=" << (top_node->GetPhi() * 180.0 / M_PI) << "°"
              << ", PathCost=" << top_node->GetPathCost() << std::endl;

    final_node_ = end_node_pq_.top();

    std::cout << "  目标位置: (" << end_node_->GetX() << ", " << end_node_->GetY()
              << "), 角度=" << (end_node_->GetPhi() * 180.0 / M_PI) << "°" << std::endl;
    double dist_to_target = std::sqrt(
        pow(final_node_->GetX() - end_node_->GetX(), 2) +
        pow(final_node_->GetY() - end_node_->GetY(), 2));
    double angle_to_target = std::fabs(common::math::AngleDiff(
        final_node_->GetPhi(), end_node_->GetPhi())) * 180.0 / M_PI;
    std::cout << "  与目标点的偏差: 距离=" << dist_to_target << "m, 角度="
              << angle_to_target << "°" << std::endl;
  }

  // 如果没有找到有效路径，返回失败
  if (final_node_ == nullptr) {
    explored_failure_times_++;
    AERROR << "[规划失败] 搜索空间耗尽，未找到可行路径";
    AERROR << "  开放集大小: " << open_set_.size();
    AERROR << "  已探索节点: " << close_set_.size();
    AERROR << "  候选终点数: " << end_node_pq_.size();
    AERROR << "  搜索时间: " << search_time << "s";
    AERROR << "  失败次数: " << explored_failure_times_;
    AERROR << "  扩展尝试总数: " << expansion_attempt_count;
    AERROR << "  碰撞通过次数: " << validity_success_count;
    AERROR << "  开放集重复跳过: " << open_set_skip_count;
    AERROR << "  碰撞失败次数: " << collision_fail_count;
    AERROR << "  已探索跳过次数: " << closed_skip_count;
    AERROR << "  节点生成失败次数: " << next_gen_fail_count;

    // 计算起终点距离
    double start_to_end_dist = std::sqrt(
        pow(end_node_->GetX() - start_node_->GetX(), 2) +
        pow(end_node_->GetY() - start_node_->GetY(), 2));
    AERROR << "  起终点直线距离: " << start_to_end_dist << "m";
    AERROR << "  起点: (" << start_node_->GetX() << ", " << start_node_->GetY()
           << "), 角度: " << start_node_->GetPhi() * 180.0 / M_PI << "°";
    AERROR << "  终点: (" << end_node_->GetX() << ", " << end_node_->GetY()
           << "), 角度: " << end_node_->GetPhi() * 180.0 / M_PI << "°";
    AERROR << "  障碍物数量: " << obstacles_segments_vec.size();

    // 给出建议
    if (start_to_end_dist > 20.0) {
      AERROR << "  建议: 起终点距离过大(>20m)，考虑分段规划";
    }
    if (obstacles_segments_vec.size() > 50) {
      AERROR << "  建议: 障碍物过多(>50)，考虑简化环境或增加搜索时间";
    }
    if (close_set_.size() > 10000) {
      AERROR << "  建议: 已探索节点过多(>10000)，考虑优化网格分辨率或增加step_size";
    }
    AERROR << "  建议: 检查障碍物是否阻塞路径，或增加max_iterations/max_exploration_time";

    result->reset();
    return false;
  }

  // ========== 第15步：记录Reed-Shepp连接点 ==========
  // 如果最终节点有Reed-Shepp路径，记录连接点
  if (!final_node_->GetXs().empty()) {
    result->rs_connect_point.x = final_node_->GetXs().front();
    result->rs_connect_point.y = final_node_->GetYs().front();
    result->rs_connect_point.theta = final_node_->GetPhis().front();
  }

  // ========== 第16步：提取完整路径 ==========
  // 从终点回溯到起点，构建完整路径
  if (!GetResult(result)) {
    ADEBUG << "GetResult failed";
    result->reset();
    return false;
  }

  // 标记路径类型（扩展路径 or 普通搜索路径）
  result->path_type =
      has_search_extension_path
          ? static_cast<int>(planning_internal::PathType::SEARCH_EXTENSION_PATH)
          : static_cast<int>(planning_internal::PathType::SEARCH_PATH);

  // LCOV_EXCL_START
#ifndef ISMDC
  if (FLAGS_enable_one_shoot_log) {
    std::cout << "[HybridAStar] Coarse path found: points=" << (*result).x.size()
              << ", start=(" << (*result).x.front() << ", " << (*result).y.front() << ")"
              << ", end=(" << (*result).x.back() << ", " << (*result).y.back() << ")" << std::endl;
  }
#endif
  // LCOV_EXCL_STOP

  // ========== 第17步：重置计数器并输出统计信息 ==========
  explored_overtime_times_ = 0;
  explored_failure_times_ = 0;
  ADEBUG << "explored node num is " << explored_node_num;
  ADEBUG << "heuristic time is " << heuristic_time;
  ADEBUG << "reed shepp time is " << rs_time;
  ADEBUG << "hybrid astar total time is "
         << Clock::NowInSeconds() - astar_start_time;
  ADEBUG << "end_node_pq size is " << end_node_pq_.size();

  return true;  // 成功找到路径
}

// ========== 函数：节点生成参数决策 ==========
// 【用最简单的话解释】：
// 根据泊车场景类型（侧方/垂直）和历史失败次数，选择合适的搜索参数
// 类似于根据停车位大小调整倒车的精细程度
void HybridAStar::NodeGenerationParameterDecision(
    const common::PathPoint& start_point, const common::PathPoint& end_point) {
  static constexpr double kCutOffYChangeStepTime = 2.0;  // Y方向裁剪步进倍数

  // ========== 第1步：根据泊车场景选择参数 ==========
  // 侧方泊车（LAT_PARK_LOT）使用水平参数，垂直泊车使用默认参数
  if (path_search_strategy_.space_structure == SpaceStructure::LAT_PARK_LOT) {
    // 【侧方泊车参数】
    step_size_ = warm_start_config_.horizontal_step_size;  // 步长
    xy_grid_resolution_ = warm_start_config_.horizontal_xy_grid_resolution;  // XY网格分辨率
    phi_grid_resolution_ = warm_start_config_.horizontal_phi_grid_resolution;  // 角度网格分辨率
    max_y_cut_off_ = warm_start_config_.horizontal_max_y_cut_off;  // Y方向最大裁剪距离
  } else {
    // 【垂直泊车或其他场景参数】
    step_size_ = warm_start_config_.step_size;
    xy_grid_resolution_ = warm_start_config_.xy_grid_resolution;
    phi_grid_resolution_ = warm_start_config_.phi_grid_resolution;

    // 动态调整Y方向裁剪距离
    // 如果之前多次失败/超时，减小裁剪距离以扩大搜索范围
    max_y_cut_off_ = (path_search_strategy_.is_dead_end_scenario
                          ? warm_start_config_.dead_end_scenario_max_y_cut_off  // 死路场景特殊值
                          : warm_start_config_.max_y_cut_off)  // 正常值
                     - kCutOffYChangeStepTime * std::max(explored_overtime_times_,
                                                       explored_failure_times_);
    // 失败次数越多，搜索范围越大（max_y_cut_off_越小）
  }

  // ========== 第2步：设置RS曲线转向裕量 ==========
  // 根据是否使用更大曲率，选择转向角裕量
  lateral_rs_steer_angle_margin_ =
      path_search_strategy_.use_larger_curvature
          ? warm_start_config_.max_steer_angle_margin  // 大曲率：使用最大裕量
          : warm_start_config_.lateral_spot_steer_angle_margin;  // 小曲率：使用侧方泊车裕量

  // ========== 第3步：计算当前曲率对应的转向角 ==========
  // 根据规划方向选择参考点的曲率
  double cur_kappa = path_search_strategy_.is_plan_from_start
                         ? end_point.kappa   // 从起点规划：使用终点曲率
                         : start_point.kappa;  // 从终点规划：使用起点曲率
  cur_steer_angle_ = TransKappaToSteering(cur_kappa);  // 曲率转换为转向角

  // ========== 第4步：初始化转向角序列 ==========
  // 调用InitParam生成所有可能的转向角动作序列
  InitParam();
}

// ========== 函数：生成局部扩展路径 ==========
// 第1-4步：参数检查和起点验证
// 第5步：分离真实障碍物和虚拟障碍物（如泊出时的入口限制）
// 第6步：设置无碰撞距离阈值（根据失败次数动态调整）
// 第7-10步：初始化探索参数和搜索容器
// 第11步：A*主搜索循环，寻找满足条件的中间点：
// 距离障碍物足够远（>= collision_free_dist）
// 单方向行驶长度足够（避免过短的扩展）
// 与所有障碍物都无碰撞
// 第12-14步：结果检查、统计输出、路径提取
bool HybridAStar::GenerateLocalExtensionPath(
    const common::PathPoint& start_point,  // 起点
    const std::vector<std::pair<common::math::LineSegment2d, double>>&
        obstacles_segments_vec,  // 障碍物线段+膨胀距离
    int* const forced_path_direction_ptr,  // 输出：强制路径方向（1=前进，-1=后退）
    PathGeneratorResult* const result,     // 输出：扩展路径结果
    common::PathPoint* const intermediate_point_ptr,  // 输出：中间点（扩展路径终点）
    bool* const has_search_extension_path) {  // 输出：是否生成了扩展路径

  // ========== 第1步：输入参数检查 ==========
  if (result == nullptr || intermediate_point_ptr == nullptr ||
      forced_path_direction_ptr == nullptr ||
      has_search_extension_path == nullptr) {
    AERROR << "GenerateLocalExtensionPath input check fails";
    return false;
  }

  // ========== 第2步：特殊场景直接返回 ==========
  // NNS调整场景不需要扩展路径
  if (path_search_strategy_.is_nns_adjust_senario) {
    return true;
  }

  // ========== 第3步：创建起点节点 ==========
  start_node_ = std::make_shared<Node3d>(
      start_point.x, start_point.y, start_point.theta, xy_bounds_,
      xy_grid_resolution_, phi_grid_resolution_);

  // ========== 第4步：检查是否需要扩展路径 ==========
  // 如果不是重规划、首次探索、且起点无碰撞，则不需要扩展
  if (!path_search_strategy_.collision_free_search_strategy
           .replan_due_to_collision &&
      explored_overtime_times_ == 0 &&
      ValidityCheck(start_node_, obstacles_segments_vec)) {
    return true;  // 直接使用原起点
  }

  // ========== 第5步：分离真实障碍物和虚拟障碍物 ==========
  // 真实障碍物必须避开，虚拟障碍物仅在泊出时考虑
  std::vector<std::pair<common::math::LineSegment2d, double>>
      obstacles_segments_vec_pure;     // 真实障碍物（实体障碍）
  std::vector<std::pair<common::math::LineSegment2d, double>>
      obstacles_segments_vec_virtual;  // 虚拟障碍物（如泊出时的入口限制）

  obstacles_segments_vec_pure.reserve(obstacles_segments_vec.size());
  obstacles_segments_vec_virtual.reserve(obstacles_segments_vec.size());

  for (const auto& obstacles_segment : obstacles_segments_vec) {
    // 泊入场景或膨胀距离>0的视为真实障碍物
    if (path_search_strategy_.park_direction == PARKIN ||
        obstacles_segment.second > kEpsilon) {
      obstacles_segments_vec_pure.emplace_back(obstacles_segment.first,
                                               kEpsilon);
    } else {
      // 泊出场景的虚拟障碍物（如车位入口边界）
      obstacles_segments_vec_virtual.emplace_back(obstacles_segment);
    }
  }

  // ========== 第6步：设置无碰撞距离阈值 ==========
  // 扩展路径需要达到的最小安全距离
  double collision_free_dist =
      path_search_strategy_.collision_free_search_strategy.collision_free_dist;

  // 如果多次失败，增加安全距离要求（探索更远）
  if (path_search_strategy_.park_direction == PARKIN ||
      start_point.y > kDisableOverTimeExtraDistHeight) {
    collision_free_dist += explored_overtime_times_ * kExtensionExtraDist;
  }
  ADEBUG << "collision_free_dist: " << collision_free_dist;

  // ========== 第7步：初始化探索参数 ==========
  double exploration_start_time = Clock::NowInSeconds();
  size_t explored_node_num = 0;
  double exploration_max_time = 0.2;  // 最大探索时间200ms
  double current_dist_to_obstacle = kDefaultIgnorableDist;

  // ========== 第8步：检查起点是否与真实障碍物碰撞 ==========
  // 如果起点本身就碰撞，无法生成扩展路径
  if (!ValidityCheck(start_node_, obstacles_segments_vec_pure,
                     &current_dist_to_obstacle)) {
    AERROR << "start point is collision with obstacle";
    return false;
  }

  // ========== 第9步：判断是否需要扩展 ==========
  *has_search_extension_path = true;  // 默认需要扩展

  // 如果起点已经足够安全（距离障碍物>=阈值），不需要扩展
  if (current_dist_to_obstacle >= collision_free_dist &&
      ValidityCheck(start_node_, obstacles_segments_vec)) {
    ADEBUG
        << "start point is already collision free, current_dist_to_obstacle: "
        << current_dist_to_obstacle;
    *has_search_extension_path = false;  // 不需要扩展路径
  }

  // ========== 第10步：初始化搜索容器 ==========
  // 清空并初始化开放集和优先队列
  open_set_.clear();
  open_pq_ = decltype(open_pq_)();
  open_set_.emplace(start_node_->GetIndex(), start_node_);
  open_pq_.emplace(start_node_->GetIndex(), start_node_->GetCost());
  bool is_path_found = false;  // 扩展路径是否找到  // 扩展路径是否找到

  // ========== 第11步：主搜索循环（A*探索扩展路径） ==========
  // 从起点向外扩展，寻找距离障碍物足够远的中间点
  while (!open_pq_.empty()) {
    // 超时检查
    if (Clock::NowInSeconds() - exploration_start_time > exploration_max_time) {
      AERROR << "exploration time exceeds max accepatble time";
      return false;
    }

    // 取出代价最小的节点
    const std::string current_id = open_pq_.top().first;
    open_pq_.pop();
    auto& current_node = open_set_[current_id];
    close_set_.emplace(current_node->GetIndex(), current_node);

    // 计算当前节点到障碍物的距离
    ValidityCheck(current_node, obstacles_segments_vec_pure,
                  &current_dist_to_obstacle);
    ADEBUG << "current_dist_to_obstacle: " << current_dist_to_obstacle;

    double distance_to_obstalce = kDefaultIgnorableDist;

    // ========== 扩展相邻节点 ==========
    for (size_t i = 0; i < next_node_num_; ++i) {
      // 生成下一个候选节点（第3个参数true表示探索模式）
      const auto& next_node = Next_node_generator(current_node, i, true);
      if (next_node == nullptr) {
        continue;
      }

      // 跳过已探索的节点
      if (close_set_.find(next_node->GetIndex()) != close_set_.end()) {
        continue;
      }

      // 如果是新节点，进行碰撞检查和评估
      if (open_set_.find(next_node->GetIndex()) == open_set_.end()) {
        // 【检查1】：与真实障碍物碰撞 或 离障碍物更近（不符合"远离"目标）
        if (!ValidityCheck(next_node, obstacles_segments_vec_pure,
                           &distance_to_obstalce) ||
            distance_to_obstalce < current_dist_to_obstacle) {
          continue;  // 不满足"远离障碍物"要求，跳过
        }

        // 【检查2】：泊出场景需检查虚拟障碍物（如入口限制）
        if (path_search_strategy_.park_direction != PARKIN &&
            !ValidityCheck(next_node, obstacles_segments_vec_virtual)) {
          continue;
        }

        explored_node_num++;

        // 计算节点代价并加入开放集
        CalculateNodeCost(current_node, next_node, distance_to_obstalce, true);
        open_set_.emplace(next_node->GetIndex(), next_node);
        open_pq_.emplace(next_node->GetIndex(), next_node->GetCost());
        ADEBUG << "generate node: " << next_node->GetX() << ", "
               << next_node->GetY() << ", " << next_node->GetPhi();
        ADEBUG << "distance_to_obstalce: " << distance_to_obstalce;

        // ========== 检查是否找到合格的中间点 ==========
        // 满足3个条件即可作为扩展路径终点：
        // 1. 距离障碍物 >= 安全阈值
        // 2. 单方向行驶长度 >= 最小要求（避免过短的扩展）
        // 3. 与所有障碍物（包括虚拟）都无碰撞
        if (distance_to_obstalce >= collision_free_dist &&
            next_node->GetNearestGearShiftLength() >=
                warm_start_config_.min_one_direction_length &&
            ValidityCheck(next_node, obstacles_segments_vec)) {
          // 找到合格的中间点，记录结果
          final_node_ = next_node;
          intermediate_point_ptr->x = next_node->GetXs().back();
          intermediate_point_ptr->y = next_node->GetYs().back();
          intermediate_point_ptr->theta = next_node->GetPhis().back();
          *forced_path_direction_ptr = next_node->GetDirec() ? 1 : -1;  // 强制方向
          is_path_found = true;
          AINFO << "collision free path is found";
          break;  // 找到路径，退出内层循环
        }
      }
    }

    // 如果找到路径，退出外层循环
    if (is_path_found) {
      break;
    }
  }

  // ========== 第12步：检查搜索结果 ==========
  // 如果没有找到扩展路径
  if (nullptr == final_node_) {
    AERROR << " coliision free path is not found";
    // 如果原本就不需要扩展（起点已安全），返回true；否则返回false
    return !(*has_search_extension_path);
  }

  // ========== 第13步：输出统计信息 ==========
  ADEBUG << "collsion free path search cost time: "
         << (Clock::NowInSeconds() - exploration_start_time) * 1000 << " ms";
  ADEBUG << "explored_node_num :" << explored_node_num;

  // ========== 第14步：提取扩展路径结果 ==========
  // 第2个参数true表示这是无碰撞探索路径（需要移除第一个点）
  return GetResult(result, true);
}

}  // namespace planning
}  // namespace TL
