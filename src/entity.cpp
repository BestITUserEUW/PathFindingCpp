#include "entity.h"

#include <cassert>
#include <ranges>
#include <algorithm>

namespace st {
namespace {

static constexpr size_t kTrailSize = 20;

void DrawMission(Drawer *drawer, const Mission &mission) {
    for (const auto &item : mission) {
        drawer->SetPixel(item, '~');
    }
    drawer->SetPixel(mission.back(), '?');
}

void DrawEntity(Drawer *drawer, const Point &position, const EntityShape &shape) {
    drawer->SetPixel(position, shape.look);
}

void DrawTrail(Drawer *drawer, const Trail &trail, const EntityShape &shape) {
    if (!trail.empty()) {
        drawer->SetPixel(trail.back(), shape.trail);
    }
}

}  // namespace

void EntityRegistry::Reserve(size_t size) {
    shapes_.reserve(size);
    positions_.reserve(size);
    missions_.reserve(size);
    missions_idx_.reserve(size);
    trails_.reserve(size);
}

auto EntityRegistry::Create(const Point &start_position, const EntityShape &shape) -> EntityID {
    shapes_.push_back(shape);
    positions_.push_back(start_position);
    missions_.push_back({});
    missions_idx_.push_back({});
    trails_.push_back({});
    EntityID id = num_entities_;
    num_entities_++;
    return num_entities_;
}

void EntityRegistry::AssignMission(EntityID id, Mission &&mission) {
    assert(id < missions_.size() && "Uknown entitiy id passed");
    assert(missions_[id].empty() && "Tried assigning mission to already active mission");
    missions_[id] = std::forward<Mission>(mission);
}

auto EntityRegistry::Update() -> std::vector<EntityID> {
    pending_removals_.clear();

    std::vector<EntityID> want_new_mission;
    for (EntityID id = 0; id < positions_.size(); id++) {
        auto &position = positions_[id];
        auto &mission = missions_[id];
        auto &mission_idx = missions_idx_[id];
        auto &trail = trails_[id];

        // Update position
        if (!mission.empty()) {
            trail.push_back(position);
            position = mission[mission_idx];
            mission_idx++;
        }

        // Update missions
        if (!mission.empty()) {
            if (mission.back() == position) {
                mission.clear();
                mission_idx = 0;
                want_new_mission.push_back(id);
            }
        } else {
            want_new_mission.push_back(id);
        }

        // Update trails
        if (trail.size() == kTrailSize) {
            pending_removals_.push_back(trail.front());
            trail.pop_front();
        }
    }
    return want_new_mission;
}

void EntityRegistry::Draw(Drawer *drawer) const {
    auto zipped = std::views::zip(positions_, shapes_, trails_, missions_, missions_idx_);
    std::ranges::for_each(zipped, [drawer](auto view) {
        DrawEntity(drawer, std::get<0>(view), std::get<1>(view));
        DrawTrail(drawer, std::get<2>(view), std::get<1>(view));

        // If this is a new mission draw it
        if (std::get<4>(view) == 1) {
            DrawMission(drawer, std::get<3>(view));
        }
    });

    for (auto &removal : pending_removals_) {
        drawer->ClearPixel(removal);
    }
}

auto EntityRegistry::NumEntities() -> size_t const { return num_entities_; }

}  // namespace st