#pragma once

#include <algorithm>
#include <iostream>

#include <color.hpp>
#include <hittable.hpp>
#include <interval.hpp>
#include <material.hpp>
#include <thread_pool.hpp>
#include <vec3.hpp>

class Camera {
public:
  Camera(const double aspect_ratio = 1.0, const unsigned int image_width = 100,
         const unsigned int samples_per_pixel = 10,
         const unsigned int max_depth = 10,
         const Vec3 camera_position = Point3(0, 0, 0),
         const Vec3 looking_at = Point3(0, 0, -1),
         const Vec3 up_direction = Vec3(0, 1, 0),
         const double vertical_field_of_view = 90,
         const double defocus_angle = 0, const double focus_dist = 10)
      : m_aspect_ratio(aspect_ratio)
      , m_image_width(image_width)
      , m_image_height(std::max<unsigned int>(
            1, (unsigned int)(m_image_width / m_aspect_ratio)))
      , m_image_matrix(m_image_width * m_image_height, Color(0, 0, 0))
      , m_camera_position(camera_position)
      , m_defocus_angle(defocus_angle)
      , m_focus_dist(focus_dist)
      , m_frame_basis(
            calculate_frame_basis(camera_position, looking_at, up_direction))
      , m_defocus_radius(focus_dist *
                         std::tan(degrees_to_radians(defocus_angle / 2)))
      , m_defocus_disk_u(m_frame_basis[0] * m_defocus_radius)
      , m_defocus_disk_v(m_frame_basis[1] * m_defocus_radius)
      , m_vertical_field_of_view(vertical_field_of_view)
      , m_viewport_height(
            calculate_viewport_height(m_vertical_field_of_view, focus_dist))
      , m_viewport_width(m_viewport_height *
                         (double(m_image_width) / m_image_height))
      , m_pixel_delta_u(m_viewport_width * m_frame_basis[0] / m_image_width)
      , m_pixel_delta_v(m_viewport_height * (-m_frame_basis[1]) /
                        m_image_height)
      , m_pixel00_loc(calc_pixel00_loc())
      , m_samples_per_pixel(std::max<unsigned int>(1, samples_per_pixel))
      , m_pixel_samples_scale(1.0 / m_samples_per_pixel)
      , m_max_depth(max_depth)
      , m_thread_pool(std::make_unique<ThreadPool>()) {}

  void render(const Hittable &world) {
    m_thread_pool->start();
    std::cout << "P3\n" << m_image_width << " " << m_image_height << "\n255\n";

    for (unsigned int row = 0; row < m_image_height; ++row) {
      std::clog << "\rScanline: " << row + 1 << " / " << m_image_height
                << std::flush;
      m_thread_pool->add_job([this, row, &world] { render_row(row, world); });
    }

    m_thread_pool->wait_for_empty_job_queue();
    m_thread_pool->stop();

    std::clog << "\rWrite to file       " << std::flush;
    for (const auto &color : m_image_matrix) {
      write_color(std::cout, color);
    }
    std::clog << "\rDone.                              \n";
  }

private:
  void render_pixel(const unsigned int row, const unsigned int column,
                    const Hittable &world) {
    const unsigned int flat_pixel_position = row * m_image_width + column;
    Color pixel_color(0, 0, 0);
    for (unsigned int sample = 0; sample < m_samples_per_pixel; ++sample) {
      Ray ray = get_ray(column, row);
      pixel_color += ray_color(ray, m_max_depth, world);
    }
    m_image_matrix[flat_pixel_position] = m_pixel_samples_scale * pixel_color;
  }

  void render_row(const unsigned int row, const Hittable &world) {
    for (unsigned int column = 0; column < m_image_width; ++column) {
      render_pixel(row, column, world);
    }
  }

  Ray get_ray(const int i, const int j) const {
    const auto offset = sample_square();
    const auto pixel_sample = m_pixel00_loc +
                              ((i + offset.x()) * m_pixel_delta_u) +
                              ((j + offset.y()) * m_pixel_delta_v);

    const auto ray_origin =
        (m_defocus_angle <= 0) ? m_camera_position : defocus_disk_sample();

    const auto ray_direction = pixel_sample - ray_origin;
    const auto ray_time = random_double();
    return Ray(ray_origin, ray_direction, ray_time);
  }

  Point3 defocus_disk_sample() const {
    // Return a random point in the camera defocus disk
    const auto point = random_in_unit_disk();
    return m_camera_position + (point[0] * m_defocus_disk_u) +
           (point[1] * m_defocus_disk_v);
  }

  // Returns sample in range x:[-0.5, 0.5) y:[-0.5, 0.5)
  static Vec3 sample_square() {
    return Vec3(random_double() - 0.5, random_double() - 0.5, 0);
  }

  Color ray_color(const Ray &ray, const unsigned int depth,
                  const Hittable &world) const {
    if (depth == 0) {
      return Color(0, 0, 0);
    }
    HitRecord hit_record;

    if (world.hit(ray, Interval(0.001, infinity), hit_record)) {
      Ray scattered;
      Color attenuation;
      if (hit_record.m_material->scatter(ray, hit_record, attenuation,
                                         scattered)) {
        return attenuation * ray_color(scattered, depth - 1, world);
      }
      return Color(0, 0, 0);
    }
    const Vec3 unit_direction = unit_vector(ray.direction());
    const auto alpha = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - alpha) * Color(1.0, 1.0, 1.0) + alpha * Color(0.5, 0.7, 1.0);
  }

private:
  Point3 calc_pixel00_loc() const {
    const auto viewport_upper_left =
        m_camera_position - (m_focus_dist * m_frame_basis[2]) -
        (m_viewport_width * m_frame_basis[0]) / 2 -
        (-m_viewport_height * m_frame_basis[1]) / 2;
    return viewport_upper_left + 0.5 * (m_pixel_delta_u + m_pixel_delta_v);
  }

  static double calculate_viewport_height(const double vertical_field_of_view,
                                          const double focus_dist) {
    const auto theta = degrees_to_radians(vertical_field_of_view);

    const auto h = std::tan(theta / 2);

    return 2 * h * focus_dist;
  }

  static std::array<Vec3, 3> calculate_frame_basis(const Vec3 &camera_position,
                                                   const Vec3 &looking_at,
                                                   const Vec3 &up_direction) {
    const auto w = unit_vector(camera_position - looking_at);
    const auto u = unit_vector(cross(up_direction, w));
    const auto v = cross(w, u);
    return std::array<Vec3, 3>{u, v, w};
  }

private:
  const double m_aspect_ratio;
  const unsigned int m_image_width;
  const unsigned int m_image_height;
  std::vector<Color> m_image_matrix;
  const Point3 m_camera_position;
  const double m_defocus_angle;
  const double m_focus_dist;
  const std::array<Vec3, 3> m_frame_basis;
  const double m_defocus_radius;
  const Vec3 m_defocus_disk_u;
  const Vec3 m_defocus_disk_v;
  const double m_vertical_field_of_view;
  const double m_viewport_height;
  const double m_viewport_width;
  const Vec3 m_pixel_delta_u;
  const Vec3 m_pixel_delta_v;
  const Point3 m_pixel00_loc;
  const unsigned int m_samples_per_pixel;
  const double m_pixel_samples_scale;
  const unsigned int m_max_depth;
  std::unique_ptr<ThreadPool> m_thread_pool;
};
