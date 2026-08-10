#pragma once

#include "domain/ledger_types.hpp"

#include <optional>
#include <string>
#include <vector>

struct UserRow {
    int id{0};
    std::string username;
    std::string passwordHash;
    std::string phoneNumber;
};

struct AccountRow {
    int id{0};
    int userId{0};
    double cashBalance{0.0};
    std::string status{"ACTIVE"};
    std::string currency{"USD"};
};

struct AuthSession {
    int userId{0};
    int accountId{0};
    std::string username;
};

class IUserRepository {
public:
    virtual ~IUserRepository() = default;
    virtual std::optional<UserRow> findByUsername(const std::string& username) = 0;
    virtual int insertUser(const std::string& username,
                           const std::string& passwordHash,
                           const std::string& phoneNumber = "") = 0;
};

class IAccountRepository {
public:
    virtual ~IAccountRepository() = default;
    virtual int createForUser(int userId, double initialBalance = 0.0) = 0;
    virtual std::optional<AccountRow> findById(int accountId) = 0;
    virtual std::optional<AccountRow> findByUserId(int userId) = 0;
    virtual void updateCash(int accountId, double newBalance) = 0;
};

class ILedgerRepository {
public:
    virtual ~ILedgerRepository() = default;
    virtual void add(int accountId,
                     LedgerType type,
                     double amount,
                     double balanceAfter,
                     const std::string& refType = "",
                     std::optional<int> refId = std::nullopt,
                     const std::string& note = "") = 0;
    virtual std::vector<LedgerEntry> listRecent(int accountId, int limit) = 0;
};

class IPasswordHasher {
public:
    virtual ~IPasswordHasher() = default;
    virtual std::string hash(const std::string& password) = 0;
    virtual bool verify(const std::string& password, const std::string& passwordHash) = 0;
};
