#include <iostream>
#include <fstream>
#include <cmath>
#include "graph.hpp"

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

bool Graph::loadGraphFromFile(const string& filename) {
    extractAndSetName(filename);
    ifstream file(filename);
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
    ifstream file(filename);
    if (!file.is_open()) return false;

    string line;
    numVertices = 0;
    string weightType = "EUC_2D";
    string weightFormat = "FULL_MATRIX";
    
    while (std::getline(file, line)) {
        if (line.find("DIMENSION") != string::npos) {
            size_t colonPos = line.find(':');
            if (colonPos != string::npos) {
                numVertices = std::stoi(line.substr(colonPos + 1));
            }
        }
        else if (line.find("EDGE_WEIGHT_TYPE") != string::npos) {
            size_t colonPos = line.find(':');
            if (colonPos != string::npos) {
                weightType = line.substr(colonPos + 1);
                weightType.erase(0, weightType.find_first_not_of(" \t\r\n"));
                weightType.erase(weightType.find_last_not_of(" \t\r\n") + 1);
            }
        }
        else if (line.find("EDGE_WEIGHT_FORMAT") != string::npos) {
            size_t colonPos = line.find(':');
            if (colonPos != string::npos) {
                weightFormat = line.substr(colonPos + 1);
                weightFormat.erase(0, weightFormat.find_first_not_of(" \t\r\n"));
                weightFormat.erase(weightFormat.find_last_not_of(" \t\r\n") + 1);
            }
        }
        else if (line.find("NODE_COORD_SECTION") != string::npos) {
            break; 
        }
        else if (line.find("EDGE_WEIGHT_SECTION") != string::npos) {
            break;
        }
    }

    if (numVertices == 0) return false;

    matrix.assign(numVertices, vector<int>(numVertices, 0));

    if (weightType == "EUC_2D" || weightType == "GEO") {
        vector<std::pair<double, double>> coords(numVertices);
        for (int i = 0; i < numVertices; ++i) {
            int id;
            file >> id >> coords[i].first >> coords[i].second;
        }

        for (int i = 0; i < numVertices; ++i) {
            for (int j = 0; j < numVertices; ++j) {
                if (i == j) {
                    matrix[i][j] = -1;
                } else {
                    if (weightType == "EUC_2D") {
                        double dx = coords[i].first - coords[j].first;
                        double dy = coords[i].second - coords[j].second;
                        matrix[i][j] = static_cast<int>(std::round(std::sqrt(dx * dx + dy * dy)));
                    } else if (weightType == "GEO") {
                        double PI = 3.141592;
                        auto toRad = [PI](double x) {
                            int deg = static_cast<int>(x);
                            double min = x - deg;
                            return PI * (deg + 5.0 * min / 3.0) / 180.0;
                        };
                        double lat1 = toRad(coords[i].first);
                        double lon1 = toRad(coords[i].second);
                        double lat2 = toRad(coords[j].first);
                        double lon2 = toRad(coords[j].second);

                        double RRR = 6378.388;
                        double q1 = std::cos(lon1 - lon2);
                        double q2 = std::cos(lat1 - lat2);
                        double q3 = std::cos(lat1 + lat2);
                        matrix[i][j] = static_cast<int>(RRR * std::acos(0.5 * ((1.0 + q1) * q2 - (1.0 - q1) * q3)) + 1.0);
                    }
                }
            }
        }
    } else if (weightType == "EXPLICIT") {
        if (weightFormat == "FULL_MATRIX") {
            for (int i = 0; i < numVertices; ++i) {
                for (int j = 0; j < numVertices; ++j) {
                    file >> matrix[i][j];
                    if (i == j) matrix[i][j] = -1;
                }
            }
        } else if (weightFormat == "LOWER_DIAG_ROW") {
            for (int i = 0; i < numVertices; ++i) {
                for (int j = 0; j <= i; ++j) {
                    int val;
                    file >> val;
                    matrix[i][j] = (i == j) ? -1 : val;
                    matrix[j][i] = matrix[i][j];
                }
            }
        } else if (weightFormat == "UPPER_DIAG_ROW") {
            for (int i = 0; i < numVertices; ++i) {
                for (int j = i; j < numVertices; ++j) {
                    int val;
                    file >> val;
                    matrix[i][j] = (i == j) ? -1 : val;
                    matrix[j][i] = matrix[i][j];
                }
            }
        }
    }

    file.close();
    return true;
}

bool Graph::loadGraphAuto(const string& filename) {
    if (filename.length() >= 4 && (filename.substr(filename.length() - 4) == ".tsp" || filename.substr(filename.length() - 5) == ".atsp")) {
        return loadGraphFromTSPLIB(filename);
    }
    return loadGraphFromFile(filename);
}

vector<vector<int>> Graph::getMatrixCopy() const {
    return matrix;
}
