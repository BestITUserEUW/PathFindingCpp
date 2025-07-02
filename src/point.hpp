#pragma once

#include <cmath>
#include <vector>

namespace oryx {
struct Size {
    int width;
    int height;
};

struct Point {
    int x;
    int y;

    auto DistanceTo(const Point &dest) const -> int { return std::abs(dest.x - x) + std::abs(dest.y - y); }
    auto IsWithin(const Size &size) const -> bool { return (x >= 0 && x < size.width) && (y >= 0 && y < size.height); }
    auto operator==(const Point &other) const -> bool = default;
    auto operator!=(const Point &other) const -> bool = default;
};

using PointVec = std::vector<Point>;
}  // namespace oryx