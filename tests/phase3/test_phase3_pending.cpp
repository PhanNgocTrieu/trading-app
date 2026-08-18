#include <gtest/gtest.h>

// Built only when Qt is not found. Real tests: test_controllers.cpp

TEST(Phase3Skipped, RequiresQt) {
    GTEST_SKIP() << "Qt6 not available — skipping controller tests";
}
