#include <iostream>
#include <fstream>
#include "algorithms/algorithm_core.hpp"

using std::cout;

namespace AlgorithmCore {
    vector<int> createBaseTour(int n, int startNode) {
        vector<int> tour;
        tour.push_back(startNode);
        for (int i = 0; i < n; ++i) if (i != startNode) tour.push_back(i);
        return tour;
    }

    void evaluateAndUpdate(const Graph& graph, const vector<int>& currentTour, Algorithms::AlgorithmResult& result, Utils::Timer& timer) {
        int currentCost = Utils::calculateTourCost(currentTour, graph);
        if (currentCost != -1) {
            if (result.bestCost == -1 || currentCost < result.bestCost) {
                result.bestCost = currentCost;
                result.bestTour = currentTour;
                result.progressHistory.push_back({timer.getMicroseconds(), currentCost});
            }
        }
    }

    pair<int, int> findNearestNeighbor(const Graph& graph, int currentNode, const vector<bool>& visited) {
        int minDistance = -1;
        int bestNextNode = -1;
        int n = graph.getNumVertices();

        for (int neighbor = 0; neighbor < n; ++neighbor) {
            if (!visited[neighbor]) {
                int dist = graph.getDistance(currentNode, neighbor);
                if (dist != -1 && (minDistance == -1 || dist < minDistance)) {
                    minDistance = dist;
                    bestNextNode = neighbor;
                }
            }
        }
        return {minDistance, bestNextNode};
    }

    bool isTimeout(Utils::Timer& timer, int timeLimitSeconds) {
        return (timer.getMicroseconds() / 1000000.0 >= timeLimitSeconds);
    }

    void printResult(const string& name, const Algorithms::AlgorithmResult& res, int maxNodesToPrint) {
        cout << "\n--- WYNIKI " << name << " ---\n";
        if (res.bestCost == -1) {
            cout << "[BLAD] Brak znalezionej trasy (lub uplynal czas przed znalezieniem pierwszej)\n";
            return;
        }
        cout << "Najlepszy koszt: " << res.bestCost << "\n";
        cout << "Czas dzialania: " << res.executionTime << " us\n";
        cout << "Sprawdzonych wezlow/tras: " << res.iterations << "\n";
        if(res.maxMemoryNodes > 0) cout << "Zloz. pamieciowa (max wezlow): " << res.maxMemoryNodes << "\n";
        
        cout << "Trasa: ";
        if (res.bestTour.size() <= maxNodesToPrint) {
            for (int city : res.bestTour) cout << city << " -> ";
            cout << res.bestTour.front() << "\n";
        } else cout << "[Ukryto ze wzgledu na rozmiar grafu]\n";
    }

    bool appendSummaryToCSV(const string& filename, const string& instanceName, const string& method, int graphSize, bool usedUB, const Algorithms::AlgorithmResult& result) {
        std::ifstream checkFile(filename);
        bool fileExists = checkFile.good();
        checkFile.close();

        std::ofstream file(filename, std::ios::app);
        if (!file.is_open()) return false;

        if (!fileExists) file << "Instancja;Metoda;Rozmiar_Grafu;Czas[us];Najlepszy_Koszt;Uzyto_UB;Max_Wezlow_w_Pamieci;Sprawdzonych_Wezlow\n";
        file << instanceName << ";" 
             << method << ";" 
             << graphSize << ";" 
             << result.executionTime << ";" 
             << result.bestCost << ";" 
             << (usedUB ? "TAK" : "NIE") << ";"
             << result.maxMemoryNodes << ";"
             << result.iterations << "\n";

        file.close();
        return true;
    }
}
