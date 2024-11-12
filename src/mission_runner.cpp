#include "mission_runner.h"

namespace st {
void MissionRunner::Next() {
    if (IsComplete() || !entity_) {
        return;
    }
    entity_->MoveTo(mission_[pos_++]);
}

void MissionRunner::DrawMission(Monitor &monitor) {
    if (mission_.empty()) {
        return;
    }

    for (const auto &item : mission_) {
        monitor.SetPixel(item, '~');
    }
    monitor.SetPixel(mission_.back(), '?');
}

void MissionRunner::AssignMission(const PointVec &mission) {
    pos_ = 0;
    mission_ = mission;
}
}  // namespace st