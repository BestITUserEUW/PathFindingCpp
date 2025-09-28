#pragma once

#include <vector>
#include <string>

#include "point.hpp"
#include "drawer.hpp"

namespace oryx {
class Monitor : public Drawer {
public:
    using Row = std::string;
    using PixelMap = std::vector<Row>;

    explicit Monitor(Size size);

    void Render();
    void SetPixel(Point pos, char ch) override;
    void ClearPixel(Point pos) override;
    auto GetPixel(Point pos) -> char;
    void SetText(Point begin, std::string_view text, bool diagonal = false);
    void SetTitle(std::string title);
    void SetHeader(std::string text);
    void SetHeader2(std::string text);
    auto IsValid(Point pos) const -> bool;
    auto size() const { return size_; }

private:
    std::string out_buffer_;
    Size size_;
    PixelMap pixel_map_;
    std::string title_;
    std::string header_;
    std::string header2_;
    void *stdout_handle_;
};

}  // namespace oryx
