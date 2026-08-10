#include "bank.h"

#include <string>

namespace Service {

BankAccountService::BankAccountService(LoggerService& logger,
                                       WalletAppService& wallet,
                                       LoginService& login)
    : logger_(logger)
    , wallet_(wallet)
    , login_(login) {}

Result<double> BankAccountService::deposit(double amount) {
    if (!login_.isLoggedIn()) {
        logger_.logError("Deposit failed: no active session");
        return Result<double>::fail(ErrorCode::Unauthorized, "not logged in");
    }

    auto result = wallet_.deposit(login_.authSession(), amount);
    if (!result.ok()) {
        logger_.logWarning("Deposit rejected: " + result.message());
        return result;
    }
    login_.syncSessionCash();
    logger_.logInfo("Deposit ok. cash=" + std::to_string(result.value()));
    return result;
}

Result<double> BankAccountService::withdraw(double amount) {
    if (!login_.isLoggedIn()) {
        logger_.logError("Withdraw failed: no active session");
        return Result<double>::fail(ErrorCode::Unauthorized, "not logged in");
    }

    auto result = wallet_.withdraw(login_.authSession(), amount);
    if (!result.ok()) {
        logger_.logWarning("Withdraw rejected: " + result.message());
        return result;
    }
    login_.syncSessionCash();
    logger_.logInfo("Withdraw ok. cash=" + std::to_string(result.value()));
    return result;
}

Result<double> BankAccountService::balance() const {
    if (!login_.isLoggedIn()) {
        return Result<double>::fail(ErrorCode::Unauthorized, "not logged in");
    }
    return wallet_.balance(login_.authSession());
}

Result<std::vector<LedgerEntry>> BankAccountService::recentLedger(int limit) const {
    if (!login_.isLoggedIn()) {
        return Result<std::vector<LedgerEntry>>::fail(ErrorCode::Unauthorized, "not logged in");
    }
    return wallet_.recentLedger(login_.authSession(), limit);
}

} // namespace Service
