#ifndef LOGGER_SERVICE_H
#define LOGGER_SERVICE_H

#include "color.hpp"

#include <iostream>
#include <string>

namespace Service {

class LoggerService {
public:
    static LoggerService& getInstance() {
        static LoggerService instance;
        return instance;
    }

    virtual ~LoggerService() = default;

    void logInfo(const std::string& message) {
        std::cout << Color::wrap(Color::BrightGreen, "[INFO]") << ": " << message << std::endl;
    }

    void logWarning(const std::string& message) {
        std::cout << Color::wrap(Color::BrightYellow, "[WARNING]") << ": " << message
                  << std::endl;
    }

    void logError(const std::string& message) {
        std::cerr << Color::wrap(Color::BrightRed, "[ERROR]", stderr) << ": " << message
                  << std::endl;
    }

private:
    LoggerService() = default;
    LoggerService(const LoggerService&) = delete;
    LoggerService& operator=(const LoggerService&) = delete;
};

} // namespace Service

#endif
