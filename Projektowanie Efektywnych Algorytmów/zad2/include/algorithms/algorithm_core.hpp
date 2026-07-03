#pragma once
#include <vector>
#include <string>
#include "graph.hpp"
#include "utils.hpp"

using std::vector;
using std::string;
using std::pair;

namespace Algorithms {
    struct AlgorithmResult {
        int bestCost = -1;
        vector<int> bestTour;
        long long executionTime = 0;
        long long iterations = 0;
        vector<pair<long long, int>> progressHistory;
        long long maxMemoryNodes = 0;
    };
    AlgorithmResult repetitiveNearestNeighbour(const Graph& graph, int startNode, int timeLimitSeconds);
}

namespace AlgorithmCore {
    vector<int> createBaseTour(int n, int startNode);
    void evaluateAndUpdate(const Graph& graph, const vector<int>& currentTour, Algorithms::AlgorithmResult& result, Utils::Timer& timer);
    pair<int, int> findNearestNeighbor(const Graph& graph, int currentNode, const vector<bool>& visited);
    bool isTimeout(Utils::Timer& timer, int timeLimitSeconds);
    void printResult(const string& name, const Algorithms::AlgorithmResult& res, int maxNodesToPrint = 20);
    bool appendSummaryToCSV(const string& filename, const string& instanceName, const string& method, int graphSize, bool usedUB, const Algorithms::AlgorithmResult& result);
}
