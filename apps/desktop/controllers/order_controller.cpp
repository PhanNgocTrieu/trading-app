#include "controllers/order_controller.hpp"

#include "domain/order_types.hpp"

#include <vector>

namespace desktop {
namespace {

OrderUiDto toUi(const OrderDto& order) {
    OrderUiDto dto;
    dto.orderId = order.orderId;
    dto.symbol = toQ(order.symbol);
    dto.side = toQ(toString(order.side));
    dto.type = toQ(toString(order.type));
    dto.quantity = order.quantity;
    dto.fillPrice = order.fillPrice;
    dto.status = toQ(toString(order.status));
    if (order.limitPrice) {
        dto.hasLimitPrice = true;
        dto.limitPrice = *order.limitPrice;
    }
    if (order.realizedPnl) {
        dto.hasRealizedPnl = true;
        dto.realizedPnl = *order.realizedPnl;
    }
    return dto;
}

OrderUiDto toUi(const OrderRow& order) {
    OrderUiDto dto;
    dto.orderId = order.id;
    dto.symbol = toQ(order.symbol);
    dto.side = toQ(toString(order.side));
    dto.type = toQ(toString(order.type));
    dto.quantity = order.quantity;
    dto.status = toQ(toString(order.status));
    if (order.limitPrice) {
        dto.hasLimitPrice = true;
        dto.limitPrice = *order.limitPrice;
    }
    return dto;
}

QVector<BookLevelUiDto> toBookUi(const std::vector<BookLevelDto>& levels) {
    QVector<BookLevelUiDto> rows;
    rows.reserve(static_cast<int>(levels.size()));
    for (const auto& level : levels) {
        BookLevelUiDto dto;
        dto.side = toQ(toString(level.side));
        dto.price = level.price;
        dto.quantity = level.quantity;
        dto.orderCount = level.orderCount;
        rows.push_back(dto);
    }
    return rows;
}

} // namespace

OrderController::OrderController(OrderAppService& orders,
                                 AuthController& auth,
                                 QObject* parent)
    : QObject(parent)
    , orders_(orders)
    , auth_(auth) {}

void OrderController::placeMarketOrder(const QString& symbol, const QString& side, int qty) {
    if (!auth_.isLoggedIn()) {
        emit orderRejected(QStringLiteral("not logged in"));
        return;
    }
    if (symbol.trimmed().isEmpty()) {
        emit orderRejected(QStringLiteral("symbol is required"));
        return;
    }
    if (qty <= 0) {
        emit orderRejected(QStringLiteral("quantity must be > 0"));
        return;
    }

    const OrderSide orderSide = sideFromString(toStd(side));
    auto result =
        orders_.placeMarketOrder(auth_.authSession(), toStd(symbol), orderSide, qty);
    if (!result.ok()) {
        emit orderRejected(toQ(result.message()));
        refreshWorkingOrders();
        return;
    }

    emit orderAccepted(toUi(result.value()));
    emit cashNeedsRefresh();
    refreshPortfolio();
    refreshWorkingOrders();
    refreshOrderBook(symbol);
}

void OrderController::placeLimitOrder(const QString& symbol,
                                      const QString& side,
                                      int qty,
                                      double limitPrice) {
    if (!auth_.isLoggedIn()) {
        emit orderRejected(QStringLiteral("not logged in"));
        return;
    }
    if (symbol.trimmed().isEmpty()) {
        emit orderRejected(QStringLiteral("symbol is required"));
        return;
    }
    if (qty <= 0) {
        emit orderRejected(QStringLiteral("quantity must be > 0"));
        return;
    }
    if (limitPrice <= 0.0) {
        emit orderRejected(QStringLiteral("limit price must be > 0"));
        return;
    }

    const OrderSide orderSide = sideFromString(toStd(side));
    auto result = orders_.placeLimitOrder(auth_.authSession(), toStd(symbol), orderSide, qty,
                                          limitPrice);
    if (!result.ok()) {
        emit orderRejected(toQ(result.message()));
        refreshWorkingOrders();
        return;
    }

    emit orderAccepted(toUi(result.value()));
    emit cashNeedsRefresh();
    refreshPortfolio();
    refreshWorkingOrders();
    refreshOrderBook(symbol);
}

void OrderController::cancelOrder(int orderId) {
    if (!auth_.isLoggedIn()) {
        emit orderRejected(QStringLiteral("not logged in"));
        return;
    }
    auto result = orders_.cancelOrder(auth_.authSession(), orderId);
    if (!result.ok()) {
        emit orderRejected(toQ(result.message()));
        return;
    }
    emit orderCanceled(orderId);
    refreshWorkingOrders();
    refreshPortfolio();
}

void OrderController::refreshPortfolio() {
    if (!auth_.isLoggedIn()) {
        emit portfolioUpdated({});
        return;
    }

    auto result = orders_.portfolio(auth_.authSession());
    if (!result.ok()) {
        emit orderRejected(toQ(result.message()));
        return;
    }

    QVector<PositionUiDto> rows;
    rows.reserve(static_cast<int>(result.value().size()));
    for (const auto& row : result.value()) {
        PositionUiDto dto;
        dto.symbol = toQ(row.symbol);
        dto.quantity = row.quantity;
        dto.avgCost = row.avgCost;
        dto.marketPrice = row.marketPrice;
        dto.marketValue = row.marketValue;
        dto.unrealizedPnl = row.unrealizedPnl;
        rows.push_back(dto);
    }
    emit portfolioUpdated(rows);
}

void OrderController::refreshQuotes() {
    auto result = orders_.listQuotes();
    if (!result.ok()) {
        emit orderRejected(toQ(result.message()));
        return;
    }
    QVector<QuoteUiDto> quotes;
    quotes.reserve(static_cast<int>(result.value().size()));
    for (const auto& quote : result.value()) {
        QuoteUiDto dto;
        dto.symbol = toQ(quote.symbol);
        dto.name = toQ(quote.name);
        dto.lastPrice = quote.lastPrice;
        quotes.push_back(dto);
    }
    emit quotesUpdated(quotes);
}

void OrderController::refreshWorkingOrders() {
    if (!auth_.isLoggedIn()) {
        emit workingOrdersUpdated({});
        return;
    }
    auto result = orders_.workingOrders(auth_.authSession());
    if (!result.ok()) {
        emit orderRejected(toQ(result.message()));
        return;
    }
    QVector<OrderUiDto> rows;
    rows.reserve(static_cast<int>(result.value().size()));
    for (const auto& order : result.value()) {
        rows.push_back(toUi(order));
    }
    emit workingOrdersUpdated(rows);
}

void OrderController::refreshOrderBook(const QString& symbol) {
    if (symbol.trimmed().isEmpty()) {
        emit orderBookUpdated({}, {});
        return;
    }
    auto result = orders_.orderBook(toStd(symbol));
    if (!result.ok()) {
        emit orderRejected(toQ(result.message()));
        return;
    }
    emit orderBookUpdated(toBookUi(result.value().bids), toBookUi(result.value().asks));
}

} // namespace desktop
