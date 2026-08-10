#include "domain/position.hpp"

#include <gtest/gtest.h>

TEST(PositionTest, DefaultIsEmpty) {
    const Position position;
    EXPECT_TRUE(position.symbol().empty());
    EXPECT_EQ(position.quantity(), 0);
    EXPECT_DOUBLE_EQ(position.avgCost(), 0.0);
}

TEST(PositionTest, ConstructorSetsFields) {
    const Position position{"AAPL", 10, 190.0};
    EXPECT_EQ(position.symbol(), "AAPL");
    EXPECT_EQ(position.quantity(), 10);
    EXPECT_DOUBLE_EQ(position.avgCost(), 190.0);
}

TEST(PositionTest, ApplyBuyFromEmptySetsQtyAndAvg) {
    Position position{"AAPL", 0, 0.0};
    const auto result = position.applyBuy(10, 100.0);
    EXPECT_TRUE(result.ok());
    EXPECT_EQ(position.quantity(), 10);
    EXPECT_DOUBLE_EQ(position.avgCost(), 100.0);
}

TEST(PositionTest, ApplyBuyRecomputesAverageCost) {
    Position position{"AAPL", 10, 100.0};
    const auto result = position.applyBuy(10, 120.0);
    EXPECT_TRUE(result.ok());
    EXPECT_EQ(position.quantity(), 20);
    EXPECT_DOUBLE_EQ(position.avgCost(), 110.0);
}

TEST(PositionTest, ApplyBuyRejectsInvalidQtyOrPrice) {
    Position position{"AAPL", 1, 10.0};
    EXPECT_EQ(position.applyBuy(0, 10.0).code(), ErrorCode::ValidationFailed);
    EXPECT_EQ(position.applyBuy(-1, 10.0).code(), ErrorCode::ValidationFailed);
    EXPECT_EQ(position.applyBuy(1, 0.0).code(), ErrorCode::ValidationFailed);
    EXPECT_EQ(position.applyBuy(1, -1.0).code(), ErrorCode::ValidationFailed);
    EXPECT_EQ(position.quantity(), 1);
    EXPECT_DOUBLE_EQ(position.avgCost(), 10.0);
}

TEST(PositionTest, ApplySellReducesQtyAndReturnsRealizedPnl) {
    Position position{"AAPL", 20, 110.0};
    const auto result = position.applySell(5, 130.0);
    ASSERT_TRUE(result.ok());
    EXPECT_DOUBLE_EQ(result.value(), 100.0); // (130-110)*5
    EXPECT_EQ(position.quantity(), 15);
    EXPECT_DOUBLE_EQ(position.avgCost(), 110.0); // avg unchanged on sell
}

TEST(PositionTest, ApplySellRejectsOversell) {
    Position position{"AAPL", 5, 100.0};
    const auto result = position.applySell(6, 120.0);
    EXPECT_FALSE(result.ok());
    EXPECT_EQ(result.code(), ErrorCode::InsufficientPosition);
    EXPECT_EQ(position.quantity(), 5);
}

TEST(PositionTest, ApplySellRejectsInvalidQtyOrPrice) {
    Position position{"AAPL", 5, 100.0};
    EXPECT_EQ(position.applySell(0, 100.0).code(), ErrorCode::ValidationFailed);
    EXPECT_EQ(position.applySell(1, 0.0).code(), ErrorCode::ValidationFailed);
}
