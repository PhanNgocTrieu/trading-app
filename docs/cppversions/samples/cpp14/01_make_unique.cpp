// c++ -std=c++14 -Wall -Wextra 01_make_unique.cpp -o /tmp/a && /tmp/a
#include <iostream>
#include <memory>
#include <string>

struct User {
    std::string name;
    explicit User(std::string n) : name(std::move(n)) {}
};

int main() {
    auto user = std::make_unique<User>("alice");
    auto ids = std::make_unique<int[]>(5);
    ids[0] = 42;

    std::cout << user->name << " firstId=" << ids[0] << "\n";
    return 0;
}
