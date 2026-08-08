// c++ -std=c++14 -Wall -Wextra 02_generic_lambda.cpp -o /tmp/a && /tmp/a
#include <iostream>
#include <string>
#include <vector>

int main() {
    auto printAll = [](const auto& xs) {
        for (const auto& x : xs) std::cout << x << ' ';
        std::cout << '\n';
    };

    printAll(std::vector<int>{1, 2, 3});
    printAll(std::vector<std::string>{"AAPL", "MSFT"});

    auto add = [](auto a, auto b) { return a + b; };
    std::cout << add(2, 3) << " " << add(1.5, 2.5) << "\n";
    return 0;
}
