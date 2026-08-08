#pragma once

#include "domain/result.hpp"

class Account {
public:
    Account(int id, int userId, double cashBalance = 0.0)
        : id_(id), userId_(userId), cashBalance_(cashBalance) {}

    int id() const { return id_; }
    int userId() const { return userId_; }
    double cashBalance() const { return cashBalance_; }

    Result<OkTag> deposit(double amount) {
        if (amount <= 0.0) {
            return failResult(ErrorCode::ValidationFailed, "amount must be > 0");
        }
        cashBalance_ += amount;
        return okResult();
    }

    Result<OkTag> withdraw(double amount) {
        if (amount <= 0.0) {
            return failResult(ErrorCode::ValidationFailed, "amount must be > 0");
        }
        if (cashBalance_ < amount) {
            return failResult(ErrorCode::InsufficientFunds, "insufficient funds");
        }
        cashBalance_ -= amount;
        return okResult();
    }

private:
    int id_;
    int userId_;
    double cashBalance_;
};
