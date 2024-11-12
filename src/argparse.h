#pragma once

#include <string_view>
#include <chrono>
#include <memory>

#include "types.h"
#include "point.h"
#include "path_finding.h"

namespace st::argparse {

struct Args {
    static constexpr string_view khelp = "--help";
    static constexpr string_view kWidth = "--width";
    static constexpr string_view kHeight = "--height";
    static constexpr string_view kThreads = "--threads";
    static constexpr string_view kEntities = "--entities";
    static constexpr string_view kObstacles = "--obstacles";
    static constexpr string_view kLoopTime = "--loopTime";
    static constexpr string_view kAlgorithm = "--algorithm";

    static constexpr int kDefaultEntities = 20;
    static constexpr int kDefaultLoopTime = 20;
    static constexpr PathAlgorithm kDefaultAlgo = PathAlgorithm::AStar;

    Size monitor_size;
    int thread_count;
    int num_obstacles;
    PathAlgorithm algorithm{kDefaultAlgo};
    int num_entities{kDefaultEntities};
    std::chrono::milliseconds loop_time{kDefaultLoopTime};
};

void PrintHelpMessage();
auto ParseArguments(int argc, char *argv[]) -> unique_ptr<Args>;

}  // namespace st::argparse