#pragma once
#include <vector>
#include <thread>
#include <mutex>
#include <string>
#include <random>
#include <chrono>

#include "Task.hpp"
#include "SafePriorityQueue.hpp"
#include "SharedResources.hpp"

// klasa reprezentująca usługę przetwarzającą zadania
class Service {
public:
    Service(std::string name, SharedResources& shared, int workerCount);
    ~Service();

    void start();
    void stop();

    void submit(const Task& task);      // dodanie zadania do kolejki

    int produced_count() const;         // liczba stworzonych zadań
    int done_count() const;             // liczba wykonanych zadań

    // oczekiwanie na wykonanie określonej liczby zadań (do testów)
    bool wait_until_done(int expectedDone, std::chrono::milliseconds timeout) const;

    const std::string& name() const { return serviceName; }

private:
    void worker_loop(int workerId);     // funkcja wykonywana przez wątki robocze

    std::string serviceName;
    SharedResources& sharedResources;
    SafePriorityQueue<Task, TaskCompare> taskQueue;
    std::vector<std::thread> workers;
    mutable std::mutex mtx;
    int producedCount{0};
    int doneCount{0};
    bool running{false};
};
