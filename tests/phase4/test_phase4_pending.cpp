#include <gtest/gtest.h>

// Phase 4 market feed / polish is not implemented yet.

TEST(Phase4MockMarketFeedTest, DISABLED_Start_UpdatesQuotesOnTick) {
    GTEST_SKIP() << "Implement MockMarketDataFeed::start / onTick";
}

TEST(Phase4MockMarketFeedTest, DISABLED_Stop_StopsTimerUpdates) {
    GTEST_SKIP() << "Implement MockMarketDataFeed::stop";
}

TEST(Phase4PortfolioMarkToMarketTest, DISABLED_UnrealizedPnlTracksQuoteChanges) {
    GTEST_SKIP() << "Implement mark-to-market portfolio recalculation";
}

TEST(Phase4IntegrationTest, DISABLED_RegisterDepositBuySell_CashConsistent) {
    GTEST_SKIP() << "End-to-end Phase 4 integration scenario";
}
