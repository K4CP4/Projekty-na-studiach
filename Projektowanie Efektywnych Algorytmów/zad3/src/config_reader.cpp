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
    
    // parametry domyślne dla SA
    config.startTemperature = 10000.0;
    config.coolingFactor = 0.999;
    config.epochLength = 100;
    config.coolingSchedule = "geometric";
    config.neighborMethod = "swap";

    std::ifstream file(filename);
    if (!file.is_open()) {
        cout << "\n[BLAD] Nie mozna otworzyc pliku konfiguracyjnego: " << filename << "\n";
        return config;
    }

    string line;
    while (std::getline(file, line)) {
        string trimmedLine = trim(line);
        
        if (trimmedLine.empty() || trimmedLine[0] == '#') {
            continue;
        }

        size_t equalPosition = trimmedLine.find('=');
        if (equalPosition != string::npos) {
            string key = trim(trimmedLine.substr(0, equalPosition));
            string value = trim(trimmedLine.substr(equalPosition + 1));
            
            if (key == "GRAPH_PATH") config.graphPath = value;
            else if (key == "TIME_LIMIT_SECONDS") config.timeLimitSeconds = std::stoi(value);
            else if (key == "REPETITIONS") config.repetitions = std::stoi(value);
            else if (key == "USE_INITIAL_UB") config.useInitialUB = (value == "true" || value == "1");
            else if (key == "SAVE_TO_CSV") config.saveToCSV = (value == "true" || value == "1");
            else if (key == "OPTIMAL_TOUR_COST") config.optimalTourCost = std::stod(value);
            else if (key == "START_TEMPERATURE") config.startTemperature = std::stod(value);
            else if (key == "COOLING_FACTOR") config.coolingFactor = std::stod(value);
            else if (key == "EPOCH_LENGTH") config.epochLength = std::stoi(value);
            else if (key == "COOLING_SCHEDULE") config.coolingSchedule = value;
            else if (key == "NEIGHBOR_METHOD") config.neighborMethod = value;
        }
    }
    
    file.close();
    return config;
}

void ConfigReader::printConfig(const Config& config) {
    cout << "\n--- WCZYTANA KONFIGURACJA PROGRAMU ---\n";
    cout << "Sciezka docelowa: " << config.graphPath << "\n";
    cout << "Uzycie poczatkowego UB z RNN: " << (config.useInitialUB ? "TAK" : "NIE") << "\n";
    cout << "Limit czasu (s): " << config.timeLimitSeconds << "\n";
    cout << "Liczba powtorzen: " << config.repetitions << "\n";
    cout << "Zapis do CSV: " << (config.saveToCSV ? "TAK" : "NIE") << "\n";
    cout << "Optymalny koszt trasy: " << (config.optimalTourCost >= 0 ? std::to_string(config.optimalTourCost) : "NIE PODANO") << "\n";
    cout << "--- Parametry SA ---\n";
    cout << "Startowa temp.: " << config.startTemperature << "\n";
    cout << "Chlodzenie (" << config.coolingSchedule << "): " << config.coolingFactor << "\n";
    cout << "Dlugosc epoki: " << config.epochLength << "\n";
    cout << "Metoda chlodzenia: " << config.coolingSchedule << "\n";
    cout << "Metoda sasiedztwa: " << config.neighborMethod << "\n";
    cout << "--------------------------------------\n";
}

void ConfigReader::printParameters(const Config& config) {
    cout << "\n--- PARAMETRY ALGORYTMU SA ---\n";
    cout << "Startowa temp.: " << config.startTemperature << "\n";
    cout << "Chlodzenie (" << config.coolingSchedule << "): " << config.coolingFactor << "\n";
    cout << "Dlugosc epoki: " << config.epochLength << "\n";
    cout << "Metoda chlodzenia: " << config.coolingSchedule << "\n";
    cout << "Metoda sasiedztwa: " << config.neighborMethod << "\n";
    cout << "------------------------------\n";
}
