#include "dashboard.h"

#include "domain/order_types.hpp"

#include <iostream>
#include <limits>

Dashboard::Dashboard(std::string title,
                     Service::LoginService& loginService,
                     Service::BankAccountService& bankService,
                     Service::LoggerService& loggerService)
    : title_(std::move(title))
    , loginService_(loginService)
    , bankService_(bankService)
    , loggerService_(loggerService) {}

void Dashboard::showLoginDashboard() const {
    std::cout << "=== " << title_ << " ===\n";
    std::cout << "Please login or register (SQLite-backed Phase 1).\n";
    std::cout << "=== End of Dashboard ===\n";
}

void Dashboard::showDashboard() const {
    drawHeader();
    drawContent();
    drawFooter();
}

void Dashboard::drawHeader() const {
    std::cout << "=== " << title_ << " ===\n";
}

void Dashboard::drawFooter() const {
    std::cout << "=== End of Dashboard ===\n";
}

void Dashboard::drawContent() const {
    if (const Session* session = loginService_.session()) {
        std::cout << "Welcome, " << session->user.username() << "!\n";
        std::cout << "Cash: " << session->account.cashBalance() << "\n";
    } else {
        std::cout << "Welcome to the dashboard!\n";
    }
}

void Dashboard::actionDashboard() {
    std::cout << "1. View Account\n";
    std::cout << "2. Deposit Funds\n";
    std::cout << "3. Withdraw Funds\n";
    std::cout << "4. Trade Stocks (Phase 2)\n";
    std::cout << "5. Logout\n";
    std::cout << "Select an option (1-5): ";

    int choice = 0;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input.\n";
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    switch (choice) {
        case 1:
            viewAccount();
            break;
        case 2:
            depositFunds();
            break;
        case 3:
            withdrawFunds();
            break;
        case 4:
            tradeStocksPlaceholder();
            break;
        case 5:
            loginService_.logout();
            std::cout << "Logged out.\n";
            break;
        default:
            std::cout << "Invalid option. Please try again.\n";
            break;
    }
}

void Dashboard::viewAccount() const {
    const Session* session = loginService_.session();
    if (!session) {
        std::cout << "Not logged in.\n";
        return;
    }

    auto balance = bankService_.balance();
    std::cout << "--- Account ---\n";
    std::cout << "User ID   : " << session->user.id() << "\n";
    std::cout << "Username  : " << session->user.username() << "\n";
    std::cout << "Account ID: " << session->account.id() << "\n";
    if (balance.ok()) {
        std::cout << "Cash      : " << balance.value() << "\n";
    } else {
        std::cout << "Cash      : (error) " << balance.message() << "\n";
    }

    auto ledger = bankService_.recentLedger(5);
    if (ledger.ok() && !ledger.value().empty()) {
        std::cout << "Recent ledger:\n";
        for (const auto& entry : ledger.value()) {
            std::cout << "  [" << entry.createdAt << "] " << toString(entry.type)
                      << " amount=" << entry.amount
                      << " balance_after=" << entry.balanceAfter << "\n";
        }
    }
    std::cout << "--------------\n";
}

void Dashboard::depositFunds() {
    std::cout << "Amount to deposit: ";
    double amount = 0.0;
    if (!(std::cin >> amount)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid amount.\n";
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    auto result = bankService_.deposit(amount);
    if (result.ok()) {
        std::cout << "Deposit OK. New cash: " << result.value() << "\n";
    } else {
        std::cout << "Deposit failed: " << result.message() << "\n";
    }
}

void Dashboard::withdrawFunds() {
    std::cout << "Amount to withdraw: ";
    double amount = 0.0;
    if (!(std::cin >> amount)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid amount.\n";
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    auto result = bankService_.withdraw(amount);
    if (result.ok()) {
        std::cout << "Withdraw OK. New cash: " << result.value() << "\n";
    } else {
        std::cout << "Withdraw failed: " << result.message() << "\n";
    }
}

void Dashboard::tradeStocksPlaceholder() const {
    loggerService_.logInfo("Trade Stocks is planned for Phase 2 (MatchingEngine).");
    std::cout << "Trading is not available until Phase 2.\n";
    std::cout << "Order types ready: " << toString(OrderSide::Buy) << "/"
              << toString(OrderSide::Sell) << ", " << toString(OrderType::Market)
              << "/" << toString(OrderType::Limit) << "\n";
}
