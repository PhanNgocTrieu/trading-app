#pragma once
// docs/samples/phase2/position.hpp

#include "../phase0/result.hpp"
#include <string>

class Position {
public:
    Position() = default;
    Position(std::string symbol, int qty, double avgCost)
        : symbol_(std::move(symbol)), qty_(qty), avgCost_(avgCost) {}

    const std::string& symbol() const { return symbol_; }
    int quantity() const { return qty_; }
    double avgCost() const { return avgCost_; }

    Result<OkTag> applyBuy(int qty, double price) {
        if (qty <= 0 || price <= 0.0) {
            return failResult(ErrorCode::ValidationFailed, "invalid buy");
        }
        const double oldNotional = static_cast<double>(qty_) * avgCost_;
        const double addNotional = static_cast<double>(qty) * price;
        qty_ += qty;
        avgCost_ = (oldNotional + addNotional) / static_cast<double>(qty_);
        return okResult();
    }

    // trả realized PnL
    Result<double> applySell(int qty, double price) {
        if (qty <= 0 || price <= 0.0) {
            return Result<double>::fail(ErrorCode::ValidationFailed, "invalid sell");
        }
        if (qty > qty_) {
            return Result<double>::fail(ErrorCode::InsufficientPosition, "not enough shares");
        }
        const double realized = (price - avgCost_) * static_cast<double>(qty);
        qty_ -= qty;
        // avgCost giữ nguyên khi bán
        return Result<double>::ok(realized);
    }

private:
    std::string symbol_;
    int qty_{0};
    double avgCost_{0.0};
};
