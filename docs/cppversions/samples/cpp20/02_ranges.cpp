// c++ -std=c++20 -Wall -Wextra 02_ranges.cpp -o /tmp/a && /tmp/a
#include <algorithm>
#include <iostream>
#include <ranges>
#include <vector>

int main() {
    namespace rv = std::views;

    std::vector<int> qty{1, 2, 3, 4, 5, 6};
    auto even_sq = qty
        | rv::filter([](int x) { return x % 2 == 0; })
        | rv::transform([](int x) { return x * x; });

    for (int v : even_sq) std::cout << v << ' ';
    std::cout << "\n";

    std::vector<double> prices{190.0, -1.0, 420.0, 0.0, 250.0};
    auto positive = prices | rv::filter([](double p) { return p > 0; });
    std::cout << "positive count="
              << std::ranges::distance(positive)
              << "\n";
    return 0;
}
