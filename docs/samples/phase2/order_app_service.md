# OrderAppService — Pseudo đầy đủ (Phase 2)

Đây là đặc tả để bạn implement `.hpp/.cpp` thật. Giữ **một transaction** cho toàn bộ.

```cpp
Result<OrderDto> OrderAppService::placeMarketOrder(const Session& session,
                                                   const std::string& symbol,
                                                   OrderSide side,
                                                   int qty) {
    if (qty <= 0) return fail(ValidationFailed, "qty");

    Transaction tx(db_, /*immediate=*/true);

    auto account = accountRepo_->findById(session.accountId);
    if (!account || account->status != Active) return fail(...);

    auto quote = quoteRepo_->find(symbol);
    if (!quote) {
        // optional: insert REJECTED order
        return fail(NotFound, "symbol");
    }

    IncomingOrder in{symbol, side, OrderType::Market, qty, 0.0};
    MatchResult m = engine_.match(in, {symbol, quote->lastPrice});
    if (!m.filled) {
        orderRepo_->insertRejected(...);
        tx.commit();
        return fail(ValidationFailed, m.rejectReason);
    }

    const double notional = m.fillPrice * m.fillQty;

    if (side == OrderSide::Buy) {
        if (account->cash < notional) {
            orderRepo_->insertRejected(...);
            tx.commit();
            return fail(InsufficientFunds, "insufficient funds");
        }
        accountRepo_->updateCash(account->id, account->cash - notional);

        Position pos = positionRepo_->findOrEmpty(account->id, symbol);
        pos.applyBuy(m.fillQty, m.fillPrice);
        positionRepo_->upsert(account->id, pos);

        ledgerRepo_->add(account->id, LedgerType::Buy, -notional,
                         account->cash - notional, "order", orderId);
    } else { // Sell
        Position pos = positionRepo_->findOrEmpty(account->id, symbol);
        auto sold = pos.applySell(m.fillQty, m.fillPrice);
        if (!sold.ok()) {
            orderRepo_->insertRejected(...);
            tx.commit();
            return fail(sold.code(), sold.message());
        }
        accountRepo_->updateCash(account->id, account->cash + notional);
        positionRepo_->upsert(account->id, pos);
        ledgerRepo_->add(account->id, LedgerType::Sell, +notional,
                         account->cash + notional, "order", orderId);
        // optional: lưu realized PnL
    }

    int64_t orderId = orderRepo_->insertFilled(...);
    tradeRepo_->insert(orderId, account->id, symbol, side, m.fillQty, m.fillPrice);

    tx.commit();
    return ok(toDto(orderId));
}
```

## Checklist tự review PR Phase 2

1. Có `BEGIN IMMEDIATE` không?
2. Reject có tạo side-effect cash không?
3. Avg cost có unit test không?
4. Ledger amount dấu đúng (+ sell / - buy) không?
5. Restart app còn thấy position không?
