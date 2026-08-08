#include "bank.h"
#include "dashboard.h"
#include "logger.hpp"
#include "login.h"

#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>

namespace {
std::atomic<bool> g_running{true};

void handleSignal(int signal) {
    if (signal == SIGINT) {
        g_running = false;
    }
}
} // namespace

int main() {
    Service::LoggerService& logger = Service::LoggerService::getInstance();
    Service::LoginService loginService{logger};
    Service::BankAccountService bankService{logger};
    Dashboard dashboard{"Trading App Dashboard", loginService, bankService, logger};

    std::signal(SIGINT, handleSignal);
    logger.logInfo("Trading app Phase 0 started (C++17, in-memory domain).");

    while (g_running) {
        if (!loginService.isLoggedIn()) {
            logger.logInfo("User is not logged in. Please log in to access the dashboard.");
            dashboard.showLoginDashboard();
            loginService.requestLogin();
            continue;
        }

        dashboard.showDashboard();
        dashboard.actionDashboard();

        // Small pause so the console is readable between actions.
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    logger.logInfo("Shutting down.");
    return 0;
}
