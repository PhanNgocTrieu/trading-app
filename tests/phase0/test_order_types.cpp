#include "domain/order_types.hpp"

#include <gtest/gtest.h>

TEST(OrderTypesTest, ToStringOrderSide) {
    EXPECT_STREQ(toString(OrderSide::Buy), "BUY");
    EXPECT_STREQ(toString(OrderSide::Sell), "SELL");
}

TEST(OrderTypesTest, ToStringOrderType) {
    EXPECT_STREQ(toString(OrderType::Market), "MARKET");
    EXPECT_STREQ(toString(OrderType::Limit), "LIMIT");
}

TEST(OrderTypesTest, ToStringOrderStatus) {
    EXPECT_STREQ(toString(OrderStatus::Pending), "PENDING");
    EXPECT_STREQ(toString(OrderStatus::Filled), "FILLED");
    EXPECT_STREQ(toString(OrderStatus::Rejected), "REJECTED");
    EXPECT_STREQ(toString(OrderStatus::Canceled), "CANCELED");
}

TEST(OrderTypesTest, SideFromStringSellVariants) {
    EXPECT_EQ(sideFromString("SELL"), OrderSide::Sell);
    EXPECT_EQ(sideFromString("sell"), OrderSide::Sell);
}

TEST(OrderTypesTest, SideFromStringDefaultsToBuy) {
    EXPECT_EQ(sideFromString("BUY"), OrderSide::Buy);
    EXPECT_EQ(sideFromString("buy"), OrderSide::Buy);
    EXPECT_EQ(sideFromString("unknown"), OrderSide::Buy);
}
