#include "tests/phase1/test_support.hpp"

// Phase 0 login tests now run against Phase 1 SQLite-backed LoginService.

TEST_F(Phase1Fixture, StartsLoggedOut) {
    EXPECT_FALSE(login_->isLoggedIn());
    EXPECT_EQ(login_->session(), nullptr);
}

TEST_F(Phase1Fixture, LoginFailsWhenUsernameEmpty) {
    EXPECT_EQ(login_->loginWithCredentials("", "secret1"), LoginStatus::Failure);
    EXPECT_FALSE(login_->isLoggedIn());
}

TEST_F(Phase1Fixture, LoginFailsWhenPasswordEmpty) {
    EXPECT_EQ(login_->registerWithCredentials("alice", "secret1"), LoginStatus::Success);
    login_->logout();
    EXPECT_EQ(login_->loginWithCredentials("alice", ""), LoginStatus::Failure);
}

TEST_F(Phase1Fixture, LogoutClearsSession) {
    ASSERT_EQ(login_->registerWithCredentials("bob", "secret1"), LoginStatus::Success);
    EXPECT_EQ(login_->logout(), LoginStatus::LogoutSuccess);
    EXPECT_FALSE(login_->isLoggedIn());
    EXPECT_EQ(login_->session(), nullptr);
}
