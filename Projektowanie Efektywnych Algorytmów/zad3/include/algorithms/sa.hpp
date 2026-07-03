#pragma once
#include <vector>
#include "graph.hpp"
#include "config_reader.hpp"
#include "algorithm_core.hpp"
#include "utils.hpp"

namespace Algorithms {
    class SimulatedAnnealing {
    public:
        static AlgorithmResult solve(const Graph& graph, const Config& config, const std::vector<int>& initialTour, int initialCost);
    private:
        static void generateNeighborSwap(std::vector<int>& tour, int i, int j);
        static void generateNeighborInsert(std::vector<int>& tour, int i, int j);
        static void generateNeighborInvert(std::vector<int>& tour, int i, int j);
    };
}
