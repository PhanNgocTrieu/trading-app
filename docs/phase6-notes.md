# Phase 6 notes — Limit orders + mini book

Paper matching is still **against last price**, not order-vs-order. A limit that is not marketable **rests** as `PENDING`. When `setQuotePrice` (including the mock feed) moves last price through the limit, the order fills FIFO at last price.

## Rules

- Marketable limit → fill immediately at last price (same cash/position/ledger/trade path as market).
- Resting BUY reserves `qty * limitPrice` against buying power (cash is not deducted until fill).
- Resting SELL reserves shares the same way.
- Cancel only `PENDING` orders owned by the session.
- Rejected resting attempts persist a `REJECTED` row and do not mutate cash/position.

## File map

```text
include/application/order_app_service.hpp   # placeLimitOrder, cancel, workingOrders, orderBook
src/application/order_app_service.cpp
apps/desktop/models/order_book_table_model.*
apps/desktop/models/working_order_table_model.*
apps/desktop/qml/components/OrderBookPanel.qml
apps/desktop/qml/components/WorkingOrdersPanel.qml
tests/phase6/test_limit_orders.cpp
```

QML: `app.placeLimitOrder`, `app.cancelOrder`, `app.bidModel`, `app.askModel`, `app.workingOrderModel`, `app.bookSymbol`.
