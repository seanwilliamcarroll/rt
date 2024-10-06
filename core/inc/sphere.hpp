#pragma once

#include <aabb.hpp>
#include <hittable.hpp>
#include <material.hpp>
#include <ray.hpp>

#include <cmath>

class Sphere : public Hittable {
public:
  // Stationary
  Sphere(const Point3 &static_center, double radius,
         std::shared_ptr<Material> material)
      : m_center(static_center, Vec3(0, 0, 0))
      , m_radius(std::fmax(0, radius))
      , m_material(material)
      , m_bounding_box(static_center - radius_vector(),
                       static_center + radius_vector()) {}

  // Moving
  Sphere(const Point3 &center1, const Point3 &center2, double radius,
         std::shared_ptr<Material> material)
      : m_center(center1, center2 - center1)
      , m_radius(std::fmax(0, radius))
      , m_material(material)
      , m_bounding_box(calculate_moving_bounding_box()) {}

  bool hit(const Ray &ray, Interval ray_t,
           HitRecord &hit_record) const override {
    const Vec3 current_center = m_center.at(ray.time());
    const Vec3 origin_to_center = current_center - ray.origin();
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
    Vec3 outward_normal = (hit_record.m_point - current_center) / m_radius;
    hit_record.set_face_normal(ray, outward_normal);
    return true;
  }

  AxisAlignedBoundingBox bounding_box() const override {
    return m_bounding_box;
  }

private:
  Vec3 radius_vector() { return Vec3(m_radius, m_radius, m_radius); }

  AxisAlignedBoundingBox calculate_moving_bounding_box() {
    const AxisAlignedBoundingBox box_1(m_center.at(0) - radius_vector(),
                                       m_center.at(0) + radius_vector());
    const AxisAlignedBoundingBox box_2(m_center.at(1) - radius_vector(),
                                       m_center.at(1) + radius_vector());
    return AxisAlignedBoundingBox(box_1, box_2);
  }

private:
  const Ray m_center;
  const double m_radius;
  std::shared_ptr<Material> m_material;
  const AxisAlignedBoundingBox m_bounding_box;
};
