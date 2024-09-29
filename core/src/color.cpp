#include <color.hpp>

#include <iostream>

int scale_to_int(double val) { return int(256.0 * val); }

inline double linear_to_gamma(const double linear_component) {
  if (linear_component > 0) {
    return std::sqrt(linear_component);
  }
  return 0;
}

void write_color(std::ostream &out, const Color &pixel_color) {

  static const Interval intensity(0.000, 0.999);

  out << scale_to_int(intensity.clamp(linear_to_gamma(pixel_color.x()))) << " "
      << scale_to_int(intensity.clamp(linear_to_gamma(pixel_color.y()))) << " "
      << scale_to_int(intensity.clamp(linear_to_gamma(pixel_color.z())))
      << "\n";
}
