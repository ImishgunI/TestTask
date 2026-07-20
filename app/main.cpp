#include <condition_variable>
#include <iostream>
#include "Logger.hpp"
#include <optional>
#include <string>
#include <sstream>
#include <queue>
#include <mutex>
#include <thread>

// Задача на запись сообщения в журнал.
// Если уровень не указан, используется уровень по умолчанию логгера.
struct LogTask {
    std::string text;
    std::optional<LogLevel> level;
};

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

    std::queue<LogTask> tasks;
    std::mutex queueMutex;
    std::condition_variable cv;
    bool stop = false;

    // Поток, отвечающий за обработку очереди и запись сообщений в журнал.
    std::thread worker([&]() {
        while(true) {
            std::unique_lock<std::mutex> lock(queueMutex);

            // Ожидаем появления новых задач или сигнала завершения приложения.
            cv.wait(lock, [&]() {
                    return !tasks.empty() || stop;
            });
            
            // Завершаем поток только после обработки всех сообщений из очереди.
            if (stop && tasks.empty()) {
                break;
            }

            auto task = std::move(tasks.front());
            tasks.pop();
            
            // Освобождаем очередь перед записью в файл,
            // чтобы основной поток мог продолжать принимать сообщения.
            lock.unlock();
            try {
                if (task.level.has_value()) {
                    logger.log(task.text, *task.level);
                } else {
                    logger.log(task.text);
                }
            } catch(const std::exception& e) {
                std::cerr << e.what() << '\n';
            }
        }
    });

    std::string text;
    while(std::getline(std::cin, text)) {
        std::istringstream iss(text);
        std::string first;
        iss >> first;
        if (first == "/exit") {
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                stop = true;
            }
            cv.notify_one();
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
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (level.has_value()) {
                std::string message;
                std::getline(iss >> std::ws, message); 
                if (message.empty()) {
                    std::cerr << "Empty message\n";
                    continue;
                }
                tasks.push(LogTask {message, level});
            } else {
                tasks.push(LogTask { text, std::nullopt});
            }
        }
        cv.notify_one();
    }
    cv.notify_one();
    worker.join();
}
