#pragma once

#include "application/auth_app_service.hpp"
#include "application/ports.hpp"
#include "dto.hpp"

#include <QObject>

namespace desktop {

class AuthController : public QObject {
    Q_OBJECT
public:
    AuthController(AuthAppService& auth, IAccountRepository& accounts, QObject* parent = nullptr);

    bool isLoggedIn() const { return loggedIn_; }
    const AuthSession& authSession() const { return session_; }

public slots:
    void login(const QString& username, const QString& password);
    void registerUser(const QString& username, const QString& password);
    void logout();

signals:
    void loginSucceeded(const desktop::SessionDto& session);
    void authFailed(const QString& message);
    void loggedOut();

private:
    void emitSuccess(const AuthSession& session);

    AuthAppService& auth_;
    IAccountRepository& accounts_;
    AuthSession session_{};
    bool loggedIn_{false};
};

} // namespace desktop
