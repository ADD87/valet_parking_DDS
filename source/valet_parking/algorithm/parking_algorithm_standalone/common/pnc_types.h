/******************************************************************************
 * Copyright 2017 The Apollo Authors. All Rights Reserved.
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
 * @file pnc_types.h
 * @brief Simplified PNC (Planning and Control) types to replace proto dependencies
 */

#pragma once

#include <string>
#include <sstream>

// 如果 proto_convert/pnc_point_convert.h 已经定义了轻量级struct版本，
// 就不再定义class版本，避免重复定义
#ifndef PNC_POINT_CONVERT_H_LIGHTWEIGHT_TYPES_DEFINED

namespace TL {
namespace common {

/**
 * @brief S-L coordinate point (Frenet coordinate)
 */
class SLPoint {
 public:
  SLPoint() : s_(0.0), l_(0.0) {}

  double s() const { return s_; }
  double l() const { return l_; }

  void set_s(double s) { s_ = s; }
  void set_l(double l) { l_ = l; }

  std::string DebugString() const {
    std::ostringstream oss;
    oss << "SLPoint(s=" << s_ << ", l=" << l_ << ")";
    return oss.str();
  }

 private:
  double s_;
  double l_;
};

/**
 * @brief Path point with position and orientation
 */
class PathPoint {
 public:
  PathPoint() : x_(0.0), y_(0.0), z_(0.0), theta_(0.0),
                kappa_(0.0), s_(0.0), dkappa_(0.0), ddkappa_(0.0) {}

  double x() const { return x_; }
  double y() const { return y_; }
  double z() const { return z_; }
  double theta() const { return theta_; }
  double kappa() const { return kappa_; }
  double s() const { return s_; }
  double dkappa() const { return dkappa_; }
  double ddkappa() const { return ddkappa_; }

  void set_x(double x) { x_ = x; }
  void set_y(double y) { y_ = y; }
  void set_z(double z) { z_ = z; }
  void set_theta(double theta) { theta_ = theta; }
  void set_kappa(double kappa) { kappa_ = kappa; }
  void set_s(double s) { s_ = s; }
  void set_dkappa(double dkappa) { dkappa_ = dkappa; }
  void set_ddkappa(double ddkappa) { ddkappa_ = ddkappa; }

  std::string DebugString() const {
    std::ostringstream oss;
    oss << "PathPoint(x=" << x_ << ", y=" << y_ << ", theta=" << theta_
        << ", kappa=" << kappa_ << ", s=" << s_ << ")";
    return oss.str();
  }

 private:
  double x_;
  double y_;
  double z_;
  double theta_;
  double kappa_;
  double s_;
  double dkappa_;
  double ddkappa_;
};

/**
 * @brief Trajectory point with path point and velocity/acceleration
 */
class TrajectoryPoint {
 public:
  TrajectoryPoint() : has_path_point_(true), v_(0.0), a_(0.0), da_(0.0),
                      relative_time_(0.0), steer_(0.0) {}

  bool has_path_point() const { return has_path_point_; }
  const PathPoint& path_point() const { return path_point_; }
  PathPoint* mutable_path_point() { return &path_point_; }

  double v() const { return v_; }
  double a() const { return a_; }
  double da() const { return da_; }
  double relative_time() const { return relative_time_; }
  double steer() const { return steer_; }

  void set_v(double v) { v_ = v; }
  void set_a(double a) { a_ = a; }
  void set_da(double da) { da_ = da; }
  void set_relative_time(double t) { relative_time_ = t; }
  void set_steer(double steer) { steer_ = steer; }

  std::string DebugString() const {
    std::ostringstream oss;
    oss << "TrajectoryPoint(v=" << v_ << ", a=" << a_
        << ", t=" << relative_time_ << ")";
    return oss.str();
  }

 private:
  bool has_path_point_;
  PathPoint path_point_;
  double v_;
  double a_;
  double da_;
  double relative_time_;
  double steer_;
};

}  // namespace common
}  // namespace TL

#endif  // PNC_POINT_CONVERT_H_LIGHTWEIGHT_TYPES_DEFINED
