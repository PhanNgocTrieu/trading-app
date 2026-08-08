// c++ -std=c++20 -Wall -Wextra 01_concepts.cpp -o /tmp/a && /tmp/a
#include <concepts>
#include <iostream>
#include <string>
#include <string_view>

template <std::integral T>
T addIds(T a, T b) {
    return a + b;
}

template <typename T>
concept PriceLike = std::floating_point<T> || std::integral<T>;

template <PriceLike T>
T notional(T qty, T price) {
    return qty * price;
}

template <typename S>
concept SymbolLike = requires(S s) {
    { std::string_view(s) } -> std::convertible_to<std::string_view>;
};

void printSymbol(SymbolLike auto const& s) {
    std::cout << std::string_view(s) << "\n";
}

int main() {
    std::cout << addIds(10, 32) << "\n";
    std::cout << notional(10.0, 190.5) << "\n";
    printSymbol("AAPL");
    printSymbol(std::string{"MSFT"});
    return 0;
}
