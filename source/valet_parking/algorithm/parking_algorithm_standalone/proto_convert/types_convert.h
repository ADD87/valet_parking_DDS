/******************************************************************************
 * Types Proto to C++ Conversion Header
 *
 * Description: Convert common/types.proto to C++ structures
 * Original Proto: proto/common/types.proto
 * Priority: P1 (Basic geometric types)
 *
 * Date: 2025-12-15
 * Note: This file replaces Protobuf dependencies for standalone compilation
 *****************************************************************************/

#pragma once

#include <cstdint>
#include <vector>
#include <limits>
#include <sstream>
#include <string>

namespace TL {
namespace common {

/**
 * @brief 2D Point
 */
struct Point2D {
  double x = 0.0;  ///< X coordinate
  double y = 0.0;  ///< Y coordinate
};

/**
 * @brief 3D Point
 */
struct Point3D {
  double x = 0.0;  ///< X coordinate
  double y = 0.0;  ///< Y coordinate
  double z = 0.0;  ///< Z coordinate
};

/**
 * @brief 3x3 Matrix (for rotation, transformation, etc.)
 */
struct Matrix3d {
  double m00 = 0.0;  ///< Element [0][0]
  double m01 = 0.0;  ///< Element [0][1]
  double m02 = 0.0;  ///< Element [0][2]
  double m10 = 0.0;  ///< Element [1][0]
  double m11 = 0.0;  ///< Element [1][1]
  double m12 = 0.0;  ///< Element [1][2]
  double m20 = 0.0;  ///< Element [2][0]
  double m21 = 0.0;  ///< Element [2][1]
  double m22 = 0.0;  ///< Element [2][2]
};

/**
 * @brief Quaternion (for 3D rotation)
 */
struct Quaternion {
  double qx = 0.0;  ///< X component
  double qy = 0.0;  ///< Y component
  double qz = 0.0;  ///< Z component
  double qw = 1.0;  ///< W component (scalar part, default to identity)
};

/**
 * @brief 2D Rectangle (axis-aligned bounding box)
 */
struct Rect2D {
  Point2D min_corner;  ///< Minimum corner (bottom-left)
  Point2D max_corner;  ///< Maximum corner (top-right)
};

/**
 * @brief 3D Rectangle (axis-aligned bounding box)
 */
struct Rect3D {
  Point3D min_corner;  ///< Minimum corner
  Point3D max_corner;  ///< Maximum corner
};

/**
 * @brief Polygon (2D/3D shape defined by vertices)
 */
struct Polygon {
  std::vector<Point3D> point;  ///< Array of vertices (3D for compatibility with geometry.proto)
  bool is_closure = false;     ///< Whether the polygon is closed

  // 兼容方法（与 class 接口兼容）
  void add_point() { point.emplace_back(); }
  int point_size() const { return static_cast<int>(point.size()); }
  const Point3D& get_point(int index) const { return point[index]; }
  Point3D* mutable_point(int index) { return &point[index]; }

  std::string DebugString() const {
    std::ostringstream oss;
    oss << "Polygon(points=" << point.size() << ", closed=" << is_closure << ")";
    return oss.str();
  }
};

/**
 * @brief Polyline (2D path defined by points)
 */
struct Polyline {
  std::vector<Point2D> point;  ///< Array of points
};

/**
 * @brief Point in ENU (East-North-Up) coordinate system
 *
 * Map reference frame point. The map defines an origin at (0, 0, 0).
 * Uses Universal Transverse Mercator (UTM) projection.
 */
struct PointENU {
  double x = std::numeric_limits<double>::quiet_NaN();  ///< East from origin (meters)
  double y = std::numeric_limits<double>::quiet_NaN();  ///< North from origin (meters)
  double z = 0.0;  ///< Up from WGS-84 ellipsoid (meters), can be omitted for 2D

  // Getter 方法（与 Protobuf 接口兼容）
  double x_getter() const { return x; }
  double y_getter() const { return y; }
  double z_getter() const { return z; }

  // Setter 方法（与 class 接口兼容）
  void set_x(double val) { x = val; }
  void set_y(double val) { y = val; }
  void set_z(double val) { z = val; }

  void CopyFrom(const PointENU& other) {
    x = other.x;
    y = other.y;
    z = other.z;
  }

  std::string DebugString() const {
    std::ostringstream oss;
    oss << "PointENU(x=" << x << ", y=" << y << ", z=" << z << ")";
    return oss.str();
  }
};

/**
 * @brief Point in LLH (Latitude-Longitude-Height) coordinate system
 *
 * Global reference frame using geographic coordinates.
 */
struct PointLLH {
  double lon = std::numeric_limits<double>::quiet_NaN();  ///< Longitude in degrees [-180, 180]
  double lat = std::numeric_limits<double>::quiet_NaN();  ///< Latitude in degrees [-90, 90]
  double height = 0.0;  ///< WGS-84 ellipsoid height (meters), can be omitted for 2D
};

/**
 * @brief Affine 3D transformation
 *
 * Combines rotation (quaternion) and translation (point)
 */
struct Affine3D {
  Quaternion quaternion;  ///< Rotation component
  Point3D point3d;        ///< Translation component
};

}  // namespace common
}  // namespace TL
