#include "domain/stock.hpp"

#include <gtest/gtest.h>

TEST(StockTest, ConstructorAndGetters) {
    const Stock stock{"AAPL", "Apple Inc.", 190.0};
    EXPECT_EQ(stock.symbol(), "AAPL");
    EXPECT_EQ(stock.name(), "Apple Inc.");
    EXPECT_DOUBLE_EQ(stock.lastPrice(), 190.0);
}

TEST(StockTest, DefaultPriceIsZero) {
    const Stock stock{"MSFT", "Microsoft"};
    EXPECT_DOUBLE_EQ(stock.lastPrice(), 0.0);
}

TEST(StockTest, UpdatePriceChangesLastPrice) {
    Stock stock{"TSLA", "Tesla", 200.0};
    stock.updatePrice(210.5);
    EXPECT_DOUBLE_EQ(stock.lastPrice(), 210.5);
}
