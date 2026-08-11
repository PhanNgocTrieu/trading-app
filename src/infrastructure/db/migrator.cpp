#include "infrastructure/db/migrator.hpp"

#include "infrastructure/db/transaction.hpp"

#include <stdexcept>
#include <string>

namespace {

// Kept in sync with sql/001_init.sql (embedded so tests/runtime don't depend on cwd).
constexpr const char* kMigration001 = R"SQL(
CREATE TABLE IF NOT EXISTS schema_version (
    version     INTEGER NOT NULL,
    applied_at  TEXT    NOT NULL DEFAULT (datetime('now'))
);

CREATE TABLE IF NOT EXISTS users (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    username        TEXT    NOT NULL UNIQUE,
    password_hash   TEXT    NOT NULL,
    phone_number    TEXT    NOT NULL DEFAULT '',
    created_at      TEXT    NOT NULL DEFAULT (datetime('now'))
);

CREATE TABLE IF NOT EXISTS accounts (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id         INTEGER NOT NULL UNIQUE,
    cash_balance    REAL    NOT NULL DEFAULT 0 CHECK (cash_balance >= 0),
    currency        TEXT    NOT NULL DEFAULT 'USD',
    status          TEXT    NOT NULL DEFAULT 'ACTIVE'
                    CHECK (status IN ('ACTIVE', 'FROZEN')),
    created_at      TEXT    NOT NULL DEFAULT (datetime('now')),
    FOREIGN KEY (user_id) REFERENCES users(id)
);

CREATE TABLE IF NOT EXISTS ledger_entries (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    account_id      INTEGER NOT NULL,
    type            TEXT    NOT NULL
                    CHECK (type IN ('DEPOSIT','WITHDRAW','BUY','SELL','FEE','ADJUSTMENT')),
    amount          REAL    NOT NULL,
    balance_after   REAL    NOT NULL,
    ref_type        TEXT,
    ref_id          INTEGER,
    note            TEXT    NOT NULL DEFAULT '',
    created_at      TEXT    NOT NULL DEFAULT (datetime('now')),
    FOREIGN KEY (account_id) REFERENCES accounts(id)
);

CREATE INDEX IF NOT EXISTS idx_ledger_account_created
    ON ledger_entries(account_id, created_at DESC);
)SQL";

// Kept in sync with sql/002_trading.sql
constexpr const char* kMigration002 = R"SQL(
CREATE TABLE IF NOT EXISTS stocks (
    symbol          TEXT PRIMARY KEY,
    name            TEXT NOT NULL,
    is_active       INTEGER NOT NULL DEFAULT 1
);

CREATE TABLE IF NOT EXISTS market_quotes (
    symbol          TEXT PRIMARY KEY,
    last_price      REAL NOT NULL CHECK (last_price > 0),
    updated_at      TEXT NOT NULL DEFAULT (datetime('now')),
    FOREIGN KEY (symbol) REFERENCES stocks(symbol)
);

CREATE TABLE IF NOT EXISTS orders (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    account_id      INTEGER NOT NULL,
    symbol          TEXT    NOT NULL,
    side            TEXT    NOT NULL CHECK (side IN ('BUY', 'SELL')),
    type            TEXT    NOT NULL CHECK (type IN ('MARKET', 'LIMIT')),
    quantity        INTEGER NOT NULL CHECK (quantity > 0),
    limit_price     REAL,
    status          TEXT    NOT NULL CHECK (status IN ('PENDING','FILLED','REJECTED','CANCELED')),
    created_at      TEXT    NOT NULL DEFAULT (datetime('now')),
    updated_at      TEXT    NOT NULL DEFAULT (datetime('now')),
    FOREIGN KEY (account_id) REFERENCES accounts(id),
    FOREIGN KEY (symbol) REFERENCES stocks(symbol)
);

CREATE TABLE IF NOT EXISTS trades (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    order_id        INTEGER NOT NULL,
    account_id      INTEGER NOT NULL,
    symbol          TEXT    NOT NULL,
    side            TEXT    NOT NULL CHECK (side IN ('BUY', 'SELL')),
    quantity        INTEGER NOT NULL CHECK (quantity > 0),
    price           REAL    NOT NULL CHECK (price > 0),
    executed_at     TEXT    NOT NULL DEFAULT (datetime('now')),
    FOREIGN KEY (order_id) REFERENCES orders(id),
    FOREIGN KEY (account_id) REFERENCES accounts(id),
    FOREIGN KEY (symbol) REFERENCES stocks(symbol)
);

CREATE TABLE IF NOT EXISTS positions (
    account_id      INTEGER NOT NULL,
    symbol          TEXT    NOT NULL,
    quantity        INTEGER NOT NULL CHECK (quantity >= 0),
    avg_cost        REAL    NOT NULL CHECK (avg_cost >= 0),
    updated_at      TEXT    NOT NULL DEFAULT (datetime('now')),
    PRIMARY KEY (account_id, symbol),
    FOREIGN KEY (account_id) REFERENCES accounts(id),
    FOREIGN KEY (symbol) REFERENCES stocks(symbol)
);

CREATE INDEX IF NOT EXISTS idx_orders_account_created
    ON orders(account_id, created_at DESC);

CREATE INDEX IF NOT EXISTS idx_trades_account_executed
    ON trades(account_id, executed_at DESC);

INSERT OR IGNORE INTO stocks(symbol, name) VALUES
 ('AAPL', 'Apple Inc.'),
 ('MSFT', 'Microsoft Corp.'),
 ('TSLA', 'Tesla Inc.');

INSERT OR IGNORE INTO market_quotes(symbol, last_price) VALUES
 ('AAPL', 190.0),
 ('MSFT', 420.0),
 ('TSLA', 250.0);
)SQL";

int currentSchemaVersion(SqliteConnection& db) {
    db.exec("CREATE TABLE IF NOT EXISTS schema_version ("
            "version INTEGER NOT NULL,"
            "applied_at TEXT NOT NULL DEFAULT (datetime('now'))"
            ");");

    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT COALESCE(MAX(version), 0) FROM schema_version;";
    if (sqlite3_prepare_v2(db.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("prepare schema_version failed");
    }
    int version = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        version = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return version;
}

} // namespace

void runMigrations(SqliteConnection& db) {
    int version = currentSchemaVersion(db);

    if (version < 1) {
        Transaction tx(db, true);
        db.exec(kMigration001);
        db.exec("INSERT INTO schema_version(version) VALUES (1);");
        tx.commit();
        version = 1;
    }

    if (version < 2) {
        Transaction tx(db, true);
        db.exec(kMigration002);
        db.exec("INSERT INTO schema_version(version) VALUES (2);");
        tx.commit();
    }
}
