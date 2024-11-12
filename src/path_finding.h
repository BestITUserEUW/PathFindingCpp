#pragma once

#include <vector>
#include <string_view>

#include "types.h"
#include "point.h"

namespace st {
enum class PathAlgorithm { Greedy, AStar };

namespace impl {

auto FindPathAStar(const Point &src, const Point &dest, const Size &bounds, const PointVec &obstacles) -> PointVec;
auto FindPathGreedy(const Point &src, const Point &dest, const Size &bounds, const PointVec &obstacles) -> PointVec;
}  // namespace impl

auto FindPath(const Point &src,
              const Point &dest,
              const Size &bounds,
              const PointVec &obstacles,
              PathAlgorithm algo = PathAlgorithm::AStar) -> PointVec;

auto PathAlgorithmToString(PathAlgorithm algo) -> string_view;
}  // namespace st