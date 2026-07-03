#include "config_reader.hpp"
#include <fstream>
#include <iostream>
#include <string>

using std::cout;

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

Config ConfigReader::readConfig(const string& filename) {
    Config config;
    
    // ustawienia domyslne
    config.graphPath = "data/tsplib/tsp/berlin52.tsp";
    config.method = "DFS";
    config.timeLimitSeconds = 900;
    config.repetitions = 1;
    config.useInitialUB = true;
    config.saveToCSV = true;

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
            else if (key == "METHOD") config.method = value;
            else if (key == "TIME_LIMIT_SECONDS") config.timeLimitSeconds = std::stoi(value);
            else if (key == "REPETITIONS") config.repetitions = std::stoi(value);
            else if (key == "USE_INITIAL_UB") config.useInitialUB = (value == "true");
            else if (key == "SAVE_TO_CSV") config.saveToCSV = (value == "true");
        }
    }
    
    file.close();
    return config;
}

void ConfigReader::printConfig(const Config& config) {
    cout << "\n--- WCZYTANA KONFIGURACJA PROGRAMU ---\n";
    cout << "Sciezka docelowa: " << config.graphPath << "\n";
    cout << "Metoda: " << config.method << "\n";
    cout << "Uzycie poczatkowego UB z RNN: " << (config.useInitialUB ? "TAK" : "NIE") << "\n";
    cout << "Limit czasu (s): " << config.timeLimitSeconds << "\n";
    cout << "Liczba powtorzen: " << config.repetitions << "\n";
    cout << "Zapis do CSV: " << (config.saveToCSV ? "TAK" : "NIE") << "\n";
}
