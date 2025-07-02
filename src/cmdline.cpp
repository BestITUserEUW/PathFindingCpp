#include "cmdline.hpp"

#include <print>
#include <thread>
#include <utility>

#include <windows.h>

#include <oryx/argparse.hpp>
#include <oryx/enchantum.hpp>

#include "path_finding.hpp"

namespace oryx {
namespace {

constexpr std::string_view kHelp = "--help";
constexpr std::string_view kWidth = "--width";
constexpr std::string_view kHeight = "--height";
constexpr std::string_view kThreads = "--threads";
constexpr std::string_view kEntities = "--entities";
constexpr std::string_view kObstacles = "--obstacles";
constexpr std::string_view kLoopTime = "--loopTime";
constexpr std::string_view kAlgorithm = "--algorithm";

constexpr int kDefaultEntities = 20;
constexpr PathAlgorithm kDefaultAlgo = PathAlgorithm::AStar;
constexpr std::chrono::milliseconds kDefaultLoopTime(20);

auto GetTerminalSize() -> Size {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return {};
    }

    const auto x = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    const auto y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    return Size(x - 3, y - 6);
}

auto CalculateDefaultObstacles(const Size &monitor_size) { return monitor_size.width * monitor_size.height / 10; }

}  // namespace

template <typename T>
void PrintlnOption(std::string_view option, std::string_view description, T _default) {
    std::println("{:<15}{:<40}{}", option, description, _default);
}

void PrintlnOption(std::string_view option, std::string_view description) {
    std::println("{:<15}{}", option, description);
}

template <typename T>
void PrintlnOption(std::string_view option,
                   std::string_view description,
                   std::vector<std::pair<std::string_view, T>> selection) {
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
            std::make_pair(enchantum::to_string(PathAlgorithm::Greedy), std::to_underlying(PathAlgorithm::Greedy)),
            std::make_pair(enchantum::to_string(PathAlgorithm::AStar), std::to_underlying(PathAlgorithm::AStar))});
    std::exit(0);
}

auto ParseArguments(int argc, char *argv[]) -> Arguments {
    argparse::CLI cli(argc, argv);
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
        auto algorithm = enchantum::cast<PathAlgorithm>(val);
        if (!algorithm) {
            std::println("[Argparse] Unknown algorithm: {} !", val);
            PrintHelpMessageAndExit();
        }

        args.algorithm = *algorithm;
    });
    return args;
}

}  // namespace oryx
