#include "bridge/trading_app_bridge.hpp"

#include <optional>
#include <vector>

namespace desktop {

TradingAppBridge::TradingAppBridge(const std::string& dbPath, QObject* parent)
    : QObject(parent)
    , bootstrap_(std::make_unique<AppBootstrap>(AppBootstrap::open(dbPath)))
    , auth_(bootstrap_->auth(), bootstrap_->accounts(), this)
    , wallet_(bootstrap_->wallet(), auth_, this)
    , orders_(bootstrap_->orders(), auth_, this)
    , feed_(this)
    , quoteModel_(this)
    , positionModel_(this)
    , dbPath_(QString::fromStdString(bootstrap_->dbPath())) {
    // Feed reads/writes last prices through the quote repository (same DB as orders).
    feed_.setPriceIO(
        [this](const std::string& symbol) -> std::optional<double> {
            auto quote = bootstrap_->quotes().find(symbol);
            if (!quote) {
                return std::nullopt;
            }
            return quote->lastPrice;
        },
        [this](const std::string& symbol, double price) {
            bootstrap_->quotes().setLastPrice(symbol, price);
        });

    wireControllers();
    setStatus(QStringLiteral("Ready"));
}

void TradingAppBridge::wireControllers() {
    // Auth → drive StackView Login/Shell via loggedInChanged in Main.qml.
    connect(&auth_, &AuthController::loginSucceeded, this, &TradingAppBridge::onLoginSucceeded);
    connect(&auth_, &AuthController::authFailed, this, [this](const QString& message) {
        setAuthError(message);
    });
    connect(&auth_, &AuthController::loggedOut, this, [this] {
        loggedIn_ = false;
        username_.clear();
        cash_ = 0.0;
        equity_ = 0.0;
        unrealizedPnl_ = 0.0;
        positions_.clear();
        positionModel_.setPositions({});
        emit loggedInChanged();
        emit usernameChanged();
        emit cashChanged();
        emit equityChanged();
        emit unrealizedPnlChanged();
        setStatus(QStringLiteral("Logged out"));
    });

    connect(&wallet_, &WalletController::cashUpdated, this, [this](double cash) {
        cash_ = cash;
        rebuildOverview();
        emit cashChanged();
    });
    connect(&wallet_, &WalletController::walletSucceeded, this, [this](const QString& message) {
        setStatus(message);
    });
    connect(&wallet_, &WalletController::walletFailed, this, [this](const QString& message) {
        setStatus(message);
    });

    connect(&orders_, &OrderController::orderAccepted, this, [this](const OrderUiDto& order) {
        setStatus(QStringLiteral("%1 %2 qty=%3 @ %4")
                      .arg(order.side, order.symbol)
                      .arg(order.quantity)
                      .arg(order.fillPrice, 0, 'f', 2));
    });
    connect(&orders_, &OrderController::orderRejected, this, [this](const QString& reason) {
        setStatus(reason);
    });
    connect(&orders_, &OrderController::portfolioUpdated, this,
            [this](const QVector<PositionUiDto>& rows) {
                positions_ = rows;
                positionModel_.setPositions(rows);
                rebuildOverview();
            });
    connect(&orders_, &OrderController::quotesUpdated, this,
            [this](const QVector<QuoteUiDto>& quotes) {
                quoteModel_.setQuotes(quotes);
                QStringList next;
                next.reserve(quotes.size());
                for (const auto& quote : quotes) {
                    next << quote.symbol;
                }
                if (next != symbols_) {
                    symbols_ = next;
                    emit symbolsChanged();
                }
            });
    connect(&orders_, &OrderController::cashNeedsRefresh, &wallet_, &WalletController::refreshCash);

    // Each timer tick updates DB prices then refreshes quote/portfolio models for QML.
    connect(&feed_, &MockMarketDataFeed::quotesUpdated, this, [this] {
        refreshMarketUi();
        emit metricsFlashed();
    });
}

void TradingAppBridge::login(const QString& username, const QString& password) {
    clearAuthError();
    auth_.login(username, password);
}

void TradingAppBridge::registerUser(const QString& username, const QString& password) {
    clearAuthError();
    auth_.registerUser(username, password);
}

void TradingAppBridge::logout() {
    setFeedActive(false);
    auth_.logout();
}

void TradingAppBridge::deposit(double amount) {
    if (amount <= 0.0) {
        setStatus(QStringLiteral("Deposit amount must be > 0"));
        return;
    }
    wallet_.deposit(amount);
}

void TradingAppBridge::placeMarketOrder(const QString& symbol, const QString& side, int qty) {
    if (symbol.trimmed().isEmpty()) {
        setStatus(QStringLiteral("Select a symbol"));
        return;
    }
    if (qty <= 0) {
        setStatus(QStringLiteral("Quantity must be > 0"));
        return;
    }
    orders_.placeMarketOrder(symbol, side, qty);
}

void TradingAppBridge::refresh() {
    wallet_.refreshCash();
    refreshMarketUi();
    setStatus(QStringLiteral("Refreshed"));
}

void TradingAppBridge::setFeedActive(bool active) {
    if (active == feedActive_) {
        return;
    }
    if (active) {
        loadFeedSymbols();
        feed_.start(1000);
        feedActive_ = true;
        setStatus(QStringLiteral("Market feed started"));
    } else {
        feed_.stop();
        feedActive_ = false;
        setStatus(QStringLiteral("Market feed stopped"));
    }
    emit feedActiveChanged();
}

void TradingAppBridge::clearAuthError() {
    if (authError_.isEmpty()) {
        return;
    }
    authError_.clear();
    emit authErrorChanged();
}

void TradingAppBridge::onLoginSucceeded(const SessionDto& session) {
    loggedIn_ = true;
    username_ = session.username;
    cash_ = session.cashBalance;
    emit loggedInChanged();
    emit usernameChanged();
    emit cashChanged();

    loadFeedSymbols();
    refreshMarketUi();
    wallet_.refreshCash();
    setFeedActive(true);
    setStatus(QStringLiteral("Logged in — live mock feed"));
    clearAuthError();
}

void TradingAppBridge::refreshMarketUi() {
    orders_.refreshQuotes();
    orders_.refreshPortfolio();
}

void TradingAppBridge::loadFeedSymbols() {
    auto quotes = bootstrap_->quotes().listAll();
    std::vector<std::string> symbols;
    symbols.reserve(quotes.size());
    for (const auto& quote : quotes) {
        symbols.push_back(quote.symbol);
    }
    feed_.setSymbols(std::move(symbols));
}

void TradingAppBridge::rebuildOverview() {
    double marketValue = 0.0;
    double unrealized = 0.0;
    for (const auto& row : positions_) {
        marketValue += row.marketValue;
        unrealized += row.unrealizedPnl;
    }
    const double nextEquity = cash_ + marketValue;
    const bool equityDirty = nextEquity != equity_;
    const bool pnlDirty = unrealized != unrealizedPnl_;
    equity_ = nextEquity;
    unrealizedPnl_ = unrealized;
    if (equityDirty) {
        emit equityChanged();
    }
    if (pnlDirty) {
        emit unrealizedPnlChanged();
    }
}

void TradingAppBridge::setStatus(const QString& message) {
    if (statusMessage_ == message) {
        return;
    }
    statusMessage_ = message;
    emit statusMessageChanged();
}

void TradingAppBridge::setAuthError(const QString& message) {
    authError_ = message;
    emit authErrorChanged();
}

} // namespace desktop
