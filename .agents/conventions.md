# Conventions

Mirror neighboring files. When `docs/08-coding-standards.md` and this file disagree, **this file + live code win**.

## C++

- Standard: **C++17**. `#pragma once`. `enum class`.
- Types: `PascalCase`. Files: `snake_case.hpp` / `.cpp`.
- Members: trailing `_` (`db_`, `qty_`). Not `m_` in new code.
- Methods: `camelCase` (`placeMarketOrder`, `registerUser`).
- One primary class per header/source pair (small header-only types OK).
- Include order: own header, project headers, third-party, std.
- `[[nodiscard]]` on `Result<T>` APIs.
- Members that are references (repos, `SqliteConnection`) are non-owning; `AppBootstrap` owns.

## Error handling

```cpp
if (!result.ok()) {
    return Result<T>::fail(result.code(), result.message());
}
```

- Business rule failures → `Result<T>` + `ErrorCode` (`ValidationFailed`, `InsufficientFunds`, `InsufficientPosition`, `Unauthorized`, `Conflict`, `NotFound`).
- DB/open failures may throw at bootstrap; do not `catch (...)`.
- Log at the application/controller boundary, never passwords or tokens.
- Invalid credentials: generic message (do not reveal whether the user exists).

## Layers

**Domain** (`include/domain/`): invariants only. No Qt, no SQLite, no filesystem.

**Engine** (`MatchingEngine`): pure function `match(IncomingOrder, MarketSnapshot) → MatchResult`. No persistence.

**Application**: orchestrate one use-case, open `Transaction`, call ports, map to DTO. Session required for mutating money/positions.

**Infrastructure**: parameterized SQL, hash, migrations. Implements `I*Repository` from `ports.hpp`.

**Desktop**:

- QML: presentation. Bind `app.*`. Use `Theme.qml`.
- `TradingAppBridge`: QObject façade; forwards to controllers; rebuilds overview metrics.
- Controllers: `QObject` + signals; call `*AppService`; emit DTOs.
- Models: `QAbstractTableModel` with `roleNames()` for QML.
- `MockMarketDataFeed`: `QTimer`; updates quotes via `OrderAppService::setQuotePrice`.

**Composition:** only `main.cpp` / `AppBootstrap` / `TradingAppBridge` construct the graph.

## SQL

- Keywords upper-case in `sql/*.sql`.
- Placeholders `?` only. Never concatenate user input.
- Every cash or position mutate: `Transaction` RAII (rollback on destroy if not committed).
- Ledger types: `DEPOSIT`, `WITHDRAW`, `BUY`, `SELL`, `FEE`, `ADJUSTMENT`.
- Order side/type/status stored as the strings from `toString(...)`.
- New tables/columns: add `sql/00N_description.sql` and register it in `runMigrations` (`src/infrastructure/db/migrator.cpp`). Do not silently `ALTER` from C++.

## Qt / QML

- `Q_OBJECT` only when signals, slots, or properties are needed.
- `connect` with functor syntax (`&Class::signal`).
- QObject parent tree for QObject children; `std::unique_ptr` for non-QObject graph (`AppBootstrap`).
- New `.qml` files must be listed in `apps/desktop/qml.qrc` or they will not load from `qrc:/`.
- Do not put business math in QML (avg cost, buying power, fill rules).
- `apps/desktop/windows/` is unused; do not add features there.

## Tests

- GoogleTest. Names: `Suite.Unit_Behavior` (e.g. `Phase1Fixture.Deposit_PersistsBalanceAndLedger`).
- Domain tests must not create `QApplication`.
- Phase 1+ reuse `Phase1Fixture` (`:memory:`). Call `resetCurrentSession()` when touching global session.
- Prefer `loginWithCredentials` over UI/stdin helpers.
- Production bug → add a regression test in the matching phase when feasible.

## Git

- Commit only when the user asks.
- Do not add `*.db`, `build/`, `_deps/`, secrets.
- Prefer small use-case commits when asked (`feat(wallet): persist withdraw ledger`).
