#pragma once
#include <filesystem>
#include <fstream>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <vector>
#include <iostream>

class SharedResources {
public:
    class Logger {
    public:
        // wlaczenie/wylaczenie logow (wspolne dla terminala i pliku)
        void setEnabled(bool value) {
            std::lock_guard<std::mutex> lock(logMtx);
            enabled = value;
        }

        // sterowanie wypisywaniem logow w terminalu
        void setConsoleEnabled(bool value) {
            std::lock_guard<std::mutex> lock(logMtx);
            consoleEnabled = value;
        }

        // wypisanie logu (buforowanie w pamieci + opcjonalnie terminal)
        void log(const std::string& msg) {
            std::lock_guard<std::mutex> lock(logMtx);
            if (!enabled) return;
            logBuffer.push_back(msg);
            if (consoleEnabled) {
                std::cout << msg << std::endl;
            }
        }

        // zapis bufora do pliku
        void dumpToFile(const std::filesystem::path& filePath, bool append = true) {
            std::lock_guard<std::mutex> lock(logMtx);
            if (!enabled) return;
            if (filePath.empty()) return;
            if (logBuffer.empty()) return;

            std::ofstream file;
            file.open(filePath, append ? (std::ios::out | std::ios::app) : std::ios::out);
            if (!file.is_open()) {
                if (consoleEnabled) {
                    std::cerr << "Nie udalo sie otworzyc pliku logow do zapisu: " << filePath.string() << std::endl;
                }
                return;
            }

            for (const auto& line : logBuffer) {
                file << line << "\n";
            }
            file.close();
            logBuffer.clear();
        }
    private:
        std::mutex logMtx;      // mutex do synchronizacji wypisywania logów
        bool enabled{true};     // flaga wlaczenia/wylaczenia logow
        bool consoleEnabled{true};
        std::vector<std::string> logBuffer;
    };

    class DataPool {
    public:
        int read(size_t idx) const {
            std::shared_lock<std::shared_mutex> lock(sharedMtx);
            if (idx < dataVector.size()) return dataVector[idx];
            return -1;
        }
        void write(size_t idx, int value) {
            std::unique_lock<std::shared_mutex> lock(sharedMtx);
            if (idx >= dataVector.size()) dataVector.resize(idx + 1, 0);
            dataVector[idx] = value;
        }
        size_t size() const {
            std::shared_lock<std::shared_mutex> lock(sharedMtx);
            return dataVector.size();
        }
    private:
        mutable std::shared_mutex sharedMtx;    // mutex do synchronizacji dostępu do danych
        std::vector<int> dataVector;            // wektor przechowujący dane
    };

    Logger logger;
    DataPool data;
};
