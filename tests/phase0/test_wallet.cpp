#include "support/app_fixture.hpp"

TEST_F(Phase0WalletTest, BalanceReturnsZeroAfterRegister) {
    ASSERT_TRUE(registerUser("alice", "secret1").ok());
    const auto balance = app_->wallet().balance(session_);
    ASSERT_TRUE(balance.ok());
    EXPECT_DOUBLE_EQ(balance.value(), 0.0);
}

TEST_F(Phase0WalletTest, DepositUpdatesBalance) {
    ASSERT_TRUE(registerUser("alice", "secret1").ok());
    const auto deposited = app_->wallet().deposit(session_, 1000.0);
    ASSERT_TRUE(deposited.ok());
    EXPECT_DOUBLE_EQ(deposited.value(), 1000.0);
}

TEST_F(Phase0WalletTest, DepositRejectsInvalidAmount) {
    ASSERT_TRUE(registerUser("alice", "secret1").ok());
    const auto result = app_->wallet().deposit(session_, -5.0);
    EXPECT_FALSE(result.ok());
    EXPECT_EQ(result.code(), ErrorCode::ValidationFailed);
}

TEST_F(Phase0WalletTest, WithdrawUpdatesBalance) {
    ASSERT_TRUE(registerUser("alice", "secret1").ok());
    ASSERT_TRUE(app_->wallet().deposit(session_, 1000.0).ok());
    const auto withdrawn = app_->wallet().withdraw(session_, 250.0);
    ASSERT_TRUE(withdrawn.ok());
    EXPECT_DOUBLE_EQ(withdrawn.value(), 750.0);
}

TEST_F(Phase0WalletTest, OperationsFailWithoutSession) {
    AuthSession empty;
    EXPECT_EQ(app_->wallet().balance(empty).code(), ErrorCode::NotFound);
    EXPECT_EQ(app_->wallet().deposit(empty, 10.0).code(), ErrorCode::NotFound);
    EXPECT_EQ(app_->wallet().withdraw(empty, 10.0).code(), ErrorCode::NotFound);
}
