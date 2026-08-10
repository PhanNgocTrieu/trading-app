#include "domain/user.hpp"

#include <gtest/gtest.h>

TEST(UserTest, ConstructorAndGetters) {
    const User user{3, "alice", "0900123456"};
    EXPECT_EQ(user.id(), 3);
    EXPECT_EQ(user.username(), "alice");
    EXPECT_EQ(user.phoneNumber(), "0900123456");
}

TEST(UserTest, DefaultPhoneIsEmpty) {
    const User user{1, "bob"};
    EXPECT_TRUE(user.phoneNumber().empty());
}

TEST(UserTest, RenameUpdatesUsername) {
    User user{1, "old"};
    user.rename("new");
    EXPECT_EQ(user.username(), "new");
}

TEST(UserTest, SetPhoneNumberUpdatesPhone) {
    User user{1, "alice"};
    user.setPhoneNumber("111");
    EXPECT_EQ(user.phoneNumber(), "111");
}
