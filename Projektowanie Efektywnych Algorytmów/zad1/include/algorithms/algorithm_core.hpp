#ifndef ALGORITHM_CORE_HPP
#define ALGORITHM_CORE_HPP

#include <vector>
#include <utility>
#include <string>
#include "graph.hpp"
#include "utils.hpp"

using std::vector;
using std::pair;
using std::string;

struct AlgorithmResult {
    vector<int> bestTour;
    int bestCost;
    double executionTime;
    long long iterations;
    vector<pair<double, int>> progressHistory;
};

namespace AlgorithmCore {
    vector<int> createBaseTour(int n, int startNode);
    void evaluateAndUpdate(const Graph& graph, const vector<int>& currentTour, AlgorithmResult& result, Utils::Timer& timer);
    pair<int, int> findNearestNeighbor(const Graph& graph, int currentNode, const std::vector<bool>& visited);
    
    bool isTimeout(Utils::Timer& timer, int timeLimitSeconds);
    
    void printResult(const std::string& name, const AlgorithmResult& res, int maxNodesToPrint = 50);

    bool appendSummaryToCSV(const string& filename, const string& instanceName, const string& algoName, int graphSize, int optimalCost, const AlgorithmResult& result);
    bool appendHistoryToCSV(const string& filename, const string& instanceName, const AlgorithmResult& result);
}

#endif
