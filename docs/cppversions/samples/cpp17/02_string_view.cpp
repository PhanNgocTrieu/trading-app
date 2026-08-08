// c++ -std=c++17 -Wall -Wextra 02_string_view.cpp -o /tmp/a && /tmp/a
#include <iostream>
#include <string>
#include <string_view>

bool isBuy(std::string_view side) {
    return side == "BUY" || side == "buy";
}

// DANGER demo — đừng làm vậy trong production
// std::string_view dangling() {
//     std::string tmp = "AAPL";
//     return tmp;
// }

int main() {
    std::cout << std::boolalpha;
    std::cout << isBuy("BUY") << "\n";

    std::string s = "sell";
    std::cout << isBuy(s) << "\n";
    return 0;
}
