#include "domain/result.hpp"

#include <gtest/gtest.h>
#include <string>

TEST(ResultTest, OkStoresValueAndOkFlag) {
    const auto result = Result<int>::ok(42);
    EXPECT_TRUE(result.ok());
    EXPECT_EQ(result.code(), ErrorCode::Ok);
    EXPECT_EQ(result.value(), 42);
    EXPECT_TRUE(result.message().empty());
}

TEST(ResultTest, FailStoresCodeAndMessage) {
    const auto result =
        Result<int>::fail(ErrorCode::NotFound, "missing user");
    EXPECT_FALSE(result.ok());
    EXPECT_EQ(result.code(), ErrorCode::NotFound);
    EXPECT_EQ(result.message(), "missing user");
}

TEST(ResultTest, OkResultHelper) {
    const auto result = okResult();
    EXPECT_TRUE(result.ok());
    EXPECT_EQ(result.code(), ErrorCode::Ok);
}

TEST(ResultTest, FailResultHelper) {
    const auto result =
        failResult(ErrorCode::ValidationFailed, "bad input");
    EXPECT_FALSE(result.ok());
    EXPECT_EQ(result.code(), ErrorCode::ValidationFailed);
    EXPECT_EQ(result.message(), "bad input");
}

TEST(ResultTest, MutableValueAccessor) {
    auto result = Result<std::string>::ok("AAPL");
    result.value() += "-X";
    EXPECT_EQ(result.value(), "AAPL-X");
}
