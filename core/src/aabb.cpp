#include <aabb.hpp>

const AxisAlignedBoundingBox AxisAlignedBoundingBox::empty =
    AxisAlignedBoundingBox(Interval::empty, Interval::empty, Interval::empty);

const AxisAlignedBoundingBox AxisAlignedBoundingBox::universe =
    AxisAlignedBoundingBox(Interval::universe, Interval::universe,
                           Interval::universe);
