#pragma once

#include "application/ports.hpp"
#include "domain/order_types.hpp"
#include "domain/position.hpp"
#include "domain/result.hpp"
#include "engine/matching_engine.hpp"
#include "infrastructure/db/sqlite_connection.hpp"

#include <optional>
#include <string>
#include <utility>
#include <vector>

struct OrderDto {
    int orderId{0};
    std::string symbol;
    OrderSide side{OrderSide::Buy};
    OrderType type{OrderType::Market};
    int quantity{0};
    double fillPrice{0.0};
    std::optional<double> limitPrice;
    OrderStatus status{OrderStatus::Rejected};
    std::optional<double> realizedPnl;
};

struct BookLevelDto {
    OrderSide side{OrderSide::Buy};
    double price{0.0};
    int quantity{0};
    int orderCount{0};
};

struct OrderBookDto {
    std::string symbol;
    std::vector<BookLevelDto> bids;
    std::vector<BookLevelDto> asks;
};

struct PositionDto {
    std::string symbol;
    int quantity{0};
    double avgCost{0.0};
    double marketPrice{0.0};
    double marketValue{0.0};
    double unrealizedPnl{0.0};
};

class OrderAppService {
public:
    OrderAppService(SqliteConnection& db,
                    IAccountRepository& accounts,
                    ILedgerRepository& ledger,
                    IQuoteRepository& quotes,
                    IPositionRepository& positions,
                    IOrderRepository& orders,
                    ITradeRepository& trades);

    [[nodiscard]] Result<OrderDto> placeMarketOrder(const AuthSession& session,
                                                    const std::string& symbol,
                                                    OrderSide side,
                                                    int qty);

    [[nodiscard]] Result<OrderDto> placeLimitOrder(const AuthSession& session,
                                                   const std::string& symbol,
                                                   OrderSide side,
                                                   int qty,
                                                   double limitPrice);

    [[nodiscard]] Result<OkTag> cancelOrder(const AuthSession& session, int orderId);

    [[nodiscard]] Result<std::vector<OrderRow>> workingOrders(const AuthSession& session) const;
    [[nodiscard]] Result<OrderBookDto> orderBook(const std::string& symbol) const;

    [[nodiscard]] Result<std::vector<QuoteRow>> listQuotes() const;
    [[nodiscard]] Result<std::vector<PositionDto>> portfolio(const AuthSession& session) const;
    [[nodiscard]] Result<std::vector<TradeRow>> recentTrades(const AuthSession& session,
                                                             int limit = 10) const;

    // Updates last price then fills any newly marketable resting limits (same transaction).
    [[nodiscard]] Result<OkTag> setQuotePrice(const std::string& symbol, double lastPrice);

private:
    Position loadPosition(int accountId, const std::string& symbol) const;
    double reservedBuyNotional(int accountId, int excludeOrderId) const;
    int reservedSellQty(int accountId, const std::string& symbol, int excludeOrderId) const;
    std::pair<Result<OrderDto>, bool> placeOrderInTx(const AuthSession& session,
                                                     const std::string& symbol,
                                                     OrderSide side,
                                                     OrderType type,
                                                     int qty,
                                                     std::optional<double> limitPrice);
    void tryFillPendingInTx(const std::string& symbol);

    SqliteConnection& db_;
    IAccountRepository& accounts_;
    ILedgerRepository& ledger_;
    IQuoteRepository& quotes_;
    IPositionRepository& positions_;
    IOrderRepository& orders_;
    ITradeRepository& trades_;
    MatchingEngine engine_{};
};
