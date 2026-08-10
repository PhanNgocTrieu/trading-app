#pragma once

#include "application/ports.hpp"
#include "domain/ledger_types.hpp"
#include "domain/result.hpp"
#include "infrastructure/db/sqlite_connection.hpp"

#include <vector>

class WalletAppService {
public:
    WalletAppService(SqliteConnection& db,
                     IAccountRepository& accounts,
                     ILedgerRepository& ledger);

    [[nodiscard]] Result<double> deposit(const AuthSession& session, double amount);
    [[nodiscard]] Result<double> withdraw(const AuthSession& session, double amount);
    [[nodiscard]] Result<double> balance(const AuthSession& session) const;
    [[nodiscard]] Result<std::vector<LedgerEntry>> recentLedger(const AuthSession& session,
                                                                int limit = 5) const;

private:
    SqliteConnection& db_;
    IAccountRepository& accounts_;
    ILedgerRepository& ledger_;
};
