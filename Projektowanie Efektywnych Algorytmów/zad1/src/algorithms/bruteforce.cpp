#include "algorithms/algorithms.hpp"
#include <algorithm>
#include <iostream>

namespace Algorithms {
    AlgorithmResult bruteForce(const Graph& graph, int startNode, int timeLimitSeconds) {
        AlgorithmResult result;
        result.bestCost = -1;
        result.iterations = 0;

        int n = graph.getNumVertices();
        if (n <= 1) return { {startNode}, 0, 0.0, 1, {} };
        
        bool timeOut = false;
        Utils::Timer timer;
        timer.start();

        vector<int> currentTour = AlgorithmCore::createBaseTour(n, startNode);
        
        do {
            AlgorithmCore::evaluateAndUpdate(graph, currentTour, result, timer);
            result.iterations++;

           if (AlgorithmCore::isTimeout(timer, timeLimitSeconds)) {
                timeOut = true;
                break;
            }
        } while (std::next_permutation(currentTour.begin() + 1, currentTour.end()));

        timer.stop();
        result.executionTime = timer.getMicroseconds();

        if (timeOut) std::cout << "\n[UWAGA] Brute-Force przerwany z powodu limitu czasu.\n";
        return result;
    }
}
