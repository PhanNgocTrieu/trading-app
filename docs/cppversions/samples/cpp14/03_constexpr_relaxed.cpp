// c++ -std=c++14 -Wall -Wextra 03_constexpr_relaxed.cpp -o /tmp/a && /tmp/a
#include <iostream>

constexpr int factorial(int n) {
    int r = 1;
    for (int i = 2; i <= n; ++i) r *= i;
    return r;
}

constexpr long long cashCents(int dollars) {
    return static_cast<long long>(dollars) * 100;
}

static_assert(factorial(5) == 120, "factorial");
static_assert(cashCents(10) == 1000, "cents");

int main() {
    std::cout << factorial(6) << " " << cashCents(25) << "\n";
    return 0;
}
