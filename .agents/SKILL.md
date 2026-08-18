---
name: trading-app
description: >-
  Project playbook for the trading-app C++17 / Qt 6 Quick / SQLite paper-trading
  repo. Routes work to the correct layer, enforces hexagonal-lite boundaries,
  and lists build/test commands. Use when implementing features, fixing bugs,
  adding tests, changing schema, or editing QML/C++ in this repository.
---

# trading-app agent playbook

Self-prompt from this file on every task. Details: [project-map.md](project-map.md), [conventions.md](conventions.md), [workflows.md](workflows.md).

## Product (v1 done)

Desktop paper trading: register/login, deposit/withdraw with ledger, mock quotes, market **and limit** BUY/SELL (resting book vs last price), positions + uPnL. Not a live broker.

| In | Out unless asked |
|----|------------------|
| Auth, wallet, market + limit orders, mini book, mock feed, QML UI, SQLite | Live broker, margin/short, options, Postgres, fees, true CLOB, integer cents |

Optional next work lives in `docs/07-implementation-plan.md` (Phase 5+).

## Self-prompting loop

Copy and fill before editing:

```
1. Intent: <one sentence>
2. Layer: domain | engine | application | infrastructure | desktop | tests | docs
3. Read first: <existing files that already own this>
4. Touch: <paths>
5. Do not touch: <paths>
6. Verify: <gtest filter or manual QML check>
```

Rules for the loop:

1. **Classify before coding.** Use the routing table in `AGENTS.md`.
2. **Read the current owner.** Prefer extending `AuthAppService` / `WalletAppService` / `OrderAppService` / `TradingAppBridge` over new facades.
3. **Smallest vertical slice.** One use-case (e.g. withdraw validation), not a layer rewrite.
4. **Match existing shape.** Mirror neighboring headers: `Result<T>`, ports in `include/application/ports.hpp`, DTOs in `apps/desktop/dto.hpp`.
5. **Test at the right phase.** Domain → `tests/phase0` or `phase2`; persistence → `phase1`; order flow → `phase2`; limits → `phase6`; Qt → `phase3`/`phase4`.
6. **Stop at the boundary.** UI bugs stay in `apps/desktop`. Matching bugs stay in `MatchingEngine` / `Position`.

If the request spans layers (e.g. “add withdraw to the UI”), implement **application first**, then controller/bridge, then QML — never the reverse.

## Composition root

```
main.cpp
  → TradingAppBridge(dbPath)          # QML façade, context property `app`
      → AppBootstrap                  # SQLite + repos + *AppService
      → AuthController / WalletController / OrderController
      → MockMarketDataFeed
      → QuoteTableModel / PositionTableModel / book + working-order models
```

Default DB: `~/.trading-app/trading.db`. Override: `./build/apps/desktop/trading-app --db /tmp/demo.db`.

## Canonical types (do not reinvent)

| Concern | Type | Header |
|---------|------|--------|
| Success/fail | `Result<T>`, `ErrorCode` | `include/domain/result.hpp` |
| Session | `AuthSession` | `include/application/ports.hpp` |
| Ports / rows | `I*Repository`, `*Row` | `include/application/ports.hpp` |
| Wiring | `AppBootstrap` | `include/application/app_bootstrap.hpp` |
| Auth | `AuthAppService` | `include/application/auth_app_service.hpp` |
| Cash | `WalletAppService` | `include/application/wallet_app_service.hpp` |
| Orders | `OrderAppService` | `include/application/order_app_service.hpp` |
| Fill | `MatchingEngine` | `include/engine/matching_engine.hpp` |
| Avg cost / sell PnL | `Position` | `include/domain/position.hpp` |
| QML API | `TradingAppBridge` | `apps/desktop/bridge/trading_app_bridge.hpp` |

Enums: `OrderSide`, `OrderType`, `OrderStatus`, `LedgerType`. SQL stores `'BUY'`/`'SELL'` etc. via `toString` / `*FromString`.

## Money and matching (invariants)

- v1 money is `double`. Do not switch to cents unless asked.
- **Never** change `cash_balance` without a `ledger_entries` row in the same `Transaction`.
- Rejected orders must not mutate cash, position, or ledger.
- Market fill price = `market_quotes.last_price` read **inside** the order transaction (do not use a stale feed tick).
- Limit that is not marketable rests as `PENDING`. Fill happens inside `setQuotePrice` (same transaction as the quote write).
- Resting BUY/SELL reserve buying power / shares (`qty * limit` / qty) without changing `cash_balance` until fill.
- `Position::applyBuy` / `applySell` own average-cost math. Persist via `IPositionRepository::upsert`.

## QML contract

Context property: **`app`**.

| UI | Bridge |
|----|--------|
| Login / Register | `login` / `registerUser` |
| Header chips | `cash`, `equity`, `unrealizedPnl` |
| Market watch | `quoteModel` |
| Portfolio | `positionModel` |
| Ticket | `placeMarketOrder`, `placeLimitOrder`, `symbols` |
| Working orders | `workingOrderModel`, `cancelOrder` |
| Mini book | `bidModel`, `askModel`, `bookSymbol` / `setBookSymbol` |
| Feed toggle | `setFeedActive` |

New UI state → `Q_PROPERTY` / `Q_INVOKABLE` on the bridge (and a controller if it is a use-case). Theme tokens live in `apps/desktop/qml/Theme.qml`.

## Build and test (macOS)

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH="$HOME/Qt/6.11.1/macos"
cmake --build build -j
ctest --test-dir build --output-on-failure
./build/tests/trading_unit_tests --gtest_filter='Phase2*'
./build/apps/desktop/trading-app --db /tmp/demo.db
```

Full filter recipes: `docs/testing.md`.

## Docs to read when stuck

| Topic | Doc |
|-------|-----|
| Layers / ADRs | `docs/02-system-architecture.md` |
| Entities / rules | `docs/03-domain-model.md` |
| Schema | `docs/04-database-design.md` |
| Use-case steps | `docs/05-workflows.md` |
| QML / threading | `docs/06-qt-ui-architecture.md` |
| Style | `docs/08-coding-standards.md` |
| Tree / targets | `docs/project-structure.md` |
| QML map | `docs/phase5-notes.md` |
| Limit book | `docs/phase6-notes.md` |

Live API is `include/` + `apps/desktop/`. Do not add a `service/` adapter or Widgets windows.
