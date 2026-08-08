// c++ -std=c++23 -Wall -Wextra 02_print_and_to_underlying.cpp -o /tmp/a && /tmp/a
#include <print>
#include <utility>

enum class OrderSide : int { Buy = 1, Sell = 2 };

int main() {
    const auto side = OrderSide::Buy;
    std::println("side underlying={}", std::to_underlying(side));
    std::print("Filled {} x{} @{:.2f}\n", "AAPL", 10, 190.5);
    return 0;
}
