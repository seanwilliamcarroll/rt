#include <camera.hpp>
#include <hittable.hpp>
#include <hittable_list.hpp>
#include <material.hpp>
#include <rt.hpp>
#include <sphere.hpp>

#include <memory>

HittableList scene_rt_one_weekend() {

  HittableList world;

  auto ground_material = std::make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
  world.add(
      std::make_shared<Sphere>(Point3(0, -1000, 0), 1000, ground_material));

  for (int a = -11; a < 11; ++a) {
    for (int b = -11; b < 11; ++b) {
      const auto choose_material = random_double();

      const Point3 center(a + 0.6 * random_double(), 0.2,
                          b + 0.6 * random_double());

      if ((center - Point3(4, 0.2, 0)).length() > 0.9) {
        std::shared_ptr<Material> sphere_material;

        if (choose_material < 0.8) {
          // Diffuse
          const auto albedo = Color::random() * Color::random();
          sphere_material = std::make_shared<Lambertian>(albedo);
          const auto center2 = center + Vec3(0, random_double(0, 0.5), 0);
          world.add(
              std::make_shared<Sphere>(center, center2, 0.2, sphere_material));
        } else if (choose_material < 0.95) {
          // metal
          const auto albedo = Color::random(0.5, 1.0);
          const auto fuzz = random_double(0.0, 0.5);
          sphere_material = std::make_shared<Metal>(albedo, fuzz);
          world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
        } else {
          // glass
          sphere_material = std::make_shared<Dielectric>(1.50);
          world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
        }
      }
    }
  }

  const auto material1 = std::make_shared<Dielectric>(1.5);
  const auto material2 = std::make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
  const auto material3 = std::make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);

  world.add(std::make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));
  world.add(std::make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));
  world.add(std::make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

  return world;
}

Camera camera_rt_one_weekend() {

  // Image
  const double aspect_ratio = 16.0 / 9.0;
  const unsigned int image_width = 400;
  const unsigned int samples_per_pixel = 100;
  const unsigned int max_depth = 50;
  const Point3 camera_position = Point3(13, 2, 3);
  const Point3 looking_at = Point3(0, 0, 0);
  const Vec3 up_direction = Point3(0, 1, 0);
  const double vertical_field_of_view = 20;
  const double defocus_angle = 0.6;
  const double focus_dist = 10.0;
  Camera camera(aspect_ratio, image_width, samples_per_pixel, max_depth,
                camera_position, looking_at, up_direction,
                vertical_field_of_view, defocus_angle, focus_dist);

  return camera;
}
