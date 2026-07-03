#include <iostream>
#include <algorithm>
#include <queue>
#include <stack>
#include "algorithms/branch_and_bound.hpp"

namespace Algorithms {
    BranchAndBound::BranchAndBound(const Graph& graph) : originalGraph(graph) {}

    int BranchAndBound::reduceMatrix(vector<vector<int>>& matrix) {
        int n = matrix.size();
        int cost = 0;

        for (int i = 0; i < n; i++) {
            int rowMin = INF;
            for (int j = 0; j < n; j++) rowMin = std::min(rowMin, matrix[i][j]);
            if (rowMin != INF && rowMin > 0) {
                cost += rowMin;
                for (int j = 0; j < n; j++) if (matrix[i][j] != INF) matrix[i][j] -= rowMin;
            }
        }

        for (int j = 0; j < n; j++) {
            int colMin = INF;
            for (int i = 0; i < n; i++) colMin = std::min(colMin, matrix[i][j]);
            if (colMin != INF && colMin > 0) {
                cost += colMin;
                for (int i = 0; i < n; i++) if (matrix[i][j] != INF) matrix[i][j] -= colMin;
            }
        }
        return cost;
    }

    Node BranchAndBound::createChildNode(const Node& parent, int toCity) {
        Node child = parent;
        child.path.push_back(toCity);
        child.currentCity = toCity;
        child.level++;

        int fromCity = parent.currentCity;
        int stepCost = child.matrix[fromCity][toCity];

        for (int i = 0; i < originalGraph.getNumVertices(); i++) {
            child.matrix[fromCity][i] = INF;
            child.matrix[i][toCity] = INF;
        }
        child.matrix[toCity][child.path[0]] = INF;

        child.lowerBound += stepCost + reduceMatrix(child.matrix);
        return child;
    }

    AlgorithmResult BranchAndBound::solve(const std::string& method, int initialUB, int timeLimitSeconds) {
        AlgorithmResult result;
        result.maxMemoryNodes = 1;
        long long nodesCount = 1;
        
        int n = originalGraph.getNumVertices();
        int minCost = (initialUB == -1) ? INF : initialUB;

        Utils::Timer timer;
        timer.start();

        Node root;
        root.path = {0};
        root.level = 0;
        root.currentCity = 0;
        root.matrix = originalGraph.getMatrixCopy();
        
        for(int i = 0; i < n; i++) root.matrix[i][i] = INF;
        root.lowerBound = reduceMatrix(root.matrix);
        
        if (method == "BFS") { 
            std::queue<Node> q;
            q.push(root);
            
            while (!q.empty() && !AlgorithmCore::isTimeout(timer, timeLimitSeconds)) {
                Node current = q.front(); q.pop();
                
                if (current.lowerBound >= minCost) continue;

                if (current.level == n - 1) {
                    minCost = result.bestCost = current.lowerBound;
                    result.bestTour = current.path;
                    continue;
                }

                for (int i = 0; i < n; i++) {
                    if (std::find(current.path.begin(), current.path.end(), i) == current.path.end()) {
                        Node child = createChildNode(current, i);
                        if (child.lowerBound < minCost) { q.push(child); nodesCount++; }
                    }
                }
                if (q.size() > (size_t)result.maxMemoryNodes) result.maxMemoryNodes = q.size();
            }
        } 
        else if (method == "DFS") {
            std::stack<Node> s;
            s.push(root);
            
            while (!s.empty() && !AlgorithmCore::isTimeout(timer, timeLimitSeconds)) {
                Node current = s.top(); s.pop();
                
                if (current.lowerBound >= minCost) continue;

                if (current.level == n - 1) {
                    minCost = result.bestCost = current.lowerBound;
                    result.bestTour = current.path;
                    continue;
                }

                for (int i = n - 1; i >= 0; i--) { 
                    if (std::find(current.path.begin(), current.path.end(), i) == current.path.end()) {
                        Node child = createChildNode(current, i);
                        if (child.lowerBound < minCost) { s.push(child); nodesCount++; }
                    }
                }
                if (s.size() > (size_t)result.maxMemoryNodes) result.maxMemoryNodes = s.size();
            }
        } 
        else if (method == "LC") {
            std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;
            pq.push(root);
            
            while (!pq.empty() && !AlgorithmCore::isTimeout(timer, timeLimitSeconds)) {
                Node current = pq.top(); pq.pop();
                
                if (current.lowerBound >= minCost) continue;

                if (current.level == n - 1) {
                    minCost = result.bestCost = current.lowerBound;
                    result.bestTour = current.path;
                    continue; 
                }

                for (int i = 0; i < n; i++) {
                    if (std::find(current.path.begin(), current.path.end(), i) == current.path.end()) {
                        Node child = createChildNode(current, i);
                        if (child.lowerBound < minCost) { pq.push(child); nodesCount++; }
                    }
                }
                if (pq.size() > (size_t)result.maxMemoryNodes) result.maxMemoryNodes = pq.size();
            }
        }

        timer.stop();
        result.executionTime = timer.getMicroseconds();
        result.iterations = nodesCount;

        if (AlgorithmCore::isTimeout(timer, timeLimitSeconds)) {
            std::cout << "\n[UWAGA] Branch & Bound przerwany z powodu limitu czasu!\n";
        }

        return result;
    }
}
