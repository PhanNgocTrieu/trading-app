#include "controllers/auth_controller.hpp"

namespace desktop {

AuthController::AuthController(AuthAppService& auth,
                               IAccountRepository& accounts,
                               QObject* parent)
    : QObject(parent)
    , auth_(auth)
    , accounts_(accounts) {}

void AuthController::login(const QString& username, const QString& password) {
    auto result = auth_.login(toStd(username), toStd(password));
    if (!result.ok()) {
        emit authFailed(toQ(result.message()));
        return;
    }
    emitSuccess(result.value());
}

void AuthController::registerUser(const QString& username, const QString& password) {
    auto result = auth_.registerUser(toStd(username), toStd(password));
    if (!result.ok()) {
        emit authFailed(toQ(result.message()));
        return;
    }
    emitSuccess(result.value());
}

void AuthController::logout() {
    session_ = {};
    loggedIn_ = false;
    emit loggedOut();
}

void AuthController::emitSuccess(const AuthSession& session) {
    session_ = session;
    loggedIn_ = true;

    SessionDto dto;
    dto.userId = session.userId;
    dto.accountId = session.accountId;
    dto.username = toQ(session.username);
    dto.cashBalance = 0.0;
    if (auto account = accounts_.findById(session.accountId)) {
        dto.cashBalance = account->cashBalance;
    }
    emit loginSucceeded(dto);
}

} // namespace desktop
