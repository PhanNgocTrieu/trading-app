#include "login.h"

namespace Service {

    LOGIN_STATUS LoginService::login(const std::string& username, const std::string& password) {

        if (username.empty() || password.empty()) {
            return LOGIN_FAILURE; // Return failure if username or password is empty
        }

        // Implementation for user login
        return LOGIN_SUCCESS; // Placeholder return value
    }

    LOGIN_STATUS LoginService::logout(const std::string& username) {

        // Logging out of account
        
        // Update Status of signin

        // Implementation for user logout
        return LOGOUT_SUCCESS; // Placeholder return value
    }

    LOGIN_STATUS LoginService::requestLogin() {
        std::string username;
        std::string password;

        std::cout << "Enter username: ";
        std::getline(std::cin, username);
        std::cout << "Enter password: ";
        std::getline(std::cin, password);

        LOGIN_STATUS status = login(username, password);
        if (status == LOGIN_SUCCESS) {
            m_loginStatus = true;
            std::cout << "Login successful!" << std::endl;
        } else {
            m_loginStatus = false;
            std::cout << "Login failed!" << std::endl;
        }
        return status;
    }
    
}