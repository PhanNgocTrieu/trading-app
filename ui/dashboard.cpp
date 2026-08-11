#include "dashboard.h"

#include "domain/order_types.hpp"

#include <iostream>
#include <limits>

Dashboard::Dashboard(std::string title,
                     Service::LoginService& loginService,
                     Service::BankAccountService& bankService,
                     Service::TradingService& tradingService)
    : title_(std::move(title))
    , loginService_(loginService)
    , bankService_(bankService)
    , tradingService_(tradingService) {}

void Dashboard::showLoginDashboard() const {
    std::cout << "=== " << title_ << " ===\n";
    std::cout << "Please login or register (SQLite-backed).\n";
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
    std::cout << "4. Trade Stocks\n";
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
            tradeMenu();
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

void Dashboard::tradeMenu() {
    std::cout << "--- Trade ---\n";
    std::cout << "1. List Quotes\n";
    std::cout << "2. Buy Market\n";
    std::cout << "3. Sell Market\n";
    std::cout << "4. Portfolio\n";
    std::cout << "5. Back\n";
    std::cout << "Select (1-5): ";

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
            listQuotes();
            break;
        case 2:
            buyMarket();
            break;
        case 3:
            sellMarket();
            break;
        case 4:
            viewPortfolio();
            break;
        case 5:
            break;
        default:
            std::cout << "Invalid option.\n";
            break;
    }
}

void Dashboard::listQuotes() const {
    auto quotes = tradingService_.listQuotes();
    if (!quotes.ok()) {
        std::cout << "Failed to load quotes: " << quotes.message() << "\n";
        return;
    }
    std::cout << "Symbol  Name                 Last\n";
    for (const auto& q : quotes.value()) {
        std::cout << q.symbol << "\t" << q.name << "\t" << q.lastPrice << "\n";
    }
}

void Dashboard::buyMarket() {
    std::string symbol;
    int qty = 0;
    std::cout << "Symbol: ";
    std::getline(std::cin, symbol);
    std::cout << "Quantity: ";
    if (!(std::cin >> qty)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid quantity.\n";
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    auto result = tradingService_.buyMarket(symbol, qty);
    if (result.ok()) {
        const auto& order = result.value();
        std::cout << "BUY FILLED order#" << order.orderId << " " << order.symbol
                  << " qty=" << order.quantity << " @ " << order.fillPrice << "\n";
    } else {
        std::cout << "Buy failed: " << result.message() << "\n";
    }
}

void Dashboard::sellMarket() {
    std::string symbol;
    int qty = 0;
    std::cout << "Symbol: ";
    std::getline(std::cin, symbol);
    std::cout << "Quantity: ";
    if (!(std::cin >> qty)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid quantity.\n";
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    auto result = tradingService_.sellMarket(symbol, qty);
    if (result.ok()) {
        const auto& order = result.value();
        std::cout << "SELL FILLED order#" << order.orderId << " " << order.symbol
                  << " qty=" << order.quantity << " @ " << order.fillPrice;
        if (order.realizedPnl) {
            std::cout << " realizedPnL=" << *order.realizedPnl;
        }
        std::cout << "\n";
    } else {
        std::cout << "Sell failed: " << result.message() << "\n";
    }
}

void Dashboard::viewPortfolio() const {
    auto portfolio = tradingService_.portfolio();
    if (!portfolio.ok()) {
        std::cout << "Portfolio failed: " << portfolio.message() << "\n";
        return;
    }
    if (portfolio.value().empty()) {
        std::cout << "No open positions.\n";
        return;
    }

    std::cout << "Symbol  Qty  AvgCost  Last  MktValue  uPnL\n";
    for (const auto& p : portfolio.value()) {
        std::cout << p.symbol << "  " << p.quantity << "  " << p.avgCost << "  "
                  << p.marketPrice << "  " << p.marketValue << "  " << p.unrealizedPnl << "\n";
    }

    auto trades = tradingService_.recentTrades(5);
    if (trades.ok() && !trades.value().empty()) {
        std::cout << "Recent trades:\n";
        for (const auto& t : trades.value()) {
            std::cout << "  #" << t.id << " " << toString(t.side) << " " << t.symbol
                      << " qty=" << t.quantity << " @ " << t.price << "\n";
        }
    }
}
