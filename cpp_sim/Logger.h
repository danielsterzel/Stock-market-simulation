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
    // void writeAction(const std::string& actionDescription) {
    //
    // }
    template<typename AgentType, typename OrderT>
    void writeAction(const AgentType& agentType,const OrderT& order) {

        file << std::format("{:%Y-%m-%d %H:%M:%S} ", std::chrono::system_clock::now());
        writeField("Agent of type: ");
        writeField(agentType);
        file << '\n';
        writeField("Created Order: ");
        writeField(order);
        file << "\n\n";
        file.flush();
    }
    void writeCsvHeaders(std::initializer_list<std::string> headers) {
        if (not isFileCreated) throw std::runtime_error("File not opened before writing header");

        bool first = true;
        for (const auto& h: headers) {
            if (not first) file << ',';
            file << escapeCsv(h);
            first = false;
        }
        file << '\n';
        file.flush();
    }

    template<typename First, typename... Rest>
    void logToCsvFormat(First &&first, Rest &&... rest){
        if (not isFileCreated) {
            throw std::runtime_error("Log file has not been opened before trying to write in it");
        }
        // std::lock_guard<std::mutex> lock(mtx); // for thread safety maybe if we decide to run market sims on multiple threads
        // if we decide that we need to think if using flush() everytime we write one line will hinder performance.
        file << std::format("{:%Y-%m-%d %H:%M:%S},",std::chrono::system_clock::now());
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
    template<typename  T>
    void writeField(T&& value) {
        // if it can be implicitly converted to a string like object use escapeCsv
        if constexpr(std::is_convertible_v<T, std::string_view>) {
            file << escapeCsv(std::string_view(value));
            return;
        }
        file << std::forward<T>(value);
    }
    // dałem static bo mnie clang męczy :((((
    static std::string escapeCsv(const std::string_view inputString){

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

    std::ofstream file;
    bool isFileCreated{false};

};
