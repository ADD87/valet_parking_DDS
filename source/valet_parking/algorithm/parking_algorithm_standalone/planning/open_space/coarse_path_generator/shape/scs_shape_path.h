/*
 * Copyright (c) TL Technologies Co., Ltd. 2022. All rights reserved.
 */

#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "planning/common/open_space_info_lite.h"
#include "planning/open_space/coarse_path_generator/path_generator.h"
#include "planning/open_space/coarse_path_generator/utils/penalty_function_method.h"
#include "planning/open_space/hybrid_a_star_config.h"
#include "proto_convert/pnc_point_convert.h"
#include "proto_convert/open_space_types_convert.h"

namespace MAGNA {
namespace planning {

class SCSShapePath {
 public:
  SCSShapePath() = default;
  virtual ~SCSShapePath() = default;

  bool GenerateSCSShapePath(const common::PathPoint& start_node,
                            const common::PathPoint& end_node,
                            const DestRegionWithAng& dest_region_with_angle,
                            double min_radius, bool is_park_out,
                            PathGeneratorResult* path_result);

 private:
  template <typename T>
  void TransPointBasedOnQuadrantint(int quadrant, T* const p) {
    if (nullptr == p) {
      return;
    }
    switch (quadrant) {
      case 2: {
        p->set_x(-p->x());
        break;
      }
      case 3: {
        p->set_x(-p->x());
        p->set_y(-p->y());
        break;
      }
      case 4: {
        p->set_y(-p->y());
        break;
      }
      default:
        break;
    }
  }

  static double TransAngleBasedOnQuadrantint(const int quadrant,
                                             const double angle) {
    return (quadrant == 1 || quadrant == 3) ? angle : -angle;
  }

  void Trans2LocalCoor(const common::PathPoint& origin,
                       common::PathPoint* end_node_ptr,
                       DestRegionWithAng* dest_region_with_angle_ptr,
                       int* quadrant);
  bool GetSLSInfo(const DestRegionWithAng& dest_region_with_angle,
                  double min_radius, bool is_park_out,
                  common::PathPoint* target_pose_ptr, double* radius_ptr);
  bool GenerateSLSPath(bool is_collinear, const common::PathPoint& end_node,
                       double radius, PathGeneratorResult* sls_path_ptr);
};
}  // namespace planning
}  // namespace MAGNA