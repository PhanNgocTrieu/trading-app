// c++ -std=c++23 -Wall -Wextra 01_expected.cpp -o /tmp/a && /tmp/a
// Nếu fail: thử -std=c++2b hoặc compiler mới hơn (GCC 13+/Clang 16+).
#include <expected>
#include <iostream>
#include <string>

enum class OrderError { InvalidQty, InsufficientFunds, UnknownSymbol };

std::expected<int, OrderError> placeBuy(std::string_view symbol, int qty, double cash, double price) {
    if (symbol.empty()) return std::unexpected{OrderError::UnknownSymbol};
    if (qty <= 0) return std::unexpected{OrderError::InvalidQty};
    if (cash < static_cast<double>(qty) * price) {
        return std::unexpected{OrderError::InsufficientFunds};
    }
    return 42;
}

const char* toString(OrderError e) {
    switch (e) {
        case OrderError::InvalidQty: return "InvalidQty";
        case OrderError::InsufficientFunds: return "InsufficientFunds";
        case OrderError::UnknownSymbol: return "UnknownSymbol";
    }
    return "Unknown";
}

int main() {
    auto ok = placeBuy("AAPL", 10, 5000, 190);
    if (ok) std::cout << "orderId=" << *ok << "\n";

    auto bad = placeBuy("AAPL", 10, 100, 190);
    if (!bad) std::cout << "error=" << toString(bad.error()) << "\n";
    return 0;
}
