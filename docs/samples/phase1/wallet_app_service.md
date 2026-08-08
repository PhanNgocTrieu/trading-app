# WalletAppService — Pseudo (Phase 1)

```cpp
Result<double> WalletAppService::deposit(const Session& session, double amount) {
    if (amount <= 0) return fail(ValidationFailed, "amount");

    Transaction tx(db_, true);
    auto acc = accountRepo_->findById(session.accountId);
    if (!acc || acc->status != Active) return fail(...);

    const double next = acc->cash + amount;
    accountRepo_->updateCash(acc->id, next);
    ledgerRepo_->add(acc->id, LedgerType::Deposit, +amount, next, "manual", std::nullopt);
    tx.commit();
    return ok(next);
}

Result<double> WalletAppService::withdraw(const Session& session, double amount) {
    if (amount <= 0) return fail(ValidationFailed, "amount");

    Transaction tx(db_, true);
    auto acc = accountRepo_->findById(session.accountId);
    if (!acc) return fail(NotFound, "account");
    if (acc->cash < amount) return fail(InsufficientFunds, "insufficient funds");

    const double next = acc->cash - amount;
    accountRepo_->updateCash(acc->id, next);
    ledgerRepo_->add(acc->id, LedgerType::Withdraw, -amount, next, "manual", std::nullopt);
    tx.commit();
    return ok(next);
}
```

Gắn vào CLI hiện tại:

- Menu 2 Deposit → gọi `deposit`
- Menu 3 Withdraw → gọi `withdraw`
- Menu 1 View Account → `SELECT cash_balance` + in ledger gần nhất
