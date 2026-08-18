#pragma once

#include "application/order_app_service.hpp"
#include "controllers/auth_controller.hpp"
#include "dto.hpp"

#include <QObject>
#include <QVector>

namespace desktop {

class OrderController : public QObject {
    Q_OBJECT
public:
    OrderController(OrderAppService& orders,
                    AuthController& auth,
                    QObject* parent = nullptr);

    OrderAppService& orders() { return orders_; }

public slots:
    void placeMarketOrder(const QString& symbol, const QString& side, int qty);
    void placeLimitOrder(const QString& symbol, const QString& side, int qty, double limitPrice);
    void cancelOrder(int orderId);
    void refreshPortfolio();
    void refreshQuotes();
    void refreshWorkingOrders();
    void refreshOrderBook(const QString& symbol);

signals:
    void orderAccepted(const desktop::OrderUiDto& order);
    void orderRejected(const QString& reason);
    void orderCanceled(int orderId);
    void portfolioUpdated(const QVector<desktop::PositionUiDto>& rows);
    void quotesUpdated(const QVector<desktop::QuoteUiDto>& quotes);
    void workingOrdersUpdated(const QVector<desktop::OrderUiDto>& rows);
    void orderBookUpdated(const QVector<desktop::BookLevelUiDto>& bids,
                          const QVector<desktop::BookLevelUiDto>& asks);
    void cashNeedsRefresh();

private:
    OrderAppService& orders_;
    AuthController& auth_;
};

} // namespace desktop
