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
    const int version = currentSchemaVersion(db);
    if (version >= 1) {
        return;
    }

    Transaction tx(db, true);
    db.exec(kMigration001);
    db.exec("INSERT INTO schema_version(version) VALUES (1);");
    tx.commit();
}
