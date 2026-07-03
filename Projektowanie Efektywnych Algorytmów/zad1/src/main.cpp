#include <iostream>
#include "config_reader.hpp"
#include "graph.hpp"
#include "algorithms/algorithms.hpp"

using std::string;
using std::cout;

bool test_mode = false;

int main() {
    cout << "Start programu.\n";

    Config config = ConfigReader::readConfig("config.txt");
    if (test_mode) ConfigReader::printConfig(config);

    Graph graph;
    
    if (config.generateNew) {
        graph.generateRandomGraph(config.graphSize, config.graphType == 1);
        graph.saveGraphToFile(config.graphPath);
    } else {
        string path = config.graphPath;
        
        if (path.length() >= 4 && (path.substr(path.length() - 4) == ".tsp" || path.substr(path.length() - 5) == ".atsp")) {
            if (!graph.loadGraphFromTSPLIB(path)) {
                cout << "[BLAD] Nie udalo sie wczytac pliku TSPLIB: " << path << "\n";
                return 1;
            }
        } else {
            if (!graph.loadGraphFromFile(path)) {
                cout << "[BLAD] Nie udalo sie wczytac grafu: " << path << "\n";
                return 1;
            }
        }
    }

    if (test_mode && config.graphSize <= 15) graph.printGraph();

    int startNode = config.startNode;

    auto runWithRepetitions = [&](const string& name, auto algorithmFunc) {
        AlgorithmResult overallBest;
        overallBest.bestCost = -1;
        double totalTime = 0.0;
        long long totalIterations = 0;
        long long totalCost = 0;
        int validRuns = 0;

        for (int i = 1; i <= config.repetitions; ++i) {
            cout << "\nUruchomiono algorytm: " << name << " (" << i << ")\n";
            
            // uruchomienie algorytmu
            AlgorithmResult res = algorithmFunc();
            
            totalTime += res.executionTime;
            totalIterations += res.iterations;

            if (res.bestCost != -1) {
                totalCost += res.bestCost;
                validRuns++;

                if (overallBest.bestCost == -1 || res.bestCost < overallBest.bestCost) {
                    overallBest = res;
                }
            }
        }

        overallBest.executionTime = totalTime / config.repetitions;
        overallBest.iterations = totalIterations / config.repetitions;

        AlgorithmCore::printResult(name, overallBest, 30);

        if (config.saveToCSV) {
            AlgorithmCore::appendSummaryToCSV("results/all_results.csv", graph.getInstanceName(), name, graph.getNumVertices(), config.optimalCost, overallBest);

            if (name == "RAND") {
                string historyFilename = "results/history_" + name + ".csv";
                AlgorithmCore::appendHistoryToCSV(historyFilename, graph.getInstanceName(), overallBest);
            }
            
            cout << "\n[INFO] Zapisano pomiary do plikow CSV.\n";
        }
    };

    if (config.algorithm == "RAND" || config.algorithm == "ALL" || config.algorithm == "HEURISTICS") {
        runWithRepetitions("RAND", [&]() {
            return Algorithms::randomSearch(graph, startNode, config.timeLimitSeconds, config.optimalCost);
        });
    }

    if (config.algorithm == "NN" || config.algorithm == "ALL" || config.algorithm == "HEURISTICS" || config.algorithm == "GREEDY") {
        runWithRepetitions("NN", [&]() {
            return Algorithms::nearestNeighbour(graph, startNode, config.timeLimitSeconds);
        });
    }

    if (config.algorithm == "RNN" || config.algorithm == "ALL" || config.algorithm == "HEURISTICS" || config.algorithm == "GREEDY") {
        runWithRepetitions("RNN", [&]() {
            return Algorithms::repetitiveNearestNeighbour(graph, startNode, config.timeLimitSeconds);
        });
    }

    if (config.algorithm == "BRUTE_FORCE" || config.algorithm == "ALL") {
        runWithRepetitions("BRUTE-FORCE", [&]() {
            return Algorithms::bruteForce(graph, startNode, config.timeLimitSeconds);
        });
    }

    cout << "\nKoniec programu.\n";
    return 0;
}
