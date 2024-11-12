#pragma once

#include <vector>
#include <string>

#include "point.h"

namespace st {
class Monitor {
public:
    using PixelMap = std::vector<std::vector<char>>;

    explicit Monitor(Size size);

    void Render();
    void SetPixel(const Point &pos, char ch);
    void ClearPixel(const Point &pos);
    char GetPixel(const Point &pos);
    void SetText(const Point &begin, const std::string &text, bool diagonal = false);
    void SetTitle(const std::string &title);
    void SetHeader(const std::string &text);
    void SetHeader2(const std::string &text);
    bool IsValid(const Point &pos) const;
    Size size() const { return size_; }

private:
    static constexpr char kOutline = '+';
    static constexpr char kEmptyPixel = ' ';

    Size size_;
    PixelMap pixel_map_;
    std::string title_;
    std::string header_;
    std::string header2_;
};

}  // namespace st
