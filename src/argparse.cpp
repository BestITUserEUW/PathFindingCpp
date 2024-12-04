#include "argparse.h"

#include <algorithm>
#include <string>
#include <sstream>
#include <print>
#include <thread>
#include <utility>

#include <windows.h>

namespace st::argparse {
namespace {

constexpr string_view kHelp = "--help";
constexpr string_view kWidth = "--width";
constexpr string_view kHeight = "--height";
constexpr string_view kThreads = "--threads";
constexpr string_view kEntities = "--entities";
constexpr string_view kObstacles = "--obstacles";
constexpr string_view kLoopTime = "--loopTime";
constexpr string_view kAlgorithm = "--algorithm";

constexpr int kDefaultEntities = 20;
constexpr PathAlgorithm kDefaultAlgo = PathAlgorithm::AStar;
constexpr std::chrono::milliseconds kDefaultLoopTime(20);

auto GetTerminalSize() -> Size {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return Size{0, 0};
    }
    return Size{csbi.dwSize.X - 10, csbi.dwSize.Y - 10};
}

int CalculateDefaultObstacles(const Size &monitor_size) { return monitor_size.width * monitor_size.height / 10; }

}  // namespace

template <typename T>
void PrintlnOption(string_view option, string_view description, T _default) {
    std::println("{:<15}{:<40}{}", option, description, _default);
}

void PrintlnOption(string_view option, string_view description) { std::println("{:<15}{}", option, description); }

template <typename T>
void PrintlnOption(string_view option, string_view description, std::vector<std::pair<string_view, T>> selection) {
    std::println("{:<15}{:<40}", option, description);
    for (auto &[desc, value] : selection) {
        std::println("{:>15}{} = {}", "", desc, value);
    }
    std::println("");
}

void PrintHelpMessageAndExit() {
    const auto monitor_size = GetTerminalSize();
    std::println("Help");
    std::println("");
    std::println("{:<15}{:<40}{}", "Option", "Description", "Default");
    std::println("");
    PrintlnOption(kHelp, "Print this help message");
    PrintlnOption(kWidth, "Monitor width", monitor_size.width);
    PrintlnOption(kHeight, "Monitor height", monitor_size.height);
    PrintlnOption(kEntities, "Number of entities to spawn", kDefaultEntities);
    PrintlnOption(kObstacles, "Number of obstacles to spawn", CalculateDefaultObstacles(monitor_size));
    PrintlnOption(kThreads, "Threads available for pool", std::thread::hardware_concurrency());
    PrintlnOption(kLoopTime, "Main loop sleep time", kDefaultLoopTime);
    PrintlnOption(
        kAlgorithm, "Algorithm to use for path finding",
        std::vector{
            std::make_pair(PathAlgorithmToString(PathAlgorithm::Greedy), std::to_underlying(PathAlgorithm::Greedy)),
            std::make_pair(PathAlgorithmToString(PathAlgorithm::AStar), std::to_underlying(PathAlgorithm::AStar))});
    std::exit(0);
}

auto ParseArguments(int argc, char *argv[]) -> Arguments {
    CLI cli(argc, argv);
    Arguments args;
    args.monitor_size = GetTerminalSize();
    args.thread_count = std::thread::hardware_concurrency();
    args.algorithm = kDefaultAlgo;
    args.loop_time = kDefaultLoopTime;
    args.num_entities = kDefaultEntities;

    if (cli.Contains(kHelp)) {
        PrintHelpMessageAndExit();
    }

    cli.VisitIfContains<int>(kWidth, [&args](int val) { args.monitor_size.width = val; });
    cli.VisitIfContains<int>(kHeight, [&args](int val) { args.monitor_size.height = val; });
    // Set the default here as we now have our monitor size set
    args.num_obstacles = CalculateDefaultObstacles(args.monitor_size);

    cli.VisitIfContains<int>(kEntities, [&args](int val) { args.num_entities = val; });
    cli.VisitIfContains<int>(kObstacles, [&args](int val) { args.num_obstacles = val; });
    cli.VisitIfContains<int>(kThreads, [&args](int val) { args.thread_count = val; });
    cli.VisitIfContains<int>(kLoopTime, [&args](int val) { args.loop_time = std::chrono::milliseconds(val); });
    cli.VisitIfContains<int>(kAlgorithm, [&args](int val) {
        // NOTE: For now best solution i could come up with if Algorithm order changes this will be UB
        if (val < std::to_underlying(PathAlgorithm::Greedy) || val > std::to_underlying(PathAlgorithm::AStar)) {
            std::println("[Argparse] Unknown algorithm: {} !", val);
            PrintHelpMessageAndExit();
        }
        args.algorithm = static_cast<PathAlgorithm>(val);
    });
    return args;
}

}  // namespace st::argparse
