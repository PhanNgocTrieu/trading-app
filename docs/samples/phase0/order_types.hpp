#pragma once
// docs/samples/phase0/order_types.hpp

#include <string>

enum class OrderSide { Buy, Sell };
enum class OrderType { Market, Limit };
enum class OrderStatus { Pending, Filled, Rejected, Canceled };

inline const char* toString(OrderSide s) {
    return s == OrderSide::Buy ? "BUY" : "SELL";
}

inline const char* toString(OrderType t) {
    return t == OrderType::Market ? "MARKET" : "LIMIT";
}

inline const char* toString(OrderStatus st) {
    switch (st) {
        case OrderStatus::Pending:  return "PENDING";
        case OrderStatus::Filled:   return "FILLED";
        case OrderStatus::Rejected: return "REJECTED";
        case OrderStatus::Canceled: return "CANCELED";
    }
    return "UNKNOWN";
}

inline OrderSide sideFromString(const std::string& s) {
    return (s == "SELL" || s == "sell") ? OrderSide::Sell : OrderSide::Buy;
}
