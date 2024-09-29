#pragma once

#include <constants.hpp>

#include <cmath>
#include <iostream>

class Vec3 {
public:
  Vec3()
      : m_elements{0.0, 0.0, 0.0} {}

  Vec3(double elem_0, double elem_1, double elem_2)
      : m_elements{elem_0, elem_1, elem_2} {}

  double x() const { return m_elements[0]; }
  double y() const { return m_elements[1]; }
  double z() const { return m_elements[2]; }

  Vec3 operator-() const {
    return Vec3(-m_elements[0], -m_elements[1], -m_elements[2]);
  }
  double operator[](int i) const { return m_elements[i]; }
  double &operator[](int i) { return m_elements[i]; }

  Vec3 &operator+=(const Vec3 &other) {
    m_elements[0] += other.m_elements[0];
    m_elements[1] += other.m_elements[1];
    m_elements[2] += other.m_elements[2];

    return *this;
  }

  Vec3 &operator*=(double t) {
    m_elements[0] *= t;
    m_elements[1] *= t;
    m_elements[2] *= t;
    return *this;
  }

  Vec3 &operator/=(double t) { return *this *= 1 / t; }

  double length() const { return std::sqrt(length_squared()); }

  double length_squared() const {
    return m_elements[0] * m_elements[0] + m_elements[1] * m_elements[1] +
           m_elements[2] * m_elements[2];
  }

  bool near_zero() const {
    // Return true if the vector is close to zero in all dimensions
    constexpr double near_zero_value = 1e-8;

    return (std::fabs(m_elements[0]) < near_zero_value) &&
           (std::fabs(m_elements[1]) < near_zero_value) &&
           (std::fabs(m_elements[2]) < near_zero_value);
  }

  static Vec3 random() {
    return Vec3(random_double(), random_double(), random_double());
  }

  static Vec3 random(const double min, const double max) {
    return Vec3(random_double(min, max), random_double(min, max),
                random_double(min, max));
  }

private:
  double m_elements[3];
};

using Point3 = Vec3;

inline std::ostream &operator<<(std::ostream &out, const Vec3 &v) {
  return out << v[0] << ' ' << v[1] << ' ' << v[2];
}

inline Vec3 operator+(const Vec3 &u, const Vec3 &v) {
  return Vec3(u[0] + v[0], u[1] + v[1], u[2] + v[2]);
}

inline Vec3 operator-(const Vec3 &u, const Vec3 &v) {
  return Vec3(u[0] - v[0], u[1] - v[1], u[2] - v[2]);
}

inline Vec3 operator*(const Vec3 &u, const Vec3 &v) {
  return Vec3(u[0] * v[0], u[1] * v[1], u[2] * v[2]);
}

inline Vec3 operator*(double t, const Vec3 &v) {
  return Vec3(t * v[0], t * v[1], t * v[2]);
}

inline Vec3 operator*(const Vec3 &v, double t) { return t * v; }

inline Vec3 operator/(const Vec3 &v, double t) { return (1 / t) * v; }

inline double dot(const Vec3 &u, const Vec3 &v) {
  return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

inline Vec3 cross(const Vec3 &u, const Vec3 &v) {
  return Vec3(u[1] * v[2] - u[2] * v[1], u[2] * v[0] - u[0] * v[2],
              u[0] * v[1] - u[1] * v[0]);
}

inline Vec3 unit_vector(const Vec3 &v) { return v / v.length(); }

inline Vec3 random_in_unit_disk() {
  while (true) {
    const auto point = Vec3(random_double(-1, 1), random_double(-1, 1), 0);
    if (point.length_squared() < 1) {
      return point;
    }
  }
}

inline Vec3 random_unit_vector() {
  while (true) {
    const auto vec = Vec3::random(-1, 1);
    const auto length_squared = vec.length_squared();
    if (1e-160 < length_squared && length_squared <= 1.0) {
      return vec / std::sqrt(length_squared);
    }
  }
}

inline Vec3 random_on_hemisphere(const Vec3 &normal) {
  const Vec3 random_on_unit_sphere = random_unit_vector();
  if (dot(normal, random_on_unit_sphere) > 0.0) {
    return random_on_unit_sphere;
  }
  return -random_on_unit_sphere;
}

inline Vec3 reflect(const Vec3 &incoming, const Vec3 &normal) {
  return incoming - 2 * dot(incoming, normal) * normal;
}

inline Vec3 refract(const Vec3 &uv, const Vec3 &n,
                    const double etai_over_etat) {
  const auto cos_theta = std::fmin(dot(-uv, n), 1.0);
  const Vec3 r_out_perpendicular = etai_over_etat * (uv + cos_theta * n);
  const Vec3 r_out_parallel =
      -std::sqrt(std::fabs(1.0 - r_out_perpendicular.length_squared())) * n;
  return r_out_perpendicular + r_out_parallel;
}
