#include <camera.hpp>
#include <hittable_list.hpp>
#include <rt.hpp>

#include <memory>

// We draw the image from top left corner across and then down

int main() {

  Camera camera = camera_rt_one_weekend();

  HittableList world = scene_rt_one_weekend();

  // Render
  camera.render(world);

  return 0;
}
