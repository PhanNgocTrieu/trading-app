#include "login.h"

#include <iostream>

namespace Service {

LoginService::LoginService(LoggerService& logger)
    : logger_(logger) {}

bool LoginService::isLoggedIn() const {
    return loggedIn_ && currentSession().has_value();
}

const Session* LoginService::session() const {
    if (!currentSession()) {
        return nullptr;
    }
    return &(*currentSession());
}

LoginStatus LoginService::login(const std::string& username, const std::string& password) {
    if (username.empty() || password.empty()) {
        logger_.logWarning("Login rejected: empty username or password");
        return LoginStatus::Failure;
    }

    // Phase 0: accept any non-empty credentials (no DB yet).
    User user{nextUserId_++, username};
    Account account{nextAccountId_++, user.id(), 0.0};
    currentSession() = Session{std::move(user), std::move(account)};
    loggedIn_ = true;

    logger_.logInfo("Login success for user '" + username + "'");
    return LoginStatus::Success;
}

LoginStatus LoginService::logout() {
    currentSession().reset();
    loggedIn_ = false;
    logger_.logInfo("Logout success");
    return LoginStatus::LogoutSuccess;
}

LoginStatus LoginService::requestLogin() {
    std::string username;
    std::string password;

    std::cout << "Enter username: ";
    std::getline(std::cin, username);
    std::cout << "Enter password: ";
    std::getline(std::cin, password);

    const LoginStatus status = login(username, password);
    if (status == LoginStatus::Success) {
        std::cout << "Login successful!\n";
    } else {
        std::cout << "Login failed!\n";
    }
    return status;
}

} // namespace Service
