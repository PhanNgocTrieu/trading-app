#include "application/password_hasher.hpp"

#include <gtest/gtest.h>

TEST(PasswordHasherTest, HashAndVerifyRoundTrip) {
    SimplePasswordHasher hasher;
    const auto stored = hasher.hash("secret1");
    EXPECT_NE(stored.find("v1$"), std::string::npos);
    EXPECT_TRUE(hasher.verify("secret1", stored));
    EXPECT_FALSE(hasher.verify("wrong", stored));
}

TEST(PasswordHasherTest, SamePasswordProducesDifferentSalts) {
    SimplePasswordHasher hasher;
    const auto a = hasher.hash("secret1");
    const auto b = hasher.hash("secret1");
    EXPECT_NE(a, b);
    EXPECT_TRUE(hasher.verify("secret1", a));
    EXPECT_TRUE(hasher.verify("secret1", b));
}

TEST(PasswordHasherTest, RejectsMalformedHash) {
    SimplePasswordHasher hasher;
    EXPECT_FALSE(hasher.verify("secret1", "not-a-hash"));
    EXPECT_FALSE(hasher.verify("secret1", "v1$onlysalt"));
}
