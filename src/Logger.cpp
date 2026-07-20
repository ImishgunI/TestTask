#include "Logger.hpp"
#include <chrono>
#include <ctime>
#include <stdexcept>
#include <iomanip>

Logger::Logger(const std::string& filename, LogLevel defaultLevel) : file(filename, std::ios::app), defaultLevel(defaultLevel) {
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open log file: " + filename);
    }
}

void Logger::log(const std::string& text, LogLevel level) {
    std::lock_guard<std::mutex> lock(mu);
    if (level < defaultLevel) {
        return;
    }
    auto timestamp = std::chrono::system_clock::now();
    auto time_now = std::chrono::system_clock::to_time_t(timestamp);
    std::tm tm{};
    localtime_r(&time_now, &tm);
    file << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " [" << levelToString(level) << "] " << text << "\n";
    file.flush();
    if (file.fail()) {
        throw std::ios::failure("Failed to write to log file");
    }
}

void Logger::log(const std::string& text) {
    log(text, defaultLevel);
}

void Logger::setDefaultLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(mu);
    defaultLevel = level;
}

std::string_view Logger::levelToString(LogLevel level) {
    switch(level) {
        case LogLevel::Debug:
            return "DEBUG";
        case LogLevel::Error:
            return "ERROR";
        case LogLevel::Info:
            return "INFO";
        default:
            return "UNKNOWN LEVEL";
    }
    return "";
}
