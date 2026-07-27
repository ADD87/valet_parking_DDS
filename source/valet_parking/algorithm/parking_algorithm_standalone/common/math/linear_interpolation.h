/*
 * Copyright (c) TL Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description:  common math interpolation
 * Author: ROC
 */

/**
 * @file
 * @brief Defines linear interpolation functions.
 */

#pragma once

#include <vector>

#include "common/geometry_types.h"
// 独立编译改造：先include pnc_point_convert.h 确保使用 struct 版本
// pnc_types.h 内部有 guard，会跳过重复的 class 定义
#include "proto_convert/pnc_point_convert.h"
#include "common/pnc_types.h"

namespace TL {
namespace common {
namespace math {

//! Linear interpolation helper
double lerp(const double v0, const double t0, const double v1, const double t1,
            const double t);

//! Spherical linear interpolation for angles
double slerp(const double a0, const double t0, const double a1, const double t1,
             const double t);

//! Linear interpolation for SLPoint
common::SLPoint InterpolateUsingLinearApproximation(
    const common::SLPoint& p0, const common::SLPoint& p1, const double w);

//! Linear interpolation for PathPoint
common::PathPoint InterpolateUsingLinearApproximation(
    const common::PathPoint& p0, const common::PathPoint& p1, const double w);

//! Linear interpolation for TrajectoryPoint
common::TrajectoryPoint InterpolateUsingLinearApproximation(
    const common::TrajectoryPoint& tp0, const common::TrajectoryPoint& tp1,
    const double t);

//! Piecewise linear interpolation (lookup table)
double InterpolationOne(const double& input_x,
                        const std::vector<double>& input_v,
                        const std::vector<double>& output_v);

}  // namespace math
}  // namespace common
}  // namespace TL
