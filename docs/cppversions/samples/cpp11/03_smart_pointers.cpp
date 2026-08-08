// c++ -std=c++11 -Wall -Wextra 03_smart_pointers.cpp -o /tmp/a && /tmp/a
#include <iostream>
#include <memory>
#include <string>

class Account {
public:
    explicit Account(std::string owner) : owner_(owner) {}
    void deposit(double v) { balance_ += v; }
    double balance() const { return balance_; }
    const std::string& owner() const { return owner_; }

private:
    std::string owner_;
    double balance_{0.0};
};

int main() {
    // C++11: chưa có std::make_unique
    std::unique_ptr<Account> acc(new Account("alice"));
    acc->deposit(1000);
    std::cout << acc->owner() << " cash=" << acc->balance() << "\n";

    std::shared_ptr<Account> shared = std::make_shared<Account>("bob");
    std::shared_ptr<Account> shared2 = shared; // refcount 2
    std::cout << "use_count=" << shared.use_count() << "\n";

    // unique_ptr transfer ownership
    std::unique_ptr<Account> other = std::move(acc);
    if (!acc) std::cout << "acc is empty after move\n";
    std::cout << other->owner() << "\n";
    return 0;
}
