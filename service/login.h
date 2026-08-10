#ifndef LOGIN_SERVICE_H
#define LOGIN_SERVICE_H

#include "application/auth_app_service.hpp"
#include "application/ports.hpp"
#include "domain/session.hpp"
#include "logger.hpp"
#include "status.h"

#include <string>

namespace Service {

class LoginService {
public:
    LoginService(LoggerService& logger,
                 AuthAppService& auth,
                 IAccountRepository& accounts);

    bool isLoggedIn() const;
    const Session* session() const;
    AuthSession authSession() const;

    LoginStatus loginWithCredentials(const std::string& username,
                                     const std::string& password);
    LoginStatus registerWithCredentials(const std::string& username,
                                        const std::string& password);
    LoginStatus requestLogin();
    LoginStatus requestRegister();
    LoginStatus logout();

    // Refresh cached domain Session cash from DB.
    void syncSessionCash();

private:
    void activateSession(const AuthSession& auth);

    LoggerService& logger_;
    AuthAppService& auth_;
    IAccountRepository& accounts_;
    bool loggedIn_{false};
    AuthSession authSession_{};
};

} // namespace Service

#endif
