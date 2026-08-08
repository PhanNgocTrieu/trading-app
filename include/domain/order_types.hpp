#pragma once

#include <string>

enum class OrderSide { Buy, Sell };
enum class OrderType { Market, Limit };
enum class OrderStatus { Pending, Filled, Rejected, Canceled };

inline const char* toString(OrderSide side) {
    return side == OrderSide::Buy ? "BUY" : "SELL";
}

inline const char* toString(OrderType type) {
    return type == OrderType::Market ? "MARKET" : "LIMIT";
}

inline const char* toString(OrderStatus status) {
    switch (status) {
        case OrderStatus::Pending:  return "PENDING";
        case OrderStatus::Filled:   return "FILLED";
        case OrderStatus::Rejected: return "REJECTED";
        case OrderStatus::Canceled: return "CANCELED";
    }
    return "UNKNOWN";
}

inline OrderSide sideFromString(const std::string& value) {
    return (value == "SELL" || value == "sell") ? OrderSide::Sell : OrderSide::Buy;
}
