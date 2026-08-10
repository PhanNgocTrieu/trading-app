#pragma once

#include "infrastructure/db/sqlite_connection.hpp"

class Transaction {
public:
    explicit Transaction(SqliteConnection& db, bool immediate = true)
        : db_(db) {
        db_.exec(immediate ? "BEGIN IMMEDIATE;" : "BEGIN;");
        active_ = true;
    }

    void commit() {
        if (!active_) {
            return;
        }
        db_.exec("COMMIT;");
        active_ = false;
    }

    void rollback() {
        if (!active_) {
            return;
        }
        db_.exec("ROLLBACK;");
        active_ = false;
    }

    ~Transaction() {
        if (active_) {
            try {
                rollback();
            } catch (...) {
            }
        }
    }

    Transaction(const Transaction&) = delete;
    Transaction& operator=(const Transaction&) = delete;

private:
    SqliteConnection& db_;
    bool active_{false};
};
