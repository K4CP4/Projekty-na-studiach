#include <iostream>
#include "utils.hpp"


using std::cout;

namespace Utils {

    int calculateTourCost(const vector<int>& tour, const Graph& graph) {
        if (tour.empty()) return 0;

        int totalCost = 0;
        int numVertices = tour.size();

        for (int i = 0; i < numVertices - 1; ++i) {
            int cost = graph.getDistance(tour[i], tour[i+1]);
            if (cost == -1) {
                cout << "[BLAD] Brak krawedzi miedzy " << tour[i] << " a " << tour[i+1] << "!\n";
                return -1;
            }
            totalCost += cost;
        }

        int returnCost = graph.getDistance(tour.back(), tour.front());
        if (returnCost == -1) {
            cout << "[BLAD] Brak krawedzi powrotnej miedzy " << tour.back() << " a " << tour.front() << "!\n";
            return -1;
        }
        totalCost += returnCost;

        return totalCost;
    }

    Timer::Timer() : running(false) {}

    void Timer::start() {
        startTime = high_resolution_clock::now();
        running = true;
    }

    void Timer::stop() {
        endTime = high_resolution_clock::now();
        running = false;
    }

    double Timer::getMilliseconds() const {
        time_point<high_resolution_clock> end;
        if (running) {
            end = high_resolution_clock::now();
        } else {
            end = endTime;
        }
        duration<double, std::milli> elapsed = end - startTime;
        return elapsed.count();
    }

    double Timer::getMicroseconds() const {
        time_point<high_resolution_clock> end;
        if (running) {
            end = high_resolution_clock::now();
        } else {
            end = endTime;
        }
        duration<double, std::micro> elapsed = end - startTime;
        return elapsed.count();
    }

}
