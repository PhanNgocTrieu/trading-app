#ifndef BANK_SERVICE_H
#define BANK_SERVICE_H

#include "application/wallet_app_service.hpp"
#include "domain/ledger_types.hpp"
#include "domain/result.hpp"
#include "logger.hpp"
#include "login.h"

#include <vector>

namespace Service {

class BankAccountService {
public:
    BankAccountService(LoggerService& logger,
                       WalletAppService& wallet,
                       LoginService& login);

    [[nodiscard]] Result<double> deposit(double amount);
    [[nodiscard]] Result<double> withdraw(double amount);
    [[nodiscard]] Result<double> balance() const;
    [[nodiscard]] Result<std::vector<LedgerEntry>> recentLedger(int limit = 5) const;

private:
    LoggerService& logger_;
    WalletAppService& wallet_;
    LoginService& login_;
};

} // namespace Service

#endif
