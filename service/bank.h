#ifndef BANK_SERVICE_H
#define BANK_SERVICE_H

#include <iostream>

namespace Service {

    class BankAccountService {
        public:
            static BankAccountService& getInstance() {
                static BankAccountService instance;
                return instance;
            }

            virtual ~BankAccountService() = default;

            void createAccount(const std::string& name, const std::string& phoneNumber, double initialBalance);
            void deleteAccount(const std::string& accountNumber);
            void deposit(const std::string& accountNumber, double amount);
            void withdraw(const std::string& accountNumber, double amount);
            void transfer(const std::string& fromAccount, const std::string& toAccount, double amount);
            bool checkBalance(const std::string& accountNumber);

        private:
            BankAccountService() = default;
            BankAccountService(const BankAccountService&) = delete;
            BankAccountService& operator=(const BankAccountService&) = delete;

            void _deposit(const std::string& accountNumber, double amount, const std::string& opt);

            std::string m_accountNumber;
            std::string m_accountHolderName;
            double m_balance;
            
    };
};

#endif