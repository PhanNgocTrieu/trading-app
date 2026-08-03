#include <iostream>
#include "login.h"
#include "logger.hpp"
#include <signal.h>
#include <atomic>
#include <thread>
#include <chrono>
#include "dashboard.h"

using namespace std;

std::atomic<bool> g_running(true);

void handleSignal(int signal) {
    if (signal == SIGINT) {
        g_running = false;
    }
}


int main() {
    // Setup signal handler for graceful shutdown
    Service::LoginService& loginService = Service::LoginService::getInstance();
    Service::LoggerService& loggerService = Service::LoggerService::getInstance();
    Dashboard& dashboard = Dashboard::getInstance("Trading App Dashboard");

    // connect signal handler
    signal(SIGINT, handleSignal);


    while (g_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        system("clear"); // Clear the console for better readability.

        do {
            if (!loginService.getLoginStatus()) {
                loggerService.logInfo("User is not logged in. Please log in to access the dashboard.");
                dashboard.showLoginDashboard();
                loginService.requestLogin();
                break; // Exit the do-while loop if not logged in
            }
            
            dashboard.showDashboard();
            dashboard.actionDashboard();
        } while (0);
        // Simulate some work
        
    }

    return 0;
}