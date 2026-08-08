// c++ -std=c++17 -Wall -Wextra 03_structured_bindings.cpp -o /tmp/a && /tmp/a
#include <iostream>
#include <map>
#include <string>
#include <tuple>

std::pair<bool, int> place(int qty) {
    if (qty <= 0) return {false, 0};
    return {true, 42};
}

int main() {
    std::map<std::string, double> quotes{{"AAPL", 190.0}, {"MSFT", 420.0}};
    for (const auto& [symbol, price] : quotes) {
        std::cout << symbol << "=" << price << "\n";
    }

    if (auto [ok, orderId] = place(10); ok) {
        std::cout << "orderId=" << orderId << "\n";
    }

    auto t = std::make_tuple(std::string{"TSLA"}, 5, 250.0);
    auto [sym, qty, px] = t;
    std::cout << sym << " x" << qty << " @" << px << "\n";
    return 0;
}
