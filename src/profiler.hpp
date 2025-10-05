#pragma once

#include <chrono>

#include <cstdint>
#include <oryx/crt/stopwatch.hpp>

namespace oryx {
class Profiler {
public:
    using Clock = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    Profiler() = default;
    void Start() { start_ = Clock::now(); }

    void Stop() {
        elapsed_ = Clock::now() - start_;
        elapsed_sum_ += GetElapsedMs().count();
        iterations_++;
    }

    void Reset() {
        start_ = {};
        elapsed_ = {};
        elapsed_sum_ = 0;
        iterations_ = 0;
    }

    auto GetElapsedMs() const -> std::chrono::milliseconds {
        return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_);
    }

    auto GetAverageMs() const -> uint64_t { return elapsed_sum_ / iterations_; }
    auto GetIterations() const -> uint64_t { return iterations_; }
    auto GetElapsed() const -> std::chrono::nanoseconds { return elapsed_; }

private:
    TimePoint start_{};
    std::chrono::nanoseconds elapsed_{};
    uint64_t elapsed_sum_{};
    uint64_t iterations_{};
};
}  // namespace oryx