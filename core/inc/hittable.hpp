#pragma once

#include <aabb.hpp>
#include <interval.hpp>
#include <ray.hpp>

#include <memory>

class Material;

struct HitRecord {
  Point3 m_point;
  Vec3 m_normal;
  double m_t;
  double m_u;
  double m_v;
  bool m_front_face;
  std::shared_ptr<Material> m_material;

  void set_face_normal(const Ray &ray, const Vec3 &outward_normal) {
    // Assumes outward_normal has unit length

    m_front_face = dot(ray.direction(), outward_normal) < 0.0;
    m_normal = m_front_face ? outward_normal : -outward_normal;
  }
};

struct Hittable {
  virtual ~Hittable() = default;

  virtual bool hit(const Ray &ray, Interval ray_t,
                   HitRecord &hit_record) const = 0;

  virtual AxisAlignedBoundingBox bounding_box() const = 0;
};
