#include "bank.h"

#include "domain/session.hpp"

#include <string>

namespace Service {

BankAccountService::BankAccountService(LoggerService& logger)
    : logger_(logger) {}

Result<double> BankAccountService::deposit(double amount) {
    auto& session = currentSession();
    if (!session) {
        logger_.logError("Deposit failed: no active session");
        return Result<double>::fail(ErrorCode::Unauthorized, "not logged in");
    }

    auto result = session->account.deposit(amount);
    if (!result.ok()) {
        logger_.logWarning("Deposit rejected: " + result.message());
        return Result<double>::fail(result.code(), result.message());
    }

    const double cash = session->account.cashBalance();
    logger_.logInfo("Deposit ok. cash=" + std::to_string(cash));
    return Result<double>::ok(cash);
}

Result<double> BankAccountService::withdraw(double amount) {
    auto& session = currentSession();
    if (!session) {
        logger_.logError("Withdraw failed: no active session");
        return Result<double>::fail(ErrorCode::Unauthorized, "not logged in");
    }

    auto result = session->account.withdraw(amount);
    if (!result.ok()) {
        logger_.logWarning("Withdraw rejected: " + result.message());
        return Result<double>::fail(result.code(), result.message());
    }

    const double cash = session->account.cashBalance();
    logger_.logInfo("Withdraw ok. cash=" + std::to_string(cash));
    return Result<double>::ok(cash);
}

Result<double> BankAccountService::balance() const {
    const auto& session = currentSession();
    if (!session) {
        return Result<double>::fail(ErrorCode::Unauthorized, "not logged in");
    }
    return Result<double>::ok(session->account.cashBalance());
}

} // namespace Service
