#include <iostream>
#include "Logger.hpp"
#include <optional>
#include <string>
#include <sstream>

std::optional<LogLevel> parseLogLevel(std::string_view level) {
    if (level == "INFO") return LogLevel::Info;
    if (level == "ERROR") return LogLevel::Error;
    if (level == "DEBUG") return LogLevel::Debug;

    return {};
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Argc must be more than " << argc << '\n';
        std::cerr << "Usage: ./main <log_file> <DEBUG|INFO|ERROR>";
        return 1;
    }
    auto filename = argv[1];
    auto level = parseLogLevel(argv[2]);
    if (!level.has_value()) {
        std::cerr << "Unknown log level" << "\n";
        return 1;
    }
    Logger logger(filename, *level);

    std::string text;
    while(std::getline(std::cin, text)) {
        std::istringstream iss(text);
        std::string first;
        iss >> first;
        if (first == "/exit") {
            break;
        }
        if (first == "/set") {
            std::string second;
            iss >> second;
            auto level = parseLogLevel(second);
            if (!level.has_value()) {
                std::cerr << "Unknown log level\n";
                continue;
            }
            logger.setDefaultLevel(*level);
            continue;
        }
        auto level = parseLogLevel(first);
        if (level.has_value()) {
            std::string message;
            std::getline(iss >> std::ws, message); 
            logger.log(message, *level);
        } else {
            logger.log(text);
        }
    }
}
