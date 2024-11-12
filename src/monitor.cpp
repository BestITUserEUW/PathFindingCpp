#include "monitor.h"

#include <cassert>
#include <sstream>
#include <iomanip>

#include "windows_helpers.h"

namespace st {
namespace {

void WriteStdout(const std::string &str) {
    std::fwrite(str.data(), sizeof(char), str.size(), stdout);
    std::fflush(stdout);
}

auto CreatePixelMap(Size size, char init) -> Monitor::PixelMap {
    Monitor::PixelMap map;
    for (int i = 0; i < size.height; i++) {
        std::vector<char> row;
        row.resize(size.width, init);
        map.push_back(std::move(row));
    }
    return map;
}

}  // namespace

Monitor::Monitor(Size size)
    : size_(size), pixel_map_{CreatePixelMap(size, kEmptyPixel)}, title_{"Monitor"}, header_{} {}

void Monitor::Render() {
    helpers::SetCursorPosition(0, 0);
    std::stringstream buffer;
    buffer << std::setw((size_.width / 2)) << title_ << "\n";
    buffer << header_ << std::setfill(' ') << std::setw((size_.width - header_.size())) << "\n";
    buffer << header2_ << std::setfill(' ') << std::setw((size_.width - header2_.size())) << "\n";
    buffer << std::setfill(kOutline) << std::setw(pixel_map_[0].size()) << "\n";

    for (const auto &x : pixel_map_) {
        buffer << kOutline;
        for (const char ch : x) {
            buffer << ch;
        }
        buffer << kOutline << "\n";
    }
    buffer << std::setfill(kOutline) << std::setw(pixel_map_[0].size()) << "\n";
    WriteStdout(buffer.str());
}

void Monitor::SetPixel(const Point &pos, char ch) {
    assert(IsValid(pos) && "Boundary violation!");
    pixel_map_[pos.y][pos.x] = ch;
}

void Monitor::ClearPixel(const Point &pos) {
    assert(IsValid(pos) && "Boundary violation!");
    pixel_map_[pos.y][pos.x] = kEmptyPixel;
}

char Monitor::GetPixel(const Point &pos) {
    assert(IsValid(pos) && "Boundary violation!");
    return pixel_map_[pos.y][pos.x];
}

bool Monitor::IsValid(const Point &pos) const { return pos.IsWithin(size_); }

void Monitor::SetText(const Point &begin, const std::string &text, bool diagonal) {
    Point current = begin;
    for (char letter : text) {
        SetPixel(current, letter);
        diagonal ? current.y++ : current.x++;
    }
}

void Monitor::SetTitle(const std::string &title) { title_ = title; }
void Monitor::SetHeader(const std::string &text) { header_ = text; }
void Monitor::SetHeader2(const std::string &text) { header2_ = text; }
}  // namespace st