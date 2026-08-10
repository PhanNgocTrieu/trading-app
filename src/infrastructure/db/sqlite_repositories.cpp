#include "infrastructure/db/sqlite_repositories.hpp"

#include <stdexcept>

namespace {

void throwPrepareError(sqlite3* db, const char* what) {
    throw std::runtime_error(std::string(what) + ": " + sqlite3_errmsg(db));
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
