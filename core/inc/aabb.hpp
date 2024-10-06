#pragma once

#include <interval.hpp>
#include <ray.hpp>
#include <vec3.hpp>

class AxisAlignedBoundingBox {
public:
  AxisAlignedBoundingBox() {}

  AxisAlignedBoundingBox(const Interval &x, const Interval &y,
                         const Interval &z)
      : m_x(x)
      , m_y(y)
      , m_z(z) {}

  AxisAlignedBoundingBox(const Point3 &a, const Point3 &b)
      : m_x(constructInterval(a, b, 0))
      , m_y(constructInterval(a, b, 1))
      , m_z(constructInterval(a, b, 2)) {}

  AxisAlignedBoundingBox(const AxisAlignedBoundingBox &a,
                         const AxisAlignedBoundingBox &b)
      : m_x(a.m_x, b.m_x)
      , m_y(a.m_y, b.m_y)
      , m_z(a.m_z, b.m_z) {}

  const Interval &axis_interval(const size_t axis_index) const {
    if (axis_index == 0) {
      return m_x;
    }
    if (axis_index == 1) {
      return m_y;
    }
    return m_z;
  }

  bool hit(const Ray &ray, Interval ray_t) const {
    const Point3 &ray_origin = ray.origin();
    const Vec3 &ray_direction = ray.direction();

    for (size_t axis_index = 0; axis_index < 3; ++axis_index) {
      const Interval &axis = axis_interval(axis_index);
      const double d_inverse = 1.0 / ray_direction[axis_index];

      const auto t0 = (axis.m_min - ray_origin[axis_index]) * d_inverse;
      const auto t1 = (axis.m_max - ray_origin[axis_index]) * d_inverse;

      if (t0 < t1) {
        if (t0 > ray_t.m_min) {
          ray_t.m_min = t0;
        }
        if (t1 < ray_t.m_max) {
          ray_t.m_max = t1;
        }
      } else {
        if (t1 > ray_t.m_min) {
          ray_t.m_min = t1;
        }
        if (t0 < ray_t.m_max) {
          ray_t.m_max = t0;
        }
      }

      if (ray_t.m_max <= ray_t.m_min) {
        return false;
      }
    }

    return true;
  }

private:
  Interval constructInterval(const Point3 &a, const Point3 &b,
                             const size_t index) {
    if (a[index] <= b[index]) {
      return Interval(a[index], b[index]);
    }
    return Interval(b[index], a[index]);
  }

  Interval m_x;
  Interval m_y;
  Interval m_z;
};
