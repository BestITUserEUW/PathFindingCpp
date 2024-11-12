#pragma once

#include <deque>

#include "point.h"
#include "monitor.h"

namespace st {
class Entity {
public:
    struct Shape {
        char look;
        char trail;
    };

    Entity(const Shape &shape, const Point &start);
    void MoveTo(const Point &pos);
    void Draw(Monitor &drawer);
    auto shape() const -> Shape { return shape_; }
    auto pos() const -> Point { return pos_; }
    static auto Create(const Point &start) -> Entity;

private:
    static constexpr size_t kTrailSize = 20;

    Shape shape_;
    Point pos_;
    std::deque<Point> trail_;
};
}  // namespace st