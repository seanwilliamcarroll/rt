#pragma once

#include <interval.hpp>
#include <vec3.hpp>

#include <iosfwd>

using Color = Vec3;

void write_color(std::ostream &, const Color &);
