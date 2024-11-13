#pragma once

#include <vector>
#include <string>

#include "point.h"
#include "drawer.h"

namespace st {
class Monitor : public Drawer {
public:
    using Row = std::string;
    using PixelMap = std::vector<Row>;

    explicit Monitor(Size size);

    void Render();
    void SetPixel(const Point &pos, char ch) override;
    void ClearPixel(const Point &pos) override;
    char GetPixel(const Point &pos);
    void SetText(const Point &begin, const std::string &text, bool diagonal = false);
    void SetTitle(const std::string &title);
    void SetHeader(const std::string &text);
    void SetHeader2(const std::string &text);
    bool IsValid(const Point &pos) const;
    Size size() const { return size_; }

private:
    void *stdout_handle_;
    std::string out_buffer_;
    Size size_;
    PixelMap pixel_map_;
    std::string title_;
    std::string header_;
    std::string header2_;
};

}  // namespace st
