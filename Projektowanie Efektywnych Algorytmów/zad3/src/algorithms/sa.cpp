#include "algorithms/sa.hpp"
#include <cmath>
#include <random>
#include <algorithm>

namespace Algorithms {

    void SimulatedAnnealing::generateNeighborSwap(vector<int>& tour, int i, int j) { std::swap(tour[i], tour[j]); }

    void SimulatedAnnealing::generateNeighborInsert(vector<int>& tour, int i, int j) {
        int city = tour[i];
        tour.erase(tour.begin() + i);
        if (j > i) j--; 
        tour.insert(tour.begin() + j, city);
    }

    void SimulatedAnnealing::generateNeighborInvert(vector<int>& tour, int i, int j) {
        if (i > j) std::swap(i, j);
        std::reverse(tour.begin() + i, tour.begin() + j + 1);
    }

    AlgorithmResult SimulatedAnnealing::solve(const Graph& graph, const Config& config, const vector<int>& initialTour, int initialCost) {
        AlgorithmResult result;
        int n = graph.getNumVertices();

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distNode(0, n - 1);
        std::uniform_real_distribution<> distProb(0.0, 1.0);

        vector<int> currentTour = initialTour;
        int currentCost = initialCost;
        
        result.bestTour = currentTour;
        result.bestCost = currentCost;

        double T = config.startTemperature;
        int k = 1; 
        long long evaluatedNodes = 0;
        
        int noImprovementCounter = 0;
        int stagnationLimit = n * 1000000;

        int sampleRate = config.epochLength;
        int epochLength = config.epochLength;

        Utils::Timer timer;
        timer.start();

        while (timer.getMicroseconds() / 1000000.0 < config.timeLimitSeconds) {
            
            vector<int> neighborTour = currentTour;
            int i = distNode(gen);
            int j = distNode(gen);
            while (i == j) j = distNode(gen); 

            if (config.neighborMethod == "swap") generateNeighborSwap(neighborTour, i, j);
            else if (config.neighborMethod == "insert") generateNeighborInsert(neighborTour, i, j);
            else if (config.neighborMethod == "invert") generateNeighborInvert(neighborTour, i, j);

            int neighborCost = Utils::calculateTourCost(neighborTour, graph);
            evaluatedNodes++;
            noImprovementCounter++; 

            int delta = neighborCost - currentCost;
            
            if (delta < 0 || exp(-delta / T) > distProb(gen)) {
                currentTour = neighborTour;
                currentCost = neighborCost;

                if (currentCost < result.bestCost) {
                    result.bestCost = currentCost;
                    result.bestTour = currentTour;
                    noImprovementCounter = 0; 
                }
            }

            if (evaluatedNodes % sampleRate == 0) {
                result.history.push_back({
                    evaluatedNodes,
                    timer.getMicroseconds(),
                    currentCost,
                    result.bestCost
                });
            }

            if (config.optimalTourCost > 0 && result.bestCost <= config.optimalTourCost) break; 
            if (noImprovementCounter >= stagnationLimit) break; 

            if (evaluatedNodes % epochLength == 0) {
                if (config.coolingSchedule == "geometric") T = T * config.coolingFactor;
                else if (config.coolingSchedule == "logarithmic") T = config.startTemperature / log(1 + k);
                k++;
            }
            
            if (T < 0.0001) break; 
        }

        timer.stop();
        result.timeTakenMicroseconds = timer.getMicroseconds();
        result.nodesEvaluated = evaluatedNodes;

        return result;
    }
}
