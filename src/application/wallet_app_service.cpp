#include "application/wallet_app_service.hpp"

#include "infrastructure/db/transaction.hpp"

#include <exception>

WalletAppService::WalletAppService(SqliteConnection& db,
                                   IAccountRepository& accounts,
                                   ILedgerRepository& ledger)
    : db_(db)
    , accounts_(accounts)
    , ledger_(ledger) {}

Result<double> WalletAppService::deposit(const AuthSession& session, double amount) {
    if (amount <= 0.0) {
        return Result<double>::fail(ErrorCode::ValidationFailed, "amount must be > 0");
    }

    try {
        Transaction tx(db_, true);
        auto account = accounts_.findById(session.accountId);
        if (!account) {
            return Result<double>::fail(ErrorCode::NotFound, "account not found");
        }
        if (account->status != "ACTIVE") {
            return Result<double>::fail(ErrorCode::ValidationFailed, "account not active");
        }

        const double next = account->cashBalance + amount;
        accounts_.updateCash(account->id, next);
        ledger_.add(account->id, LedgerType::Deposit, amount, next, "manual");
        tx.commit();
        return Result<double>::ok(next);
    } catch (const std::exception& ex) {
        return Result<double>::fail(ErrorCode::DbError, ex.what());
    }
}

Result<double> WalletAppService::withdraw(const AuthSession& session, double amount) {
    if (amount <= 0.0) {
        return Result<double>::fail(ErrorCode::ValidationFailed, "amount must be > 0");
    }

    try {
        Transaction tx(db_, true);
        auto account = accounts_.findById(session.accountId);
        if (!account) {
            return Result<double>::fail(ErrorCode::NotFound, "account not found");
        }
        if (account->status != "ACTIVE") {
            return Result<double>::fail(ErrorCode::ValidationFailed, "account not active");
        }
        if (account->cashBalance < amount) {
            return Result<double>::fail(ErrorCode::InsufficientFunds, "insufficient funds");
        }

        const double next = account->cashBalance - amount;
        accounts_.updateCash(account->id, next);
        ledger_.add(account->id, LedgerType::Withdraw, -amount, next, "manual");
        tx.commit();
        return Result<double>::ok(next);
    } catch (const std::exception& ex) {
        return Result<double>::fail(ErrorCode::DbError, ex.what());
    }
}

Result<double> WalletAppService::balance(const AuthSession& session) const {
    try {
        auto account = accounts_.findById(session.accountId);
        if (!account) {
            return Result<double>::fail(ErrorCode::NotFound, "account not found");
        }
        return Result<double>::ok(account->cashBalance);
    } catch (const std::exception& ex) {
        return Result<double>::fail(ErrorCode::DbError, ex.what());
    }
}

Result<std::vector<LedgerEntry>> WalletAppService::recentLedger(const AuthSession& session,
                                                                int limit) const {
    try {
        return Result<std::vector<LedgerEntry>>::ok(ledger_.listRecent(session.accountId, limit));
    } catch (const std::exception& ex) {
        return Result<std::vector<LedgerEntry>>::fail(ErrorCode::DbError, ex.what());
    }
}
