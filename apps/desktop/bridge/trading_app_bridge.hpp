#pragma once

// TradingAppBridge — QML façade (Phase 5–6)
//
// QML must not talk to SQLite or domain services directly. This QObject:
//   1. Owns AppBootstrap + Auth/Wallet/Order controllers + mock price feed
//   2. Exposes Q_PROPERTY / Q_INVOKABLE for binding and button handlers in QML
//   3. Forwards controller signals into property change notifications
//
// Context property name in QML: `app`  (see apps/desktop/main.cpp)

#include "application/app_bootstrap.hpp"
#include "controllers/auth_controller.hpp"
#include "controllers/order_controller.hpp"
#include "controllers/wallet_controller.hpp"
#include "dto.hpp"
#include "market/mock_market_data_feed.hpp"
#include "models/order_book_table_model.hpp"
#include "models/position_table_model.hpp"
#include "models/quote_table_model.hpp"
#include "models/working_order_table_model.hpp"

#include <QAbstractTableModel>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>

#include <memory>

namespace desktop {

class TradingAppBridge : public QObject {
    Q_OBJECT
    // Session / overview metrics bound from ShellPage header chips.
    Q_PROPERTY(bool loggedIn READ loggedIn NOTIFY loggedInChanged)
    Q_PROPERTY(QString username READ username NOTIFY usernameChanged)
    Q_PROPERTY(double cash READ cash NOTIFY cashChanged)
    Q_PROPERTY(double equity READ equity NOTIFY equityChanged)
    Q_PROPERTY(double unrealizedPnl READ unrealizedPnl NOTIFY unrealizedPnlChanged)
    Q_PROPERTY(bool feedActive READ feedActive NOTIFY feedActiveChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(QString authError READ authError NOTIFY authErrorChanged)
    Q_PROPERTY(QString dbPath READ dbPath CONSTANT)
    // List models for Market Watch / Portfolio panels (roleNames set for QML).
    Q_PROPERTY(QAbstractTableModel* quoteModel READ quoteModel CONSTANT)
    Q_PROPERTY(QAbstractTableModel* positionModel READ positionModel CONSTANT)
    Q_PROPERTY(QAbstractTableModel* bidModel READ bidModel CONSTANT)
    Q_PROPERTY(QAbstractTableModel* askModel READ askModel CONSTANT)
    Q_PROPERTY(QAbstractTableModel* workingOrderModel READ workingOrderModel CONSTANT)
    // Symbol list for the order-ticket ComboBox.
    Q_PROPERTY(QStringList symbols READ symbols NOTIFY symbolsChanged)
    Q_PROPERTY(QString bookSymbol READ bookSymbol WRITE setBookSymbol NOTIFY bookSymbolChanged)

public:
    explicit TradingAppBridge(const std::string& dbPath, QObject* parent = nullptr);

    bool loggedIn() const { return loggedIn_; }
    QString username() const { return username_; }
    double cash() const { return cash_; }
    double equity() const { return equity_; }
    double unrealizedPnl() const { return unrealizedPnl_; }
    bool feedActive() const { return feedActive_; }
    QString statusMessage() const { return statusMessage_; }
    QString authError() const { return authError_; }
    QString dbPath() const { return dbPath_; }
    QAbstractTableModel* quoteModel() { return &quoteModel_; }
    QAbstractTableModel* positionModel() { return &positionModel_; }
    QAbstractTableModel* bidModel() { return &bidModel_; }
    QAbstractTableModel* askModel() { return &askModel_; }
    QAbstractTableModel* workingOrderModel() { return &workingOrderModel_; }
    QStringList symbols() const { return symbols_; }
    QString bookSymbol() const { return bookSymbol_; }

    // Invoked from LoginPage / OrderTicket / ShellPage buttons.
    Q_INVOKABLE void login(const QString& username, const QString& password);
    Q_INVOKABLE void registerUser(const QString& username, const QString& password);
    Q_INVOKABLE void logout();
    Q_INVOKABLE void deposit(double amount);
    Q_INVOKABLE void withdraw(double amount);
    Q_INVOKABLE void placeMarketOrder(const QString& symbol, const QString& side, int qty);
    Q_INVOKABLE void placeLimitOrder(const QString& symbol, const QString& side, int qty,
                                     double limitPrice);
    Q_INVOKABLE void cancelOrder(int orderId);
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void setFeedActive(bool active);
    Q_INVOKABLE void setBookSymbol(const QString& symbol);
    Q_INVOKABLE void clearAuthError();

signals:
    void loggedInChanged();
    void usernameChanged();
    void cashChanged();
    void equityChanged();
    void unrealizedPnlChanged();
    void feedActiveChanged();
    void statusMessageChanged();
    void authErrorChanged();
    void symbolsChanged();
    void bookSymbolChanged();
    // Fired on each mock-feed tick so ShellPage can flash the uPnL chip.
    void metricsFlashed();

private:
    void setStatus(const QString& message);
    void setAuthError(const QString& message);
    // equity = cash + sum(position market value); uPnL = sum(unrealized).
    void rebuildOverview();
    void wireControllers();
    void onLoginSucceeded(const SessionDto& session);
    void refreshMarketUi();
    void loadFeedSymbols();

    std::unique_ptr<AppBootstrap> bootstrap_;
    AuthController auth_;
    WalletController wallet_;
    OrderController orders_;
    MockMarketDataFeed feed_;
    QuoteTableModel quoteModel_;
    PositionTableModel positionModel_;
    OrderBookTableModel bidModel_;
    OrderBookTableModel askModel_;
    WorkingOrderTableModel workingOrderModel_;

    QString dbPath_;
    bool loggedIn_{false};
    QString username_;
    double cash_{0.0};
    double equity_{0.0};
    double unrealizedPnl_{0.0};
    bool feedActive_{false};
    QString statusMessage_;
    QString authError_;
    QStringList symbols_;
    QString bookSymbol_;
    QVector<PositionUiDto> positions_;
};

} // namespace desktop
