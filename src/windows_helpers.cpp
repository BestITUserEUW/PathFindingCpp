#include "windows_helpers.h"

#include <windows.h>

namespace st::helpers {

void SetCursorPosition(short C, short R) {
    COORD xy;
    xy.X = C;
    xy.Y = R;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), xy);
}

auto GetTerminalSize() -> Size {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return Size{0, 0};
    }
    return Size{csbi.dwSize.X - 10, csbi.dwSize.Y - 10};
}

}  // namespace st::helpers