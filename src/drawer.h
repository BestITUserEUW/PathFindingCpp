#pragma once

#include "point.h"

namespace st {

class Drawer {
public:
    virtual ~Drawer() = default;
    virtual void SetPixel(const Point &pos, char ch) = 0;
    virtual void ClearPixel(const Point &pos) = 0;
};

}  // namespace st