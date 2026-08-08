// Minh họa evolution theo __cplusplus.
// Compile nhiều lần:
//   c++ -std=c++11 ...
//   c++ -std=c++17 ...
//   c++ -std=c++23 ...  (nếu có expected)
#include <iostream>
#include <string>
#include <vector>

struct User {
    int id;
    std::string name;
};

#if __cplusplus >= 202302L && defined(__cpp_lib_expected)
#include <expected>
#include <string_view>
enum class FindError { NotFound };
std::expected<User, FindError> findUser(const std::vector<User>& users, std::string_view name) {
    for (const auto& u : users) if (u.name == name) return u;
    return std::unexpected{FindError::NotFound};
}
#elif __cplusplus >= 201703L
#include <optional>
#include <string_view>
std::optional<User> findUser(const std::vector<User>& users, std::string_view name) {
    for (const auto& u : users) if (u.name == name) return u;
    return std::nullopt;
}
#else
User* findUser(std::vector<User>& users, const std::string& name) {
    for (auto& u : users) if (u.name == name) return &u;
    return nullptr;
}
#endif

int main() {
    std::vector<User> users{{1, "alice"}, {2, "bob"}};
#if __cplusplus >= 202302L && defined(__cpp_lib_expected)
    auto r = findUser(users, "alice");
    if (r) std::cout << "C++23 expected id=" << r->id << "\n";
    else std::cout << "not found\n";
#elif __cplusplus >= 201703L
    if (auto u = findUser(users, "alice")) std::cout << "C++17 optional id=" << u->id << "\n";
#else
    if (User* u = findUser(users, "alice")) std::cout << "C++11 pointer id=" << u->id << "\n";
#endif
    std::cout << "__cplusplus=" << __cplusplus << "\n";
    return 0;
}
