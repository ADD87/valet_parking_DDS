/**
 * @file open_space_status_convert.h
 * @brief 独立编译改造：将 OpenSpaceStatus proto (planning_status.proto) 转换为 C++ 结构体
 * @description 用于替代 proto/planning/planning_status.pb.h 中的 OpenSpaceStatus
 *              包含 Replan 枚举值（位标志），用于路径分段决策中的重规划状态跟踪
 */

#pragma once

#include <cstdint>
#include <string>

namespace TL {
namespace planning {

/**
 * @brief OpenSpaceStatus - 开放空间规划状态
 * 原 Proto: message OpenSpaceStatus (planning_status.proto)
 *
 * Replan 枚举为位标志(bit flags)，可通过 | 组合使用
 */
struct OpenSpaceStatus {
  enum Replan : uint32_t {
    NONE = 0,
    NO_VALID_PATH = 1,
    TARGET_UPDATE = 2,
    BLOCK_BY_STATIC_OBSTACLE = 4,
    BLOCK_BY_USS = 8,
    FREEZE_NEAR_END = 16,
    TRACK_ABNORMAL = 32,
    YAW_TRACK_ABNORMAL = 64,
    ENTER_SPECIAL_DOMAIN = 128,
    DYNAMIC_REPLAN = 256,
    TRACE_REPLAN = 512,
    END_ANGLE_UNREACHABLE = 1024,
    TARGET_UPDATE_SLIGHTLY = 2048,
    REPLAN_FOR_SPEED_WARN = 4096,
  };

  // Protobuf 兼容接口
  static constexpr uint32_t Replan_MIN = NONE;
  static constexpr uint32_t Replan_MAX = REPLAN_FOR_SPEED_WARN;

  static bool Replan_IsValid(int value) {
    switch (value) {
      case 0: case 1: case 2: case 4: case 8: case 16: case 32:
      case 64: case 128: case 256: case 512: case 1024: case 2048: case 4096:
        return true;
      default:
        return false;
    }
  }

  static std::string Replan_Name(uint32_t value) {
    switch (value) {
      case NONE: return "NONE";
      case NO_VALID_PATH: return "NO_VALID_PATH";
      case TARGET_UPDATE: return "TARGET_UPDATE";
      case BLOCK_BY_STATIC_OBSTACLE: return "BLOCK_BY_STATIC_OBSTACLE";
      case BLOCK_BY_USS: return "BLOCK_BY_USS";
      case FREEZE_NEAR_END: return "FREEZE_NEAR_END";
      case TRACK_ABNORMAL: return "TRACK_ABNORMAL";
      case YAW_TRACK_ABNORMAL: return "YAW_TRACK_ABNORMAL";
      case ENTER_SPECIAL_DOMAIN: return "ENTER_SPECIAL_DOMAIN";
      case DYNAMIC_REPLAN: return "DYNAMIC_REPLAN";
      case TRACE_REPLAN: return "TRACE_REPLAN";
      case END_ANGLE_UNREACHABLE: return "END_ANGLE_UNREACHABLE";
      case TARGET_UPDATE_SLIGHTLY: return "TARGET_UPDATE_SLIGHTLY";
      case REPLAN_FOR_SPEED_WARN: return "REPLAN_FOR_SPEED_WARN";
      default: return "UNKNOWN(" + std::to_string(value) + ")";
    }
  }

  // ========== 数据成员 ==========
  uint32_t replan = 0;  // 位组合的重规划状态

  bool has_replan() const { return replan != 0; }
  void set_replan(uint32_t v) { replan = v; }
  double current_part_path_length = 0.0;
  void set_current_part_path_length(double v) { current_part_path_length = v; }

  /**
   * @brief 更新重规划状态（添加位标志）
   * 替代 OpenSpaceInfo::UpdateReplanStatus 的简化版
   */
  static void UpdateReplanStatus(Replan flag, OpenSpaceStatus* status) {
    if (status != nullptr) {
      status->replan |= static_cast<uint32_t>(flag);
    }
  }

  OpenSpaceStatus() = default;
};

}  // namespace planning
}  // namespace TL
