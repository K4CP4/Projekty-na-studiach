#include <iostream>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <thread>
#include <chrono>

#include "Task.hpp"
#include "SharedResources.hpp"
#include "Service.hpp"

int main() {
    const bool testMode = false;         // flaga do uruchomienia trybu testowego
    const bool terminalLogMode = false;  // flaga do sterowania wypisywaniem logow w terminalu

    auto prepare_log_file_path = []() -> std::filesystem::path {
        namespace fs = std::filesystem;

        fs::path logsDir = "logs";
        try {
            if (!fs::exists(logsDir)) {
                fs::create_directories(logsDir);
            }
        } catch (...) {
            std::cerr << "Wystapil blad.\n";
            return {};
        }

        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm localTm{};
        localtime_s(&localTm, &t);

        std::ostringstream name;
        name << "logs-"
             << std::setw(2) << std::setfill('0') << localTm.tm_mday << "-"
             << std::setw(2) << std::setfill('0') << (localTm.tm_mon + 1) << "-"
             << (localTm.tm_year + 1900) << "_"
             << std::setw(2) << std::setfill('0') << localTm.tm_hour << "-"
             << std::setw(2) << std::setfill('0') << localTm.tm_min
             << ".txt";

        return logsDir / name.str();
    };

    const std::filesystem::path logFilePath = prepare_log_file_path();

    auto run_default_program = [&](bool quietLogs, bool injectHighPriority, bool logEndOfSubmitting, std::chrono::milliseconds stopAfter, int& producedOut, int& doneOut) {
        const int taskPairs = 500;              // liczba par zadan

        SharedResources shared;                 // wspólne zasoby
        shared.logger.setEnabled(!quietLogs);   // wyłączenie logów w 1 scenariuszu
        shared.logger.setConsoleEnabled(terminalLogMode && !quietLogs);

        Service dataService("DataService", shared, 50);          // usługa przetwarzająca dane
        Service reportService("ReportService", shared, 50);      // usługa generująca raporty

        dataService.start();
        reportService.start();

        for (int i = 0; i < taskPairs; ++i) {
            Task t1{ i + 1000, (i % 5 == 0) ? 10 : 1, "data_task" };
            Task t2{ i + 2000, (i % 7 == 0) ? 7 : 2, "report_task" };
            dataService.submit(t1);
            reportService.submit(t2);

            // scenariusz 2: wstrzyknięcie zadania o wysokim priorytecie jako i + 1 zadanie
            if (injectHighPriority && i == 100) {
                shared.logger.log("[TEST] Do kolejki DataService wchodzi zadanie HIGH: id=9999 priorytet=1000 dane=TEST_HIGH");
                Task high{ 9999, 1000, "TEST_HIGH" };
                dataService.submit(high);
            }

            // scenariusz 3: informacja o końcu dodawania zadań
            if (logEndOfSubmitting && (i + 1) == taskPairs) {
                shared.logger.log("[TEST] Koniec dodawania nowych zadan");
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        if (logEndOfSubmitting && taskPairs <= 0) {
            shared.logger.log("[TEST] Koniec dodawania nowych zadan");
        }


        std::this_thread::sleep_for(stopAfter);

        dataService.stop();
        reportService.stop();

        // podsumowanie wyników
        producedOut = dataService.produced_count() + reportService.produced_count();
        doneOut = dataService.done_count() + reportService.done_count();

        // Zrzut bufora logow do pliku
        if (!logFilePath.empty()) {
            shared.logger.dumpToFile(logFilePath, true);
        }
    };

    auto print_test_header = [](int scenario) {
        std::cout << "===== SCENARIUSZ " << scenario << " =====\n";
    };

    auto print_summary = [](int produced, int done) {
        std::cout << "Wygenerowane: " << produced << " Wykonane: " << done << " Status: " << (produced == done ? "OK" : "MISMATCH") << "\n";
    };


    if (testMode) {
        std::cout << "Wybierz scenariusz (1/2/3): ";

        int scenario = 0;
        if (!(std::cin >> scenario)) return 1;

        int runs = 1;
        bool quietLogs = false;
        bool injectHighPriority = false;
        bool logEndOfSubmitting = false;
        std::chrono::milliseconds stopAfter = std::chrono::seconds(1);

        switch (scenario) {
        case 1:
            runs = 10;
            quietLogs = true;
            stopAfter = std::chrono::seconds(1);
            break;
        case 2:
            injectHighPriority = true;
            stopAfter = std::chrono::seconds(1);
            break;
        case 3:
            logEndOfSubmitting = true;
            stopAfter = std::chrono::milliseconds(150);
            break;
        default:
            std::cout << "Niepoprawny numer scenariusza.\n";
            return 1;
        }

        print_test_header(scenario);

        for (int i = 1; i <= runs; ++i) {
            int produced = 0;
            int done = 0;
            run_default_program(quietLogs, injectHighPriority, logEndOfSubmitting, stopAfter, produced, done);
            if (runs > 1) {
                std::cout << "Proba " << i << ": ";
            }
            print_summary(produced, done);
        }
        return 0;
    }

    int produced = 0;
    int done = 0;
    run_default_program(false, false, false, std::chrono::seconds(1), produced, done);
    print_summary(produced, done);
    return 0;
}
