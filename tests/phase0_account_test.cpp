#include "domain/account.hpp"
#include "domain/order_types.hpp"
#include "domain/user.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <string>

namespace {

bool nearlyEqual(double a, double b) {
    return std::fabs(a - b) < 1e-9;
}

} // namespace

int main() {
    User user{1, "alice", "0900"};
    assert(user.id() == 1);
    assert(user.username() == "alice");
    // User must not own cash balance in Phase 0 domain model.

    Account account{10, user.id(), 0.0};
    assert(account.userId() == user.id());
    assert(nearlyEqual(account.cashBalance(), 0.0));

    auto deposited = account.deposit(1000.0);
    assert(deposited.ok());
    assert(nearlyEqual(account.cashBalance(), 1000.0));

    auto badDeposit = account.deposit(-1.0);
    assert(!badDeposit.ok());
    assert(badDeposit.code() == ErrorCode::ValidationFailed);

    auto withdrawn = account.withdraw(250.0);
    assert(withdrawn.ok());
    assert(nearlyEqual(account.cashBalance(), 750.0));

    auto overdraft = account.withdraw(1000.0);
    assert(!overdraft.ok());
    assert(overdraft.code() == ErrorCode::InsufficientFunds);
    assert(nearlyEqual(account.cashBalance(), 750.0));

    assert(std::string(toString(OrderSide::Buy)) == "BUY");
    assert(std::string(toString(OrderStatus::Pending)) == "PENDING");

    std::cout << "phase0_account_test: OK\n";
    return 0;
}
