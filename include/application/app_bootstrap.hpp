#pragma once

#include "application/auth_app_service.hpp"
#include "application/order_app_service.hpp"
#include "application/password_hasher.hpp"
#include "application/wallet_app_service.hpp"
#include "infrastructure/db/migrator.hpp"
#include "infrastructure/db/sqlite_connection.hpp"
#include "infrastructure/db/sqlite_repositories.hpp"

#include <cstdlib>
#include <filesystem>
#include <memory>
#include <string>

// Owns SQLite connection + repositories + app services for one process.
class AppBootstrap {
public:
    static AppBootstrap open(const std::string& dbPath) {
        return AppBootstrap(dbPath);
    }

    static std::string defaultDbPath() {
        namespace fs = std::filesystem;
        const char* home = std::getenv("HOME");
        fs::path dir = home ? fs::path(home) / ".trading-app" : fs::path(".trading-app");
        std::error_code ec;
        fs::create_directories(dir, ec);
        return (dir / "trading.db").string();
    }

    SqliteConnection& db() { return *db_; }
    const std::string& dbPath() const { return dbPath_; }
    AuthAppService& auth() { return *auth_; }
    WalletAppService& wallet() { return *wallet_; }
    OrderAppService& orders() { return *orders_; }
    IAccountRepository& accounts() { return *accounts_; }
    ILedgerRepository& ledger() { return *ledger_; }
    IQuoteRepository& quotes() { return *quotes_; }
    IPositionRepository& positions() { return *positions_; }
    ITradeRepository& trades() { return *trades_; }

private:
    explicit AppBootstrap(const std::string& dbPath)
        : dbPath_(dbPath)
        , db_(std::make_unique<SqliteConnection>(SqliteConnection::open(dbPath)))
        , users_(std::make_unique<SqliteUserRepository>(*db_))
        , accounts_(std::make_unique<SqliteAccountRepository>(*db_))
        , ledger_(std::make_unique<SqliteLedgerRepository>(*db_))
        , quotes_(std::make_unique<SqliteQuoteRepository>(*db_))
        , positions_(std::make_unique<SqlitePositionRepository>(*db_))
        , orderRepo_(std::make_unique<SqliteOrderRepository>(*db_))
        , trades_(std::make_unique<SqliteTradeRepository>(*db_))
        , hasher_(std::make_unique<SimplePasswordHasher>())
        , auth_(std::make_unique<AuthAppService>(*db_, *users_, *accounts_, *hasher_))
        , wallet_(std::make_unique<WalletAppService>(*db_, *accounts_, *ledger_))
        , orders_(std::make_unique<OrderAppService>(
              *db_, *accounts_, *ledger_, *quotes_, *positions_, *orderRepo_, *trades_)) {
        runMigrations(*db_);
    }

    std::string dbPath_;
    std::unique_ptr<SqliteConnection> db_;
    std::unique_ptr<SqliteUserRepository> users_;
    std::unique_ptr<SqliteAccountRepository> accounts_;
    std::unique_ptr<SqliteLedgerRepository> ledger_;
    std::unique_ptr<SqliteQuoteRepository> quotes_;
    std::unique_ptr<SqlitePositionRepository> positions_;
    std::unique_ptr<SqliteOrderRepository> orderRepo_;
    std::unique_ptr<SqliteTradeRepository> trades_;
    std::unique_ptr<SimplePasswordHasher> hasher_;
    std::unique_ptr<AuthAppService> auth_;
    std::unique_ptr<WalletAppService> wallet_;
    std::unique_ptr<OrderAppService> orders_;
};
