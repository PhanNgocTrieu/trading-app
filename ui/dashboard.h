#ifndef DASHBOARD_H
#define DASHBOARD_H

#include "bank.h"
#include "login.h"
#include "trading.h"

#include <string>

class Dashboard {
public:
    Dashboard(std::string title,
              Service::LoginService& loginService,
              Service::BankAccountService& bankService,
              Service::TradingService& tradingService);

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
    void tradeMenu();
    void listQuotes() const;
    void buyMarket();
    void sellMarket();
    void viewPortfolio() const;

    std::string title_;
    Service::LoginService& loginService_;
    Service::BankAccountService& bankService_;
    Service::TradingService& tradingService_;
};

#endif
