#pragma once
#include <vector>
#include <chrono>

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::time_point;

class Graph;

namespace Utils {
    class Timer {
        time_point<high_resolution_clock> startTime;
        time_point<high_resolution_clock> stopTime;
        bool running = false;
    public:
        void start();
        void stop();
        long long getMicroseconds();
    };
    int calculateTourCost(const std::vector<int>& tour, const Graph& graph);
}
