#include "controllers/order_controller.hpp"

#include "domain/order_types.hpp"

namespace desktop {

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
        return;
    }

    OrderUiDto dto;
    dto.orderId = result.value().orderId;
    dto.symbol = toQ(result.value().symbol);
    dto.side = toQ(toString(result.value().side));
    dto.quantity = result.value().quantity;
    dto.fillPrice = result.value().fillPrice;
    dto.status = toQ(toString(result.value().status));
    if (result.value().realizedPnl) {
        dto.hasRealizedPnl = true;
        dto.realizedPnl = *result.value().realizedPnl;
    }

    emit orderAccepted(dto);
    emit cashNeedsRefresh();
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

} // namespace desktop
