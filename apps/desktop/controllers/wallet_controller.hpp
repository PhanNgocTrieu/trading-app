#pragma once

#include "application/ports.hpp"
#include "application/wallet_app_service.hpp"
#include "controllers/auth_controller.hpp"
#include "dto.hpp"

#include <QObject>

namespace desktop {

class WalletController : public QObject {
    Q_OBJECT
public:
    WalletController(WalletAppService& wallet,
                     AuthController& auth,
                     QObject* parent = nullptr);

public slots:
    void deposit(double amount);
    void withdraw(double amount);
    void refreshCash();

signals:
    void cashUpdated(double cash);
    void walletSucceeded(const QString& message);
    void walletFailed(const QString& message);

private:
    WalletAppService& wallet_;
    AuthController& auth_;
};

} // namespace desktop
