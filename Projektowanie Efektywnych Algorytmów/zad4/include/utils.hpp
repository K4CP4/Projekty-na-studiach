#pragma once
#include <string>
#include <vector>
#include <chrono>
#include "config_reader.hpp"

using std::vector;
using std::string;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::time_point;

class Graph;

namespace Algorithms {
    struct AlgorithmResult;
}

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
    int calculateTourCost(const vector<int>& tour, const Graph& graph);
    vector<int> generateRandomTour(int numVertices);
    
    void saveToCSV(const string& filename, const string& instance, int n, const Config& cfg, const Algorithms::AlgorithmResult& res);
    void saveDetailsToCSV(const string& filename, const Algorithms::AlgorithmResult& res);
}
