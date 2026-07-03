#pragma once
#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>

template <typename T, typename Compare = std::less<T>>
class SafePriorityQueue {
public:
    SafePriorityQueue() = default;

    // dodanie elementu
    void push(const T& item) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (closed) return;    // jeśli kolejka jest zamknięta to nie dodajemy
            queue.push(item);
        }
        cvNotEmpty.notify_one();
    }

    // próba pobrania elementu
    bool try_pop(T& out) {
        std::lock_guard<std::mutex> lock(mtx);
        if (queue.empty()) return false;
        out = queue.top();
        queue.pop();
        return true;
    }

    // pobranie elementu z oczekiwaniem
    bool wait_and_pop(T& out) {
        std::unique_lock<std::mutex> lock(mtx);
        cvNotEmpty.wait(lock, [this] { return closed || !queue.empty(); });  // czekaj aż kolejka nie będzie pusta lub zostanie zamknięta
        if (queue.empty()) return false;
        out = queue.top();
        queue.pop();
        return true;
    }

    // zamknięcie kolejki
    void close() {
        {
            std::lock_guard<std::mutex> lock(mtx);
            closed = true;
        }
        cvNotEmpty.notify_all();
    }

    // sprawdzenie czy kolejka jest pusta
    bool empty() const {
        std::lock_guard<std::mutex> lock(mtx);
        return queue.empty();
    }

    // pobranie rozmiaru kolejki
    size_t size() const {
        std::lock_guard<std::mutex> lock(mtx);
        return queue.size();
    }

    // sprawdzenie czy kolejka jest zamknięta
    bool isClosed() const {
        std::lock_guard<std::mutex> lock(mtx);
        return closed;
    }

private:
    std::priority_queue<T, std::vector<T>, Compare> queue;     // wewnętrzna kolejka priorytetowa
    mutable std::mutex mtx;                // mutex do synchronizacji dostępu
    std::condition_variable cvNotEmpty;    // warunek do powiadamiania o dostępności zasobów
    bool closed = false;                   // flaga zamknięcia kolejki
};
