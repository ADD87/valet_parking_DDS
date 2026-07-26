#pragma once

#include <utility>
#include <vector>

#include "proto_convert/pnc_point_convert.h"

namespace TL {
namespace planning {

class DiscretizedPath : public std::vector<TL::common::PathPoint> {
 public:
  DiscretizedPath() = default;
  explicit DiscretizedPath(std::vector<TL::common::PathPoint> path_points)
      : std::vector<TL::common::PathPoint>(std::move(path_points)) {}
};

}  // namespace planning
}  // namespace TL
