#pragma once
#include <vector>
#include <string>
#include "algorithms/algorithm_core.hpp"
#include "graph.hpp"

namespace Algorithms {
    struct Node {
        std::vector<int> path;
        std::vector<std::vector<int>> matrix;
        int currentCity;
        int lowerBound;
        int level;
        Node() : currentCity(-1), lowerBound(0), level(0) {}
        bool operator>(const Node& other) const {
            return lowerBound > other.lowerBound;
        }
    };

    class BranchAndBound {
        const Graph& originalGraph;
        static const int INF = 2147483647; 
        int reduceMatrix(std::vector<std::vector<int>>& matrix);
        Node createChildNode(const Node& parent, int toCity);
    public:
        BranchAndBound(const Graph& graph);
        AlgorithmResult solve(const std::string& strategy, int initialUB, int timeLimitSeconds);
    };
}
