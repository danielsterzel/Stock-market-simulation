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
    void logToCsvFormat(First &&first, Rest &&... rest){
        if (not isFileCreated) {
            throw std::runtime_error("Log file has not been opened before trying to write in it");
        }
        // std::lock_guard<std::mutex> lock(mtx); // for thread safety
        file << std::format("{:%Y-%m-%d %H:%M:%S},",std::chrono::system_clock::now());
        // file << std::forward<First>(first);
        writeField(std::forward<First>(first));
        ((file << ',', writeField(std::forward<Rest>(rest))), ...);
        file << '\n';
        file.flush();
    }
    ~Logger() {
        if (file.is_open()) {
            file.flush();
            file.close();
        }
    }

private:
    std::ofstream file;
    bool isFileCreated{false};
    template<typename  T>
    void writeField(T&& value) {
        // if it can be implicitly converted to a string like object use escapeCsv
        if constexpr(std::is_convertible_v<T, std::string_view>) {
            file << escapeCsv(std::string_view(value));
            return;
        }
        file << std::forward<T>(value);
    }
    static inline std::string escapeCsv(const std::string_view inputString){

        if (inputString.find_first_of("\",\n") == std::string::npos) return std::string(inputString);
        std::string out;
        out.reserve(inputString.size() + 4);
        out.push_back('"');
        for (const char& c : inputString) {
            if (c == '"') {
                out.push_back('"'); // double it
            }
            out.push_back(c);
        }
        out.push_back('"');
        return out;
    }

};
