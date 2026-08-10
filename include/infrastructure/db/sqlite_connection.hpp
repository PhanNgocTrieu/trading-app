#pragma once

#include <sqlite3.h>

#include <stdexcept>
#include <string>
#include <utility>

class SqliteConnection {
public:
    static SqliteConnection open(const std::string& path) {
        SqliteConnection connection;
        if (sqlite3_open(path.c_str(), &connection.db_) != SQLITE_OK) {
            const char* err = connection.db_ ? sqlite3_errmsg(connection.db_) : "unknown";
            if (connection.db_) {
                sqlite3_close(connection.db_);
                connection.db_ = nullptr;
            }
            throw std::runtime_error(std::string("cannot open db: ") + err);
        }
        connection.exec("PRAGMA foreign_keys = ON;");
        return connection;
    }

    SqliteConnection(const SqliteConnection&) = delete;
    SqliteConnection& operator=(const SqliteConnection&) = delete;

    SqliteConnection(SqliteConnection&& other) noexcept
        : db_(std::exchange(other.db_, nullptr)) {}

    SqliteConnection& operator=(SqliteConnection&& other) noexcept {
        if (this != &other) {
            if (db_) {
                sqlite3_close(db_);
            }
            db_ = std::exchange(other.db_, nullptr);
        }
        return *this;
    }

    ~SqliteConnection() {
        if (db_) {
            sqlite3_close(db_);
        }
    }

    sqlite3* handle() const { return db_; }

    void exec(const std::string& sql) {
        char* err = nullptr;
        if (sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err) != SQLITE_OK) {
            std::string message = err ? err : "exec failed";
            sqlite3_free(err);
            throw std::runtime_error(message);
        }
    }

    long long lastInsertRowId() const {
        return sqlite3_last_insert_rowid(db_);
    }

private:
    SqliteConnection() = default;
    sqlite3* db_{nullptr};
};
