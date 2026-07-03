#include <iostream>
#include "algorithms/algorithm_core.hpp"

namespace AlgorithmCore {
    void evaluateAndUpdate(const Graph& graph, const std::vector<int>& currentTour, Algorithms::AlgorithmResult& result, Utils::Timer& timer) {
        int currentCost = Utils::calculateTourCost(currentTour, graph);
        if (currentCost != -1) {
            if (result.bestCost == -1 || currentCost < result.bestCost) {
                result.bestCost = currentCost;
                result.bestTour = currentTour;
            }
        }
    }

    std::pair<int, int> findNearestNeighbor(const Graph& graph, int currentNode, const std::vector<bool>& visited) {
        int minDistance = -1;
        int bestNextNode = -1;
        int n = graph.getNumVertices();

        for (int neighbor = 0; neighbor < n; ++neighbor) {
            if (!visited[neighbor]) {
                int dist = graph.getDistance(currentNode, neighbor);
                if (dist != -1 && (minDistance == -1 || dist < minDistance)) {
                    minDistance = dist;
                    bestNextNode = neighbor;
                }
            }
        }
        return {minDistance, bestNextNode};
    }

    bool isTimeout(Utils::Timer& timer, int timeLimitSeconds) {
        return (timer.getMicroseconds() / 1000000.0) >= timeLimitSeconds;
    }
}
