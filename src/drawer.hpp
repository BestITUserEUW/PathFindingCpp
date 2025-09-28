#pragma once

#include "point.hpp"

namespace oryx {

class Drawer {
public:
    virtual ~Drawer() = default;
    virtual void SetPixel(Point pos, char ch) = 0;
    virtual void ClearPixel(Point pos) = 0;
};

}  // namespace oryx