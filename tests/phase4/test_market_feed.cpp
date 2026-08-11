#include "application/app_bootstrap.hpp"
#include "controllers/auth_controller.hpp"
#include "controllers/order_controller.hpp"
#include "controllers/wallet_controller.hpp"
#include "domain/order_types.hpp"
#include "dto.hpp"
#include "market/mock_market_data_feed.hpp"

#include <QCoreApplication>
#include <QSignalSpy>
#include <QTest>
#include <QVector>

#include <gtest/gtest.h>

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace {

int& qtArgc() {
    static int argc = 1;
    return argc;
}

char** qtArgv() {
    static char arg0[] = "trading_unit_tests";
    static char* argv[] = {arg0, nullptr};
    return argv;
}

void ensureQtApp() {
    if (QCoreApplication::instance() == nullptr) {
        static QCoreApplication app(qtArgc(), qtArgv());
        (void)app;
    }
    qRegisterMetaType<desktop::SessionDto>("desktop::SessionDto");
    qRegisterMetaType<desktop::OrderUiDto>("desktop::OrderUiDto");
    qRegisterMetaType<desktop::PositionUiDto>("desktop::PositionUiDto");
    qRegisterMetaType<desktop::QuoteUiDto>("desktop::QuoteUiDto");
    qRegisterMetaType<QVector<desktop::PositionUiDto>>("QVector<desktop::PositionUiDto>");
    qRegisterMetaType<QVector<desktop::QuoteUiDto>>("QVector<desktop::QuoteUiDto>");
}

class Phase4Fixture : public ::testing::Test {
protected:
    static void SetUpTestSuite() { ensureQtApp(); }

    void SetUp() override {
        app_ = std::make_unique<AppBootstrap>(AppBootstrap::open(":memory:"));
        auth_ = std::make_unique<desktop::AuthController>(app_->auth(), app_->accounts());
        wallet_ = std::make_unique<desktop::WalletController>(app_->wallet(), *auth_);
        orders_ = std::make_unique<desktop::OrderController>(app_->orders(), *auth_);
    }

    void TearDown() override {
        orders_.reset();
        wallet_.reset();
        auth_.reset();
        app_.reset();
    }

    std::unique_ptr<AppBootstrap> app_;
    std::unique_ptr<desktop::AuthController> auth_;
    std::unique_ptr<desktop::WalletController> wallet_;
    std::unique_ptr<desktop::OrderController> orders_;
};

} // namespace

TEST(Phase4MockMarketFeedTest, Start_UpdatesQuotesOnTick) {
    ensureQtApp();
    auto bootstrap = AppBootstrap::open(":memory:");

    desktop::MockMarketDataFeed feed;
    feed.setSeed(42);
    feed.setSymbols({"AAPL", "MSFT"});
    feed.setPriceIO(
        [&bootstrap](const std::string& symbol) -> std::optional<double> {
            auto quote = bootstrap.quotes().find(symbol);
            if (!quote) {
                return std::nullopt;
            }
            return quote->lastPrice;
        },
        [&bootstrap](const std::string& symbol, double price) {
            bootstrap.quotes().setLastPrice(symbol, price);
        });

    const double aaplBefore = bootstrap.quotes().find("AAPL")->lastPrice;
    QSignalSpy spy(&feed, &desktop::MockMarketDataFeed::quotesUpdated);

    feed.start(50);
    ASSERT_TRUE(feed.isActive());
    ASSERT_TRUE(spy.wait(500));
    feed.stop();

    EXPECT_GE(feed.tickCount(), 1);
    EXPECT_GE(spy.count(), 1);
    const double aaplAfter = bootstrap.quotes().find("AAPL")->lastPrice;
    EXPECT_NE(aaplAfter, aaplBefore);
    EXPECT_GT(aaplAfter, 0.0);
}

TEST(Phase4MockMarketFeedTest, Stop_StopsTimerUpdates) {
    ensureQtApp();
    auto bootstrap = AppBootstrap::open(":memory:");

    desktop::MockMarketDataFeed feed;
    feed.setSymbols({"AAPL"});
    feed.setPriceIO(
        [&bootstrap](const std::string& symbol) -> std::optional<double> {
            return bootstrap.quotes().find(symbol)->lastPrice;
        },
        [&bootstrap](const std::string& symbol, double price) {
            bootstrap.quotes().setLastPrice(symbol, price);
        });

    feed.start(50);
    ASSERT_TRUE(QSignalSpy(&feed, &desktop::MockMarketDataFeed::quotesUpdated).wait(300));
    feed.stop();
    EXPECT_FALSE(feed.isActive());

    const int ticksAtStop = feed.tickCount();
    QTest::qWait(150);
    EXPECT_EQ(feed.tickCount(), ticksAtStop);
}

TEST_F(Phase4Fixture, UnrealizedPnlTracksQuoteChanges) {
    auth_->registerUser(QStringLiteral("mtm"), QStringLiteral("secret1"));
    wallet_->deposit(10000.0);
    ASSERT_TRUE(
        orders_->orders().placeMarketOrder(auth_->authSession(), "AAPL", OrderSide::Buy, 10).ok());

    QSignalSpy portfolioSpy(orders_.get(), &desktop::OrderController::portfolioUpdated);
    orders_->refreshPortfolio();
    ASSERT_GE(portfolioSpy.count(), 1);
    auto rows = portfolioSpy.takeLast().at(0).value<QVector<desktop::PositionUiDto>>();
    ASSERT_EQ(rows.size(), 1);
    EXPECT_DOUBLE_EQ(rows[0].marketPrice, 190.0);
    EXPECT_DOUBLE_EQ(rows[0].unrealizedPnl, 0.0);

    ASSERT_TRUE(app_->orders().setQuotePrice("AAPL", 200.0).ok());
    orders_->refreshPortfolio();
    ASSERT_GE(portfolioSpy.count(), 1);
    rows = portfolioSpy.takeLast().at(0).value<QVector<desktop::PositionUiDto>>();
    ASSERT_EQ(rows.size(), 1);
    EXPECT_DOUBLE_EQ(rows[0].marketPrice, 200.0);
    EXPECT_DOUBLE_EQ(rows[0].marketValue, 2000.0);
    EXPECT_DOUBLE_EQ(rows[0].unrealizedPnl, 100.0); // (200-190)*10
}

TEST_F(Phase4Fixture, RegisterDepositBuySell_CashConsistent) {
    auth_->registerUser(QStringLiteral("e2e"), QStringLiteral("secret1"));
    wallet_->deposit(10000.0);

    QSignalSpy accepted(orders_.get(), &desktop::OrderController::orderAccepted);
    QSignalSpy cashSpy(wallet_.get(), &desktop::WalletController::cashUpdated);

    orders_->placeMarketOrder(QStringLiteral("AAPL"), QStringLiteral("BUY"), 10);
    ASSERT_EQ(accepted.count(), 1);
    wallet_->refreshCash();
    ASSERT_FALSE(cashSpy.isEmpty());
    EXPECT_DOUBLE_EQ(cashSpy.takeLast().at(0).toDouble(), 8100.0);

    ASSERT_TRUE(app_->orders().setQuotePrice("AAPL", 220.0).ok());
    orders_->placeMarketOrder(QStringLiteral("AAPL"), QStringLiteral("SELL"), 5);
    wallet_->refreshCash();
    EXPECT_DOUBLE_EQ(cashSpy.takeLast().at(0).toDouble(), 9200.0); // 8100 + 1100

    auto portfolio = app_->orders().portfolio(auth_->authSession());
    ASSERT_TRUE(portfolio.ok());
    ASSERT_EQ(portfolio.value().size(), 1u);
    EXPECT_EQ(portfolio.value()[0].quantity, 5);
}
