#include "tests/phase1/test_support.hpp"

TEST_F(Phase1Fixture, BalanceReturnsZeroAfterRegister) {
    ASSERT_EQ(login_->registerWithCredentials("alice", "secret1"), LoginStatus::Success);
    const auto balance = bank_->balance();
    ASSERT_TRUE(balance.ok());
    EXPECT_DOUBLE_EQ(balance.value(), 0.0);
}

TEST_F(Phase1Fixture, DepositUpdatesBalance) {
    ASSERT_EQ(login_->registerWithCredentials("alice", "secret1"), LoginStatus::Success);
    const auto deposited = bank_->deposit(1000.0);
    ASSERT_TRUE(deposited.ok());
    EXPECT_DOUBLE_EQ(deposited.value(), 1000.0);
}

TEST_F(Phase1Fixture, DepositRejectsInvalidAmount) {
    ASSERT_EQ(login_->registerWithCredentials("alice", "secret1"), LoginStatus::Success);
    const auto result = bank_->deposit(-5.0);
    EXPECT_FALSE(result.ok());
    EXPECT_EQ(result.code(), ErrorCode::ValidationFailed);
}

TEST_F(Phase1Fixture, WithdrawUpdatesBalance) {
    ASSERT_EQ(login_->registerWithCredentials("alice", "secret1"), LoginStatus::Success);
    ASSERT_TRUE(bank_->deposit(1000.0).ok());
    const auto withdrawn = bank_->withdraw(250.0);
    ASSERT_TRUE(withdrawn.ok());
    EXPECT_DOUBLE_EQ(withdrawn.value(), 750.0);
}

TEST_F(Phase1Fixture, OperationsFailWhenLoggedOut) {
    ASSERT_EQ(login_->registerWithCredentials("alice", "secret1"), LoginStatus::Success);
    login_->logout();
    EXPECT_EQ(bank_->balance().code(), ErrorCode::Unauthorized);
    EXPECT_EQ(bank_->deposit(10.0).code(), ErrorCode::Unauthorized);
    EXPECT_EQ(bank_->withdraw(10.0).code(), ErrorCode::Unauthorized);
}
