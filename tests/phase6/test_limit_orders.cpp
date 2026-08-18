#include "support/app_fixture.hpp"

#include "domain/order_types.hpp"

TEST_F(Phase6LimitOrderTest, LimitBuy_Marketable_FillsImmediatelyAtLastPrice) {
    ASSERT_TRUE(registerUser("trader", "secret1").ok());
    ASSERT_TRUE(app_->wallet().deposit(session_, 10000.0).ok());

    auto result = app_->orders().placeLimitOrder(session_, "AAPL", OrderSide::Buy, 10, 200.0);
    ASSERT_TRUE(result.ok()) << result.message();
    EXPECT_EQ(result.value().status, OrderStatus::Filled);
    EXPECT_EQ(result.value().type, OrderType::Limit);
    EXPECT_DOUBLE_EQ(result.value().fillPrice, 190.0);

    auto cash = app_->wallet().balance(session_);
    ASSERT_TRUE(cash.ok());
    EXPECT_DOUBLE_EQ(cash.value(), 8100.0);
}

TEST_F(Phase6LimitOrderTest, LimitBuy_NotMarketable_RestsWithoutCashChange) {
    ASSERT_TRUE(registerUser("trader", "secret1").ok());
    ASSERT_TRUE(app_->wallet().deposit(session_, 10000.0).ok());

    auto result = app_->orders().placeLimitOrder(session_, "AAPL", OrderSide::Buy, 10, 180.0);
    ASSERT_TRUE(result.ok()) << result.message();
    EXPECT_EQ(result.value().status, OrderStatus::Pending);
    EXPECT_DOUBLE_EQ(*result.value().limitPrice, 180.0);

    auto cash = app_->wallet().balance(session_);
    ASSERT_TRUE(cash.ok());
    EXPECT_DOUBLE_EQ(cash.value(), 10000.0);
    EXPECT_EQ(app_->trades().countByAccount(session_.accountId), 0);

    auto working = app_->orders().workingOrders(session_);
    ASSERT_TRUE(working.ok());
    ASSERT_EQ(working.value().size(), 1u);
    EXPECT_EQ(working.value()[0].status, OrderStatus::Pending);
}

TEST_F(Phase6LimitOrderTest, LimitBuy_FillsWhenQuoteCrosses) {
    ASSERT_TRUE(registerUser("trader", "secret1").ok());
    ASSERT_TRUE(app_->wallet().deposit(session_, 10000.0).ok());

    auto resting = app_->orders().placeLimitOrder(session_, "AAPL", OrderSide::Buy, 10, 180.0);
    ASSERT_TRUE(resting.ok());
    EXPECT_EQ(resting.value().status, OrderStatus::Pending);

    ASSERT_TRUE(app_->orders().setQuotePrice("AAPL", 180.0).ok());

    auto cash = app_->wallet().balance(session_);
    ASSERT_TRUE(cash.ok());
    EXPECT_DOUBLE_EQ(cash.value(), 8200.0);

    auto portfolio = app_->orders().portfolio(session_);
    ASSERT_TRUE(portfolio.ok());
    ASSERT_EQ(portfolio.value().size(), 1u);
    EXPECT_EQ(portfolio.value()[0].quantity, 10);
    EXPECT_DOUBLE_EQ(portfolio.value()[0].avgCost, 180.0);
    EXPECT_EQ(app_->trades().countByAccount(session_.accountId), 1);

    auto working = app_->orders().workingOrders(session_);
    ASSERT_TRUE(working.ok());
    EXPECT_TRUE(working.value().empty());
}

TEST_F(Phase6LimitOrderTest, LimitBuy_Cancel_DoesNotFillOnCross) {
    ASSERT_TRUE(registerUser("trader", "secret1").ok());
    ASSERT_TRUE(app_->wallet().deposit(session_, 10000.0).ok());

    auto resting = app_->orders().placeLimitOrder(session_, "AAPL", OrderSide::Buy, 10, 180.0);
    ASSERT_TRUE(resting.ok());
    ASSERT_TRUE(app_->orders().cancelOrder(session_, resting.value().orderId).ok());

    auto working = app_->orders().workingOrders(session_);
    ASSERT_TRUE(working.ok());
    EXPECT_TRUE(working.value().empty());

    ASSERT_TRUE(app_->orders().setQuotePrice("AAPL", 180.0).ok());

    auto cash = app_->wallet().balance(session_);
    ASSERT_TRUE(cash.ok());
    EXPECT_DOUBLE_EQ(cash.value(), 10000.0);
    EXPECT_EQ(app_->trades().countByAccount(session_.accountId), 0);
}

TEST_F(Phase6LimitOrderTest, LimitSell_RestsThenFillsAtLastPrice) {
    ASSERT_TRUE(registerUser("trader", "secret1").ok());
    ASSERT_TRUE(app_->wallet().deposit(session_, 10000.0).ok());
    ASSERT_TRUE(app_->orders().placeMarketOrder(session_, "AAPL", OrderSide::Buy, 10).ok());

    auto resting = app_->orders().placeLimitOrder(session_, "AAPL", OrderSide::Sell, 5, 200.0);
    ASSERT_TRUE(resting.ok()) << resting.message();
    EXPECT_EQ(resting.value().status, OrderStatus::Pending);

    auto cash = app_->wallet().balance(session_);
    ASSERT_TRUE(cash.ok());
    EXPECT_DOUBLE_EQ(cash.value(), 8100.0);

    ASSERT_TRUE(app_->orders().setQuotePrice("AAPL", 205.0).ok());

    cash = app_->wallet().balance(session_);
    ASSERT_TRUE(cash.ok());
    EXPECT_DOUBLE_EQ(cash.value(), 9125.0);

    auto portfolio = app_->orders().portfolio(session_);
    ASSERT_TRUE(portfolio.ok());
    ASSERT_EQ(portfolio.value().size(), 1u);
    EXPECT_EQ(portfolio.value()[0].quantity, 5);
}

TEST_F(Phase6LimitOrderTest, LimitBuy_RejectsWhenReservedCashInsufficient) {
    ASSERT_TRUE(registerUser("trader", "secret1").ok());
    ASSERT_TRUE(app_->wallet().deposit(session_, 2000.0).ok());

    auto first = app_->orders().placeLimitOrder(session_, "AAPL", OrderSide::Buy, 10, 180.0);
    ASSERT_TRUE(first.ok());
    EXPECT_EQ(first.value().status, OrderStatus::Pending);

    auto second = app_->orders().placeLimitOrder(session_, "AAPL", OrderSide::Buy, 10, 180.0);
    EXPECT_FALSE(second.ok());
    EXPECT_EQ(second.code(), ErrorCode::InsufficientFunds);

    auto working = app_->orders().workingOrders(session_);
    ASSERT_TRUE(working.ok());
    ASSERT_EQ(working.value().size(), 1u);
}

TEST_F(Phase6LimitOrderTest, MarketBuy_RespectsReservedBuyingPower) {
    ASSERT_TRUE(registerUser("trader", "secret1").ok());
    ASSERT_TRUE(app_->wallet().deposit(session_, 2000.0).ok());

    auto resting = app_->orders().placeLimitOrder(session_, "AAPL", OrderSide::Buy, 10, 180.0);
    ASSERT_TRUE(resting.ok());

    auto market = app_->orders().placeMarketOrder(session_, "AAPL", OrderSide::Buy, 10);
    EXPECT_FALSE(market.ok());
    EXPECT_EQ(market.code(), ErrorCode::InsufficientFunds);

    auto cash = app_->wallet().balance(session_);
    ASSERT_TRUE(cash.ok());
    EXPECT_DOUBLE_EQ(cash.value(), 2000.0);

    auto working = app_->orders().workingOrders(session_);
    ASSERT_TRUE(working.ok());
    ASSERT_EQ(working.value().size(), 1u);
}

TEST_F(Phase6LimitOrderTest, OrderBook_AggregatesPendingLevels) {
    ASSERT_TRUE(registerUser("trader", "secret1").ok());
    ASSERT_TRUE(app_->wallet().deposit(session_, 20000.0).ok());
    ASSERT_TRUE(app_->orders().placeMarketOrder(session_, "AAPL", OrderSide::Buy, 20).ok());

    ASSERT_TRUE(app_->orders().placeLimitOrder(session_, "AAPL", OrderSide::Buy, 10, 180.0).ok());
    ASSERT_TRUE(app_->orders().placeLimitOrder(session_, "AAPL", OrderSide::Buy, 5, 180.0).ok());
    ASSERT_TRUE(app_->orders().placeLimitOrder(session_, "AAPL", OrderSide::Sell, 4, 200.0).ok());

    auto book = app_->orders().orderBook("AAPL");
    ASSERT_TRUE(book.ok()) << book.message();
    ASSERT_EQ(book.value().bids.size(), 1u);
    EXPECT_DOUBLE_EQ(book.value().bids[0].price, 180.0);
    EXPECT_EQ(book.value().bids[0].quantity, 15);
    EXPECT_EQ(book.value().bids[0].orderCount, 2);
    ASSERT_EQ(book.value().asks.size(), 1u);
    EXPECT_DOUBLE_EQ(book.value().asks[0].price, 200.0);
    EXPECT_EQ(book.value().asks[0].quantity, 4);
}

TEST_F(Phase6LimitOrderTest, Cancel_RejectsFilledOrForeignOrder) {
    ASSERT_TRUE(registerUser("trader", "secret1").ok());
    ASSERT_TRUE(app_->wallet().deposit(session_, 10000.0).ok());
    auto filled = app_->orders().placeMarketOrder(session_, "AAPL", OrderSide::Buy, 10);
    ASSERT_TRUE(filled.ok());

    auto cancelFilled = app_->orders().cancelOrder(session_, filled.value().orderId);
    EXPECT_FALSE(cancelFilled.ok());
    EXPECT_EQ(cancelFilled.code(), ErrorCode::ValidationFailed);

    auto missing = app_->orders().cancelOrder(session_, 999999);
    EXPECT_FALSE(missing.ok());
    EXPECT_EQ(missing.code(), ErrorCode::NotFound);
}
