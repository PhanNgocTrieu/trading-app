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

class SqliteQuoteRepository final : public IQuoteRepository {
public:
    explicit SqliteQuoteRepository(SqliteConnection& db);
    std::optional<QuoteRow> find(const std::string& symbol) override;
    std::vector<QuoteRow> listAll() override;
    void setLastPrice(const std::string& symbol, double lastPrice) override;

private:
    SqliteConnection& db_;
};

class SqlitePositionRepository final : public IPositionRepository {
public:
    explicit SqlitePositionRepository(SqliteConnection& db);
    std::optional<PositionRow> find(int accountId, const std::string& symbol) override;
    std::vector<PositionRow> listByAccount(int accountId) override;
    void upsert(int accountId, const std::string& symbol, int quantity, double avgCost) override;

private:
    SqliteConnection& db_;
};

class SqliteOrderRepository final : public IOrderRepository {
public:
    explicit SqliteOrderRepository(SqliteConnection& db);
    int insert(int accountId,
               const std::string& symbol,
               OrderSide side,
               OrderType type,
               int quantity,
               std::optional<double> limitPrice,
               OrderStatus status) override;
    std::optional<OrderRow> findById(int orderId) override;
    std::vector<OrderRow> listRecent(int accountId, int limit) override;
    std::vector<OrderRow> listPendingByAccount(int accountId) override;
    std::vector<OrderRow> listPendingBySymbol(const std::string& symbol) override;
    void updateStatus(int orderId, OrderStatus status) override;

private:
    SqliteConnection& db_;
};

class SqliteTradeRepository final : public ITradeRepository {
public:
    explicit SqliteTradeRepository(SqliteConnection& db);
    int insert(int orderId,
               int accountId,
               const std::string& symbol,
               OrderSide side,
               int quantity,
               double price) override;
    std::vector<TradeRow> listRecent(int accountId, int limit) override;
    int countByAccount(int accountId) override;

private:
    SqliteConnection& db_;
};
