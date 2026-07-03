#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <string>

using std::vector;
using std::string;

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

    void generateRandomGraph(int size, bool graphType);
    
    bool saveGraphToFile(const string& filename) const;
    bool loadGraphFromFile(const string& filename);
    bool loadGraphFromTSPLIB(const string& filename);

    void printGraph() const;
};

#endif
