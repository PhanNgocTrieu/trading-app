// c++ -std=c++11 -Wall -Wextra 01_auto_range_for.cpp -o /tmp/a && /tmp/a
#include <iostream>
#include <string>
#include <vector>

int main() {
    std::vector<std::string> symbols;
    symbols.push_back("AAPL");
    symbols.push_back("MSFT");
    symbols.push_back("TSLA");

    for (std::vector<std::string>::const_iterator it = symbols.begin();
         it != symbols.end(); ++it) {
        std::cout << "(old) " << *it << "\n";
    }

    for (const auto& symbol : symbols) {
        std::cout << "(range-for) " << symbol << "\n";
    }

    auto count = symbols.size(); // size_t
    std::cout << "count=" << count << "\n";
    return 0;
}
