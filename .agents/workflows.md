# Workflows

Run the matching checklist. Keep diffs inside one use-case.

## A. New application use-case (auth / wallet / order)

1. Read the existing `*AppService` method that is closest.
2. Add/extend the API in `include/application/` returning `Result<T>`.
3. Implement in `src/application/`: `Transaction`, validate, ports, commit.
4. If a new persistence field is required, do **workflow C** first.
5. Wire through `AppBootstrap` only if new dependencies appear.
6. Controller (if UI): call the service, emit success/error signals.
7. Bridge: `Q_INVOKABLE` / `Q_PROPERTY`, update metrics via `rebuildOverview()`.
8. QML: bind the new API; no extra logic.
9. Test: `tests/phase1` (auth/wallet) or `tests/phase2` (orders). Add the `.cpp` to `TRADING_TEST_SOURCES`.
10. Build and filter-run the new suite.

Cash rule: update `accounts.cash_balance` and insert `ledger_entries` in the same transaction, or abort.

## B. Domain / matching change

1. Change `include/domain/` or `include/engine/matching_engine.hpp` only.
2. Add/adjust `tests/phase0` or `tests/phase2` (pure, no SQLite required for engine/position unit tests).
3. If `OrderAppService` assumed old behavior, update it and `tests/phase2/test_order_flow.cpp`.
4. Do not “fix” avg cost in QML or SQL.

## C. Schema change

1. Add `sql/00N_name.sql` (`PRAGMA foreign_keys = ON;`, `CREATE TABLE IF NOT EXISTS` / `CREATE INDEX IF NOT EXISTS`).
2. Register the file in `runMigrations`.
3. Extend row structs + `I*Repository` in `ports.hpp`.
4. Implement SQL in `sqlite_repositories.*`.
5. Update `docs/04-database-design.md` if the live schema meaning changed.
6. Test with `SqliteInfrastructureTest` and a persistence fixture (`Phase1PersistenceTest` pattern).

Never edit a shipped migration in place if local DBs may already have applied it; add `003_…` instead.

## D. QML-only UI change

1. Reuse `Theme.qml` tokens (`bgTop`, fonts, accent).
2. Pages in `qml/pages/`, controls in `qml/components/`.
3. Register the file in `qml.qrc`.
4. If you need new data, stop and do workflow A/B — do not fetch from SQLite in QML.
5. Keep `TradingAppBridge` as the only object QML calls (`app`).

## E. Mock market feed

- Feed belongs in `apps/desktop/market/`.
- Price writes go through `OrderAppService::setQuotePrice` (or the same repo path used today), not raw SQL in the timer slot.
- UI refresh: models + `rebuildOverview()`; `metricsFlashed` for chip animation.
- Tests: `tests/phase4/`.

## F. Verify

```bash
cmake --build build -j
./build/tests/trading_unit_tests --gtest_filter='<Suite>.*'
```

| You changed | Filter |
|-------------|--------|
| `Result` / account / user / enums | `ResultTest.*:AccountTest.*:UserTest.*:OrderTypesTest.*` |
| Auth / wallet / sqlite | `Phase1Fixture.*:SqliteInfrastructureTest.*:PasswordHasherTest.*` |
| Position / matching / place order | `PositionTest.*:MatchingEngineTest.*:Phase2*` |
| Controllers | `Phase3*` |
| Feed | `Phase4*` |

Manual UI: `./build/apps/desktop/trading-app --db /tmp/demo.db` then register → deposit → BUY → confirm cash, position, ledger.

## G. Stop and ask

- Live market data, broker APIs, or moving money to integer cents
- Replacing SQLite with Postgres
- Rebuilding the Widgets windows as the main UI
- Large drive-by refactors (`service/` deletion, renaming all members)

## Anti-patterns

- SQL inside QML, window slots, or domain headers
- Updating balance with no ledger
- Committing a transaction after a validation/`Result` failure
- Using feed last-tick price from outside the order `Transaction`
- New god class beside `AppBootstrap` / `TradingAppBridge`
- Editing `docs/samples/` instead of production headers
- Adding files to disk but not to CMake / `qml.qrc`
