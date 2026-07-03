#pragma once
#include <string>

using std::string;

struct Config {
    string graphPath;
    int timeLimitSeconds;
    int repetitions;
    bool useInitialUB;
    bool saveToCSV;
    double optimalTourCost;

    double populationSize;
    double mutationRate;
    int generations;
    string crossoverMethod;
    string mutationMethod;
};

class ConfigReader {
public:
    static Config readConfig(const string& filename);
    static void printParameters(const Config& config);
};
