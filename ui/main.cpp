#include "application/app_bootstrap.hpp"
#include "bank.h"
#include "dashboard.h"
#include "logger.hpp"
#include "login.h"

#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <limits>
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
    try {
        auto app = AppBootstrap::open(AppBootstrap::defaultDbPath());
        Service::LoggerService& logger = Service::LoggerService::getInstance();
        Service::LoginService loginService{logger, app.auth(), app.accounts()};
        Service::BankAccountService bankService{logger, app.wallet(), loginService};
        Dashboard dashboard{"Trading App Dashboard", loginService, bankService, logger};

        std::signal(SIGINT, handleSignal);
        logger.logInfo("Trading app Phase 1 started (SQLite auth + wallet).");
        logger.logInfo("DB: " + AppBootstrap::defaultDbPath());

        while (g_running) {
            if (!loginService.isLoggedIn()) {
                dashboard.showLoginDashboard();
                std::cout << "1. Login\n2. Register\nSelect (1-2): ";
                int choice = 0;
                if (!(std::cin >> choice)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (choice == 2) {
                    loginService.requestRegister();
                } else {
                    loginService.requestLogin();
                }
                continue;
            }

            dashboard.showDashboard();
            dashboard.actionDashboard();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        logger.logInfo("Shutting down.");
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Fatal: " << ex.what() << "\n";
        return 1;
    }
}
