#ifndef DASHBOARD_H
#define DASHBOARD_H

#include "bank.h"
#include "logger.hpp"
#include "login.h"

#include <string>

class Dashboard {
public:
    Dashboard(std::string title,
              Service::LoginService& loginService,
              Service::BankAccountService& bankService,
              Service::LoggerService& loggerService);

    void showLoginDashboard() const;
    void showDashboard() const;
    void actionDashboard();

private:
    void drawHeader() const;
    void drawFooter() const;
    void drawContent() const;

    void viewAccount() const;
    void depositFunds();
    void withdrawFunds();
    void tradeStocksPlaceholder() const;

    std::string title_;
    Service::LoginService& loginService_;
    Service::BankAccountService& bankService_;
    Service::LoggerService& loggerService_;
};

#endif
