#include "tests/phase1/test_support.hpp"

#include "trading.h"

#include <filesystem>
#include <memory>

class Phase2Fixture : public ::testing::Test {
protected:
    void SetUp() override {
        resetCurrentSession();
        app_ = std::make_unique<AppBootstrap>(AppBootstrap::open(":memory:"));
        logger_ = &Service::LoggerService::getInstance();
        login_ = std::make_unique<Service::LoginService>(*logger_, app_->auth(), app_->accounts());
        bank_ = std::make_unique<Service::BankAccountService>(*logger_, app_->wallet(), *login_);
        trading_ =
            std::make_unique<Service::TradingService>(*logger_, app_->orders(), *login_);
    }

    void TearDown() override {
        trading_.reset();
        bank_.reset();
        login_.reset();
        app_.reset();
        resetCurrentSession();
    }

    std::unique_ptr<AppBootstrap> app_;
    Service::LoggerService* logger_{nullptr};
    std::unique_ptr<Service::LoginService> login_;
    std::unique_ptr<Service::BankAccountService> bank_;
    std::unique_ptr<Service::TradingService> trading_;
};

TEST_F(Phase2Fixture, SeedQuotes_AreAvailable) {
    auto quotes = trading_->listQuotes();
    ASSERT_TRUE(quotes.ok());
    ASSERT_GE(quotes.value().size(), 3u);
}

TEST_F(Phase2Fixture, Acceptance_BuySellAvgCostAndReject) {
    // 1. Register + Login
    ASSERT_EQ(login_->registerWithCredentials("trader", "secret1"), LoginStatus::Success);

    // 2. Deposit 10_000
    auto deposited = bank_->deposit(10000.0);
    ASSERT_TRUE(deposited.ok());
    EXPECT_DOUBLE_EQ(deposited.value(), 10000.0);

    // 3. Buy 10 AAPL @ 190 → cash 8100, position 10/190, trade 1
    auto buy1 = trading_->buyMarket("AAPL", 10);
    ASSERT_TRUE(buy1.ok()) << buy1.message();
    EXPECT_DOUBLE_EQ(buy1.value().fillPrice, 190.0);
    EXPECT_EQ(buy1.value().status, OrderStatus::Filled);

    auto cash = bank_->balance();
    ASSERT_TRUE(cash.ok());
    EXPECT_DOUBLE_EQ(cash.value(), 8100.0);

    auto portfolio = trading_->portfolio();
    ASSERT_TRUE(portfolio.ok());
    ASSERT_EQ(portfolio.value().size(), 1u);
    EXPECT_EQ(portfolio.value()[0].symbol, "AAPL");
    EXPECT_EQ(portfolio.value()[0].quantity, 10);
    EXPECT_DOUBLE_EQ(portfolio.value()[0].avgCost, 190.0);
    EXPECT_EQ(app_->trades().countByAccount(login_->authSession().accountId), 1);

    // 4. Buy thêm 10 @ 210 → qty 20, avg 200
    ASSERT_TRUE(app_->orders().setQuotePrice("AAPL", 210.0).ok());
    auto buy2 = trading_->buyMarket("AAPL", 10);
    ASSERT_TRUE(buy2.ok()) << buy2.message();
    EXPECT_DOUBLE_EQ(buy2.value().fillPrice, 210.0);

    cash = bank_->balance();
    ASSERT_TRUE(cash.ok());
    EXPECT_DOUBLE_EQ(cash.value(), 6000.0); // 8100 - 2100

    portfolio = trading_->portfolio();
    ASSERT_TRUE(portfolio.ok());
    ASSERT_EQ(portfolio.value().size(), 1u);
    EXPECT_EQ(portfolio.value()[0].quantity, 20);
    EXPECT_DOUBLE_EQ(portfolio.value()[0].avgCost, 200.0);

    // 5. Sell 5 @ 220 → qty 15, cash += 1100, realized = 100
    ASSERT_TRUE(app_->orders().setQuotePrice("AAPL", 220.0).ok());
    auto sell = trading_->sellMarket("AAPL", 5);
    ASSERT_TRUE(sell.ok()) << sell.message();
    ASSERT_TRUE(sell.value().realizedPnl.has_value());
    EXPECT_DOUBLE_EQ(*sell.value().realizedPnl, 100.0); // (220-200)*5

    cash = bank_->balance();
    ASSERT_TRUE(cash.ok());
    EXPECT_DOUBLE_EQ(cash.value(), 7100.0); // 6000 + 1100

    portfolio = trading_->portfolio();
    ASSERT_TRUE(portfolio.ok());
    ASSERT_EQ(portfolio.value().size(), 1u);
    EXPECT_EQ(portfolio.value()[0].quantity, 15);
    EXPECT_DOUBLE_EQ(portfolio.value()[0].avgCost, 200.0);

    // 6. Sell 1000 → rejected, không đổi cash/position
    auto oversell = trading_->sellMarket("AAPL", 1000);
    EXPECT_FALSE(oversell.ok());
    EXPECT_EQ(oversell.code(), ErrorCode::InsufficientPosition);

    cash = bank_->balance();
    ASSERT_TRUE(cash.ok());
    EXPECT_DOUBLE_EQ(cash.value(), 7100.0);

    portfolio = trading_->portfolio();
    ASSERT_TRUE(portfolio.ok());
    ASSERT_EQ(portfolio.value().size(), 1u);
    EXPECT_EQ(portfolio.value()[0].quantity, 15);
}

TEST_F(Phase2Fixture, Buy_RejectsInsufficientFundsWithoutSideEffects) {
    ASSERT_EQ(login_->registerWithCredentials("broke", "secret1"), LoginStatus::Success);
    ASSERT_TRUE(bank_->deposit(100.0).ok());

    auto result = trading_->buyMarket("AAPL", 10); // needs 1900
    EXPECT_FALSE(result.ok());
    EXPECT_EQ(result.code(), ErrorCode::InsufficientFunds);

    auto cash = bank_->balance();
    ASSERT_TRUE(cash.ok());
    EXPECT_DOUBLE_EQ(cash.value(), 100.0);

    auto portfolio = trading_->portfolio();
    ASSERT_TRUE(portfolio.ok());
    EXPECT_TRUE(portfolio.value().empty());
    EXPECT_EQ(app_->trades().countByAccount(login_->authSession().accountId), 0);
}

TEST(Phase2PersistenceTest, RestartApp_KeepsPositions) {
    const auto path =
        (std::filesystem::temp_directory_path() / "trading-app-phase2-persist.db").string();
    std::error_code ec;
    std::filesystem::remove(path, ec);

    {
        auto app = AppBootstrap::open(path);
        Service::LoggerService& logger = Service::LoggerService::getInstance();
        Service::LoginService login{logger, app.auth(), app.accounts()};
        ASSERT_EQ(login.registerWithCredentials("persist_trader", "secret1"),
                  LoginStatus::Success);
        Service::BankAccountService bank{logger, app.wallet(), login};
        Service::TradingService trading{logger, app.orders(), login};
        ASSERT_TRUE(bank.deposit(10000.0).ok());
        ASSERT_TRUE(trading.buyMarket("AAPL", 10).ok());
        login.logout();
    }

    {
        resetCurrentSession();
        auto app = AppBootstrap::open(path);
        Service::LoggerService& logger = Service::LoggerService::getInstance();
        Service::LoginService login{logger, app.auth(), app.accounts()};
        EXPECT_EQ(login.loginWithCredentials("persist_trader", "secret1"), LoginStatus::Success);
        Service::TradingService trading{logger, app.orders(), login};
        Service::BankAccountService bank{logger, app.wallet(), login};

        auto cash = bank.balance();
        ASSERT_TRUE(cash.ok());
        EXPECT_DOUBLE_EQ(cash.value(), 8100.0);

        auto portfolio = trading.portfolio();
        ASSERT_TRUE(portfolio.ok());
        ASSERT_EQ(portfolio.value().size(), 1u);
        EXPECT_EQ(portfolio.value()[0].symbol, "AAPL");
        EXPECT_EQ(portfolio.value()[0].quantity, 10);
        EXPECT_DOUBLE_EQ(portfolio.value()[0].avgCost, 190.0);
        EXPECT_EQ(app.trades().countByAccount(login.authSession().accountId), 1);
    }

    std::filesystem::remove(path, ec);
}
