#pragma once

#include <cstdint>

#include <oryx/types.hpp>

#include "point.hpp"

namespace oryx {
enum class PathAlgorithm : uint8_t { Greedy, AStar };

namespace impl {

auto FindPathAStar(const Point &src, const Point &dest, const Size &bounds, const PointVec &obstacles) -> PointVec;
auto FindPathGreedy(const Point &src, const Point &dest, const Size &bounds, const PointVec &obstacles) -> PointVec;
}  // namespace impl

auto FindPath(const Point &src,
              const Point &dest,
              const Size &bounds,
              const PointVec &obstacles,
              PathAlgorithm algo = PathAlgorithm::AStar) -> PointVec;
}  // namespace oryx