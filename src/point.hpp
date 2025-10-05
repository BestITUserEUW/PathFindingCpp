#pragma once

#include <cstdint>
#include <cmath>
#include <vector>

namespace oryx {

struct Size {
    uint16_t width;
    uint16_t height;
};

struct Point {
    uint16_t x;
    uint16_t y;

    auto DistanceTo(Point dest) const -> int { return std::abs(dest.x - x) + std::abs(dest.y - y); }
    auto IsWithin(Size size) const -> bool { return (x >= 0 && x < size.width) && (y >= 0 && y < size.height); }
    auto operator==(const Point &other) const -> bool = default;
    auto operator!=(const Point &other) const -> bool = default;
};

using PointVec = std::vector<Point>;
}  // namespace oryx