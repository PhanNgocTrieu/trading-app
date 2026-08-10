#include "domain/session.hpp"

#include <gtest/gtest.h>

class SessionTest : public ::testing::Test {
protected:
    void SetUp() override { resetCurrentSession(); }
    void TearDown() override { resetCurrentSession(); }
};

TEST_F(SessionTest, CurrentSessionStartsEmpty) {
    EXPECT_FALSE(currentSession().has_value());
}

TEST_F(SessionTest, CanAssignAndReadSession) {
    currentSession() = Session{User{1, "alice"}, Account{10, 1, 100.0}};
    ASSERT_TRUE(currentSession().has_value());
    EXPECT_EQ(currentSession()->user.username(), "alice");
    EXPECT_EQ(currentSession()->account.id(), 10);
    EXPECT_DOUBLE_EQ(currentSession()->account.cashBalance(), 100.0);
}

TEST_F(SessionTest, ResetCurrentSessionClearsState) {
    currentSession() = Session{User{1, "alice"}, Account{10, 1, 0.0}};
    resetCurrentSession();
    EXPECT_FALSE(currentSession().has_value());
}
