#pragma once

#include <deque>
#include <tuple>
#include <ranges>

#include "point.h"
#include "drawer.h"

namespace st {

// Components
struct Shape {
    char look;
    char trail;
};
using Position = Point;
using Mission = std::vector<Point>;
using Trail = std::deque<Point>;
using MissionIDX = size_t;

// Entity is just an index pointing to the position of the vector
using Entity = size_t;

class EntitySystem {
public:
    EntitySystem() = default;

    template <typename T>
    constexpr const T &View(Entity entity);

    void Reserve(size_t size);
    auto Create(const Position &start, const Shape &shape = Shape('O', '-')) -> Entity;
    void AssignMission(Entity entity, Mission &&mission);
    // Update entities and return a vector of entites that want a new mission
    auto Update() -> std::vector<Entity>;
    void Draw(Drawer *drawer) const;
    auto NumEntities() -> size_t const;

private:
    std::vector<Shape> shapes_;
    std::vector<Point> positions_;
    std::vector<Mission> missions_;
    std::vector<MissionIDX> missions_idx_;
    std::vector<Trail> trails_;
    std::vector<Position> pending_removals_;
};

template <typename T>
constexpr const T &EntitySystem::View(Entity entity) {
    if constexpr (std::is_same<T, Shape>()) {
        return shapes_[entity];
    } else if constexpr (std::is_same<T, Position>()) {
        return positions_[entity];
    } else if constexpr (std::is_same<T, Mission>()) {
        return missions_[entity];
    } else if constexpr (std::is_same<T, MissionIDX>()) {
        return missions_idx_[entity];
    } else if constexpr (std::is_same<T, Trail>()) {
        return trails_[entity];
    } else {
        static_assert(false && "Uknown component");
    }
}

}  // namespace st