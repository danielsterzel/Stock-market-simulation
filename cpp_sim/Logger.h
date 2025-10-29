#pragma once

#include <fstream>
#include <string>

class Logger {
public:
    Logger() = default;

    Logger(Logger &) = delete;

    Logger(Logger &&) = delete;

    void openFile(const std::string &filePath, const bool overwriteFile = false) {
        auto writeMode = std::ios::out;
        writeMode |= overwriteFile ? std::ios::trunc : std::ios::app;

        file.open(filePath, writeMode);
        if (not file.is_open()) {
            throw std::runtime_error("Failed to open log file " + filePath);
        }
        isFileCreated = true;
    }

    template<typename First, typename... Rest>
    void logToCsvFormat(First &&first, Rest &&... rest) {
        if (not isFileCreated) {
            throw std::runtime_error("Log file has not been opened before trying to write in it");
        }
        file << std::forward<First>(first);
        ((file << ',' << std::forward<Rest>(rest)), ...);
        file << '\n';
        file.flush();
    }

private:
    std::ofstream file;
    bool isFileCreated = false;
};
