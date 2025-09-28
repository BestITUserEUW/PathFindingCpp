#include "monitor.hpp"

#include <cassert>
#include <format>

#include "windows.hpp"

namespace oryx {
namespace {

auto CreatePixelMap(Size size, char init) -> Monitor::PixelMap {
    Monitor::PixelMap map;
    for (int i = 0; i < size.height; i++) {
        Monitor::Row row;
        row.resize(size.width, init);
        map.push_back(std::move(row));
    }
    return map;
}

}  // namespace

Monitor::Monitor(Size size)
    : size_(size),
      pixel_map_(CreatePixelMap(size, ' ')),
      title_("Monitor"),
      header_(),
      out_buffer_(),
      stdout_handle_(GetStdHandle(STD_OUTPUT_HANDLE)) {}

void Monitor::Render() {
    SetConsoleCursorPosition(stdout_handle_, COORD{0, 0});

    out_buffer_.clear();
    std::format_to(std::back_inserter(out_buffer_), "{:^{}}\n {}\n {}\n {:+^{}}\n", title_, size_.width, header_,
                   header2_, "", size_.width);
    for (const auto &row : pixel_map_) {
        std::format_to(std::back_inserter(out_buffer_), "+{}+\n", row);
    }
    std::format_to(std::back_inserter(out_buffer_), "{:+^{}}\n", "", size_.width);
    DWORD n;
    WriteConsole(stdout_handle_, out_buffer_.data(), static_cast<DWORD>(out_buffer_.size()), &n, {});
}

void Monitor::SetPixel(Point pos, char ch) {
    assert(IsValid(pos) && "Boundary violation!");
    pixel_map_[pos.y][pos.x] = ch;
}

void Monitor::ClearPixel(Point pos) {
    assert(IsValid(pos) && "Boundary violation!");
    pixel_map_[pos.y][pos.x] = ' ';
}

auto Monitor::GetPixel(Point pos) -> char {
    assert(IsValid(pos) && "Boundary violation!");
    return pixel_map_[pos.y][pos.x];
}

auto Monitor::IsValid(Point pos) const -> bool { return pos.IsWithin(size_); }

void Monitor::SetText(Point begin, std::string_view text, bool diagonal) {
    Point current = begin;
    for (char letter : text) {
        SetPixel(current, letter);
        diagonal ? current.y++ : current.x++;
    }
}

void Monitor::SetTitle(std::string title) { title_ = std::move(title); }
void Monitor::SetHeader(std::string text) { header_ = std::move(text); }
void Monitor::SetHeader2(std::string text) { header2_ = std::move(text); }
}  // namespace oryx