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
    , bidModel_(this)
    , askModel_(this)
    , workingOrderModel_(this)
    , dbPath_(QString::fromStdString(bootstrap_->dbPath())) {
    // Feed writes last prices through OrderAppService so resting limits can fill on the tick.
    feed_.setPriceIO(
        [this](const std::string& symbol) -> std::optional<double> {
            auto quote = bootstrap_->quotes().find(symbol);
            if (!quote) {
                return std::nullopt;
            }
            return quote->lastPrice;
        },
        [this](const std::string& symbol, double price) {
            (void)bootstrap_->orders().setQuotePrice(symbol, price);
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
        workingOrderModel_.setOrders({});
        bidModel_.setLevels({});
        askModel_.setLevels({});
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
        if (order.status == QLatin1String("PENDING")) {
            const QString limit = order.hasLimitPrice
                                      ? QString::number(order.limitPrice, 'f', 2)
                                      : QStringLiteral("-");
            setStatus(QStringLiteral("%1 %2 qty=%3 PENDING LMT %4")
                          .arg(order.side, order.symbol)
                          .arg(order.quantity)
                          .arg(limit));
        } else {
            setStatus(QStringLiteral("%1 %2 qty=%3 @ %4")
                          .arg(order.side, order.symbol)
                          .arg(order.quantity)
                          .arg(order.fillPrice, 0, 'f', 2));
        }
    });
    connect(&orders_, &OrderController::orderRejected, this, [this](const QString& reason) {
        setStatus(reason);
    });
    connect(&orders_, &OrderController::orderCanceled, this, [this](int orderId) {
        setStatus(QStringLiteral("Canceled order #%1").arg(orderId));
        orders_.refreshOrderBook(bookSymbol_);
    });
    connect(&orders_, &OrderController::workingOrdersUpdated, this,
            [this](const QVector<OrderUiDto>& rows) {
                workingOrderModel_.setOrders(rows);
            });
    connect(&orders_, &OrderController::orderBookUpdated, this,
            [this](const QVector<BookLevelUiDto>& bids, const QVector<BookLevelUiDto>& asks) {
                bidModel_.setLevels(bids);
                askModel_.setLevels(asks);
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

void TradingAppBridge::withdraw(double amount) {
    if (amount <= 0.0) {
        setStatus(QStringLiteral("Withdraw amount must be > 0"));
        return;
    }
    wallet_.withdraw(amount);
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
    setBookSymbol(symbol);
    orders_.placeMarketOrder(symbol, side, qty);
}

void TradingAppBridge::placeLimitOrder(const QString& symbol,
                                       const QString& side,
                                       int qty,
                                       double limitPrice) {
    if (symbol.trimmed().isEmpty()) {
        setStatus(QStringLiteral("Select a symbol"));
        return;
    }
    if (qty <= 0) {
        setStatus(QStringLiteral("Quantity must be > 0"));
        return;
    }
    if (limitPrice <= 0.0) {
        setStatus(QStringLiteral("Limit price must be > 0"));
        return;
    }
    setBookSymbol(symbol);
    orders_.placeLimitOrder(symbol, side, qty, limitPrice);
}

void TradingAppBridge::cancelOrder(int orderId) {
    if (orderId <= 0) {
        setStatus(QStringLiteral("Invalid order"));
        return;
    }
    orders_.cancelOrder(orderId);
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

void TradingAppBridge::setBookSymbol(const QString& symbol) {
    const QString next = symbol.trimmed().toUpper();
    if (next == bookSymbol_) {
        orders_.refreshOrderBook(bookSymbol_);
        return;
    }
    bookSymbol_ = next;
    emit bookSymbolChanged();
    orders_.refreshOrderBook(bookSymbol_);
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
    orders_.refreshWorkingOrders();
    if (bookSymbol_.isEmpty() && !symbols_.isEmpty()) {
        bookSymbol_ = symbols_.first();
        emit bookSymbolChanged();
    }
    orders_.refreshOrderBook(bookSymbol_);
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
