#include <gtest/gtest.h>

// Built only when Qt is not found. Real tests: test_market_feed.cpp

TEST(Phase4Skipped, RequiresQt) {
    GTEST_SKIP() << "Qt6 not available — skipping market-feed tests";
}
