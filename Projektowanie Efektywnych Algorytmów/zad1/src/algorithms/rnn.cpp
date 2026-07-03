#include "algorithms/algorithms.hpp"
#include "utils.hpp"
#include <iostream>

namespace Algorithms {
    namespace {
        void rnnHelper(const Graph& graph, int currentNode, vector<bool>& visited, vector<int>& currentTour, AlgorithmResult& bestResult, Utils::Timer& timer, int timeLimitSeconds, bool& timeOut) {
            if (AlgorithmCore::isTimeout(timer, timeLimitSeconds)) {
                timeOut = true;
                return;
            }
            if (timeOut) return;

            int n = graph.getNumVertices();

            if (currentTour.size() == n) {
                bestResult.iterations++;
                AlgorithmCore::evaluateAndUpdate(graph, currentTour, bestResult, timer);
                return;
            }

            auto [minDistance, bestNextNode] = AlgorithmCore::findNearestNeighbor(graph, currentNode, visited);
            if (minDistance == -1) return;

            vector<int> candidates;
            for (int neighbor = 0; neighbor < n; ++neighbor) {
                if (!visited[neighbor] && graph.getDistance(currentNode, neighbor) == minDistance) {
                    candidates.push_back(neighbor);
                }
            }

            for (int nextNode : candidates) {
                currentTour.push_back(nextNode);
                visited[nextNode] = true;
                
                rnnHelper(graph, nextNode, visited, currentTour, bestResult, timer, timeLimitSeconds, timeOut);
                
                currentTour.pop_back();
                visited[nextNode] = false;
            }
        }
    }

    AlgorithmResult repetitiveNearestNeighbour(const Graph& graph, int startNode, int timeLimitSeconds) {
        AlgorithmResult result;
        result.bestCost = -1;
        result.iterations = 0;

        int n = graph.getNumVertices();
        if (n <= 1) return { {startNode}, 0, 0.0, 1, {} };

        bool timeOut = false;
        Utils::Timer timer;
        timer.start();
        
        for (int i = 0; i < n; ++i) {
            if (timeOut) break;

            vector<int> currentTour;
            vector<bool> visited(n, false);
            currentTour.push_back(i);
            visited[i] = true;

            rnnHelper(graph, i, visited, currentTour, result, timer, timeLimitSeconds, timeOut);
        }

        timer.stop();
        result.executionTime = timer.getMicroseconds();

        if (timeOut) std::cout << "\n[UWAGA] RNN przerwany z powodu limitu czasu!\n";
        return result;
    }
}
