#include "engine/matching_engine.hpp"

#include <gtest/gtest.h>

TEST(MatchingEngineTest, MarketOrderFillsAtLastPrice) {
    const MatchingEngine engine;
    const IncomingOrder order{"AAPL", OrderSide::Buy, OrderType::Market, 10, 0.0};
    const MarketSnapshot quote{"AAPL", 190.5};

    const MatchResult result = engine.match(order, quote);
    EXPECT_TRUE(result.filled);
    EXPECT_DOUBLE_EQ(result.fillPrice, 190.5);
    EXPECT_EQ(result.fillQty, 10);
    EXPECT_TRUE(result.rejectReason.empty());
}

TEST(MatchingEngineTest, RejectsNonPositiveQuantity) {
    const MatchingEngine engine;
    const IncomingOrder order{"AAPL", OrderSide::Buy, OrderType::Market, 0, 0.0};
    const MarketSnapshot quote{"AAPL", 190.0};

    const MatchResult result = engine.match(order, quote);
    EXPECT_FALSE(result.filled);
    EXPECT_EQ(result.rejectReason, "quantity must be > 0");
}

TEST(MatchingEngineTest, RejectsSymbolMismatch) {
    const MatchingEngine engine;
    const IncomingOrder order{"AAPL", OrderSide::Buy, OrderType::Market, 1, 0.0};
    const MarketSnapshot quote{"MSFT", 420.0};

    const MatchResult result = engine.match(order, quote);
    EXPECT_FALSE(result.filled);
    EXPECT_EQ(result.rejectReason, "symbol mismatch");
}

TEST(MatchingEngineTest, RejectsInvalidMarketPrice) {
    const MatchingEngine engine;
    const IncomingOrder order{"AAPL", OrderSide::Buy, OrderType::Market, 1, 0.0};
    const MarketSnapshot quote{"AAPL", 0.0};

    const MatchResult result = engine.match(order, quote);
    EXPECT_FALSE(result.filled);
    EXPECT_EQ(result.rejectReason, "invalid market price");
}

TEST(MatchingEngineTest, LimitBuyFillsWhenMarketAtOrBelowLimit) {
    const MatchingEngine engine;
    const IncomingOrder order{"AAPL", OrderSide::Buy, OrderType::Limit, 5, 200.0};
    const MarketSnapshot quote{"AAPL", 190.0};

    const MatchResult result = engine.match(order, quote);
    EXPECT_TRUE(result.filled);
    EXPECT_DOUBLE_EQ(result.fillPrice, 190.0);
    EXPECT_EQ(result.fillQty, 5);
}

TEST(MatchingEngineTest, LimitBuyRejectsWhenMarketAboveLimit) {
    const MatchingEngine engine;
    const IncomingOrder order{"AAPL", OrderSide::Buy, OrderType::Limit, 5, 180.0};
    const MarketSnapshot quote{"AAPL", 190.0};

    const MatchResult result = engine.match(order, quote);
    EXPECT_FALSE(result.filled);
    EXPECT_EQ(result.rejectReason, "limit not marketable");
}

TEST(MatchingEngineTest, LimitSellFillsWhenMarketAtOrAboveLimit) {
    const MatchingEngine engine;
    const IncomingOrder order{"AAPL", OrderSide::Sell, OrderType::Limit, 3, 180.0};
    const MarketSnapshot quote{"AAPL", 190.0};

    const MatchResult result = engine.match(order, quote);
    EXPECT_TRUE(result.filled);
    EXPECT_EQ(result.fillQty, 3);
}

TEST(MatchingEngineTest, LimitSellRejectsWhenMarketBelowLimit) {
    const MatchingEngine engine;
    const IncomingOrder order{"AAPL", OrderSide::Sell, OrderType::Limit, 3, 200.0};
    const MarketSnapshot quote{"AAPL", 190.0};

    const MatchResult result = engine.match(order, quote);
    EXPECT_FALSE(result.filled);
    EXPECT_EQ(result.rejectReason, "limit not marketable");
}
