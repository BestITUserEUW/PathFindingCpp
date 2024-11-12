#include "entity.h"

namespace st {

Entity::Entity(const Shape &shape, const Point &start) : shape_(shape), pos_(start) {}

void Entity::MoveTo(const Point &pos) {
    trail_.push_back(pos_);
    pos_ = pos;
}

void Entity::Draw(Monitor &drawer) {
    drawer.SetPixel(pos_, shape_.look);

    if (!trail_.empty()) {
        drawer.SetPixel(trail_.back(), shape_.trail);
        drawer.ClearPixel(trail_.front());
    }

    if (trail_.size() == kTrailSize) {
        trail_.pop_front();
    }
}

auto Entity::Create(const Point &start) -> Entity { return Entity{Shape{'O', '-'}, start}; }

}  // namespace st