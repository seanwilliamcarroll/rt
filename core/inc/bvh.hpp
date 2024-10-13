#pragma once

#include <aabb.hpp>
#include <constants.hpp>
#include <hittable.hpp>
#include <hittable_list.hpp>
#include <interval.hpp>
#include <ray.hpp>

#include <algorithm>
#include <memory>
#include <vector>

class BoundedVolumeHierarchyNode : public Hittable {
public:
  BoundedVolumeHierarchyNode(HittableList hittable_list)
      : BoundedVolumeHierarchyNode(hittable_list.m_objects, 0,
                                   hittable_list.m_objects.size()) {
    // Do we need to make copy of HittableList here?
  }

  BoundedVolumeHierarchyNode(std::vector<std::shared_ptr<Hittable>> &objects,
                             const size_t start, const size_t end) {
    m_bounding_box = AxisAlignedBoundingBox::empty;
    for (size_t index = start; index < end; ++index) {
      m_bounding_box = AxisAlignedBoundingBox(m_bounding_box,
                                              objects[index]->bounding_box());
    }

    const int axis = m_bounding_box.longest_axis();

    const auto comparator = (axis == 0)   ? box_x_compare
                            : (axis == 1) ? box_y_compare
                                          : box_z_compare;

    const size_t object_span = end - start;

    if (object_span == 1) {
      m_left = objects[start];
      m_right = objects[start];
    } else if (object_span == 2) {
      m_left = objects[start];
      m_right = objects[start + 1];
    } else {
      std::sort(std::begin(objects) + start, std::begin(objects) + end,
                comparator);

      const auto midpoint = start + object_span / 2;
      m_left = std::make_shared<BoundedVolumeHierarchyNode>(objects, start,
                                                            midpoint);
      m_right =
          std::make_shared<BoundedVolumeHierarchyNode>(objects, midpoint, end);
    }
  }

  bool hit(const Ray &ray, Interval ray_t,
           HitRecord &hit_record) const override {
    if (!m_bounding_box.hit(ray, ray_t)) {
      return false;
    }

    const bool hit_left = m_left->hit(ray, ray_t, hit_record);
    const bool hit_right = m_right->hit(
        ray, Interval(ray_t.m_min, hit_left ? hit_record.m_t : ray_t.m_max),
        hit_record);

    return hit_left || hit_right;
  }

  AxisAlignedBoundingBox bounding_box() const override {
    return m_bounding_box;
  }

private:
  static bool box_compare(const std::shared_ptr<Hittable> a,
                          const std::shared_ptr<Hittable> b,
                          const int axis_index) {
    const auto a_axis_interval = a->bounding_box().axis_interval(axis_index);
    const auto b_axis_interval = b->bounding_box().axis_interval(axis_index);
    return a_axis_interval.m_min < b_axis_interval.m_min;
  }

  static bool box_x_compare(const std::shared_ptr<Hittable> a,
                            const std::shared_ptr<Hittable> b) {
    return box_compare(a, b, 0);
  }

  static bool box_y_compare(const std::shared_ptr<Hittable> a,
                            const std::shared_ptr<Hittable> b) {
    return box_compare(a, b, 1);
  }

  static bool box_z_compare(const std::shared_ptr<Hittable> a,
                            const std::shared_ptr<Hittable> b) {
    return box_compare(a, b, 2);
  }

private:
  std::shared_ptr<Hittable> m_left;
  std::shared_ptr<Hittable> m_right;
  AxisAlignedBoundingBox m_bounding_box;
};
