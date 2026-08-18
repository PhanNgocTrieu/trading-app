#include "support/app_fixture.hpp"

#include "domain/order_types.hpp"

#include <filesystem>

TEST_F(Phase2OrderFlowTest, SeedQuotes_AreAvailable) {
    auto quotes = app_->orders().listQuotes();
    ASSERT_TRUE(quotes.ok());
    ASSERT_GE(quotes.value().size(), 3u);
}

TEST_F(Phase2OrderFlowTest, Acceptance_BuySellAvgCostAndReject) {
    ASSERT_TRUE(registerUser("trader", "secret1").ok());

    auto deposited = app_->wallet().deposit(session_, 10000.0);
    ASSERT_TRUE(deposited.ok());
    EXPECT_DOUBLE_EQ(deposited.value(), 10000.0);

    auto buy1 = app_->orders().placeMarketOrder(session_, "AAPL", OrderSide::Buy, 10);
    ASSERT_TRUE(buy1.ok()) << buy1.message();
    EXPECT_DOUBLE_EQ(buy1.value().fillPrice, 190.0);
    EXPECT_EQ(buy1.value().status, OrderStatus::Filled);

    auto cash = app_->wallet().balance(session_);
    ASSERT_TRUE(cash.ok());
    EXPECT_DOUBLE_EQ(cash.value(), 8100.0);

    auto portfolio = app_->orders().portfolio(session_);
    ASSERT_TRUE(portfolio.ok());
    ASSERT_EQ(portfolio.value().size(), 1u);
    EXPECT_EQ(portfolio.value()[0].symbol, "AAPL");
    EXPECT_EQ(portfolio.value()[0].quantity, 10);
    EXPECT_DOUBLE_EQ(portfolio.value()[0].avgCost, 190.0);
    EXPECT_EQ(app_->trades().countByAccount(session_.accountId), 1);

    ASSERT_TRUE(app_->orders().setQuotePrice("AAPL", 210.0).ok());
    auto buy2 = app_->orders().placeMarketOrder(session_, "AAPL", OrderSide::Buy, 10);
    ASSERT_TRUE(buy2.ok()) << buy2.message();
    EXPECT_DOUBLE_EQ(buy2.value().fillPrice, 210.0);

    cash = app_->wallet().balance(session_);
    ASSERT_TRUE(cash.ok());
    EXPECT_DOUBLE_EQ(cash.value(), 6000.0);

    portfolio = app_->orders().portfolio(session_);
    ASSERT_TRUE(portfolio.ok());
    ASSERT_EQ(portfolio.value().size(), 1u);
    EXPECT_EQ(portfolio.value()[0].quantity, 20);
    EXPECT_DOUBLE_EQ(portfolio.value()[0].avgCost, 200.0);

    ASSERT_TRUE(app_->orders().setQuotePrice("AAPL", 220.0).ok());
    auto sell = app_->orders().placeMarketOrder(session_, "AAPL", OrderSide::Sell, 5);
    ASSERT_TRUE(sell.ok()) << sell.message();
    ASSERT_TRUE(sell.value().realizedPnl.has_value());
    EXPECT_DOUBLE_EQ(*sell.value().realizedPnl, 100.0);

    cash = app_->wallet().balance(session_);
    ASSERT_TRUE(cash.ok());
    EXPECT_DOUBLE_EQ(cash.value(), 7100.0);

    portfolio = app_->orders().portfolio(session_);
    ASSERT_TRUE(portfolio.ok());
    ASSERT_EQ(portfolio.value().size(), 1u);
    EXPECT_EQ(portfolio.value()[0].quantity, 15);
    EXPECT_DOUBLE_EQ(portfolio.value()[0].avgCost, 200.0);

    auto oversell = app_->orders().placeMarketOrder(session_, "AAPL", OrderSide::Sell, 1000);
    EXPECT_FALSE(oversell.ok());
    EXPECT_EQ(oversell.code(), ErrorCode::InsufficientPosition);

    cash = app_->wallet().balance(session_);
    ASSERT_TRUE(cash.ok());
    EXPECT_DOUBLE_EQ(cash.value(), 7100.0);

    portfolio = app_->orders().portfolio(session_);
    ASSERT_TRUE(portfolio.ok());
    ASSERT_EQ(portfolio.value().size(), 1u);
    EXPECT_EQ(portfolio.value()[0].quantity, 15);
}

TEST_F(Phase2OrderFlowTest, Buy_RejectsInsufficientFundsWithoutSideEffects) {
    ASSERT_TRUE(registerUser("broke", "secret1").ok());
    ASSERT_TRUE(app_->wallet().deposit(session_, 100.0).ok());

    auto result = app_->orders().placeMarketOrder(session_, "AAPL", OrderSide::Buy, 10);
    EXPECT_FALSE(result.ok());
    EXPECT_EQ(result.code(), ErrorCode::InsufficientFunds);

    auto cash = app_->wallet().balance(session_);
    ASSERT_TRUE(cash.ok());
    EXPECT_DOUBLE_EQ(cash.value(), 100.0);

    auto portfolio = app_->orders().portfolio(session_);
    ASSERT_TRUE(portfolio.ok());
    EXPECT_TRUE(portfolio.value().empty());
    EXPECT_EQ(app_->trades().countByAccount(session_.accountId), 0);
}

TEST(Phase2PersistenceTest, RestartApp_KeepsPositions) {
    const auto path =
        (std::filesystem::temp_directory_path() / "trading-app-phase2-persist.db").string();
    std::error_code ec;
    std::filesystem::remove(path, ec);

    int accountId = 0;
    {
        auto app = AppBootstrap::open(path);
        auto registered = app.auth().registerUser("persist_trader", "secret1");
        ASSERT_TRUE(registered.ok());
        accountId = registered.value().accountId;
        ASSERT_TRUE(app.wallet().deposit(registered.value(), 10000.0).ok());
        ASSERT_TRUE(
            app.orders().placeMarketOrder(registered.value(), "AAPL", OrderSide::Buy, 10).ok());
    }

    {
        auto app = AppBootstrap::open(path);
        auto loggedIn = app.auth().login("persist_trader", "secret1");
        ASSERT_TRUE(loggedIn.ok());

        auto cash = app.wallet().balance(loggedIn.value());
        ASSERT_TRUE(cash.ok());
        EXPECT_DOUBLE_EQ(cash.value(), 8100.0);

        auto portfolio = app.orders().portfolio(loggedIn.value());
        ASSERT_TRUE(portfolio.ok());
        ASSERT_EQ(portfolio.value().size(), 1u);
        EXPECT_EQ(portfolio.value()[0].symbol, "AAPL");
        EXPECT_EQ(portfolio.value()[0].quantity, 10);
        EXPECT_DOUBLE_EQ(portfolio.value()[0].avgCost, 190.0);
        EXPECT_EQ(app.trades().countByAccount(accountId), 1);
    }

    std::filesystem::remove(path, ec);
}
