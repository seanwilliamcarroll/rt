#pragma once

#include <color.hpp>
#include <hittable.hpp>
#include <ray.hpp>

class Material {
public:
  virtual ~Material() = default;

  virtual bool scatter(const Ray &ray_in, const HitRecord &hit_record,
                       Color &attenuation, Ray &scattered) const {
    return false;
  }
};

class Lambertian : public Material {
public:
  Lambertian(const Color &albedo)
      : m_albedo(albedo) {}

  virtual bool scatter(const Ray &ray_in, const HitRecord &hit_record,
                       Color &attenuation, Ray &scattered) const override {
    auto scatter_direction = hit_record.m_normal + random_unit_vector();

    // Catch degenerate scatter
    if (scatter_direction.near_zero()) {
      scatter_direction = hit_record.m_normal;
    }

    scattered = Ray(hit_record.m_point, scatter_direction);
    attenuation = m_albedo;
    return true;
  }

private:
  const Color m_albedo;
};

class Metal : public Material {
public:
  Metal(const Color &albedo, const double fuzz)
      : m_albedo(albedo)
      , m_fuzz(fuzz < 1 ? fuzz : 1.0) {}

  virtual bool scatter(const Ray &ray_in, const HitRecord &hit_record,
                       Color &attenuation, Ray &scattered) const override {

    auto reflected = reflect(ray_in.direction(), hit_record.m_normal);
    reflected = unit_vector(reflected) + (m_fuzz * random_unit_vector());
    scattered = Ray(hit_record.m_point, reflected);
    attenuation = m_albedo;
    return (dot(scattered.direction(), hit_record.m_normal) > 0);
  }

private:
  const Color m_albedo;
  const double m_fuzz;
};

class Dielectric : public Material {
public:
  Dielectric(const double refraction_index)
      : m_refraction_index(refraction_index) {}

  virtual bool scatter(const Ray &ray_in, const HitRecord &hit_record,
                       Color &attenuation, Ray &scattered) const override {
    attenuation = Color(1, 1, 1);

    const double refraction_index = hit_record.m_front_face
                                        ? (1.0 / m_refraction_index)
                                        : m_refraction_index;

    const Vec3 unit_direction = unit_vector(ray_in.direction());

    const double cos_theta =
        std::fmin(dot(-unit_direction, hit_record.m_normal), 1.0);
    const double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

    const bool cannot_refract =
        (refraction_index * sin_theta > 1.0) ||
        (reflectance(cos_theta, refraction_index) > random_double());

    const Vec3 direction =
        cannot_refract
            ? reflect(unit_direction, hit_record.m_normal)
            : refract(unit_direction, hit_record.m_normal, refraction_index);

    scattered = Ray(hit_record.m_point, direction);
    return true;
  }

private:
  static double reflectance(const double cosine,
                            const double refraction_index) {
    // Use's Schlick's approximation for reflectance
    auto r0 = (1 - refraction_index) / (1 + refraction_index);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
  }

  const double m_refraction_index;
};
