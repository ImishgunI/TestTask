#pragma once

#include "LogLevel.hpp"
#include <fstream>
#include <mutex>
#include <string>
#include <string_view>

class Logger {
public:
    Logger(const std::string& filename, LogLevel defaultLevel);
    Logger(const Logger& logger) = delete;
    Logger(Logger&&) = delete;

    Logger& operator=(const Logger& logger) = delete;
    Logger& operator=(Logger&&) = delete;

    ~Logger() = default;

    void log(const std::string& text);
    void log(const std::string& text, LogLevel level);
    void setDefaultLevel(LogLevel level);

private:
    std::string_view levelToString(LogLevel level);

private:
    std::ofstream file;
    std::mutex mu;
    LogLevel defaultLevel;
};
