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
 * @file speed_data.cc
 * 独立编译改造：
 *   - SpeedPoint proto accessor (.s(), .t(), .v()) → struct member (.s, .t, .v)
 *   - PointFactory::ToSpeedPoint() → 直接构造 SpeedPoint
 *   - DebugStringFormatter → 简化字符串输出
 *   - has_xxx() → 始终为 true（struct 成员始终存在）
 *   - set_xxx() → 直接赋值
 **/

#include "planning/common/speed/speed_data.h"

#include <algorithm>
#include <mutex>
#include <sstream>
#include <utility>

#include "common/math/linear_interpolation.h"
#include "common/util/string_util.h"

namespace TL {
namespace planning {

using TL::common::SpeedPoint;

SpeedData::SpeedData(std::vector<SpeedPoint> speed_points)
    : std::vector<SpeedPoint>(std::move(speed_points)) {
  std::sort(begin(), end(), [](const SpeedPoint& p1, const SpeedPoint& p2) {
    return p1.t < p2.t;
  });
}

void SpeedData::AppendSpeedPoint(const double s, const double time,
                                 const double v, const double a,
                                 const double da) {
  SpeedPoint sp;
  sp.s = s;
  sp.t = time;
  sp.v = v;
  sp.a = a;
  sp.da = da;
  push_back(sp);
}

bool SpeedData::EvaluateByTime(const double t,
                               common::SpeedPoint* const speed_point) const {
  if (size() < 2) {
    return false;
  }
  if (front().t >= t + 1.0e-6 || t - 1.0e-6 >= back().t) {
    return false;
  }

  auto comp = [](const common::SpeedPoint& sp, const double t) {
    return sp.t < t;
  };

  auto it_lower = std::lower_bound(begin(), end(), t, comp);
  if (it_lower == end()) {
    *speed_point = back();
  } else if (it_lower == begin()) {
    *speed_point = front();
  } else {
    const auto& p0 = *(it_lower - 1);
    const auto& p1 = *it_lower;
    double t0 = p0.t;
    double t1 = p1.t;

    *speed_point = SpeedPoint{};
    speed_point->s = common::math::lerp(p0.s, t0, p1.s, t1, t);
    speed_point->t = t;
    speed_point->v = common::math::lerp(p0.v, t0, p1.v, t1, t);
    speed_point->a = common::math::lerp(p0.a, t0, p1.a, t1, t);
    speed_point->da = common::math::lerp(p0.da, t0, p1.da, t1, t);
  }
  return true;
}

bool SpeedData::EvaluateByS(const double s,
                            common::SpeedPoint* const speed_point) const {
  if (size() < 2) {
    return false;
  }
  if (front().s >= s + 1.0e-6 || s - 1.0e-6 >= back().s) {
    return false;
  }

  auto comp = [](const common::SpeedPoint& sp, const double s) {
    return sp.s < s;
  };

  auto it_lower = std::lower_bound(begin(), end(), s, comp);
  if (it_lower == end()) {
    *speed_point = back();
  } else if (it_lower == begin()) {
    *speed_point = front();
  } else {
    const auto& p0 = *(it_lower - 1);
    const auto& p1 = *it_lower;
    double s0 = p0.s;
    double s1 = p1.s;

    *speed_point = SpeedPoint{};
    speed_point->s = s;
    speed_point->t = common::math::lerp(p0.t, s0, p1.t, s1, s);
    speed_point->v = common::math::lerp(p0.v, s0, p1.v, s1, s);
    speed_point->a = common::math::lerp(p0.a, s0, p1.a, s1, s);
    speed_point->da = common::math::lerp(p0.da, s0, p1.da, s1, s);
  }
  return true;
}

double SpeedData::TotalTime() const {
  if (empty()) {
    return 0.0;
  }
  return back().t - front().t;
}

double SpeedData::TotalLength() const {
  if (empty()) {
    return 0.0;
  }
  return back().s - front().s;
}

std::string SpeedData::DebugString() const {
  std::ostringstream oss;
  oss << "[\n";
  size_t limit = std::min(size(), static_cast<size_t>(10));
  for (size_t i = 0; i < limit; ++i) {
    const auto& sp = (*this)[i];
    oss << "  {s=" << sp.s << ", t=" << sp.t << ", v=" << sp.v
        << ", a=" << sp.a << ", da=" << sp.da << "}";
    if (i + 1 < limit) oss << ",";
    oss << "\n";
  }
  if (size() > limit) {
    oss << "  ... (" << size() - limit << " more)\n";
  }
  oss << "]\n";
  return oss.str();
}

double SpeedData::GetMaxAccel() const {
  const auto iter = std::max_element(
      begin(), end(), [](const auto& sp1, const auto& sp2) {
        return sp1.a < sp2.a;
      });
  return iter != end() ? iter->a : 0.0;
}

double SpeedData::GetMinAccel() const {
  const auto iter = std::min_element(
      begin(), end(), [](const auto& sp1, const auto& sp2) {
        return sp1.a < sp2.a;
      });
  return iter != end() ? iter->a : 0.0;
}

}  // namespace planning
}  // namespace TL
