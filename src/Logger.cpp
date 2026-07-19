#include "../include/Logger.hpp"
#include <chrono>
#include <ctime>
#include <mutex>
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
    std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();
    std::time_t time_now = std::chrono::system_clock::to_time_t(timestamp);
    file << std::put_time(std::localtime(&time_now), "%Y-%m-%d %H:%M:%S") << " [" << levelToString(level) << "] " << text << "\n";
}

void Logger::log(const std::string& text) {
    log(text, defaultLevel);
}

void Logger::setDefaultLevel(LogLevel level) {
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
            return "UKNOWN LEVEL";
    }
    return "";
}
