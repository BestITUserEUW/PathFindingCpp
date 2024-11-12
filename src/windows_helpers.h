#pragma once

#include "point.h"

namespace st::helpers {
void SetCursorPosition(short C, short R);

// Size 0,0 means failed to get
auto GetTerminalSize() -> Size;
}  // namespace st::helpers