#include "path_finding.h"

#include <algorithm>
#include <ranges>
#include <queue>
#include <unordered_map>
#include <utility>

namespace std {
template <>
struct hash<st::Point> {
    size_t operator()(const st::Point &point) const { return point.x * 31 + point.y; }
};
}  // namespace std

namespace st {
namespace impl {
auto FindPathGreedy(const Point &src, const Point &dest, const Size &bounds, const PointVec &obstacles) -> PointVec {
    Point current_pos = src;
    PointVec path;
    int distance_threshold = src.DistanceTo(dest) + 50;

    while (current_pos != dest) {
        // If we get stuck and move backward and forward to long count as failure.
        if (path.size() > distance_threshold) {
            return {};
        }

        const std::array<Point, 4> possible_moves{
            Point{current_pos.x, current_pos.y + 1},
            Point{current_pos.x, current_pos.y - 1},
            Point{current_pos.x - 1, current_pos.y},
            Point{current_pos.x + 1, current_pos.y},
        };

        auto moves = possible_moves |
                     std::views::filter([&bounds](const auto &move) { return move.IsWithin(bounds); }) |
                     std::views::filter([&obstacles](const auto &move) {
                         auto it = std::ranges::find_if(obstacles, [&move](const auto &obs) { return obs == move; });
                         return it == obstacles.end();
                     }) |
                     std::views::filter([&path](const auto &move) {
                         if (path.size() < 2)
                             return true;
                         else
                             return move != path.back() && move != *(path.end() - 2);
                     });
        // If we are stuck we failed to get a path
        if (!moves) {
            return {};
        }

        auto best_move = std::ranges::min_element(
            moves, [&dest](const Point &lhs, const Point &rhs) { return lhs.DistanceTo(dest) < rhs.DistanceTo(dest); });
        path.push_back(*best_move);
        current_pos = path.back();
    }
    return path;
}

auto FindPathAStar(const Point &src, const Point &dest, const Size &bounds, const PointVec &obstacles) -> PointVec {
    constexpr std::array<Point, 4> directions{Point{0, 1}, Point{1, 0}, Point{0, -1}, Point{-1, 0}};
    using ScorePoint = std::pair<int, Point>;

    // Priority queue for nodes to explore, ordered by f-score.
    auto cmp = [](const ScorePoint &lhs, ScorePoint &rhs) { return lhs.first > rhs.first; };
    std::priority_queue<ScorePoint, std::vector<ScorePoint>, decltype(cmp)> open_set(std::move(cmp));

    std::unordered_map<Point, int> score;        // Cost from start to each point.
    std::unordered_map<Point, Point> came_from;  // To reconstruct the path.

    score[src] = 0;
    open_set.push(std::make_pair(src.DistanceTo(dest), src));

    while (!open_set.empty()) {
        Point current = open_set.top().second;
        open_set.pop();

        if (current == dest) {
            PointVec path;
            for (Point p = dest; !(p == src); p = came_from[p]) {
                path.push_back(p);
            }
            path.push_back(src);
            std::reverse(path.begin(), path.end());
            return path;
        }

        // Explore neighbors.
        for (const auto &dir : directions) {
            Point neighbor{current.x + dir.x, current.y + dir.y};

            auto it = std::ranges::find_if(obstacles, [&neighbor](const auto &obs) { return obs == neighbor; });
            if (!neighbor.IsWithin(bounds) || it != obstacles.end()) {
                continue;
            }

            // Calculate tentative g-score for this neighbor.
            int tentative_score = score[current] + 1;

            // If this path to neighbor is better, record it.
            if (!score.count(neighbor) || tentative_score < score[neighbor]) {
                int f_score = tentative_score + neighbor.DistanceTo(dest);
                score[neighbor] = tentative_score;
                came_from[neighbor] = current;
                open_set.push(std::make_pair(f_score, neighbor));
            }
        }
    }
    return {};
}
}  // namespace impl

auto FindPath(const Point &src, const Point &dest, const Size &bounds, const PointVec &obstacles, PathAlgorithm algo)
    -> PointVec {
    switch (algo) {
        case PathAlgorithm::AStar:
            return impl::FindPathAStar(src, dest, bounds, obstacles);
        case PathAlgorithm::Greedy:
            return impl::FindPathGreedy(src, dest, bounds, obstacles);
        default:
            std::unreachable();
    }
}

auto PathAlgorithmToString(PathAlgorithm algo) -> std::string_view {
    switch (algo) {
        case PathAlgorithm::AStar:
            return "AStar";
        case PathAlgorithm::Greedy:
            return "Greedy";
        default:
            std::unreachable();
    }
}
}  // namespace st