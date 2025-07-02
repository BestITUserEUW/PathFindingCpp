#pragma once

#include "point.hpp"

namespace oryx {

class Drawer {
public:
    virtual ~Drawer() = default;
    virtual void SetPixel(const Point &pos, char ch) = 0;
    virtual void ClearPixel(const Point &pos) = 0;
};

}  // namespace oryx