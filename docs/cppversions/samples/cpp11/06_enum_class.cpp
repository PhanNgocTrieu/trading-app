// c++ -std=c++11 -Wall -Wextra 06_enum_class.cpp -o /tmp/a && /tmp/a
#include <iostream>
#include <string>

enum class OrderSide { Buy, Sell };
enum class OrderStatus { Pending, Filled, Rejected };

const char* toString(OrderSide s) {
    return s == OrderSide::Buy ? "BUY" : "SELL";
}

int main() {
    OrderSide side = OrderSide::Buy;
    OrderStatus st = OrderStatus::Pending;
    std::cout << toString(side) << " status=" << static_cast<int>(st) << "\n";
    // int x = side; // lỗi compile — tốt
    return 0;
}
