#ifndef LOGIN_SERVICE_H
#define LOGIN_SERVICE_H

#include "domain/session.hpp"
#include "logger.hpp"
#include "status.h"

#include <string>

namespace Service {

class LoginService {
public:
    explicit LoginService(LoggerService& logger);

    bool isLoggedIn() const;
    const Session* session() const;

    LoginStatus requestLogin();
    LoginStatus logout();

private:
    LoginStatus login(const std::string& username, const std::string& password);

    LoggerService& logger_;
    bool loggedIn_{false};
    int nextUserId_{1};
    int nextAccountId_{1};
};

} // namespace Service

#endif
