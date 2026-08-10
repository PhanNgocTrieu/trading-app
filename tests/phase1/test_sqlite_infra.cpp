#include "infrastructure/db/migrator.hpp"
#include "infrastructure/db/sqlite_connection.hpp"
#include "infrastructure/db/transaction.hpp"

#include <gtest/gtest.h>

TEST(SqliteInfrastructureTest, OpenAndExecMemoryDb) {
    auto db = SqliteConnection::open(":memory:");
    db.exec("CREATE TABLE t(id INTEGER PRIMARY KEY);");
    db.exec("INSERT INTO t(id) VALUES (1);");
    EXPECT_EQ(db.lastInsertRowId(), 1);
}

TEST(SqliteInfrastructureTest, TransactionCommits) {
    auto db = SqliteConnection::open(":memory:");
    db.exec("CREATE TABLE t(id INTEGER PRIMARY KEY);");
    {
        Transaction tx(db, true);
        db.exec("INSERT INTO t(id) VALUES (7);");
        tx.commit();
    }
    EXPECT_EQ(db.lastInsertRowId(), 7);
}

TEST(SqliteInfrastructureTest, TransactionRollbackOnDestroy) {
    auto db = SqliteConnection::open(":memory:");
    db.exec("CREATE TABLE t(id INTEGER PRIMARY KEY);");
    {
        Transaction tx(db, true);
        db.exec("INSERT INTO t(id) VALUES (3);");
        // no commit -> rollback
    }

    sqlite3_stmt* stmt = nullptr;
    ASSERT_EQ(sqlite3_prepare_v2(db.handle(), "SELECT COUNT(*) FROM t;", -1, &stmt, nullptr),
              SQLITE_OK);
    ASSERT_EQ(sqlite3_step(stmt), SQLITE_ROW);
    EXPECT_EQ(sqlite3_column_int(stmt, 0), 0);
    sqlite3_finalize(stmt);
}

TEST(SqliteInfrastructureTest, RunMigrationsIdempotent) {
    auto db = SqliteConnection::open(":memory:");
    runMigrations(db);
    runMigrations(db); // second call no-op
    db.exec("SELECT 1 FROM users;");
    db.exec("SELECT 1 FROM accounts;");
    db.exec("SELECT 1 FROM ledger_entries;");
}
