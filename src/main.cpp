#include <atomic>
#include <random>
#include <thread>
#include <format>
#include <print>
#include <future>
#include <csignal>
#include <algorithm>

#include <oryx/thread_pool.hpp>
#include <oryx/enchantum.hpp>
#include <oryx/types.hpp>

#include "monitor.hpp"
#include "entity.hpp"
#include "path_finding.hpp"
#include "profiler.hpp"
#include "cmdline.hpp"

using namespace oryx;

static std::atomic_bool stop_requested{};

auto CreateRandPoint(Size size) -> Point {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> xgen(0, size.width - 1);
    std::uniform_int_distribution<int> ygen(0, size.height - 1);
    return Point(xgen(rng), ygen(rng));
}

auto CreateObstacles(const Size &bounds, size_t num) -> PointVec {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> xgen(0, bounds.width - 1);
    std::uniform_int_distribution<int> ygen(0, bounds.height - 1);
    PointVec points;
    points.reserve(num);
    std::generate_n(std::back_inserter(points), num, [&] { return Point(xgen(rng), ygen(rng)); });
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

void MainLoop(const Arguments &args) {
    BS::thread_pool pool{static_cast<unsigned int>(args.thread_count)};
    Monitor monitor{args.monitor_size};
    std::vector<std::pair<Entity, std::future<PointVec>>> pending_missions;

    auto system = CreateEntitySystem(monitor.size(), args.num_entities);
    auto obstacles = CreateObstacles(monitor.size(), args.num_obstacles);

    monitor.SetTitle("Mission Path Finding Simulation 9000");
    monitor.SetHeader(std::format("Config: Loop time: {} Thread Count: {} Obstacles: {} Algorithm: {}", args.loop_time,
                                  pool.get_thread_count(), obstacles.size(), enchantum::to_string(args.algorithm)));
    Profiler profiler{};
    u64 completed_missions{};
    size_t num_entities = system.NumEntities();

    DrawObstacles(&monitor, obstacles);

    std::string info;
    info.reserve(64);
    while (!stop_requested) {
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

            auto task = [&obstacles, algo = args.algorithm, pos = system.View<Position>(id), size = monitor.size()]() {
                return FindPath(pos, CreateRandPoint(size), size, obstacles, algo);
            };
            auto fut = pool.submit_task(std::move(task));
            pending_missions.emplace_back(id, std::move(fut));
            completed_missions++;
        }

        system.Draw(&monitor);
        profiler.Stop();
        auto elapsed = profiler.GetElapsedMs();
        info = std::format(
            "Info: Executing: {:04}/{:04} Pending: {:04}/{:04} Completed: {:04} Iter time: {:04}ms avg: {:04}ms",
            num_entities - ids.size(), num_entities, pending_missions.size(), num_entities, completed_missions,
            elapsed.count(), profiler.GetAverageMs());
        monitor.SetHeader2(info);
        monitor.Render();

        if (elapsed < args.loop_time) {
            std::this_thread::sleep_for(args.loop_time - elapsed);
        }
    }

    std::system("cls");
    std::println("[MainLoop] Cleaning up threads");
    pool.purge();
    pool.wait();
}

auto main(int argc, char *argv[]) -> int {
    signal(SIGINT, [](int) { stop_requested.store(true); });
    const auto args = ParseArguments(argc, argv);
    MainLoop(args);
    std::println("Exiting");
    return 0;
}