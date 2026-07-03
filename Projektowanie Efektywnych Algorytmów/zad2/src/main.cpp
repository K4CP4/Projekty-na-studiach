#include <iostream>
#include "graph.hpp"
#include "config_reader.hpp"
#include "algorithms/algorithm_core.hpp"
#include "algorithms/branch_and_bound.hpp"

using std::cout;
using std::string;

void runExperiment(const Graph& graph, const Config& config, const Algorithms::AlgorithmResult& rnn_res, const string& strategy, bool test_mode) {
    cout << "\nRozpoczecie algorytmu Branch & Bound (metoda: " << strategy << ")\n";
    
    int initialUB = rnn_res.bestCost;

    Algorithms::AlgorithmResult overallBest;
    overallBest.bestCost = -1;
    double totalTime = 0.0;
    long long totalIterations = 0;
    long long totalMemoryNodes = 0;
    
    int reps = config.repetitions > 0 ? config.repetitions : 1; 

    for (int i = 1; i <= reps; ++i) {
        cout << "Proba " << i << "/" << reps << "\n";
        
        Algorithms::BranchAndBound bnb(graph);
        Algorithms::AlgorithmResult bnb_res = bnb.solve(strategy, initialUB, config.timeLimitSeconds);
        
        if (bnb_res.bestCost == -1 && initialUB != -1) {
            bnb_res.bestCost = rnn_res.bestCost;
            bnb_res.bestTour = rnn_res.bestTour;
        }

        totalTime += bnb_res.executionTime;
        totalIterations += bnb_res.iterations;
        totalMemoryNodes += bnb_res.maxMemoryNodes;

        if (bnb_res.bestCost != -1 && (overallBest.bestCost == -1 || bnb_res.bestCost < overallBest.bestCost)) {
            overallBest = bnb_res; 
        }
    }

    if (overallBest.bestCost != -1) {
        overallBest.executionTime = totalTime / reps;
        overallBest.iterations = totalIterations / reps;
        overallBest.maxMemoryNodes = totalMemoryNodes / reps;
        
        AlgorithmCore::printResult("B&B: " + strategy, overallBest, 30);

        if (config.saveToCSV) {
            AlgorithmCore::appendSummaryToCSV("results/results_bb.csv", graph.getInstanceName(), strategy, graph.getNumVertices(), config.useInitialUB, overallBest);
            cout << "[INFO] Zapisano pomiary do pliku results/results_bb.csv.\n";
        }
    } else {
        cout << "\n[BLAD] Algorytm nie znalazl zadnej trasy w zadanym czasie dla metody: " << strategy << "\n";
    }
}

int main() {
    bool test_mode = false;

    Config config = ConfigReader::readConfig("config.txt");
    if (test_mode) ConfigReader::printConfig(config);

    Graph graph;
    if (!graph.loadGraphAuto(config.graphPath)) {
        cout << "[BLAD] Nie udalo sie wczytac pliku: " << config.graphPath << "\n";
        return 1;
    }
    if (test_mode) cout << "\nWczytano graf: " << graph.getInstanceName() << " (Rozmiar: " << graph.getNumVertices() << ")\n";

    Algorithms::AlgorithmResult rnn_res;
    rnn_res.bestCost = -1;

    if (config.useInitialUB) {
        cout << "\nWyznaczanie poczatkowego UB za pomoca RNN\n";
        rnn_res = Algorithms::repetitiveNearestNeighbour(graph, 0, config.timeLimitSeconds);
        cout << "Znalezione UB: " << rnn_res.bestCost << "\n";
    }

    if (config.method == "BFS" || config.method == "ALL") runExperiment(graph, config, rnn_res, "BFS", test_mode);
    if (config.method == "DFS" || config.method == "ALL") runExperiment(graph, config, rnn_res, "DFS", test_mode);
    if (config.method == "LC"  || config.method == "ALL") runExperiment(graph, config, rnn_res, "LC", test_mode);

    cout << "\nKoniec programu.\n";
    return 0;
}
