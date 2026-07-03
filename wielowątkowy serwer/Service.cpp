#include "Service.hpp"

Service::Service(std::string name, SharedResources& shared, int workerCount)
    : serviceName(std::move(name)), sharedResources(shared) {
    workers.reserve(workerCount);
    for (int i = 0; i < workerCount; ++i) {
        workers.emplace_back();
    }
}

Service::~Service() {
    stop();
}

void Service::start() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        running = true;
    }
    for (size_t i = 0; i < workers.size(); ++i) {
        workers[i] = std::thread(&Service::worker_loop, this, static_cast<int>(i));
    }
}

void Service::stop() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (!running) {
            // zatrzymane wcześniej
        } else {
            running = false;
        }
    }
    taskQueue.close();
    for (auto& t : workers) {
        if (t.joinable()) t.join();
    }
}

// dodanie nowego zadania do kolejki
void Service::submit(const Task& task) {
    std::lock_guard<std::mutex> lock(mtx);
    if (running) {
        ++producedCount;
        taskQueue.push(task);
    }
}

int Service::produced_count() const {
    std::lock_guard<std::mutex> lock(mtx);
    return producedCount;
}
int Service::done_count() const {
    std::lock_guard<std::mutex> lock(mtx);
    return doneCount;
}

// oczekiwanie na wykonanie określonej liczby zadań
bool Service::wait_until_done(int expectedDone, std::chrono::milliseconds timeout) const {
    auto deadline = std::chrono::steady_clock::now() + timeout;
    while (std::chrono::steady_clock::now() < deadline) {
        if (done_count() >= expectedDone) return true;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return done_count() >= expectedDone;
}

// funkcja wykonywana przez wątki robocze
void Service::worker_loop(int workerId) {
    std::mt19937 rng(std::random_device{}());           // generator liczb losowych
    std::uniform_int_distribution<int> dist(0, 1);      // 0 - read, 1 - write
    Task task;
    // przetwarzanie zadań
    while (taskQueue.wait_and_pop(task)) {
        sharedResources.logger.log("[" + serviceName + " #" + std::to_string(workerId) + "] Przetwarzam zadanie id=" + std::to_string(task.id) + " priorytet=" + std::to_string(task.priority) + " dane=\"" + task.data + "\"");
        if (dist(rng) == 0) {
            // read
            size_t idx = static_cast<size_t>(task.id % 10);
            int val = sharedResources.data.read(idx);
            sharedResources.logger.log("[" + serviceName + "] odczyt idx " + std::to_string(idx) + " -> " + std::to_string(val));
        } else {
            // write
            size_t idx = static_cast<size_t>(task.id % 10);
            sharedResources.data.write(idx, task.priority);
            sharedResources.logger.log("[" + serviceName + "] zapis idx " + std::to_string(idx) + " = " + std::to_string(task.priority));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));     // symulacja czasu przetwarzania
        {
            std::lock_guard<std::mutex> lock(mtx);
            ++doneCount;
        }
    }
    sharedResources.logger.log("[" + serviceName + " #" + std::to_string(workerId) + "] koncze prace");    // wypis przy wychodzeniu
}
