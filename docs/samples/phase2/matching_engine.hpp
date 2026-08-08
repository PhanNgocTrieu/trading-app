#pragma once
// docs/samples/phase2/matching_engine.hpp
// MVP: market order khớp 100% tại last price.

#include "../phase0/order_types.hpp"
#include <string>

struct MarketSnapshot {
    std::string symbol;
    double lastPrice{0.0};
};

struct IncomingOrder {
    std::string symbol;
    OrderSide side;
    OrderType type;
    int quantity{0};
    double limitPrice{0.0}; // dùng khi Limit
};

struct MatchResult {
    bool filled{false};
    double fillPrice{0.0};
    int fillQty{0};
    std::string rejectReason;
};

class MatchingEngine {
public:
    MatchResult match(const IncomingOrder& order, const MarketSnapshot& quote) const {
        MatchResult r;

        if (order.quantity <= 0) {
            r.rejectReason = "quantity must be > 0";
            return r;
        }
        if (order.symbol != quote.symbol) {
            r.rejectReason = "symbol mismatch";
            return r;
        }
        if (quote.lastPrice <= 0.0) {
            r.rejectReason = "invalid market price";
            return r;
        }

        if (order.type == OrderType::Market) {
            r.filled = true;
            r.fillPrice = quote.lastPrice;
            r.fillQty = order.quantity;
            return r;
        }

        // Limit (Phase 4+): điều kiện khớp đơn giản
        const bool buyOk = order.side == OrderSide::Buy && quote.lastPrice <= order.limitPrice;
        const bool sellOk = order.side == OrderSide::Sell && quote.lastPrice >= order.limitPrice;
        if (buyOk || sellOk) {
            r.filled = true;
            r.fillPrice = quote.lastPrice;
            r.fillQty = order.quantity;
        } else {
            r.rejectReason = "limit not marketable";
        }
        return r;
    }
};
