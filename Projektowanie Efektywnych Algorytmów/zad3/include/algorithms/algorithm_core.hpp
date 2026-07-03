#pragma once
#include <vector>
#include "graph.hpp"
#include "utils.hpp"

using std::vector;

namespace Algorithms {
    struct ProgressStep {
        long long iteration;
        long long timestampUs;
        int currentCost;
        int bestCost;
    };
    struct AlgorithmResult {
        vector<int> bestTour;
        int bestCost = -1;
        long long timeTakenMicroseconds = 0; 
        long long nodesEvaluated = 0;
        vector<ProgressStep> history;      
    };
    AlgorithmResult repetitiveNearestNeighbour(const Graph& graph, int startNode, int timeLimit);
}

namespace AlgorithmCore {
    void evaluateAndUpdate(const Graph& graph, const vector<int>& currentTour, Algorithms::AlgorithmResult& result, Utils::Timer& timer);
    std::pair<int, int> findNearestNeighbor(const Graph& graph, int currentNode, const vector<bool>& visited);
    bool isTimeout(Utils::Timer& timer, int timeLimitSeconds);
}
