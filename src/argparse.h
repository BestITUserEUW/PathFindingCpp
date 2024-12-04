#pragma once

#include <string_view>
#include <chrono>
#include <memory>
#include <functional>

#include "types.h"
#include "from_chars.h"
#include "point.h"
#include "path_finding.h"

namespace st::argparse {

class CLI {
public:
    CLI(int argc, const char* const* argv) : view_(argv, argc) {}

    auto Contains(std::string_view option) const { return std::ranges::find(view_, option) != view_.end(); }

    template <typename T>
    auto GetValue(std::string_view option) const -> std::optional<T> {
        auto it = std::ranges::find(view_, option);
        if (it == view_.end()) {
            return std::nullopt;
        }

        auto vit = it + 1;
        if (vit == view_.end()) {
            return std::nullopt;
        }

        if constexpr (std::is_same<T, std::string>()) {
            return std::string(*vit);
        } else if constexpr (std::is_integral<T>()) {
            return FromChars<T>(*vit);
        } else if constexpr (std::is_floating_point<T>()) {
            return FromChars<T>(*vit);
        } else {
            static_assert(false, "Get argument only supports std::string integral and floating point types");
        }
    }

    template <typename T, typename F>
    void VisitIfContains(std::string_view option, F visitor) {
        if (!Contains(option)) {
            return;
        }

        auto value = GetValue<T>(option);
        if (value) {
            std::invoke(visitor, std::forward<T>(*value));
        }
    }

    auto Empty() const { return view_.empty(); }
    auto Size() const { return view_.size(); }

private:
    std::basic_string_view<const char*> view_;
};

struct Arguments {
    Size monitor_size;
    PathAlgorithm algorithm;
    std::chrono::milliseconds loop_time;
    int thread_count;
    int num_obstacles;
    int num_entities;
};

auto ParseArguments(int argc, char* argv[]) -> Arguments;

}  // namespace st::argparse