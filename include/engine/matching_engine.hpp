#pragma once

#include "domain/order_types.hpp"

#include <string>

struct MarketSnapshot {
    std::string symbol;
    double lastPrice{0.0};
};

struct IncomingOrder {
    std::string symbol;
    OrderSide side{OrderSide::Buy};
    OrderType type{OrderType::Market};
    int quantity{0};
    double limitPrice{0.0};
};

struct MatchResult {
    bool filled{false};
    double fillPrice{0.0};
    int fillQty{0};
    std::string rejectReason;
};

// Phase 2 paper matcher: market fills at last price; simple limit rules included.
class MatchingEngine {
public:
    MatchResult match(const IncomingOrder& order, const MarketSnapshot& quote) const {
        MatchResult result;

        if (order.quantity <= 0) {
            result.rejectReason = "quantity must be > 0";
            return result;
        }
        if (order.symbol != quote.symbol) {
            result.rejectReason = "symbol mismatch";
            return result;
        }
        if (quote.lastPrice <= 0.0) {
            result.rejectReason = "invalid market price";
            return result;
        }

        if (order.type == OrderType::Market) {
            result.filled = true;
            result.fillPrice = quote.lastPrice;
            result.fillQty = order.quantity;
            return result;
        }

        const bool buyOk =
            order.side == OrderSide::Buy && quote.lastPrice <= order.limitPrice;
        const bool sellOk =
            order.side == OrderSide::Sell && quote.lastPrice >= order.limitPrice;
        if (buyOk || sellOk) {
            result.filled = true;
            result.fillPrice = quote.lastPrice;
            result.fillQty = order.quantity;
        } else {
            result.rejectReason = "limit not marketable";
        }
        return result;
    }
};
