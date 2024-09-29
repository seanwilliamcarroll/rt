#pragma once

#include <hittable.hpp>

#include <memory>
#include <vector>

class HittableList : public Hittable {
public:
  HittableList() {}

  HittableList(std::shared_ptr<Hittable> object)
      : m_objects{object} {}

  void clear() { m_objects.clear(); }

  void add(std::shared_ptr<Hittable> object) { m_objects.push_back(object); }

  bool hit(const Ray &ray, Interval ray_t,
           HitRecord &hit_record) const override {
    HitRecord hit_sample;
    bool hit_anything = false;
    auto closest_so_far = ray_t.m_max;

    for (const auto &object : m_objects) {
      if (object->hit(ray, Interval(ray_t.m_min, closest_so_far), hit_sample)) {
        hit_anything = true;
        closest_so_far = hit_sample.m_t;
        hit_record = hit_sample;
      }
    }
    return hit_anything;
  }

private:
  std::vector<std::shared_ptr<Hittable>> m_objects;
};
