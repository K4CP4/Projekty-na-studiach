#pragma once

#include "graph.hpp"
#include "config_reader.hpp"
#include "algorithms/algorithm_core.hpp"
#include <vector>
#include <string>
#include <random>

using std::vector;
using std::string;

namespace Algorithms {

    class GeneticAlgorithm {
    public:
        static AlgorithmResult solve(const Graph& graph, const Config& config, const vector<int>& initialTour, int initialCost);

    private:
        struct Individual {
            vector<int> tour;
            int cost;
            bool operator<(const Individual& other) const {
                return cost < other.cost;
            }
        };

        static vector<Individual> initializePopulation(int popSize, const vector<int>& initialTour, int initialCost, const Graph& graph);
        static Individual tournamentSelection(const vector<Individual>& population, int tournamentSize, std::mt19937& gen);
        static vector<int> orderCrossover(const vector<int>& parent1, const vector<int>& parent2, std::mt19937& gen);
        static void mutate(vector<int>& tour, const string& method, double mutationRate, std::mt19937& gen);
    };
}
