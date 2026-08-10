#pragma once

#include "application/ports.hpp"
#include "infrastructure/db/sqlite_connection.hpp"

class SqliteUserRepository final : public IUserRepository {
public:
    explicit SqliteUserRepository(SqliteConnection& db);
    std::optional<UserRow> findByUsername(const std::string& username) override;
    int insertUser(const std::string& username,
                   const std::string& passwordHash,
                   const std::string& phoneNumber = "") override;

private:
    SqliteConnection& db_;
};

class SqliteAccountRepository final : public IAccountRepository {
public:
    explicit SqliteAccountRepository(SqliteConnection& db);
    int createForUser(int userId, double initialBalance = 0.0) override;
    std::optional<AccountRow> findById(int accountId) override;
    std::optional<AccountRow> findByUserId(int userId) override;
    void updateCash(int accountId, double newBalance) override;

private:
    SqliteConnection& db_;
};

class SqliteLedgerRepository final : public ILedgerRepository {
public:
    explicit SqliteLedgerRepository(SqliteConnection& db);
    void add(int accountId,
             LedgerType type,
             double amount,
             double balanceAfter,
             const std::string& refType = "",
             std::optional<int> refId = std::nullopt,
             const std::string& note = "") override;
    std::vector<LedgerEntry> listRecent(int accountId, int limit) override;

private:
    SqliteConnection& db_;
};
