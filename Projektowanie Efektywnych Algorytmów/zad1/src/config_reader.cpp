#include "config_reader.hpp"
#include <fstream>
#include <iostream>
#include <string>

using std::cout;

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";           // npos - not found
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

Config ConfigReader::readConfig(const string& filename) {
    Config config;
    
    // ustawienia domyślne
    config.algorithm = "NONE";
    config.generateNew = false;
    config.graphSize = 10;
    config.graphType = 1;
    config.graphPath = "";
    config.optimalCost = -1;
    config.timeLimitSeconds = 1800;
    config.repetitions = 1;
    config.startNode = 0;
    config.saveToCSV = true;

    std::ifstream file(filename);
    if (!file.is_open()) {
        cout << "\n[BLAD] Nie mozna otworzyc pliku konfiguracyjnego: " << filename << "\n";
        return config;          // zwracana jest domyślna konfiguracja
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
            if (key == "ALGORITHM") config.algorithm = value;
            else if (key == "GENERATE_NEW") config.generateNew = (value == "true");
            else if (key == "GRAPH_SIZE") config.graphSize = stoi(value);      // stoi - string to int
            else if (key == "GRAPH_TYPE") config.graphType = stoi(value);
            else if (key == "GRAPH_PATH") config.graphPath = value;
            else if (key == "OPTIMAL_COST") config.optimalCost = stoi(value);
            else if (key == "TIME_LIMIT_SECONDS") config.timeLimitSeconds = stoi(value);
            else if (key == "REPETITIONS") config.repetitions = stoi(value);
            else if (key == "START_NODE") config.startNode = stoi(value);
            else if (key == "SAVE_TO_CSV") config.saveToCSV = (value == "true");
        }
    }
    
    file.close();
    return config;
}

void ConfigReader::printConfig(const Config& config) {
    cout << "\n--- WCZYTANA KONFIGURACJA PROGRAMU ---\n";
    cout << "Algorytm: " << config.algorithm << "\n";
    cout << "Generowac nowy graf: " << (config.generateNew ? "TAK" : "NIE") << "\n";
    cout << "Rozmiar grafu: " << config.graphSize << "\n";
    cout << "Typ grafu: " << (config.graphType == 0 ? "Symetryczny" : "Asymetryczny") << "\n";
    cout << "Sciezka docelowa: " << config.graphPath << "\n";
    cout << "Optymalny koszt: " << config.optimalCost << "\n";
    cout << "Limit czasu (s): " << config.timeLimitSeconds << "\n";
    cout << "Liczba powtorzen: " << config.repetitions << "\n";
    cout << "Wierzcholek startowy: " << config.startNode << "\n";
    cout << "Zapis do CSV: " << (config.saveToCSV ? "TAK" : "NIE") << "\n";
}
