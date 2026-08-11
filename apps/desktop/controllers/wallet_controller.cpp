#include "controllers/wallet_controller.hpp"

namespace desktop {

WalletController::WalletController(WalletAppService& wallet,
                                   AuthController& auth,
                                   QObject* parent)
    : QObject(parent)
    , wallet_(wallet)
    , auth_(auth) {}

void WalletController::deposit(double amount) {
    if (!auth_.isLoggedIn()) {
        emit walletFailed(QStringLiteral("not logged in"));
        return;
    }
    auto result = wallet_.deposit(auth_.authSession(), amount);
    if (!result.ok()) {
        emit walletFailed(toQ(result.message()));
        return;
    }
    emit cashUpdated(result.value());
    emit walletSucceeded(QStringLiteral("Deposit OK"));
}

void WalletController::withdraw(double amount) {
    if (!auth_.isLoggedIn()) {
        emit walletFailed(QStringLiteral("not logged in"));
        return;
    }
    auto result = wallet_.withdraw(auth_.authSession(), amount);
    if (!result.ok()) {
        emit walletFailed(toQ(result.message()));
        return;
    }
    emit cashUpdated(result.value());
    emit walletSucceeded(QStringLiteral("Withdraw OK"));
}

void WalletController::refreshCash() {
    if (!auth_.isLoggedIn()) {
        return;
    }
    auto result = wallet_.balance(auth_.authSession());
    if (!result.ok()) {
        emit walletFailed(toQ(result.message()));
        return;
    }
    emit cashUpdated(result.value());
}

} // namespace desktop
