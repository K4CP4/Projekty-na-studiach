#ifndef CONFIG_READER_HPP
#define CONFIG_READER_HPP

#include <string>

using std::string;
using std::stoi;


struct Config {
    string algorithm;
    bool generateNew;
    int graphSize;
    int graphType;
    string graphPath;
    int optimalCost;
    int timeLimitSeconds;
    int repetitions;
    int startNode;
    bool saveToCSV;
};

namespace ConfigReader {
    Config readConfig(const string& filename);
    
    void printConfig(const Config& config);
}

#endif
