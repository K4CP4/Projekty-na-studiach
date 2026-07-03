#ifndef ALGORITHMS_HPP
#define ALGORITHMS_HPP

#include "algorithms/algorithm_core.hpp"
#include "graph.hpp"

namespace Algorithms {
    AlgorithmResult randomSearch(const Graph& graph, int startNode, int timeLimitSeconds, int optimalCost);
    AlgorithmResult nearestNeighbour(const Graph& graph, int startNode, int timeLimitSeconds);
    AlgorithmResult repetitiveNearestNeighbour(const Graph& graph, int startNode, int timeLimitSeconds);
    AlgorithmResult bruteForce(const Graph& graph, int startNode, int timeLimitSeconds);
}

#endif
