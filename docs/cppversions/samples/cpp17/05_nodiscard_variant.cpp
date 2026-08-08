// c++ -std=c++17 -Wall -Wextra 05_nodiscard_variant.cpp -o /tmp/a && /tmp/a
#include <iostream>
#include <string>
#include <variant>

enum class Err { InvalidQty, InsufficientFunds };

[[nodiscard]] std::variant<int, Err> placeBuy(int qty, double cash, double price) {
    if (qty <= 0) return Err::InvalidQty;
    if (cash < qty * price) return Err::InsufficientFunds;
    return 7; // order id
}

int main() {
    // Bỏ kết quả có thể gây warning với -Wunused-result trên một số compiler:
    // placeBuy(10, 100, 190);

    auto r = placeBuy(10, 5000, 190);
    if (std::holds_alternative<int>(r)) {
        std::cout << "orderId=" << std::get<int>(r) << "\n";
    } else {
        std::cout << "rejected\n";
    }
    return 0;
}
