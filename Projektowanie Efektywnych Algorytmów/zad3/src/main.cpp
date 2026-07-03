#include <iostream>
#include <iomanip>
#include <limits>
#include "graph.hpp"
#include "config_reader.hpp"
#include "algorithms/sa.hpp"
#include "utils.hpp"

using std::cout;

void runExperimentSA(const Graph& graph, const Config& config, const Algorithms::AlgorithmResult& rnn_res) {
    std::vector<int> initialTour;
    int initialCost;

    if (config.useInitialUB && rnn_res.bestCost != -1) {
        initialTour = rnn_res.bestTour;
        initialCost = rnn_res.bestCost;
    } else {
        initialTour = Utils::generateRandomTour(graph.getNumVertices());
        initialCost = Utils::calculateTourCost(initialTour, graph);
    }

    Algorithms::AlgorithmResult overallBest;
    overallBest.bestCost = -1;

    for (int i = 1; i <= config.repetitions; ++i) {
        Algorithms::AlgorithmResult sa_res = Algorithms::SimulatedAnnealing::solve(graph, config, initialTour, initialCost);
        
        if (overallBest.bestCost == -1 || sa_res.bestCost < overallBest.bestCost) overallBest = sa_res;
        
        double relError = (config.optimalTourCost > 0) ? ((double)overallBest.bestCost - config.optimalTourCost) / config.optimalTourCost * 100.0 : 0.0;

        cout << "[SA] " << std::left << std::setw(12) << graph.getInstanceName() 
             << " | N: " << std::setw(4) << graph.getNumVertices()
             << " | Wynik: " << std::setw(8) << overallBest.bestCost
             << " | Blad: " << std::setw(6) << std::fixed << std::setprecision(2) << relError << "%"
             << " | Czas: " << overallBest.timeTakenMicroseconds << " us\n";

        if (config.saveToCSV) {
            Utils::saveToCSV("results/results_" + graph.getInstanceName() + "_raw.csv", graph.getInstanceName(), graph.getNumVertices(), config, sa_res);
            
            std::string detailFilename = "results/histories/history_" + graph.getInstanceName() + "_trial" + std::to_string(i) + ".csv";
            Utils::saveDetailsToCSV(detailFilename, sa_res);
        }

        if (sa_res.timeTakenMicroseconds >= config.timeLimitSeconds * 1000000LL) break;
    }
}

int main() {
    bool testMode = false;

    Config config = ConfigReader::readConfig("config.txt");
    if (testMode) ConfigReader::printConfig(config);
    ConfigReader::printParameters(config);

    Graph graph;
    if (!graph.loadGraphAuto(config.graphPath)) {
        cout << "[BLAD] Nie wczytano grafu: " << config.graphPath << "\n";
        return 1;
    }

    Algorithms::AlgorithmResult rnn_res;
    if (config.useInitialUB) {
        rnn_res = Algorithms::repetitiveNearestNeighbour(graph, 0, config.timeLimitSeconds);
    }

    runExperimentSA(graph, config, rnn_res);

    cout << "\nNacisnij klawisz ENTER, aby zakonczyc...";
    std::cin.get();

    return 0;
}
