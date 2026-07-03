#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <chrono>
#include "graph.hpp"

using std::vector;
using std::chrono::high_resolution_clock;
using std::chrono::time_point;
using std::chrono::duration;

namespace Utils {
    int calculateTourCost(const vector<int>& tour, const Graph& graph);

    class Timer {
    private:
        time_point<high_resolution_clock> startTime;
        time_point<high_resolution_clock> endTime;
        bool running;

    public:
        Timer();
        void start();
        void stop();
        
        double getMilliseconds() const;
        double getMicroseconds() const;
    };

}

#endif
