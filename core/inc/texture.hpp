#pragma once

#include <color.hpp>
#include <vec3.hpp>

#include <cmath>
#include <memory>

class Texture {
public:
  virtual ~Texture() = default;

  virtual Color value(double u, double v, const Point3 &point) const = 0;
};

class SolidColor : public Texture {
public:
  SolidColor(const Color &albedo)
      : m_albedo(albedo) {}

  SolidColor(double red, double green, double blue)
      : m_albedo(red, green, blue) {}

  virtual Color value(double u, double v, const Point3 &point) const override {
    return m_albedo;
  }

private:
  const Color m_albedo;
};

class CheckerTexture : public Texture {
public:
  CheckerTexture(double scale, std::shared_ptr<Texture> even,
                 std::shared_ptr<Texture> odd)
      : m_inverse_scale(1.0 / scale)
      , m_even(even)
      , m_odd(odd) {}

  CheckerTexture(double scale, const Color &color_1, const Color &color_2)
      : CheckerTexture(scale, std::make_shared<SolidColor>(color_1),
                       std::make_shared<SolidColor>(color_2)) {}

  virtual Color value(double u, double v, const Point3 &point) const override {

    const auto x_int = int(std::floor(m_inverse_scale * point.x()));
    const auto y_int = int(std::floor(m_inverse_scale * point.y()));
    const auto z_int = int(std::floor(m_inverse_scale * point.z()));

    const bool is_even = (x_int + y_int + z_int) % 2 == 0;

    return is_even ? m_even->value(u, v, point) : m_odd->value(u, v, point);
  }

private:
  const double m_inverse_scale;
  std::shared_ptr<Texture> m_even;
  std::shared_ptr<Texture> m_odd;
};
