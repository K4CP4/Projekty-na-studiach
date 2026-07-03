#include "graph.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <cmath>
#include <iomanip>

using std::string;
using std::vector;
using std::cout;

Graph::Graph() : numVertices(0), instanceName("UNKNOWN") {}

void Graph::extractAndSetName(const string& graphPath) {
    size_t pos = graphPath.find_last_of("/\\");
    string fileName = (pos == string::npos) ? graphPath : graphPath.substr(pos + 1);

    size_t dotPos = fileName.find_last_of('.');
    if (dotPos != string::npos) {
        instanceName = fileName.substr(0, dotPos);
    } else instanceName = fileName;
}

int Graph::getNumVertices() const {
    return numVertices;
}

int Graph::getDistance(int from, int to) const {
    if (from >= 0 && from < numVertices && to >= 0 && to < numVertices) {
        return matrix[from][to];
    }
    return -1;
}

string Graph::getInstanceName() const {
    return instanceName;
}

void Graph::generateRandomGraph(int size, bool graphType) {
    instanceName = (graphType == 0 ? "sym_" : "asym_") + std::to_string(size);
    numVertices = size;
    matrix.assign(size, vector<int>(size, 0));

    // generator liczb losowych
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 100);

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (i == j) {
                matrix[i][j] = -1;      // przekątna - brak krawędzi do samego siebie
            } else if (graphType == 0 && i > j) {
                // jeśli symetryczny, wklejana jest waga z górnej połowy
                matrix[i][j] = matrix[j][i];
            } else {
                matrix[i][j] = dist(gen);
            }
        }
    }
}

bool Graph::saveGraphToFile(const string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return false;

    file << numVertices << "\n";
    for (int i = 0; i < numVertices; ++i) {
        for (int j = 0; j < numVertices; ++j) {
            file << matrix[i][j] << " ";
        }
        file << "\n";
    }
    file.close();
    return true;
}

bool Graph::loadGraphFromFile(const string& filename) {
    extractAndSetName(filename);
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    file >> numVertices;
    matrix.assign(numVertices, vector<int>(numVertices, 0));

    for (int i = 0; i < numVertices; ++i) {
        for (int j = 0; j < numVertices; ++j) {
            file >> matrix[i][j];
        }
    }
    file.close();
    return true;
}

bool Graph::loadGraphFromTSPLIB(const string& filename) {
    extractAndSetName(filename);
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    string line;
    numVertices = 0;
    bool isExplicit = false;
    
    while (std::getline(file, line)) {
        if (line.find("DIMENSION") != string::npos) {
            size_t colonPos = line.find(':');
            if (colonPos != string::npos) {
                numVertices = std::stoi(line.substr(colonPos + 1));
            }
        }
        if (line.find("NODE_COORD_SECTION") != string::npos) {
            isExplicit = false;
            break; 
        }
        if (line.find("EDGE_WEIGHT_SECTION") != string::npos) {
            isExplicit = true;
            break;
        }
    }

    if (numVertices == 0) return false;

    matrix.assign(numVertices, vector<int>(numVertices, 0));

    if (!isExplicit) {
        vector<std::pair<double, double>> coords(numVertices);
        for (int i = 0; i < numVertices; ++i) {
            int id;
            double x, y;
            file >> id >> x >> y;
            coords[i] = {x, y};
        }

        for (int i = 0; i < numVertices; ++i) {
            for (int j = 0; j < numVertices; ++j) {
                if (i == j) {
                    matrix[i][j] = -1;
                } else {
                    double dx = coords[i].first - coords[j].first;
                    double dy = coords[i].second - coords[j].second;
                    matrix[i][j] = static_cast<int>(std::round(std::sqrt(dx * dx + dy * dy)));
                }
            }
        }
    } else {
        for (int i = 0; i < numVertices; ++i) {
            for (int j = 0; j < numVertices; ++j) {
                file >> matrix[i][j];
                if (i == j) matrix[i][j] = -1;
            }
        }
    }

    file.close();
    return true;
}

void Graph::printGraph() const {
    cout << "\nRozmiar grafu: " << numVertices << "\nMacierz odleglosci:\n";
    for (int i = 0; i < numVertices; ++i) {
        for (int j = 0; j < numVertices; ++j) {
            // ustawienie szerokości pola na 4
            cout << std::setw(4) << matrix[i][j] << " ";
        }
        cout << "\n";
    }
}
