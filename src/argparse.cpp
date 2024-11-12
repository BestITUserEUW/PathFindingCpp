#include "argparse.h"

#include <algorithm>
#include <string>
#include <sstream>
#include <print>
#include <thread>
#include <utility>

#include "windows_helpers.h"

namespace st::argparse {
namespace {
bool HasArgument(char **begin, char **end, string_view option) { return std::find(begin, end, option) != end; }
auto GetArgumentValue(char **begin, char **end, string_view option) -> std::string {
    char **itr = std::find(begin, end, option);
    if (itr != end && ++itr != end) {
        return std::string(*itr);
    }
    return {};
}

int CalculateDefaultObstacles(const Size &monitor_size) { return monitor_size.width * monitor_size.height / 10; }

}  // namespace

void PrintHelpMessage() {
    const auto monitor_size = helpers::GetTerminalSize();
    const auto thread_count = std::thread::hardware_concurrency();
    std::stringstream ss;
    ss << "Help \n";
    ss << Args::khelp << "         Print this help message\n";
    ss << Args::kWidth << "        Monitor width default: " << monitor_size.width << "\n";
    ss << Args::kHeight << "       Monitor height default: " << monitor_size.height << "\n";
    ss << Args::kAlgorithm << "    Algorithms available: \n";
    ss << "                        " << std::to_underlying(PathAlgorithm::Greedy) << ": "
       << PathAlgorithmToString(PathAlgorithm::Greedy) << "\n";
    ss << "                        " << std::to_underlying(PathAlgorithm::AStar) << ": "
       << PathAlgorithmToString(PathAlgorithm::AStar) << "\n";
    ss << Args::kEntities << "     Number of entities to spawn default: " << Args::kDefaultEntities << "\n";
    ss << Args::kObstacles << "    Number of obstacles to spawn default: " << CalculateDefaultObstacles(monitor_size)
       << "\n";
    ss << Args::kThreads << "      Thread count default: " << thread_count << "\n";
    ss << Args::kLoopTime << "     Loop time default: " << Args::kDefaultLoopTime << "\n";
    std::println("{}", ss.str());
}

auto ParseArguments(int argc, char *argv[]) -> unique_ptr<Args> {
    auto args = std::make_unique<Args>();
    args->monitor_size = helpers::GetTerminalSize();
    args->thread_count = std::thread::hardware_concurrency();

    if (HasArgument(argv, argv + argc, "--help")) {
        PrintHelpMessage();
        std::exit(0);
    }

    if (HasArgument(argv, argv + argc, Args::kWidth)) {
        const auto width = GetArgumentValue(argv, argv + argc, Args::kHeight);
        if (!width.empty()) args->monitor_size.height = std::stoi(width);
    }

    if (HasArgument(argv, argv + argc, Args::kHeight)) {
        const auto height = GetArgumentValue(argv, argv + argc, Args::kHeight);
        if (!height.empty()) args->monitor_size.height = std::stoi(height);
    }

    if (HasArgument(argv, argv + argc, Args::kEntities)) {
        const auto entities = GetArgumentValue(argv, argv + argc, Args::kEntities);
        if (!entities.empty()) args->num_entities = std::stoi(entities);
    }

    args->num_obstacles = CalculateDefaultObstacles(args->monitor_size);
    if (HasArgument(argv, argv + argc, Args::kObstacles)) {
        const auto obstacles = GetArgumentValue(argv, argv + argc, Args::kObstacles);
        if (!obstacles.empty()) args->num_obstacles = std::stoi(obstacles);
    }

    if (HasArgument(argv, argv + argc, Args::kThreads)) {
        const auto thread_count = GetArgumentValue(argv, argv + argc, Args::kThreads);
        if (!thread_count.empty()) args->thread_count = std::stoi(thread_count);
    }

    if (HasArgument(argv, argv + argc, Args::kLoopTime)) {
        const auto loop_time = GetArgumentValue(argv, argv + argc, Args::kLoopTime);
        if (!loop_time.empty()) args->loop_time = std::chrono::milliseconds(std::stoi(loop_time));
    }

    if (HasArgument(argv, argv + argc, Args::kAlgorithm)) {
        const auto algorithm = GetArgumentValue(argv, argv + argc, Args::kAlgorithm);
        if (!algorithm.empty()) {
            int algo = std::stoi(algorithm);
            // NOTE: For now best solution i could come up with if Algorithm order changes this will be UB
            if (algo < std::to_underlying(PathAlgorithm::Greedy) || algo > std::to_underlying(PathAlgorithm::AStar)) {
                std::println("[Argparse] Unknown algorithm: {} !!", algo);
                return nullptr;
            }
            args->algorithm = static_cast<PathAlgorithm>(algo);
        }
    }

    return args;
}

}  // namespace st::argparse
