#include "domain/account.hpp"

#include <gtest/gtest.h>

TEST(AccountTest, ConstructorAndGetters) {
    const Account account{10, 7, 125.5};
    EXPECT_EQ(account.id(), 10);
    EXPECT_EQ(account.userId(), 7);
    EXPECT_DOUBLE_EQ(account.cashBalance(), 125.5);
}

TEST(AccountTest, DepositIncreasesBalance) {
    Account account{1, 1, 0.0};
    const auto result = account.deposit(1000.0);
    EXPECT_TRUE(result.ok());
    EXPECT_DOUBLE_EQ(account.cashBalance(), 1000.0);
}

TEST(AccountTest, DepositRejectsNonPositiveAmount) {
    Account account{1, 1, 50.0};
    const auto zero = account.deposit(0.0);
    EXPECT_FALSE(zero.ok());
    EXPECT_EQ(zero.code(), ErrorCode::ValidationFailed);
    EXPECT_DOUBLE_EQ(account.cashBalance(), 50.0);

    const auto negative = account.deposit(-10.0);
    EXPECT_FALSE(negative.ok());
    EXPECT_EQ(negative.code(), ErrorCode::ValidationFailed);
    EXPECT_DOUBLE_EQ(account.cashBalance(), 50.0);
}

TEST(AccountTest, WithdrawDecreasesBalance) {
    Account account{1, 1, 1000.0};
    const auto result = account.withdraw(250.0);
    EXPECT_TRUE(result.ok());
    EXPECT_DOUBLE_EQ(account.cashBalance(), 750.0);
}

TEST(AccountTest, WithdrawRejectsNonPositiveAmount) {
    Account account{1, 1, 100.0};
    const auto result = account.withdraw(0.0);
    EXPECT_FALSE(result.ok());
    EXPECT_EQ(result.code(), ErrorCode::ValidationFailed);
    EXPECT_DOUBLE_EQ(account.cashBalance(), 100.0);
}

TEST(AccountTest, WithdrawRejectsInsufficientFunds) {
    Account account{1, 1, 100.0};
    const auto result = account.withdraw(100.01);
    EXPECT_FALSE(result.ok());
    EXPECT_EQ(result.code(), ErrorCode::InsufficientFunds);
    EXPECT_DOUBLE_EQ(account.cashBalance(), 100.0);
}

TEST(AccountTest, WithdrawExactBalanceSucceeds) {
    Account account{1, 1, 100.0};
    const auto result = account.withdraw(100.0);
    EXPECT_TRUE(result.ok());
    EXPECT_DOUBLE_EQ(account.cashBalance(), 0.0);
}
