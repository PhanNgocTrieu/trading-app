#include "application/app_bootstrap.hpp"
#include "controllers/auth_controller.hpp"
#include "controllers/order_controller.hpp"
#include "controllers/wallet_controller.hpp"
#include "dto.hpp"
#include "market/mock_market_data_feed.hpp"
#include "windows/login_window.hpp"
#include "windows/main_window.hpp"

#include <QApplication>
#include <QCommandLineParser>
#include <QMessageBox>
#include <QVector>

#include <optional>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("Trading App"));
    QApplication::setApplicationVersion(QStringLiteral("1.0"));

    qRegisterMetaType<desktop::SessionDto>("desktop::SessionDto");
    qRegisterMetaType<desktop::OrderUiDto>("desktop::OrderUiDto");
    qRegisterMetaType<desktop::PositionUiDto>("desktop::PositionUiDto");
    qRegisterMetaType<desktop::QuoteUiDto>("desktop::QuoteUiDto");
    qRegisterMetaType<QVector<desktop::PositionUiDto>>("QVector<desktop::PositionUiDto>");
    qRegisterMetaType<QVector<desktop::QuoteUiDto>>("QVector<desktop::QuoteUiDto>");

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Paper trading desktop app"));
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption dbOption(
        QStringList{QStringLiteral("db"), QStringLiteral("database")},
        QStringLiteral("SQLite database file path"),
        QStringLiteral("path"));
    parser.addOption(dbOption);
    parser.process(app);

    const std::string dbPath = parser.isSet(dbOption)
                                   ? parser.value(dbOption).toStdString()
                                   : AppBootstrap::defaultDbPath();

    try {
        auto bootstrap = AppBootstrap::open(dbPath);

        desktop::AuthController authController(bootstrap.auth(), bootstrap.accounts());
        desktop::WalletController walletController(bootstrap.wallet(), authController);
        desktop::OrderController orderController(bootstrap.orders(), authController);

        desktop::MockMarketDataFeed feed;
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

        desktop::LoginWindow loginWindow;
        desktop::MainWindow mainWindow;
        mainWindow.setDbPath(QString::fromStdString(bootstrap.dbPath()));

        auto refreshMarketUi = [&] {
            orderController.refreshQuotes();
            orderController.refreshPortfolio();
        };

        QObject::connect(&loginWindow, &desktop::LoginWindow::loginRequested,
                         &authController, &desktop::AuthController::login);
        QObject::connect(&loginWindow, &desktop::LoginWindow::registerRequested,
                         &authController, &desktop::AuthController::registerUser);
        QObject::connect(&authController, &desktop::AuthController::authFailed,
                         &loginWindow, &desktop::LoginWindow::showError);

        QObject::connect(&authController, &desktop::AuthController::loginSucceeded,
                         &app, [&](const desktop::SessionDto& session) {
                             loginWindow.hide();
                             mainWindow.setSession(session);
                             mainWindow.show();

                             auto quotes = bootstrap.quotes().listAll();
                             std::vector<std::string> symbols;
                             symbols.reserve(quotes.size());
                             for (const auto& q : quotes) {
                                 symbols.push_back(q.symbol);
                             }
                             feed.setSymbols(std::move(symbols));

                             refreshMarketUi();
                             walletController.refreshCash();
                             feed.start(1000);
                             mainWindow.setFeedActive(true);
                             mainWindow.showStatus(QStringLiteral("Logged in — live mock feed"));
                         });

        QObject::connect(&mainWindow, &desktop::MainWindow::depositRequested,
                         &walletController, &desktop::WalletController::deposit);
        QObject::connect(&walletController, &desktop::WalletController::cashUpdated,
                         &mainWindow, &desktop::MainWindow::updateCash);
        QObject::connect(&walletController, &desktop::WalletController::walletSucceeded,
                         &mainWindow, &desktop::MainWindow::showStatus);
        QObject::connect(&walletController, &desktop::WalletController::walletFailed,
                         &mainWindow, [&](const QString& message) {
                             mainWindow.showStatus(message);
                             QMessageBox::warning(&mainWindow, QStringLiteral("Wallet"), message);
                         });

        QObject::connect(&mainWindow, &desktop::MainWindow::placeOrderRequested,
                         &orderController, &desktop::OrderController::placeMarketOrder);
        QObject::connect(&orderController, &desktop::OrderController::orderAccepted,
                         &mainWindow, [&](const desktop::OrderUiDto& order) {
                             mainWindow.showStatus(
                                 QStringLiteral("%1 %2 qty=%3 @ %4")
                                     .arg(order.side, order.symbol)
                                     .arg(order.quantity)
                                     .arg(order.fillPrice, 0, 'f', 2));
                         });
        QObject::connect(&orderController, &desktop::OrderController::orderRejected,
                         &mainWindow, [&](const QString& reason) {
                             mainWindow.showStatus(reason);
                             QMessageBox::warning(&mainWindow, QStringLiteral("Order"), reason);
                         });
        QObject::connect(&orderController, &desktop::OrderController::portfolioUpdated,
                         &mainWindow, &desktop::MainWindow::applyPortfolio);
        QObject::connect(&orderController, &desktop::OrderController::quotesUpdated,
                         &mainWindow, &desktop::MainWindow::setQuotes);
        QObject::connect(&orderController, &desktop::OrderController::cashNeedsRefresh,
                         &walletController, &desktop::WalletController::refreshCash);

        QObject::connect(&feed, &desktop::MockMarketDataFeed::quotesUpdated, &app, [&] {
            refreshMarketUi();
        });

        QObject::connect(&mainWindow, &desktop::MainWindow::feedToggled, &app, [&](bool enable) {
            if (enable) {
                feed.start(1000);
                mainWindow.setFeedActive(true);
                mainWindow.showStatus(QStringLiteral("Market feed started"));
            } else {
                feed.stop();
                mainWindow.setFeedActive(false);
                mainWindow.showStatus(QStringLiteral("Market feed stopped"));
            }
        });

        QObject::connect(&mainWindow, &desktop::MainWindow::refreshRequested, &app, [&] {
            walletController.refreshCash();
            refreshMarketUi();
            mainWindow.showStatus(QStringLiteral("Refreshed"));
        });

        QObject::connect(&mainWindow, &desktop::MainWindow::logoutRequested, &app, [&] {
            feed.stop();
            mainWindow.setFeedActive(false);
            authController.logout();
            mainWindow.hide();
            loginWindow.clearStatus();
            loginWindow.show();
        });

        loginWindow.show();
        return app.exec();
    } catch (const std::exception& ex) {
        QMessageBox::critical(nullptr, QStringLiteral("Fatal"),
                              QString::fromUtf8(ex.what()));
        return 1;
    }
}
