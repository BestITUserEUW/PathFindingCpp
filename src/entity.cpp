#include "entity.hpp"

#include <cassert>
#include <ranges>
#include <algorithm>

namespace oryx {
namespace {

void UpdateMission(
    Mission &mission, MissionIDX &mission_idx, std::vector<Entity> &want_new_mission, Entity entity, Point position) {
    if (mission.empty()) {
        want_new_mission.push_back(entity);
        return;
    }

    if (mission.back() == position) {
        mission.clear();
        mission_idx = 0;
        want_new_mission.push_back(entity);
    }
}

void UpdatePositon(Position &position, Mission &mission, MissionIDX &mission_idx, Trail &trail) {
    if (mission.empty()) {
        return;
    }

    trail.push_back(position);
    position = mission[mission_idx];
    mission_idx++;
}

void UpdateTrail(Trail &trail, std::vector<Position> &pending_removals) {
    constexpr size_t kTrailSize = 20;

    if (trail.size() == kTrailSize) {
        pending_removals.push_back(trail.front());
        trail.pop_front();
    }
}

void DrawMission(Drawer *drawer, const Mission &mission, const Entity &mission_idx) {
    // Skip if we already did draw the mission
    if (mission_idx != 1) {
        return;
    }

    for (const auto &item : mission) {
        drawer->SetPixel(item, '~');
    }
    drawer->SetPixel(mission.back(), '?');
}

void DrawPosition(Drawer *drawer, const Position &position, const Shape &shape) {
    drawer->SetPixel(position, shape.look);
}

void DrawTrail(Drawer *drawer, const Trail &trail, const Shape &shape) {
    if (!trail.empty()) {
        drawer->SetPixel(trail.back(), shape.trail);
    }
}

}  // namespace

void EntitySystem::Reserve(size_t size) {
    shapes_.reserve(size);
    positions_.reserve(size);
    missions_.reserve(size);
    missions_idx_.reserve(size);
    trails_.reserve(size);
}

auto EntitySystem::Create(Position start, Shape shape) -> Entity {
    shapes_.emplace_back(shape);
    positions_.emplace_back(start);
    missions_.emplace_back();
    missions_idx_.emplace_back();
    trails_.emplace_back();
    return positions_.size() - 1;
}

void EntitySystem::AssignMission(Entity entity, Mission &&mission) {
    assert(entity < missions_.size() && "Uknown entitiy passed");
    assert(missions_[entity].empty() && "Tried assigning mission to already active mission");
    missions_[entity] = std::forward<Mission>(mission);
}

auto EntitySystem::Update() -> std::vector<Entity> {
    pending_removals_.clear();

    std::vector<Entity> want_new_mission;
    Entity entity{};
    auto components = std::views::zip(positions_, shapes_, trails_, missions_, missions_idx_);
    std::ranges::for_each(components, [&entity, &want_new_mission, &pd = pending_removals_](auto view) {
        auto &[position, shape, trail, mission, mission_idx] = view;

        UpdatePositon(position, mission, mission_idx, trail);
        UpdateMission(mission, mission_idx, want_new_mission, entity, position);
        UpdateTrail(trail, pd);
        entity++;
    });
    return want_new_mission;
}

void EntitySystem::Draw(Drawer *drawer) const {
    auto components = std::views::zip(positions_, shapes_, trails_, missions_, missions_idx_);
    std::ranges::for_each(components, [drawer](const auto &view) {
        auto &[position, shape, trail, mission, mission_idx] = view;

        DrawPosition(drawer, position, shape);
        DrawTrail(drawer, trail, shape);
        DrawMission(drawer, mission, mission_idx);
    });

    for (auto &removal : pending_removals_) {
        drawer->ClearPixel(removal);
    }
}

auto EntitySystem::NumEntities() -> size_t const { return positions_.size(); }

}  // namespace oryx