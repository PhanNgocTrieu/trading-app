#ifndef SERVICE_H
#define SERVICE_H

#include <iostream>

namespace Service {
    class ServiceAPI {
        public:
            static ServiceAPI& getInstance() {
                static ServiceAPI instance;
                return instance;
            }

            virtual ~ServiceAPI() = default;

            void requestLogin(const std::string& username, const std::string& password);
            void requestLogout(const std::string& username);
            void requestWithdrawal(const std::string& accountNumber, double amount);
            void requestDeposit(const std::string& accountNumber, double amount);
            void requestTrading(const std::string& stockSymbol, int quantity, double price);
            
    };
}

#endif