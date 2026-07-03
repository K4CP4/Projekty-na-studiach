#pragma once
#include <vector>
#include <string>

using std::vector;
using std::string;
using std::ifstream;

class Graph {
private:
    int numVertices;
    string instanceName;
    vector<vector<int>> matrix;
    void extractAndSetName(const string& graphPath);
public:
    Graph();
    int getNumVertices() const;
    int getDistance(int from, int to) const;
    string getInstanceName() const;
    bool loadGraphFromFile(const string& filename);
    bool loadGraphFromTSPLIB(const string& filename);
    bool loadGraphAuto(const string& filename);
    vector<vector<int>> getMatrixCopy() const;
};
