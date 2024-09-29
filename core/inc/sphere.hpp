#pragma once

#include <hittable.hpp>
#include <material.hpp>
#include <ray.hpp>

#include <cmath>

class Sphere : public Hittable {
public:
  Sphere(const Point3 &center, double radius,
         std::shared_ptr<Material> material)
      : m_center(center)
      , m_radius(std::fmax(0, radius))
      , m_material(material) {}

  bool hit(const Ray &ray, Interval ray_t,
           HitRecord &hit_record) const override {
    const Vec3 origin_to_center = m_center - ray.origin();
    const auto a = ray.direction().length_squared();
    const auto h = dot(ray.direction(), origin_to_center);
    const auto c = origin_to_center.length_squared() - m_radius * m_radius;

    const auto discriminant = h * h - a * c;
    if (discriminant < 0.0) {
      return false;
    }

    const auto sqrt_discriminant = std::sqrt(discriminant);

    // Find nearest root that lies in the acceptable range
    auto root = (h - sqrt_discriminant) / a;
    if (!ray_t.surrounds(root)) {
      root = (h + sqrt_discriminant) / a;
      if (!ray_t.surrounds(root)) {
        return false;
      }
    }

    hit_record.m_t = root;
    hit_record.m_point = ray.at(root);
    hit_record.m_material = m_material;
    Vec3 outward_normal = (hit_record.m_point - m_center) / m_radius;
    hit_record.set_face_normal(ray, outward_normal);
    return true;
  }

private:
  const Point3 m_center;
  const double m_radius;
  std::shared_ptr<Material> m_material;
};
