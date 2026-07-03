#include "algorithms/algorithms.hpp"
#include "utils.hpp"
#include <iostream>

namespace Algorithms {
    AlgorithmResult nearestNeighbour(const Graph& graph, int startNode, int timeLimitSeconds) {
        AlgorithmResult result;
        result.bestCost = -1;
        result.iterations = 0;

        int n = graph.getNumVertices();
        if (n <= 1) return { {startNode}, 0, 0.0, 1, {} };
        
        bool timeOut = false;
        Utils::Timer timer;
        timer.start();

        vector<int> tour;
        vector<bool> visited(n, false);
        int currentNode = startNode;
        tour.push_back(currentNode);
        visited[currentNode] = true;

        for (int step = 1; step < n; ++step) {
            if (AlgorithmCore::isTimeout(timer, timeLimitSeconds)) {
                timeOut = true;
                break;
            }

            auto [minDistance, bestNextNode] = AlgorithmCore::findNearestNeighbor(graph, currentNode, visited);
            if (bestNextNode == -1) break;

            currentNode = bestNextNode;
            tour.push_back(currentNode);
            visited[currentNode] = true;
        }

        timer.stop();
        result.executionTime = timer.getMicroseconds();

        result.iterations = 1;

        if (timeOut || tour.size() < n) {
            result.bestCost = -1;
            result.bestTour.clear();
            if (timeOut) std::cout << "\n[UWAGA] NN przerwany z powodu limitu czasu!\n";
        } else {
            result.bestTour = tour;
            result.bestCost = Utils::calculateTourCost(tour, graph);
            result.progressHistory.push_back({result.executionTime, result.bestCost});
        }
        return result;
    }
}
