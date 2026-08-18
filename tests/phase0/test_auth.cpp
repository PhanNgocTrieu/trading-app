#include "support/app_fixture.hpp"

TEST_F(Phase0AuthTest, StartsLoggedOut) {
    EXPECT_FALSE(loggedIn_);
    EXPECT_EQ(session_.accountId, 0);
}

TEST_F(Phase0AuthTest, LoginFailsWhenUsernameEmpty) {
    auto result = login("", "secret1");
    EXPECT_FALSE(result.ok());
    EXPECT_FALSE(loggedIn_);
}

TEST_F(Phase0AuthTest, LoginFailsWhenPasswordEmpty) {
    ASSERT_TRUE(registerUser("alice", "secret1").ok());
    logout();
    auto result = login("alice", "");
    EXPECT_FALSE(result.ok());
    EXPECT_FALSE(loggedIn_);
}

TEST_F(Phase0AuthTest, LogoutClearsSession) {
    ASSERT_TRUE(registerUser("bob", "secret1").ok());
    logout();
    EXPECT_FALSE(loggedIn_);
    EXPECT_EQ(session_.accountId, 0);
}
