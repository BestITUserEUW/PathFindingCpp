#include <random>
#include <thread>
#include <format>
#include <print>
#include <future>
#include <signal.h>
#include <ranges>

#include "monitor.h"
#include "entity.h"
#include "obstacle.h"
#include "path_finding.h"
#include "mission_runner.h"
#include "thread_pool.h"
#include "argparse.h"
#include "windows_helpers.h"
#include "profiler.h"
#include "types.h"

using namespace st;

struct Stack {
    /* data */
};

auto CreateRandPoint(Size size) -> Point {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> xgen(0, size.width - 1);
    std::uniform_int_distribution<int> ygen(0, size.height - 1);
    return Point{xgen(rng), ygen(rng)};
}

auto GenerateObstacles(const Size &bounds, size_t num) -> PointVec {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> xgen(0, bounds.width - 1);
    std::uniform_int_distribution<int> ygen(0, bounds.height - 1);
    PointVec points;
    points.reserve(num);
    std::generate_n(std::back_inserter(points), num, [&] { return Point{xgen(rng), ygen(rng)}; });
    return points;
}

auto GenerateEntities(const Size &bounds, size_t num) {
    std::vector<Entity> entities;
    entities.reserve(num);
    std::generate_n(std::back_inserter(entities), num, [&] { return Entity::Create(CreateRandPoint(bounds)); });
    return entities;
}

void MainLoop(std::stop_token stoken, std::unique_ptr<argparse::Args> args) {
    BS::thread_pool pool{static_cast<unsigned int>(args->thread_count)};
    Monitor monitor{args->monitor_size};
    std::vector<MissionRunner> runners;
    std::vector<std::pair<std::future<PointVec>, Entity *>> pending;

    auto entities = GenerateEntities(monitor.size(), args->num_entities);
    auto obstacles = GenerateObstacles(monitor.size(), args->num_obstacles);
    runners.reserve(entities.size());
    pending.reserve(entities.size());

    for (auto &entity : entities) {
        runners.push_back(MissionRunner(&entity));
    }

    for (const auto &obstacle : obstacles) {
        monitor.SetPixel(obstacle, Obstacle::shape);
    }

    const std::chrono::milliseconds loop_time{args->loop_time};
    const PathAlgorithm algo{args->algorithm};
    args.reset();

    monitor.SetTitle("Mission Path Finding Simulation 9000");
    monitor.SetHeader(std::format("Config: Loop time: {}ms Thread Count: {} Obstacles: {} Algorithm: {}",
                                  loop_time.count(), pool.get_thread_count(), obstacles.size(),
                                  PathAlgorithmToString(algo)));

    Profiler profiler{};
    u64 completed_missions{};
    while (!stoken.stop_requested()) {
        profiler.Start();
        std::erase_if(runners,
                      [&pending, &pool, &obstacles, &completed_missions, algo, size = monitor.size()](auto &runner) {
                          if (!runner.IsComplete()) {
                              return false;
                          }
                          auto entity = runner.entity();
                          auto fut = pool.submit_task([&obstacles, algo, pos = entity->pos(), size]() {
                              return FindPath(pos, CreateRandPoint(size), size, obstacles, algo);
                          });
                          pending.push_back({std::move(fut), entity});
                          completed_missions++;
                          return true;
                      });

        std::erase_if(pending, [&runners, &monitor](auto &result) {
            if (result.first.wait_for(std::chrono::seconds(0)) == std::future_status::timeout) {
                return false;
            }

            MissionRunner runner{result.second};
            runner.AssignMission(result.first.get());
            runner.DrawMission(monitor);
            runners.push_back(std::move(runner));
            return true;
        });

        for (auto &runner : runners) {
            runner.Next();
        }

        for (auto &entity : entities) {
            entity.Draw(monitor);
        }
        monitor.Render();
        profiler.Stop();
        monitor.SetHeader2(std::format(
            "Info: Executing: {:04}/{:04} Finding paths: {:04}/{:04} Completed: {:04} Iter time: {:02}ms avg: {:02}ms",
            runners.size(), entities.size(), pending.size(), entities.size(), completed_missions,
            profiler.GetElapsedMs().count(), profiler.GetAverageMs()));
        std::this_thread::sleep_for(loop_time);
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