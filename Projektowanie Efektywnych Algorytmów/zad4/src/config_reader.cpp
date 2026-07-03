#include <fstream>
#include <iostream>
#include <string>
#include "config_reader.hpp"

using std::cout;
using std::string;

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

Config ConfigReader::readConfig(const string& filename) {
    Config config;
    
    // ustawienia domyślne
    config.graphPath = "data/tsplib/tsp/burma14.tsp";
    config.timeLimitSeconds = 60;
    config.repetitions = 1;
    config.useInitialUB = true;
    config.saveToCSV = true;
    config.optimalTourCost = -1.0;
    
    // parametry domyślne
    config.populationSize = 100.0;  
    config.mutationRate = 0.05;      
    config.generations = 1000;        
    config.crossoverMethod = "OX";    
    config.mutationMethod = "swap";   

    std::ifstream file(filename);
    if (!file.is_open()) {
        cout << "\n[BLAD] Nie mozna otworzyc pliku config.txt. Uzywam ustawien domyslnych.\n";
        return config;
    }

    string line;
    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos != string::npos) {
            string key = trim(line.substr(0, pos));
            string value = trim(line.substr(pos + 1));

            if (key == "GRAPH_PATH") config.graphPath = value;
            else if (key == "TIME_LIMIT_SECONDS") config.timeLimitSeconds = std::stoi(value);
            else if (key == "REPETITIONS") config.repetitions = std::stoi(value);
            else if (key == "USE_INITIAL_UB") config.useInitialUB = (value == "true" || value == "1");
            else if (key == "SAVE_TO_CSV") config.saveToCSV = (value == "true" || value == "1");
            else if (key == "OPTIMAL_TOUR_COST") config.optimalTourCost = std::stod(value);
            else if (key == "POPULATION_SIZE") config.populationSize = std::stod(value);
            else if (key == "MUTATION_RATE") config.mutationRate = std::stod(value);
            else if (key == "GENERATIONS") config.generations = std::stoi(value);
            else if (key == "CROSSOVER_METHOD") config.crossoverMethod = value;
            else if (key == "MUTATION_METHOD") config.mutationMethod = value;
        }
    }
    file.close();
    return config;
}

void ConfigReader::printParameters(const Config& config) {
    cout << "\n--- PARAMETRY ALGORYTMU GENETYCZNEGO ---\n";
    cout << "Rozmiar Populacji: " << (int)config.populationSize << "\n";
    cout << "Prawd. Mutacji: " << config.mutationRate * 100 << "%\n";
    cout << "Liczba pokolen: " << config.generations << "\n";
    cout << "Metoda krzyzowania: " << config.crossoverMethod << "\n";
    cout << "Metoda mutacji: " << config.mutationMethod << "\n";
    cout << "----------------------------------------\n";
}
