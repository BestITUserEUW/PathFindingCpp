#pragma once

#include <cstdint>
#include <span>

#include "point.hpp"

namespace oryx {
enum class PathAlgorithm : uint8_t { Greedy, AStar };

namespace impl {

auto FindPathAStar(Point src, Point dest, Size bounds, std::span<Point> obstacles) -> PointVec;
auto FindPathGreedy(Point src, Point dest, Size bounds, std::span<Point> obstacles) -> PointVec;
}  // namespace impl

auto FindPath(Point src, Point dest, Size bounds, std::span<Point> obstacles, PathAlgorithm algo = PathAlgorithm::AStar)
    -> PointVec;
}  // namespace oryx