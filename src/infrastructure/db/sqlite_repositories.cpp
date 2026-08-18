#include "infrastructure/db/sqlite_repositories.hpp"

#include <stdexcept>

namespace {

void throwPrepareError(sqlite3* db, const char* what) {
    throw std::runtime_error(std::string(what) + ": " + sqlite3_errmsg(db));
}

OrderRow orderFromStatement(sqlite3_stmt* stmt) {
    OrderRow order;
    order.id = sqlite3_column_int(stmt, 0);
    order.accountId = sqlite3_column_int(stmt, 1);
    order.symbol = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    order.side = sideFromString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
    order.type = typeFromString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
    order.quantity = sqlite3_column_int(stmt, 5);
    if (sqlite3_column_type(stmt, 6) != SQLITE_NULL) {
        order.limitPrice = sqlite3_column_double(stmt, 6);
    }
    order.status = statusFromString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7)));
    return order;
}

std::vector<OrderRow> collectOrders(sqlite3_stmt* stmt) {
    std::vector<OrderRow> rows;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        rows.push_back(orderFromStatement(stmt));
    }
    sqlite3_finalize(stmt);
    return rows;
}

} // namespace

SqliteUserRepository::SqliteUserRepository(SqliteConnection& db)
    : db_(db) {}

std::optional<UserRow> SqliteUserRepository::findByUsername(const std::string& username) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "SELECT id, username, password_hash, phone_number FROM users WHERE username = ?;";
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throwPrepareError(db_.handle(), "prepare findByUsername");
    }
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<UserRow> row;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        UserRow user;
        user.id = sqlite3_column_int(stmt, 0);
        user.username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        user.passwordHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        const auto* phone = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        user.phoneNumber = phone ? phone : "";
        row = std::move(user);
    }
    sqlite3_finalize(stmt);
    return row;
}

int SqliteUserRepository::insertUser(const std::string& username,
                                     const std::string& passwordHash,
                                     const std::string& phoneNumber) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "INSERT INTO users(username, password_hash, phone_number) VALUES (?, ?, ?);";
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throwPrepareError(db_.handle(), "prepare insertUser");
    }
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, passwordHash.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, phoneNumber.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throwPrepareError(db_.handle(), "insertUser");
    }
    sqlite3_finalize(stmt);
    return static_cast<int>(db_.lastInsertRowId());
}

SqliteAccountRepository::SqliteAccountRepository(SqliteConnection& db)
    : db_(db) {}

int SqliteAccountRepository::createForUser(int userId, double initialBalance) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "INSERT INTO accounts(user_id, cash_balance) VALUES (?, ?);";
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throwPrepareError(db_.handle(), "prepare createForUser");
    }
    sqlite3_bind_int(stmt, 1, userId);
    sqlite3_bind_double(stmt, 2, initialBalance);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throwPrepareError(db_.handle(), "createForUser");
    }
    sqlite3_finalize(stmt);
    return static_cast<int>(db_.lastInsertRowId());
}

std::optional<AccountRow> SqliteAccountRepository::findById(int accountId) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "SELECT id, user_id, cash_balance, status, currency FROM accounts WHERE id = ?;";
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throwPrepareError(db_.handle(), "prepare findById");
    }
    sqlite3_bind_int(stmt, 1, accountId);

    std::optional<AccountRow> row;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        AccountRow account;
        account.id = sqlite3_column_int(stmt, 0);
        account.userId = sqlite3_column_int(stmt, 1);
        account.cashBalance = sqlite3_column_double(stmt, 2);
        account.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        account.currency = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        row = std::move(account);
    }
    sqlite3_finalize(stmt);
    return row;
}

std::optional<AccountRow> SqliteAccountRepository::findByUserId(int userId) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "SELECT id, user_id, cash_balance, status, currency FROM accounts WHERE user_id = ?;";
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throwPrepareError(db_.handle(), "prepare findByUserId");
    }
    sqlite3_bind_int(stmt, 1, userId);

    std::optional<AccountRow> row;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        AccountRow account;
        account.id = sqlite3_column_int(stmt, 0);
        account.userId = sqlite3_column_int(stmt, 1);
        account.cashBalance = sqlite3_column_double(stmt, 2);
        account.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        account.currency = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        row = std::move(account);
    }
    sqlite3_finalize(stmt);
    return row;
}

void SqliteAccountRepository::updateCash(int accountId, double newBalance) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "UPDATE accounts SET cash_balance = ? WHERE id = ?;";
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throwPrepareError(db_.handle(), "prepare updateCash");
    }
    sqlite3_bind_double(stmt, 1, newBalance);
    sqlite3_bind_int(stmt, 2, accountId);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throwPrepareError(db_.handle(), "updateCash");
    }
    sqlite3_finalize(stmt);
}

SqliteLedgerRepository::SqliteLedgerRepository(SqliteConnection& db)
    : db_(db) {}

void SqliteLedgerRepository::add(int accountId,
                                 LedgerType type,
                                 double amount,
                                 double balanceAfter,
                                 const std::string& refType,
                                 std::optional<int> refId,
                                 const std::string& note) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "INSERT INTO ledger_entries(account_id, type, amount, balance_after, ref_type, ref_id, note) "
        "VALUES (?, ?, ?, ?, ?, ?, ?);";
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throwPrepareError(db_.handle(), "prepare ledger add");
    }
    sqlite3_bind_int(stmt, 1, accountId);
    sqlite3_bind_text(stmt, 2, toString(type), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, amount);
    sqlite3_bind_double(stmt, 4, balanceAfter);
    if (refType.empty()) {
        sqlite3_bind_null(stmt, 5);
    } else {
        sqlite3_bind_text(stmt, 5, refType.c_str(), -1, SQLITE_TRANSIENT);
    }
    if (refId) {
        sqlite3_bind_int(stmt, 6, *refId);
    } else {
        sqlite3_bind_null(stmt, 6);
    }
    sqlite3_bind_text(stmt, 7, note.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throwPrepareError(db_.handle(), "ledger add");
    }
    sqlite3_finalize(stmt);
}

std::vector<LedgerEntry> SqliteLedgerRepository::listRecent(int accountId, int limit) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "SELECT id, account_id, type, amount, balance_after, ref_type, ref_id, note, created_at "
        "FROM ledger_entries WHERE account_id = ? ORDER BY id DESC LIMIT ?;";
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throwPrepareError(db_.handle(), "prepare listRecent");
    }
    sqlite3_bind_int(stmt, 1, accountId);
    sqlite3_bind_int(stmt, 2, limit);

    std::vector<LedgerEntry> entries;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        LedgerEntry entry;
        entry.id = sqlite3_column_int(stmt, 0);
        entry.accountId = sqlite3_column_int(stmt, 1);
        entry.type = ledgerTypeFromString(
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
        entry.amount = sqlite3_column_double(stmt, 3);
        entry.balanceAfter = sqlite3_column_double(stmt, 4);
        if (sqlite3_column_type(stmt, 5) != SQLITE_NULL) {
            entry.refType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        }
        if (sqlite3_column_type(stmt, 6) != SQLITE_NULL) {
            entry.refId = sqlite3_column_int(stmt, 6);
            entry.hasRefId = true;
        }
        if (sqlite3_column_type(stmt, 7) != SQLITE_NULL) {
            entry.note = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        }
        entry.createdAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        entries.push_back(std::move(entry));
    }
    sqlite3_finalize(stmt);
    return entries;
}

SqliteQuoteRepository::SqliteQuoteRepository(SqliteConnection& db)
    : db_(db) {}

std::optional<QuoteRow> SqliteQuoteRepository::find(const std::string& symbol) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "SELECT q.symbol, s.name, q.last_price "
        "FROM market_quotes q JOIN stocks s ON s.symbol = q.symbol "
        "WHERE q.symbol = ?;";
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throwPrepareError(db_.handle(), "prepare quote find");
    }
    sqlite3_bind_text(stmt, 1, symbol.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<QuoteRow> row;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        QuoteRow quote;
        quote.symbol = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        quote.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        quote.lastPrice = sqlite3_column_double(stmt, 2);
        row = std::move(quote);
    }
    sqlite3_finalize(stmt);
    return row;
}

std::vector<QuoteRow> SqliteQuoteRepository::listAll() {
    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "SELECT q.symbol, s.name, q.last_price "
        "FROM market_quotes q JOIN stocks s ON s.symbol = q.symbol "
        "ORDER BY q.symbol;";
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throwPrepareError(db_.handle(), "prepare quote listAll");
    }

    std::vector<QuoteRow> rows;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        QuoteRow quote;
        quote.symbol = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        quote.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        quote.lastPrice = sqlite3_column_double(stmt, 2);
        rows.push_back(std::move(quote));
    }
    sqlite3_finalize(stmt);
    return rows;
}

void SqliteQuoteRepository::setLastPrice(const std::string& symbol, double lastPrice) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "UPDATE market_quotes SET last_price = ?, updated_at = datetime('now') "
        "WHERE symbol = ?;";
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throwPrepareError(db_.handle(), "prepare setLastPrice");
    }
    sqlite3_bind_double(stmt, 1, lastPrice);
    sqlite3_bind_text(stmt, 2, symbol.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throwPrepareError(db_.handle(), "setLastPrice");
    }
    const int changes = sqlite3_changes(db_.handle());
    sqlite3_finalize(stmt);
    if (changes == 0) {
        throw std::runtime_error("setLastPrice: unknown symbol " + symbol);
    }
}

SqlitePositionRepository::SqlitePositionRepository(SqliteConnection& db)
    : db_(db) {}

std::optional<PositionRow> SqlitePositionRepository::find(int accountId,
                                                          const std::string& symbol) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "SELECT account_id, symbol, quantity, avg_cost FROM positions "
        "WHERE account_id = ? AND symbol = ?;";
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throwPrepareError(db_.handle(), "prepare position find");
    }
    sqlite3_bind_int(stmt, 1, accountId);
    sqlite3_bind_text(stmt, 2, symbol.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<PositionRow> row;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        PositionRow position;
        position.accountId = sqlite3_column_int(stmt, 0);
        position.symbol = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        position.quantity = sqlite3_column_int(stmt, 2);
        position.avgCost = sqlite3_column_double(stmt, 3);
        row = std::move(position);
    }
    sqlite3_finalize(stmt);
    return row;
}

std::vector<PositionRow> SqlitePositionRepository::listByAccount(int accountId) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "SELECT account_id, symbol, quantity, avg_cost FROM positions "
        "WHERE account_id = ? AND quantity > 0 ORDER BY symbol;";
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throwPrepareError(db_.handle(), "prepare position listByAccount");
    }
    sqlite3_bind_int(stmt, 1, accountId);

    std::vector<PositionRow> rows;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        PositionRow position;
        position.accountId = sqlite3_column_int(stmt, 0);
        position.symbol = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        position.quantity = sqlite3_column_int(stmt, 2);
        position.avgCost = sqlite3_column_double(stmt, 3);
        rows.push_back(std::move(position));
    }
    sqlite3_finalize(stmt);
    return rows;
}

void SqlitePositionRepository::upsert(int accountId,
                                      const std::string& symbol,
                                      int quantity,
                                      double avgCost) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "INSERT INTO positions(account_id, symbol, quantity, avg_cost, updated_at) "
        "VALUES (?, ?, ?, ?, datetime('now')) "
        "ON CONFLICT(account_id, symbol) DO UPDATE SET "
        "quantity = excluded.quantity, "
        "avg_cost = excluded.avg_cost, "
        "updated_at = datetime('now');";
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throwPrepareError(db_.handle(), "prepare position upsert");
    }
    sqlite3_bind_int(stmt, 1, accountId);
    sqlite3_bind_text(stmt, 2, symbol.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, quantity);
    sqlite3_bind_double(stmt, 4, avgCost);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throwPrepareError(db_.handle(), "position upsert");
    }
    sqlite3_finalize(stmt);
}

SqliteOrderRepository::SqliteOrderRepository(SqliteConnection& db)
    : db_(db) {}

int SqliteOrderRepository::insert(int accountId,
                                  const std::string& symbol,
                                  OrderSide side,
                                  OrderType type,
                                  int quantity,
                                  std::optional<double> limitPrice,
                                  OrderStatus status) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "INSERT INTO orders(account_id, symbol, side, type, quantity, limit_price, status) "
        "VALUES (?, ?, ?, ?, ?, ?, ?);";
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throwPrepareError(db_.handle(), "prepare order insert");
    }
    sqlite3_bind_int(stmt, 1, accountId);
    sqlite3_bind_text(stmt, 2, symbol.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, toString(side), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, toString(type), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, quantity);
    if (limitPrice) {
        sqlite3_bind_double(stmt, 6, *limitPrice);
    } else {
        sqlite3_bind_null(stmt, 6);
    }
    sqlite3_bind_text(stmt, 7, toString(status), -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throwPrepareError(db_.handle(), "order insert");
    }
    sqlite3_finalize(stmt);
    return static_cast<int>(db_.lastInsertRowId());
}

std::optional<OrderRow> SqliteOrderRepository::findById(int orderId) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "SELECT id, account_id, symbol, side, type, quantity, limit_price, status "
        "FROM orders WHERE id = ?;";
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throwPrepareError(db_.handle(), "prepare order findById");
    }
    sqlite3_bind_int(stmt, 1, orderId);

    std::optional<OrderRow> row;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        row = orderFromStatement(stmt);
    }
    sqlite3_finalize(stmt);
    return row;
}

std::vector<OrderRow> SqliteOrderRepository::listRecent(int accountId, int limit) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "SELECT id, account_id, symbol, side, type, quantity, limit_price, status "
        "FROM orders WHERE account_id = ? ORDER BY id DESC LIMIT ?;";
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throwPrepareError(db_.handle(), "prepare order listRecent");
    }
    sqlite3_bind_int(stmt, 1, accountId);
    sqlite3_bind_int(stmt, 2, limit);
    return collectOrders(stmt);
}

std::vector<OrderRow> SqliteOrderRepository::listPendingByAccount(int accountId) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "SELECT id, account_id, symbol, side, type, quantity, limit_price, status "
        "FROM orders WHERE account_id = ? AND status = 'PENDING' ORDER BY id ASC;";
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throwPrepareError(db_.handle(), "prepare listPendingByAccount");
    }
    sqlite3_bind_int(stmt, 1, accountId);
    return collectOrders(stmt);
}

std::vector<OrderRow> SqliteOrderRepository::listPendingBySymbol(const std::string& symbol) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "SELECT id, account_id, symbol, side, type, quantity, limit_price, status "
        "FROM orders WHERE symbol = ? AND status = 'PENDING' ORDER BY id ASC;";
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throwPrepareError(db_.handle(), "prepare listPendingBySymbol");
    }
    sqlite3_bind_text(stmt, 1, symbol.c_str(), -1, SQLITE_TRANSIENT);
    return collectOrders(stmt);
}

void SqliteOrderRepository::updateStatus(int orderId, OrderStatus status) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "UPDATE orders SET status = ?, updated_at = datetime('now') WHERE id = ?;";
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throwPrepareError(db_.handle(), "prepare order updateStatus");
    }
    sqlite3_bind_text(stmt, 1, toString(status), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, orderId);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throwPrepareError(db_.handle(), "order updateStatus");
    }
    sqlite3_finalize(stmt);
}

SqliteTradeRepository::SqliteTradeRepository(SqliteConnection& db)
    : db_(db) {}

int SqliteTradeRepository::insert(int orderId,
                                  int accountId,
                                  const std::string& symbol,
                                  OrderSide side,
                                  int quantity,
                                  double price) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "INSERT INTO trades(order_id, account_id, symbol, side, quantity, price) "
        "VALUES (?, ?, ?, ?, ?, ?);";
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throwPrepareError(db_.handle(), "prepare trade insert");
    }
    sqlite3_bind_int(stmt, 1, orderId);
    sqlite3_bind_int(stmt, 2, accountId);
    sqlite3_bind_text(stmt, 3, symbol.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, toString(side), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, quantity);
    sqlite3_bind_double(stmt, 6, price);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throwPrepareError(db_.handle(), "trade insert");
    }
    sqlite3_finalize(stmt);
    return static_cast<int>(db_.lastInsertRowId());
}

std::vector<TradeRow> SqliteTradeRepository::listRecent(int accountId, int limit) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "SELECT id, order_id, account_id, symbol, side, quantity, price "
        "FROM trades WHERE account_id = ? ORDER BY id DESC LIMIT ?;";
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throwPrepareError(db_.handle(), "prepare trade listRecent");
    }
    sqlite3_bind_int(stmt, 1, accountId);
    sqlite3_bind_int(stmt, 2, limit);

    std::vector<TradeRow> rows;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        TradeRow trade;
        trade.id = sqlite3_column_int(stmt, 0);
        trade.orderId = sqlite3_column_int(stmt, 1);
        trade.accountId = sqlite3_column_int(stmt, 2);
        trade.symbol = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        trade.side = sideFromString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
        trade.quantity = sqlite3_column_int(stmt, 5);
        trade.price = sqlite3_column_double(stmt, 6);
        rows.push_back(std::move(trade));
    }
    sqlite3_finalize(stmt);
    return rows;
}

int SqliteTradeRepository::countByAccount(int accountId) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT COUNT(*) FROM trades WHERE account_id = ?;";
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throwPrepareError(db_.handle(), "prepare trade countByAccount");
    }
    sqlite3_bind_int(stmt, 1, accountId);
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return count;
}
