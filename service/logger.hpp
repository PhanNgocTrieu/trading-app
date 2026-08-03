#ifndef LOGGER_SERVICE_H
#define LOGGER_SERVICE_H

#include <iostream>

namespace Service {
    class LoggerService {
        public:
            static LoggerService& getInstance() {
                static LoggerService instance;
                return instance;
            }
            
            virtual ~LoggerService() = default;

            void logInfo(const std::string& message)
            {
                std::cout << "[INFO]: " << message << std::endl;
            }

            void logWarning(const std::string& message)
            {
                std::cout << "[WARNING]: " << message << std::endl;
            }

            void logError(const std::string& message)
            {
                std::cerr << "[ERROR]: " << message << std::endl;
            }

        private:
            LoggerService() = default;
            LoggerService(const LoggerService&) = delete;
            LoggerService& operator=(const LoggerService&) = delete;

    };
}

#endif 