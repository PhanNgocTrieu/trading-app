#pragma once
// docs/samples/phase1/sqlite_connection.hpp
// Cần link libsqlite3. Đây là skeleton học tập — bổ sung error handling khi integrate.

#include <sqlite3.h>
#include <stdexcept>
#include <string>

class SqliteConnection {
public:
    static SqliteConnection open(const std::string& path) {
        SqliteConnection c;
        if (sqlite3_open(path.c_str(), &c.db_) != SQLITE_OK) {
            throw std::runtime_error("cannot open db");
        }
        char* err = nullptr;
        if (sqlite3_exec(c.db_, "PRAGMA foreign_keys = ON;", nullptr, nullptr, &err) != SQLITE_OK) {
            std::string msg = err ? err : "pragma failed";
            sqlite3_free(err);
            throw std::runtime_error(msg);
        }
        return c;
    }

    SqliteConnection(const SqliteConnection&) = delete;
    SqliteConnection& operator=(const SqliteConnection&) = delete;

    SqliteConnection(SqliteConnection&& other) noexcept : db_(other.db_) {
        other.db_ = nullptr;
    }

    ~SqliteConnection() {
        if (db_) sqlite3_close(db_);
    }

    sqlite3* handle() const { return db_; }

    void exec(const std::string& sql) {
        char* err = nullptr;
        if (sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err) != SQLITE_OK) {
            std::string msg = err ? err : "exec failed";
            sqlite3_free(err);
            throw std::runtime_error(msg);
        }
    }

private:
    SqliteConnection() = default;
    sqlite3* db_{nullptr};
};
