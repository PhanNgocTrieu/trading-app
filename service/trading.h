#pragma once

#include "application/order_app_service.hpp"
#include "domain/order_types.hpp"
#include "domain/result.hpp"
#include "logger.hpp"
#include "login.h"

#include <string>
#include <vector>

namespace Service {

class TradingService {
public:
    TradingService(LoggerService& logger,
                   OrderAppService& orders,
                   LoginService& login);

    [[nodiscard]] Result<OrderDto> buyMarket(const std::string& symbol, int qty);
    [[nodiscard]] Result<OrderDto> sellMarket(const std::string& symbol, int qty);
    [[nodiscard]] Result<std::vector<QuoteRow>> listQuotes() const;
    [[nodiscard]] Result<std::vector<PositionDto>> portfolio() const;
    [[nodiscard]] Result<std::vector<TradeRow>> recentTrades(int limit = 10) const;

private:
    LoggerService& logger_;
    OrderAppService& orders_;
    LoginService& login_;
};

} // namespace Service
