#pragma once

#include <chrono>

#include "types.h"

namespace st {
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

    auto GetElapsedMs() -> std::chrono::milliseconds const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_);
    }

    auto GetAverageMs() -> u64 const { return elapsed_sum_ / iterations_; }
    auto GetIterations() -> u64 const { return iterations_; }
    auto GetElapsed() -> std::chrono::nanoseconds const { return elapsed_; }

private:
    TimePoint start_;
    std::chrono::nanoseconds elapsed_;
    u64 elapsed_sum_;
    u64 iterations_;
};
}  // namespace st