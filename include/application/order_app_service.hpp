#pragma once

#include "application/ports.hpp"
#include "domain/order_types.hpp"
#include "domain/position.hpp"
#include "domain/result.hpp"
#include "engine/matching_engine.hpp"
#include "infrastructure/db/sqlite_connection.hpp"

#include <optional>
#include <string>
#include <vector>

struct OrderDto {
    int orderId{0};
    std::string symbol;
    OrderSide side{OrderSide::Buy};
    int quantity{0};
    double fillPrice{0.0};
    OrderStatus status{OrderStatus::Rejected};
    std::optional<double> realizedPnl;
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

    [[nodiscard]] Result<std::vector<QuoteRow>> listQuotes() const;
    [[nodiscard]] Result<std::vector<PositionDto>> portfolio(const AuthSession& session) const;
    [[nodiscard]] Result<std::vector<TradeRow>> recentTrades(const AuthSession& session,
                                                             int limit = 10) const;

    // Test / admin helper: update last price used by market matching.
    [[nodiscard]] Result<OkTag> setQuotePrice(const std::string& symbol, double lastPrice);

private:
    Position loadPosition(int accountId, const std::string& symbol) const;

    SqliteConnection& db_;
    IAccountRepository& accounts_;
    ILedgerRepository& ledger_;
    IQuoteRepository& quotes_;
    IPositionRepository& positions_;
    IOrderRepository& orders_;
    ITradeRepository& trades_;
    MatchingEngine engine_{};
};
