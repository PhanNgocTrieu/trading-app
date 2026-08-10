#include "login.h"

#include <iostream>

namespace Service {

LoginService::LoginService(LoggerService& logger,
                           AuthAppService& auth,
                           IAccountRepository& accounts)
    : logger_(logger)
    , auth_(auth)
    , accounts_(accounts) {}

bool LoginService::isLoggedIn() const {
    return loggedIn_ && currentSession().has_value();
}

const Session* LoginService::session() const {
    if (!currentSession()) {
        return nullptr;
    }
    return &(*currentSession());
}

AuthSession LoginService::authSession() const {
    return authSession_;
}

void LoginService::activateSession(const AuthSession& auth) {
    auto account = accounts_.findById(auth.accountId);
    const double cash = account ? account->cashBalance : 0.0;
    currentSession() = Session{
        User{auth.userId, auth.username},
        Account{auth.accountId, auth.userId, cash}};
    authSession_ = auth;
    loggedIn_ = true;
}

void LoginService::syncSessionCash() {
    if (!loggedIn_) {
        return;
    }
    auto account = accounts_.findById(authSession_.accountId);
    if (!account || !currentSession()) {
        return;
    }
    currentSession()->account =
        Account{account->id, account->userId, account->cashBalance};
}

LoginStatus LoginService::loginWithCredentials(const std::string& username,
                                               const std::string& password) {
    auto result = auth_.login(username, password);
    if (!result.ok()) {
        logger_.logWarning("Login failed: " + result.message());
        return LoginStatus::Failure;
    }
    activateSession(result.value());
    logger_.logInfo("Login success for user '" + username + "'");
    return LoginStatus::Success;
}

LoginStatus LoginService::registerWithCredentials(const std::string& username,
                                                  const std::string& password) {
    auto result = auth_.registerUser(username, password);
    if (!result.ok()) {
        logger_.logWarning("Register failed: " + result.message());
        return LoginStatus::Failure;
    }
    activateSession(result.value());
    logger_.logInfo("Register+login success for user '" + username + "'");
    return LoginStatus::Success;
}

LoginStatus LoginService::logout() {
    currentSession().reset();
    loggedIn_ = false;
    authSession_ = {};
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

    const LoginStatus status = loginWithCredentials(username, password);
    std::cout << (status == LoginStatus::Success ? "Login successful!\n" : "Login failed!\n");
    return status;
}

LoginStatus LoginService::requestRegister() {
    std::string username;
    std::string password;

    std::cout << "Choose username (>=3): ";
    std::getline(std::cin, username);
    std::cout << "Choose password (>=6): ";
    std::getline(std::cin, password);

    const LoginStatus status = registerWithCredentials(username, password);
    std::cout << (status == LoginStatus::Success ? "Register successful!\n"
                                                 : "Register failed!\n");
    return status;
}

} // namespace Service
