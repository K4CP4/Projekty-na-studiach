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

    double startTemperature;
    double coolingFactor;
    int epochLength;
    string coolingSchedule;
    string neighborMethod;
};

class ConfigReader {
public:
    static Config readConfig(const string& filename);
    static void printConfig(const Config& config);
    static void printParameters(const Config& config);
};
