#pragma once

#include <cmath>
#include <vector>

namespace st {
struct Size {
    int width;
    int height;
};

struct Point {
    int x;
    int y;

    int DistanceTo(const Point &dest) const { return std::abs(dest.x - x) + std::abs(dest.y - y); }
    bool IsWithin(const Size &size) const { return (x >= 0 && x < size.width) && (y >= 0 && y < size.height); }
    bool operator==(const Point &other) const = default;
    bool operator!=(const Point &other) const = default;
};

using PointVec = std::vector<Point>;
}  // namespace st