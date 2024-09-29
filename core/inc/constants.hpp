#pragma once

#include <limits>
#include <random>

// Constants
constexpr double infinity = std::numeric_limits<double>::infinity();
constexpr double pi = 3.1415926535897932385;

// Utility functions
inline double degrees_to_radians(const double degrees) {
  return degrees * pi / 180.0;
}

// Random double in [0.0, 1.0)
inline double random_double() {
  static std::uniform_real_distribution<double> distribution(0.0, 1.0);
  static std::mt19937 generator;
  return distribution(generator);
}

// Random double in [min, max)
inline double random_double(const double min, const double max) {
  return min + (max - min) * random_double();
}
