#pragma once

#include <chrono>

#include "point.hpp"
#include "path_finding.hpp"

namespace oryx {

struct Arguments {
    Size monitor_size;
    PathAlgorithm algorithm;
    std::chrono::milliseconds loop_time;
    int thread_count;
    int num_obstacles;
    int num_entities;
};

auto ParseArguments(int argc, char* argv[]) -> Arguments;

}  // namespace oryx