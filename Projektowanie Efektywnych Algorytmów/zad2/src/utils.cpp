#include "utils.hpp"
#include "graph.hpp"
#include <iostream>

namespace Utils {
    void Timer::start() {
        startTime = high_resolution_clock::now();
        running = true;
    }

    void Timer::stop() {
        stopTime = high_resolution_clock::now();
        running = false;
    }

    long long Timer::getMicroseconds() {
        if (running) {
            auto currentTime = high_resolution_clock::now();
            return duration_cast<microseconds>(currentTime - startTime).count();
        }
        return duration_cast<microseconds>(stopTime - startTime).count();
    }

    int calculateTourCost(const std::vector<int>& tour, const Graph& graph) {
        if (tour.empty()) return 0;
        int totalCost = 0;
        int numVertices = tour.size();
        for (int i = 0; i < numVertices - 1; ++i) {
            int cost = graph.getDistance(tour[i], tour[i+1]);
            if (cost == -1) return -1;
            totalCost += cost;
        }
        int returnCost = graph.getDistance(tour.back(), tour.front());
        if (returnCost == -1) return -1;
        totalCost += returnCost;
        return totalCost;
    }
}
