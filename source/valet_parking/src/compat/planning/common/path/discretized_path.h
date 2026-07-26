#pragma once

#include <utility>
#include <vector>

#include "common/math/box2d.h"
#include "common/math/line_segment2d.h"
#include "proto_convert/pnc_point_convert.h"
#include "proto/planning/sl_boundary.pb.h"

namespace TL {
namespace planning {

class DiscretizedPath : public std::vector<TL::common::PathPoint> {
 public:
  DiscretizedPath() = default;
  explicit DiscretizedPath(std::vector<TL::common::PathPoint> path_points);

  double Length() const;
  double TotalLength() const;

  TL::common::PathPoint Evaluate(double path_s) const;
  TL::common::PathPoint EvaluateForGreaterThanMaxS(double path_s) const;
  TL::common::PathPoint EvaluateReverse(double path_s) const;

  bool GetSLBoundary(const TL::common::math::Box2d& box,
                     SLBoundary* sl_boundary) const;
  bool XYToSL(double x, double y, TL::common::SLPoint* sl_point) const;
  void GenerateStopPath(double x, double y, double theta, double kappa);

  bool IsPointIn(const TL::common::PathPoint& point,
                 double dis_threshold = 1e-2,
                 double ang_threshold = 0.02) const;
  static bool IsSamePoint(const TL::common::PathPoint& p_a,
                          const TL::common::PathPoint& p_b,
                          double dis_threshold = 1e-2,
                          double ang_threshold = 0.02);

  bool GetProjection(const TL::common::math::Vec2d& point,
                     double* accumulate_s,
                     double* lateral,
                     double* min_distance,
                     int* index_min,
                     double radius1d,
                     int index_center) const;

 protected:
  std::vector<TL::common::PathPoint>::const_iterator QueryLowerBound(
      double path_s) const;
  std::vector<TL::common::PathPoint>::const_iterator QueryUpperBound(
      double path_s) const;

  std::vector<double> accumulated_s_{};
  std::vector<TL::common::math::LineSegment2d> segments_{};
};

}  // namespace planning
}  // namespace TL
