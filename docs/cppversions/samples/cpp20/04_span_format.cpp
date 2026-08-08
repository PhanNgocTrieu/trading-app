// c++ -std=c++20 -Wall -Wextra 04_span_format.cpp -o /tmp/a && /tmp/a
// Một số compiler cần libfmt tương thích nếu <format> chưa đủ — Apple Clang mới thường OK.
#include <format>
#include <iostream>
#include <span>
#include <vector>

void bump(std::span<double> prices, double factor) {
    for (double& p : prices) p *= factor;
}

int main() {
    std::vector<double> prices{100.0, 200.0, 300.0};
    bump(prices, 1.01);

    for (double p : prices) {
        std::cout << std::format("{:.2f}\n", p);
    }

    std::string line = std::format("{} x{} @{:.2f}", "AAPL", 10, prices[0]);
    std::cout << line << "\n";
    return 0;
}
