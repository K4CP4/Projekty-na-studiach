#include <iostream>
#include <random>
#include <algorithm>
#include "algorithms/algorithms.hpp"

namespace Algorithms {
    AlgorithmResult randomSearch(const Graph& graph, int startNode, int timeLimitSeconds, int optimalCost) {
        AlgorithmResult result;
        result.bestCost = -1;
        result.iterations = 0;

        int n = graph.getNumVertices();
        if (n <= 1) return { {startNode}, 0, 0.0, 1, {} };
        
        std::random_device rd;
        std::mt19937 generator(rd());
        
        bool timeOut = false;
        Utils::Timer timer;
        timer.start();
        
        vector<int> currentTour = AlgorithmCore::createBaseTour(n, startNode);

        while (true) {
            std::shuffle(currentTour.begin() + 1, currentTour.end(), generator);
            
            AlgorithmCore::evaluateAndUpdate(graph, currentTour, result, timer);
            result.iterations++;

            if (optimalCost != -1 && result.bestCost == optimalCost) break;
            if (AlgorithmCore::isTimeout(timer, timeLimitSeconds)) {
                timeOut = true;
                break;
            }
        }

        timer.stop();
        result.executionTime = timer.getMicroseconds();

        if (timeOut) std::cout << "\n[UWAGA] RAND przerwany z powodu limitu czasu.\n";
        return result;
    }
}
