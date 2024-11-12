#pragma once

#include <vector>

#include "entity.h"

namespace st {
class MissionRunner {
public:
    MissionRunner() : entity_(nullptr), mission_(), pos_() {}
    MissionRunner(Entity *entity) : entity_(entity), pos_(0) {}

    void Next();
    void DrawMission(Monitor &monitor);
    void AssignMission(const PointVec &mission);
    void AssignEntity(Entity *entity) { entity_ = entity; }
    bool IsComplete() const { return mission_.empty() || pos_ == mission_.size(); }
    auto entity() -> Entity * { return entity_; }

private:
    Entity *entity_;
    PointVec mission_;
    size_t pos_;
};
}  // namespace st