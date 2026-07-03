#include "algorithms/ga.hpp"
#include "utils.hpp"
#include <random>
#include <algorithm>
#include <set>

namespace Algorithms {
    vector<int> GeneticAlgorithm::orderCrossover(const vector<int>& parent1, const vector<int>& parent2, std::mt19937& gen) {
        int n = parent1.size();
        vector<int> child(n, -1);
        std::uniform_int_distribution<> dist(0, n - 1);

        int start = dist(gen);
        int end = dist(gen);
        if (start > end) std::swap(start, end);

        std::set<int> usedCities;
        for (int i = start; i <= end; ++i) {
            child[i] = parent1[i];
            usedCities.insert(parent1[i]);
        }

        int currentIndex = (end + 1) % n;
        for (int i = 0; i < n; ++i) {
            int cityIndex = (end + 1 + i) % n;
            int city = parent2[cityIndex];

            if (usedCities.find(city) == usedCities.end()) {
                child[currentIndex] = city;
                currentIndex = (currentIndex + 1) % n;
            }
        }
        return child;
    }

    void GeneticAlgorithm::mutate(vector<int>& tour, const string& method, double mutationRate, std::mt19937& gen) {
        std::uniform_real_distribution<> distProb(0.0, 1.0);
        if (distProb(gen) > mutationRate) return;

        int n = tour.size();
        std::uniform_int_distribution<> distNode(0, n - 1);
        int i = distNode(gen);
        int j = distNode(gen);
        if (i == j) return;

        if (method == "swap") {
            std::swap(tour[i], tour[j]);
        } else if (method == "insert") {
            int city = tour[i];
            tour.erase(tour.begin() + i);
            if (j > i) j--; 
            tour.insert(tour.begin() + j, city);
        } else if (method == "invert") {
            if (i > j) std::swap(i, j);
            std::reverse(tour.begin() + i, tour.begin() + j + 1);
        }
    }

    GeneticAlgorithm::Individual GeneticAlgorithm::tournamentSelection(const vector<Individual>& population, int tournamentSize, std::mt19937& gen) {
        std::uniform_int_distribution<> distNode(0, population.size() - 1);
        Individual best = population[distNode(gen)];
        
        for (int i = 1; i < tournamentSize; ++i) {
            Individual competitor = population[distNode(gen)];
            if (competitor.cost < best.cost) {
                best = competitor;
            }
        }
        return best;
    }

    vector<GeneticAlgorithm::Individual> GeneticAlgorithm::initializePopulation(int popSize, const vector<int>& initialTour, int initialCost, const Graph& graph) {
        vector<Individual> population;
        
        if (initialCost != -1 && !initialTour.empty()) {
            population.push_back({initialTour, initialCost});
        }

        while (population.size() < (size_t)popSize) {
            vector<int> randomTour = Utils::generateRandomTour(graph.getNumVertices());
            int cost = Utils::calculateTourCost(randomTour, graph);
            population.push_back({randomTour, cost});
        }
        return population;
    }

    AlgorithmResult GeneticAlgorithm::solve(const Graph& graph, const Config& config, const vector<int>& initialTour, int initialCost) {
        AlgorithmResult result;
        result.bestCost = -1;
        result.nodesEvaluated = 0;

        int n = graph.getNumVertices();
        if (n <= 2) {
            result.bestTour = initialTour;
            result.bestCost = initialCost;
            return result;
        }

        std::random_device rd;
        std::mt19937 gen(rd());

        int popSize = (int)config.populationSize; 
        double mutationRate = config.mutationRate; 
        int generations = config.generations;       
        int tournamentSize = 5;

        Utils::Timer timer;
        timer.start();

        vector<Individual> population = initializePopulation(popSize, initialTour, initialCost, graph);
        
        for (const auto& ind : population) {
            if (result.bestCost == -1 || ind.cost < result.bestCost) {
                result.bestCost = ind.cost;
                result.bestTour = ind.tour;
            }
        }

        int noImprovementCounter = 0;
        int stagnationLimit = generations * 0.2;

        for (int genIdx = 0; genIdx < generations; ++genIdx) {
            if (AlgorithmCore::isTimeout(timer, config.timeLimitSeconds)) break;
            if (config.optimalTourCost > 0 && result.bestCost <= config.optimalTourCost) break;
            if (noImprovementCounter >= stagnationLimit) break;

            vector<Individual> newPopulation;
            newPopulation.push_back({result.bestTour, result.bestCost});

            while (newPopulation.size() < (size_t)popSize) {
                Individual parent1 = tournamentSelection(population, tournamentSize, gen);
                Individual parent2 = tournamentSelection(population, tournamentSize, gen);

                vector<int> childTour = orderCrossover(parent1.tour, parent2.tour, gen);
                mutate(childTour, config.mutationMethod, mutationRate, gen);

                int childCost = Utils::calculateTourCost(childTour, graph);
                newPopulation.push_back({childTour, childCost});
                result.nodesEvaluated++;
            }

            population = newPopulation;

            bool improved = false;
            for (const auto& ind : population) {
                if (ind.cost < result.bestCost) {
                    result.bestCost = ind.cost;
                    result.bestTour = ind.tour;
                    improved = true;
                }
            }

            if (improved) noImprovementCounter = 0;
            else noImprovementCounter++;
            
            if (genIdx % 10 == 0) {
                result.history.push_back({result.nodesEvaluated, timer.getMicroseconds(), result.bestCost, result.bestCost});
            }
        }

        timer.stop();
        result.timeTakenMicroseconds = timer.getMicroseconds();
        return result;
    }
}
