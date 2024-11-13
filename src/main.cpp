#include <random>
#include <thread>
#include <format>
#include <print>
#include <future>
#include <signal.h>
#include <ranges>
#include <algorithm>

#include "monitor.h"
#include "entity.h"
#include "path_finding.h"
#include "thread_pool.h"
#include "argparse.h"
#include "profiler.h"
#include "types.h"

using namespace st;

auto CreateRandPoint(Size size) -> Point {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> xgen(0, size.width - 1);
    std::uniform_int_distribution<int> ygen(0, size.height - 1);
    return Point{xgen(rng), ygen(rng)};
}

auto CreateObstacles(const Size &bounds, size_t num) -> PointVec {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> xgen(0, bounds.width - 1);
    std::uniform_int_distribution<int> ygen(0, bounds.height - 1);
    PointVec points;
    points.reserve(num);
    std::generate_n(std::back_inserter(points), num, [&] { return Point{xgen(rng), ygen(rng)}; });
    return points;
}

auto CreateEntityRegistry(const Size &bounds, size_t num) -> EntityRegistry {
    EntityRegistry registry;
    registry.Reserve(num);
    size_t size = num++;
    for (int i = 0; i < size; i++) {
        registry.Create(CreateRandPoint(bounds));
    }
    return registry;
}

void DrawObstacles(Drawer *drawer, const std::vector<Point> &obstacles) {
    for (auto &obstacle : obstacles) {
        drawer->SetPixel(obstacle, '#');
    }
}

void MainLoop(std::stop_token stoken, std::unique_ptr<argparse::Args> args) {
    BS::thread_pool pool{static_cast<unsigned int>(args->thread_count)};
    Monitor monitor{args->monitor_size};
    std::vector<std::pair<EntityID, std::future<PointVec>>> pending;

    auto registry = CreateEntityRegistry(monitor.size(), args->num_entities);
    auto obstacles = CreateObstacles(monitor.size(), args->num_obstacles);

    const std::chrono::milliseconds loop_time{args->loop_time};
    const PathAlgorithm algo{args->algorithm};
    args.reset();

    monitor.SetTitle("Mission Path Finding Simulation 9000");
    monitor.SetHeader(std::format("Config: Loop time: {}ms Thread Count: {} Obstacles: {} Algorithm: {}",
                                  loop_time.count(), pool.get_thread_count(), obstacles.size(),
                                  PathAlgorithmToString(algo)));
    Profiler profiler{};
    u64 completed_missions{};
    size_t num_entities = registry.NumEntities();

    DrawObstacles(&monitor, obstacles);
    while (!stoken.stop_requested()) {
        profiler.Start();
        auto ids = registry.Update();
        for (const auto &id : ids) {
            auto it = std::ranges::find_if(pending, [&registry, &id](auto &p) { return p.first == id; });
            if (it != pending.end()) {
                if (it->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                    registry.AssignMission(it->first, it->second.get());
                    pending.erase(it);
                }
                continue;
            }

            auto task = [&obstacles, algo, pos = registry.GetComponent<Point>(id), size = monitor.size()]() {
                return FindPath(pos, CreateRandPoint(size), size, obstacles, algo);
            };
            auto fut = pool.submit_task(std::move(task));
            pending.push_back(std::make_pair(id, std::move(fut)));
            completed_missions++;
        }

        registry.Draw(&monitor);
        monitor.Render();
        profiler.Stop();
        auto elapsed = profiler.GetElapsedMs();
        monitor.SetHeader2(std::format(
            "Info: Executing: {:04}/{:04} Finding paths: {:04}/{:04} Completed: {:04} Iter time: {:02}ms avg: {:02}ms",
            num_entities - ids.size(), num_entities, pending.size(), num_entities, completed_missions, elapsed.count(),
            profiler.GetAverageMs()));

        if (elapsed < loop_time) {
            std::this_thread::sleep_for(loop_time - elapsed);
        }
    }

    std::system("cls");
    std::println("[MainLoop] Stopping crypto miners...");
    pool.purge();
    pool.wait();
    std::println("[MainLoop] Exiting");
}

static std::jthread g_thread;

void CleanUp() {
    if (!g_thread.joinable()) {
        std::exit(1);
    }

    if (!g_thread.request_stop()) {
        std::abort();
    }
    g_thread.join();
}

void SignalHandler(int sig) {
    CleanUp();
    std::println("[SignalHandler] Exiting");
    std::exit(0);
}

int main(int argc, char *argv[]) {
    signal(SIGINT, &SignalHandler);

    auto args = argparse::ParseArguments(argc, argv);
    if (!args) {
        argparse::PrintHelpMessage();
        std::exit(1);
    }

    g_thread = std::jthread(MainLoop, std::move(args));
    for (;;) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    CleanUp();
    std::println("Exiting");
    return 0;
}