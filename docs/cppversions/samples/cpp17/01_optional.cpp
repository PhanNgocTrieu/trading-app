// c++ -std=c++17 -Wall -Wextra 01_optional.cpp -o /tmp/a && /tmp/a
#include <iostream>
#include <optional>
#include <string>
#include <vector>

struct User {
    int id;
    std::string name;
};

std::optional<User> findByName(const std::vector<User>& users, const std::string& name) {
    for (const auto& u : users) {
        if (u.name == name) return u;
    }
    return std::nullopt;
}

int main() {
    std::vector<User> users{{1, "alice"}, {2, "bob"}};

    if (auto u = findByName(users, "alice")) {
        std::cout << "found id=" << u->id << "\n";
    }

    if (auto u = findByName(users, "carol")) {
        std::cout << "found " << u->name << "\n";
    } else {
        std::cout << "carol not found\n";
    }
    return 0;
}
