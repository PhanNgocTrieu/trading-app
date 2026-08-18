#include "application/app_bootstrap.hpp"
#include "controllers/auth_controller.hpp"
#include "controllers/order_controller.hpp"
#include "controllers/wallet_controller.hpp"
#include "dto.hpp"
#include "models/position_table_model.hpp"

#include <QCoreApplication>
#include <QSignalSpy>
#include <QVector>

#include <gtest/gtest.h>
#include <memory>

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

class Phase3Fixture : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        if (QCoreApplication::instance() == nullptr) {
            // Owned for process lifetime; Qt docs allow a single app instance.
            static QCoreApplication app(qtArgc(), qtArgv());
            (void)app;
        }
        qRegisterMetaType<desktop::SessionDto>("desktop::SessionDto");
        qRegisterMetaType<desktop::OrderUiDto>("desktop::OrderUiDto");
        qRegisterMetaType<desktop::PositionUiDto>("desktop::PositionUiDto");
        qRegisterMetaType<QVector<desktop::PositionUiDto>>("QVector<desktop::PositionUiDto>");
        qRegisterMetaType<QVector<desktop::OrderUiDto>>("QVector<desktop::OrderUiDto>");
        qRegisterMetaType<QVector<desktop::BookLevelUiDto>>("QVector<desktop::BookLevelUiDto>");
    }

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

TEST_F(Phase3Fixture, Login_EmitsSucceededOnValidCredentials) {
    QSignalSpy okSpy(auth_.get(), &desktop::AuthController::loginSucceeded);
    QSignalSpy failSpy(auth_.get(), &desktop::AuthController::authFailed);

    auth_->registerUser(QStringLiteral("alice"), QStringLiteral("secret1"));
    ASSERT_EQ(okSpy.count(), 1);
    EXPECT_EQ(failSpy.count(), 0);

    const auto session = okSpy.takeFirst().at(0).value<desktop::SessionDto>();
    EXPECT_EQ(session.username, QStringLiteral("alice"));
    EXPECT_GT(session.accountId, 0);
}

TEST_F(Phase3Fixture, Login_EmitsFailedOnInvalidCredentials) {
    auth_->registerUser(QStringLiteral("bob"), QStringLiteral("secret1"));
    auth_->logout();

    QSignalSpy okSpy(auth_.get(), &desktop::AuthController::loginSucceeded);
    QSignalSpy failSpy(auth_.get(), &desktop::AuthController::authFailed);
    auth_->login(QStringLiteral("bob"), QStringLiteral("wrongpw"));

    EXPECT_EQ(okSpy.count(), 0);
    ASSERT_EQ(failSpy.count(), 1);
}

TEST_F(Phase3Fixture, RegisterUser_EmitsSucceeded) {
    QSignalSpy okSpy(auth_.get(), &desktop::AuthController::loginSucceeded);
    auth_->registerUser(QStringLiteral("carol"), QStringLiteral("secret1"));
    ASSERT_EQ(okSpy.count(), 1);
}

TEST_F(Phase3Fixture, PlaceMarketOrder_EmitsAccepted) {
    auth_->registerUser(QStringLiteral("trader"), QStringLiteral("secret1"));
    wallet_->deposit(10000.0);

    QSignalSpy accepted(orders_.get(), &desktop::OrderController::orderAccepted);
    QSignalSpy rejected(orders_.get(), &desktop::OrderController::orderRejected);
    QSignalSpy portfolio(orders_.get(), &desktop::OrderController::portfolioUpdated);

    orders_->placeMarketOrder(QStringLiteral("AAPL"), QStringLiteral("BUY"), 10);

    ASSERT_EQ(accepted.count(), 1) << (rejected.isEmpty() ? "" : rejected.takeFirst().at(0).toString().toStdString());
    EXPECT_EQ(rejected.count(), 0);
    EXPECT_GE(portfolio.count(), 1);

    const auto order = accepted.takeFirst().at(0).value<desktop::OrderUiDto>();
    EXPECT_EQ(order.symbol, QStringLiteral("AAPL"));
    EXPECT_EQ(order.quantity, 10);
    EXPECT_DOUBLE_EQ(order.fillPrice, 190.0);
}

TEST_F(Phase3Fixture, PlaceMarketOrder_EmitsRejected) {
    auth_->registerUser(QStringLiteral("broke"), QStringLiteral("secret1"));
    wallet_->deposit(100.0);

    QSignalSpy accepted(orders_.get(), &desktop::OrderController::orderAccepted);
    QSignalSpy rejected(orders_.get(), &desktop::OrderController::orderRejected);

    orders_->placeMarketOrder(QStringLiteral("AAPL"), QStringLiteral("BUY"), 10);

    EXPECT_EQ(accepted.count(), 0);
    ASSERT_EQ(rejected.count(), 1);
    EXPECT_FALSE(rejected.takeFirst().at(0).toString().isEmpty());
}

TEST_F(Phase3Fixture, PlaceLimitOrder_EmitsAcceptedWhenResting) {
    auth_->registerUser(QStringLiteral("limiter"), QStringLiteral("secret1"));
    wallet_->deposit(10000.0);

    QSignalSpy accepted(orders_.get(), &desktop::OrderController::orderAccepted);
    QSignalSpy rejected(orders_.get(), &desktop::OrderController::orderRejected);
    QSignalSpy working(orders_.get(), &desktop::OrderController::workingOrdersUpdated);

    orders_->placeLimitOrder(QStringLiteral("AAPL"), QStringLiteral("BUY"), 10, 180.0);

    ASSERT_EQ(accepted.count(), 1) << (rejected.isEmpty() ? "" : rejected.takeFirst().at(0).toString().toStdString());
    EXPECT_EQ(rejected.count(), 0);
    EXPECT_GE(working.count(), 1);

    const auto order = accepted.takeFirst().at(0).value<desktop::OrderUiDto>();
    EXPECT_EQ(order.symbol, QStringLiteral("AAPL"));
    EXPECT_EQ(order.status, QStringLiteral("PENDING"));
    EXPECT_TRUE(order.hasLimitPrice);
    EXPECT_DOUBLE_EQ(order.limitPrice, 180.0);
}

TEST(Phase3PortfolioPresentationTest, Refresh_MapsPortfolioDto) {
    if (QCoreApplication::instance() == nullptr) {
        static QCoreApplication app(qtArgc(), qtArgv());
        (void)app;
    }

    desktop::PositionTableModel model;
    QVector<desktop::PositionUiDto> rows;
    desktop::PositionUiDto row;
    row.symbol = QStringLiteral("AAPL");
    row.quantity = 10;
    row.avgCost = 190.0;
    row.marketPrice = 200.0;
    row.marketValue = 2000.0;
    row.unrealizedPnl = 100.0;
    rows.push_back(row);

    model.setPositions(rows);
    ASSERT_EQ(model.rowCount(), 1);
    ASSERT_EQ(model.columnCount(), 6);
    EXPECT_EQ(model.data(model.index(0, 0)).toString(), QStringLiteral("AAPL"));
    EXPECT_EQ(model.data(model.index(0, 1)).toInt(), 10);
    EXPECT_EQ(model.headerData(5, Qt::Horizontal, Qt::DisplayRole).toString(),
              QStringLiteral("uPnL"));
}
