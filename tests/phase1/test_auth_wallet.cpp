#include "support/app_fixture.hpp"

#include "infrastructure/db/sqlite_repositories.hpp"

#include <filesystem>

TEST_F(Phase1AuthWalletTest, RegisterUser_CreatesUserAndAccount) {
    auto result = registerUser("alice", "secret1");
    ASSERT_TRUE(result.ok());
    EXPECT_TRUE(loggedIn_);
    EXPECT_EQ(session_.username, "alice");
    EXPECT_GT(session_.accountId, 0);

    auto row = app_->accounts().findById(session_.accountId);
    ASSERT_TRUE(row.has_value());
    EXPECT_EQ(row->userId, session_.userId);
    EXPECT_DOUBLE_EQ(row->cashBalance, 0.0);
}

TEST_F(Phase1AuthWalletTest, RegisterUser_RejectsDuplicateUsername) {
    ASSERT_TRUE(registerUser("alice", "secret1").ok());
    logout();
    auto duplicate = registerUser("alice", "secret2");
    EXPECT_FALSE(duplicate.ok());
    EXPECT_EQ(duplicate.code(), ErrorCode::Conflict);
}

TEST_F(Phase1AuthWalletTest, Login_SucceedsWithValidPasswordHash) {
    ASSERT_TRUE(registerUser("bob", "secret1").ok());
    logout();
    auto result = login("bob", "secret1");
    ASSERT_TRUE(result.ok());
    EXPECT_TRUE(loggedIn_);
}

TEST_F(Phase1AuthWalletTest, Login_FailsWithWrongPassword) {
    ASSERT_TRUE(registerUser("bob", "secret1").ok());
    logout();
    auto result = login("bob", "wrongpw");
    EXPECT_FALSE(result.ok());
    EXPECT_EQ(result.code(), ErrorCode::Unauthorized);
    EXPECT_FALSE(loggedIn_);
}

TEST_F(Phase1AuthWalletTest, Deposit_PersistsBalanceAndLedger) {
    ASSERT_TRUE(registerUser("alice", "secret1").ok());
    auto deposited = app_->wallet().deposit(session_, 1000.0);
    ASSERT_TRUE(deposited.ok());
    EXPECT_DOUBLE_EQ(deposited.value(), 1000.0);

    auto balance = app_->wallet().balance(session_);
    ASSERT_TRUE(balance.ok());
    EXPECT_DOUBLE_EQ(balance.value(), 1000.0);

    auto ledger = app_->wallet().recentLedger(session_, 10);
    ASSERT_TRUE(ledger.ok());
    ASSERT_FALSE(ledger.value().empty());
    EXPECT_EQ(ledger.value().front().type, LedgerType::Deposit);
    EXPECT_DOUBLE_EQ(ledger.value().front().amount, 1000.0);
}

TEST_F(Phase1AuthWalletTest, Withdraw_PersistsBalanceAndLedger) {
    ASSERT_TRUE(registerUser("alice", "secret1").ok());
    ASSERT_TRUE(app_->wallet().deposit(session_, 1000.0).ok());
    auto withdrawn = app_->wallet().withdraw(session_, 250.0);
    ASSERT_TRUE(withdrawn.ok());
    EXPECT_DOUBLE_EQ(withdrawn.value(), 750.0);

    auto ledger = app_->wallet().recentLedger(session_, 10);
    ASSERT_TRUE(ledger.ok());
    bool sawWithdraw = false;
    for (const auto& entry : ledger.value()) {
        if (entry.type == LedgerType::Withdraw) {
            sawWithdraw = true;
            EXPECT_DOUBLE_EQ(entry.amount, -250.0);
        }
    }
    EXPECT_TRUE(sawWithdraw);
}

TEST_F(Phase1AuthWalletTest, Withdraw_RejectsInsufficientFunds) {
    ASSERT_TRUE(registerUser("alice", "secret1").ok());
    ASSERT_TRUE(app_->wallet().deposit(session_, 100.0).ok());
    auto result = app_->wallet().withdraw(session_, 200.0);
    EXPECT_FALSE(result.ok());
    EXPECT_EQ(result.code(), ErrorCode::InsufficientFunds);
    auto balance = app_->wallet().balance(session_);
    ASSERT_TRUE(balance.ok());
    EXPECT_DOUBLE_EQ(balance.value(), 100.0);
}

TEST(Phase1PersistenceTest, RestartApp_StillLogin) {
    const auto path =
        (std::filesystem::temp_directory_path() / "trading-app-phase1-persist.db").string();
    std::error_code ec;
    std::filesystem::remove(path, ec);

    {
        auto app = AppBootstrap::open(path);
        auto registered = app.auth().registerUser("persist_user", "secret1");
        ASSERT_TRUE(registered.ok());
        ASSERT_TRUE(app.wallet().deposit(registered.value(), 500.0).ok());
    }

    {
        auto app = AppBootstrap::open(path);
        auto loggedIn = app.auth().login("persist_user", "secret1");
        ASSERT_TRUE(loggedIn.ok());
        auto balance = app.wallet().balance(loggedIn.value());
        ASSERT_TRUE(balance.ok());
        EXPECT_DOUBLE_EQ(balance.value(), 500.0);

        SqliteUserRepository users(app.db());
        auto row = users.findByUsername("persist_user");
        ASSERT_TRUE(row.has_value());
        EXPECT_EQ(row->passwordHash.find("secret1"), std::string::npos);
        EXPECT_NE(row->passwordHash.find("v1$"), std::string::npos);
    }

    std::filesystem::remove(path, ec);
}
