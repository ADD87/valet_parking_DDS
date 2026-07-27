/*
 * Copyright (c) TL Technologies Co., Ltd. 2022. All rights reserved.
 * Description: path_generator_result.h
 * 独立编译改造：从 path_generator.h 提取，避免循环依赖
 */

#pragma once

#include <vector>
#include "proto_convert/pnc_point_convert.h"

namespace TL {
namespace planning {

struct PathGeneratorResult {
  std::vector<double> x;
  std::vector<double> y;
  std::vector<double> phi;
  common::PathPoint rs_connect_point;
  int path_type = 0;  // 独立编译改造：使用 int 代替 planning_internal::PathType（暂时简化）

  void reset() {
    x.clear();
    y.clear();
    phi.clear();
    rs_connect_point = common::PathPoint();  // Reset by assignment
    path_type = 0;  // Reset to DEFAULT
  }
};

}  // namespace planning
}  // namespace TL
