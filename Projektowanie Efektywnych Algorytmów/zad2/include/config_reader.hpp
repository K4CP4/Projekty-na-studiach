#pragma once
#include <string>

using std::string;

struct Config {
    string graphPath;
    string method;
    int timeLimitSeconds;
    int repetitions;
    bool useInitialUB;
    bool saveToCSV;
};

class ConfigReader {
public:
    static Config readConfig(const string& filename);
    static void printConfig(const Config& config);
};
