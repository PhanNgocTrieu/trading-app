#include "dashboard.h"

void Dashboard::showDashboard() {
    drawHeader();
    drawContent();
    drawFooter();
}

void Dashboard::drawHeader() {
    std::cout << "=== " << m_dashboardTitle << " ===" << std::endl;
}

void Dashboard::drawFooter() {
    std::cout << "=== End of Dashboard ===" << std::endl;
}

void Dashboard::drawContent() {
    std::cout << "Welcome to the dashboard!" << std::endl;
    // Additional content can be added here
}

void Dashboard::actionDashboard() {
    // Placeholder for dashboard actions
    std::cout << "Performing dashboard actions..." << std::endl;
    std::cout << "1. View Account" << std::endl;
    std::cout << "2. Deposit Funds" << std::endl;
    std::cout << "3. Withdraw Funds" << std::endl;
    std::cout << "4. Trade Stocks" << std::endl;
    std::cout << "Select an option (1-4): ";
    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    switch (choice)
    {
        case 1: {
            /* code */
            break;
        }
        case 2: {
            /* code */
            break;
        }
        case 3: {
            /* code */
            break;
        }
        case 4: {
            /* code */
            break;
        }
        default: {
            std::cout << "Invalid option. Please try again." << std::endl;
            break;
        }
    }
}