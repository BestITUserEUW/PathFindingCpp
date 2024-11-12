#pragma once

#include "point.h"

namespace st {

struct Obstacle : Point {
    static constexpr char shape = '#';
};

}  // namespace st