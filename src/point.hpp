#pragma once

#include <cmath>
#include <vector>

#include <oryx/types.hpp>

namespace oryx {

struct Size {
    u16 width;
    u16 height;
};

struct Point {
    u16 x;
    u16 y;

    auto DistanceTo(Point dest) const -> int { return std::abs(dest.x - x) + std::abs(dest.y - y); }
    auto IsWithin(Size size) const -> bool { return (x >= 0 && x < size.width) && (y >= 0 && y < size.height); }
    auto operator==(const Point &other) const -> bool = default;
    auto operator!=(const Point &other) const -> bool = default;
};

using PointVec = std::vector<Point>;
}  // namespace oryx