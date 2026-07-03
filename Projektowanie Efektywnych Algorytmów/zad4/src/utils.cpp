#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <random>
#include "utils.hpp"
#include "graph.hpp"
#include "config_reader.hpp"
#include "algorithms/algorithm_core.hpp"

namespace Utils {
    void Timer::start() {
        startTime = high_resolution_clock::now();
        running = true;
    }

    void Timer::stop() {
        stopTime = high_resolution_clock::now();
        running = false;
    }

    long long Timer::getMicroseconds() {
        if (running) {
            auto currentTime = high_resolution_clock::now();
            return duration_cast<microseconds>(currentTime - startTime).count();
        }
        return duration_cast<microseconds>(stopTime - startTime).count();
    }

    int calculateTourCost(const vector<int>& tour, const Graph& graph) {
        if (tour.empty()) return 0;
        int totalCost = 0;
        int numVertices = tour.size();
        for (int i = 0; i < numVertices - 1; ++i) {
            int cost = graph.getDistance(tour[i], tour[i+1]);
            if (cost == -1) return -1;
            totalCost += cost;
        }
        int returnCost = graph.getDistance(tour.back(), tour.front());
        if (returnCost == -1) return -1;
        totalCost += returnCost;
        return totalCost;
    }

    vector<int> generateRandomTour(int numVertices) {
        vector<int> tour(numVertices);
        std::iota(tour.begin(), tour.end(), 0);
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(tour.begin(), tour.end(), g);
        return tour;
    }

    void saveToCSV(const string& filename, const string& instance, int n, const Config& cfg, const Algorithms::AlgorithmResult& res) {
        std::ifstream check(filename);
        bool exists = check.good();
        check.close();

        std::ofstream file(filename, std::ios::app);
        
        if (!exists) file << "Instancja;N;Metoda_Mutacji;Prawd_Mutacji[%];Rozmiar_Populacji;Liczba_Pokolen;RNN_Start;Czas[us];Najlepszy_Koszt;Optimum;Blad_Wzgledny[%];Wygenerowanych_Osobnikow\n";

        double error = (cfg.optimalTourCost > 0) ? ((double)res.bestCost - cfg.optimalTourCost) / cfg.optimalTourCost * 100.0 : 0.0;

        file << instance << ";"
             << n << ";"
             << cfg.mutationMethod << ";"
             << cfg.mutationRate * 100 << ";"
             << (int)cfg.populationSize << ";"
             << cfg.generations << ";"
             << (cfg.useInitialUB ? "TAK" : "NIE") << ";"
             << res.timeTakenMicroseconds << ";"
             << res.bestCost << ";"
             << (cfg.optimalTourCost > 0 ? std::to_string((int)cfg.optimalTourCost) : "brak") << ";"
             << std::to_string(error) << ";"
             << res.nodesEvaluated << "\n";

        file.close();
    }

    void saveDetailsToCSV(const string& filename, const Algorithms::AlgorithmResult& res) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Blad: Nie udalo sie otworzyc pliku " << filename << "\n";
            return;
        }
        file << "Iteracja;Czas[us];Obecny_Koszt;Najlepszy_Koszt\n";
        for (const auto& point : res.history) {
            file << point.iteration << ";" << point.timestampUs << ";" 
                 << point.currentCost << ";" << point.bestCost << "\n";
        }
        file.close();
    }
}
