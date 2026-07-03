#include "algorithms/algorithm_core.hpp"
#include "utils.hpp"

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
                bestResult.nodesEvaluated++;
                AlgorithmCore::evaluateAndUpdate(graph, currentTour, bestResult, timer);
                return;
            }

            auto [minDistance, bestNextNode] = AlgorithmCore::findNearestNeighbor(graph, currentNode, visited);
            if (minDistance == -1 || bestNextNode == -1) return;

            // pierwszy najlepszy sąsiad, bez rozpatrywania remisów
            currentTour.push_back(bestNextNode);
            visited[bestNextNode] = true;
            
            rnnHelper(graph, bestNextNode, visited, currentTour, bestResult, timer, timeLimitSeconds, timeOut);
            
            currentTour.pop_back();
            visited[bestNextNode] = false;
        }
    }

    AlgorithmResult repetitiveNearestNeighbour(const Graph& graph, int startNode, int timeLimitSeconds) {
        AlgorithmResult result;
        result.bestCost = -1;
        result.nodesEvaluated = 0;

        int n = graph.getNumVertices();
        if (n <= 1) return { {startNode}, -1, 0, 1 };

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
        result.timeTakenMicroseconds = timer.getMicroseconds();
        return result;
    }
}
