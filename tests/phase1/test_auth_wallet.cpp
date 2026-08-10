#include "tests/phase1/test_support.hpp"

#include "application/password_hasher.hpp"
#include "infrastructure/db/sqlite_repositories.hpp"

#include <filesystem>

TEST_F(Phase1Fixture, RegisterUser_CreatesUserAndAccount) {
    EXPECT_EQ(login_->registerWithCredentials("alice", "secret1"), LoginStatus::Success);
    ASSERT_TRUE(login_->isLoggedIn());
    ASSERT_NE(login_->session(), nullptr);
    EXPECT_EQ(login_->session()->user.username(), "alice");
    EXPECT_DOUBLE_EQ(login_->session()->account.cashBalance(), 0.0);

    auto row = app_->accounts().findById(login_->authSession().accountId);
    ASSERT_TRUE(row.has_value());
    EXPECT_EQ(row->userId, login_->authSession().userId);
}

TEST_F(Phase1Fixture, RegisterUser_RejectsDuplicateUsername) {
    ASSERT_EQ(login_->registerWithCredentials("alice", "secret1"), LoginStatus::Success);
    login_->logout();
    EXPECT_EQ(login_->registerWithCredentials("alice", "secret2"), LoginStatus::Failure);
}

TEST_F(Phase1Fixture, Login_SucceedsWithValidPasswordHash) {
    ASSERT_EQ(login_->registerWithCredentials("bob", "secret1"), LoginStatus::Success);
    login_->logout();
    EXPECT_EQ(login_->loginWithCredentials("bob", "secret1"), LoginStatus::Success);
    EXPECT_TRUE(login_->isLoggedIn());
}

TEST_F(Phase1Fixture, Login_FailsWithWrongPassword) {
    ASSERT_EQ(login_->registerWithCredentials("bob", "secret1"), LoginStatus::Success);
    login_->logout();
    EXPECT_EQ(login_->loginWithCredentials("bob", "wrongpw"), LoginStatus::Failure);
    EXPECT_FALSE(login_->isLoggedIn());
}

TEST_F(Phase1Fixture, Deposit_PersistsBalanceAndLedger) {
    ASSERT_EQ(login_->registerWithCredentials("alice", "secret1"), LoginStatus::Success);
    auto deposited = bank_->deposit(1000.0);
    ASSERT_TRUE(deposited.ok());
    EXPECT_DOUBLE_EQ(deposited.value(), 1000.0);

    auto balance = bank_->balance();
    ASSERT_TRUE(balance.ok());
    EXPECT_DOUBLE_EQ(balance.value(), 1000.0);

    auto ledger = bank_->recentLedger(10);
    ASSERT_TRUE(ledger.ok());
    ASSERT_FALSE(ledger.value().empty());
    EXPECT_EQ(ledger.value().front().type, LedgerType::Deposit);
    EXPECT_DOUBLE_EQ(ledger.value().front().amount, 1000.0);
}

TEST_F(Phase1Fixture, Withdraw_PersistsBalanceAndLedger) {
    ASSERT_EQ(login_->registerWithCredentials("alice", "secret1"), LoginStatus::Success);
    ASSERT_TRUE(bank_->deposit(1000.0).ok());
    auto withdrawn = bank_->withdraw(250.0);
    ASSERT_TRUE(withdrawn.ok());
    EXPECT_DOUBLE_EQ(withdrawn.value(), 750.0);

    auto ledger = bank_->recentLedger(10);
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

TEST_F(Phase1Fixture, Withdraw_RejectsInsufficientFunds) {
    ASSERT_EQ(login_->registerWithCredentials("alice", "secret1"), LoginStatus::Success);
    ASSERT_TRUE(bank_->deposit(100.0).ok());
    auto result = bank_->withdraw(200.0);
    EXPECT_FALSE(result.ok());
    EXPECT_EQ(result.code(), ErrorCode::InsufficientFunds);
    auto balance = bank_->balance();
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
        Service::LoggerService& logger = Service::LoggerService::getInstance();
        Service::LoginService login{logger, app.auth(), app.accounts()};
        ASSERT_EQ(login.registerWithCredentials("persist_user", "secret1"), LoginStatus::Success);
        Service::BankAccountService bank{logger, app.wallet(), login};
        ASSERT_TRUE(bank.deposit(500.0).ok());
        login.logout();
    }

    {
        resetCurrentSession();
        auto app = AppBootstrap::open(path);
        Service::LoggerService& logger = Service::LoggerService::getInstance();
        Service::LoginService login{logger, app.auth(), app.accounts()};
        EXPECT_EQ(login.loginWithCredentials("persist_user", "secret1"), LoginStatus::Success);
        Service::BankAccountService bank{logger, app.wallet(), login};
        auto balance = bank.balance();
        ASSERT_TRUE(balance.ok());
        EXPECT_DOUBLE_EQ(balance.value(), 500.0);

        // Password must not be stored in plaintext.
        SqliteUserRepository users(app.db());
        auto row = users.findByUsername("persist_user");
        ASSERT_TRUE(row.has_value());
        EXPECT_EQ(row->passwordHash.find("secret1"), std::string::npos);
        EXPECT_NE(row->passwordHash.find("v1$"), std::string::npos);
    }

    std::filesystem::remove(path, ec);
}
