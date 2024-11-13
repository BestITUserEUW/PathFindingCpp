#pragma once

#include <deque>
#include <tuple>

#include "point.h"
#include "drawer.h"

namespace st {

struct EntityShape {
    char look;
    char trail;
};

using Mission = std::vector<Point>;
using Trail = std::deque<Point>;
using EntityID = size_t;

class EntityRegistry {
public:
    EntityRegistry() = default;

    template <typename T>
    constexpr T &GetComponent(EntityID id);

    void Reserve(size_t size);
    auto Create(const Point &start_position, const EntityShape &shape = EntityShape('O', '-')) -> EntityID;
    void AssignMission(EntityID id, Mission &&mission);
    // Update entities and return a vector of entites with ids that
    auto Update() -> std::vector<EntityID>;
    void Draw(Drawer *drawer) const;
    auto NumEntities() -> size_t const;

private:
    std::vector<EntityShape> shapes_;
    std::vector<Point> positions_;
    std::vector<Mission> missions_;
    std::vector<EntityID> missions_idx_;
    std::vector<Trail> trails_;
    std::vector<Point> pending_removals_;
    size_t num_entities_{};
};

template <typename T>
constexpr T &EntityRegistry::GetComponent(EntityID id) {
    if constexpr (std::is_same<T, EntityShape>()) {
        return shapes_[id];
    } else if constexpr (std::is_same<T, Point>()) {
        return positions_[id];
    } else if constexpr (std::is_same<T, Mission>()) {
        return missions_[id];
    } else if constexpr (std::is_same<T, EntityID>()) {
        return missions_idx_[id];
    } else if constexpr (std::is_same<T, Trail>()) {
        return trails_[id];
    } else {
        static_assert(false && "Failed to find component");
    }
}

}  // namespace st