#pragma once

#include "domain/ledger_types.hpp"
#include "domain/order_types.hpp"

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

struct QuoteRow {
    std::string symbol;
    std::string name;
    double lastPrice{0.0};
};

struct PositionRow {
    int accountId{0};
    std::string symbol;
    int quantity{0};
    double avgCost{0.0};
};

struct OrderRow {
    int id{0};
    int accountId{0};
    std::string symbol;
    OrderSide side{OrderSide::Buy};
    OrderType type{OrderType::Market};
    int quantity{0};
    std::optional<double> limitPrice;
    OrderStatus status{OrderStatus::Pending};
};

struct TradeRow {
    int id{0};
    int orderId{0};
    int accountId{0};
    std::string symbol;
    OrderSide side{OrderSide::Buy};
    int quantity{0};
    double price{0.0};
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

class IQuoteRepository {
public:
    virtual ~IQuoteRepository() = default;
    virtual std::optional<QuoteRow> find(const std::string& symbol) = 0;
    virtual std::vector<QuoteRow> listAll() = 0;
    virtual void setLastPrice(const std::string& symbol, double lastPrice) = 0;
};

class IPositionRepository {
public:
    virtual ~IPositionRepository() = default;
    virtual std::optional<PositionRow> find(int accountId, const std::string& symbol) = 0;
    virtual std::vector<PositionRow> listByAccount(int accountId) = 0;
    virtual void upsert(int accountId, const std::string& symbol, int quantity, double avgCost) = 0;
};

class IOrderRepository {
public:
    virtual ~IOrderRepository() = default;
    virtual int insert(int accountId,
                       const std::string& symbol,
                       OrderSide side,
                       OrderType type,
                       int quantity,
                       std::optional<double> limitPrice,
                       OrderStatus status) = 0;
    virtual std::vector<OrderRow> listRecent(int accountId, int limit) = 0;
};

class ITradeRepository {
public:
    virtual ~ITradeRepository() = default;
    virtual int insert(int orderId,
                       int accountId,
                       const std::string& symbol,
                       OrderSide side,
                       int quantity,
                       double price) = 0;
    virtual std::vector<TradeRow> listRecent(int accountId, int limit) = 0;
    virtual int countByAccount(int accountId) = 0;
};

class IPasswordHasher {
public:
    virtual ~IPasswordHasher() = default;
    virtual std::string hash(const std::string& password) = 0;
    virtual bool verify(const std::string& password, const std::string& passwordHash) = 0;
};
