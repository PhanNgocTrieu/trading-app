#include "trading.h"

#include <string>

namespace Service {

TradingService::TradingService(LoggerService& logger,
                               OrderAppService& orders,
                               LoginService& login)
    : logger_(logger)
    , orders_(orders)
    , login_(login) {}

Result<OrderDto> TradingService::buyMarket(const std::string& symbol, int qty) {
    if (!login_.isLoggedIn()) {
        logger_.logError("Buy failed: no active session");
        return Result<OrderDto>::fail(ErrorCode::Unauthorized, "not logged in");
    }

    auto result = orders_.placeMarketOrder(login_.authSession(), symbol, OrderSide::Buy, qty);
    if (!result.ok()) {
        logger_.logWarning("Buy rejected: " + result.message());
        return result;
    }
    login_.syncSessionCash();
    logger_.logInfo("Buy filled " + symbol + " qty=" + std::to_string(qty) +
                    " px=" + std::to_string(result.value().fillPrice));
    return result;
}

Result<OrderDto> TradingService::sellMarket(const std::string& symbol, int qty) {
    if (!login_.isLoggedIn()) {
        logger_.logError("Sell failed: no active session");
        return Result<OrderDto>::fail(ErrorCode::Unauthorized, "not logged in");
    }

    auto result = orders_.placeMarketOrder(login_.authSession(), symbol, OrderSide::Sell, qty);
    if (!result.ok()) {
        logger_.logWarning("Sell rejected: " + result.message());
        return result;
    }
    login_.syncSessionCash();
    logger_.logInfo("Sell filled " + symbol + " qty=" + std::to_string(qty) +
                    " px=" + std::to_string(result.value().fillPrice));
    return result;
}

Result<std::vector<QuoteRow>> TradingService::listQuotes() const {
    return orders_.listQuotes();
}

Result<std::vector<PositionDto>> TradingService::portfolio() const {
    if (!login_.isLoggedIn()) {
        return Result<std::vector<PositionDto>>::fail(ErrorCode::Unauthorized, "not logged in");
    }
    return orders_.portfolio(login_.authSession());
}

Result<std::vector<TradeRow>> TradingService::recentTrades(int limit) const {
    if (!login_.isLoggedIn()) {
        return Result<std::vector<TradeRow>>::fail(ErrorCode::Unauthorized, "not logged in");
    }
    return orders_.recentTrades(login_.authSession(), limit);
}

} // namespace Service
