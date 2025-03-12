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

auto CreateEntitySystem(const Size &bounds, size_t num) -> EntitySystem {
    EntitySystem system;
    system.Reserve(num);
    size_t size = num++;
    for (int i = 0; i < size; i++) {
        system.Create(CreateRandPoint(bounds));
    }
    return system;
}

void DrawObstacles(Drawer *drawer, const std::vector<Point> &obstacles) {
    for (auto &obstacle : obstacles) {
        drawer->SetPixel(obstacle, '#');
    }
}

void MainLoop(std::stop_token stoken, argparse::Arguments args) {
    BS::thread_pool pool{static_cast<unsigned int>(args.thread_count)};
    Monitor monitor{args.monitor_size};
    std::vector<std::pair<Entity, std::future<PointVec>>> pending_missions;

    auto system = CreateEntitySystem(monitor.size(), args.num_entities);
    auto obstacles = CreateObstacles(monitor.size(), args.num_obstacles);

    const std::chrono::milliseconds loop_time{args.loop_time};
    const PathAlgorithm algo{args.algorithm};

    monitor.SetTitle("Mission Path Finding Simulation 9000");
    monitor.SetHeader(std::format("Config: Loop time: {}ms Thread Count: {} Obstacles: {} Algorithm: {}",
                                  loop_time.count(), pool.get_thread_count(), obstacles.size(),
                                  PathAlgorithmToString(algo)));
    Profiler profiler{};
    u64 completed_missions{};
    size_t num_entities = system.NumEntities();

    DrawObstacles(&monitor, obstacles);
    while (!stoken.stop_requested()) {
        profiler.Start();
        auto ids = system.Update();
        for (const auto &id : ids) {
            auto it = std::ranges::find_if(pending_missions, [&system, &id](auto &p) { return p.first == id; });
            if (it != pending_missions.end()) {
                if (it->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                    system.AssignMission(it->first, it->second.get());
                    pending_missions.erase(it);
                }
                continue;
            }

            auto task = [&obstacles, algo, pos = system.View<Position>(id), size = monitor.size()]() {
                return FindPath(pos, CreateRandPoint(size), size, obstacles, algo);
            };
            auto fut = pool.submit_task(std::move(task));
            pending_missions.push_back(std::make_pair(id, std::move(fut)));
            completed_missions++;
        }

        system.Draw(&monitor);
        profiler.Stop();
        auto elapsed = profiler.GetElapsedMs();
        auto info = std::format(
            "Info: Executing: {:04}/{:04} Pending: {:04}/{:04} Completed: {:04} Iter time: {:02}ms avg: {:02}ms",
            num_entities - ids.size(), num_entities, pending_missions.size(), num_entities, completed_missions,
            elapsed.count(), profiler.GetAverageMs());
        monitor.SetHeader2(info);
        monitor.Render();

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

static std::jthread worker;

void CleanUp() {
    if (!worker.joinable()) {
        std::exit(1);
    }

    if (!worker.request_stop()) {
        std::abort();
    }
    worker.join();
}

void SignalHandler(int sig) {
    CleanUp();
    std::println("[SignalHandler] Exiting");
    std::exit(0);
}

int main(int argc, char *argv[]) {
    signal(SIGINT, &SignalHandler);

    auto args = argparse::ParseArguments(argc, argv);
    worker = std::jthread(MainLoop, std::move(args));
    for (;;) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    CleanUp();
    std::println("Exiting");
    return 0;
}