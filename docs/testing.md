# Running Unit Tests (GoogleTest)

This guide explains how to build and run tests for **trading-app**.

---

## 1. Prerequisites

- CMake ≥ 3.16
- C++17 compiler (Apple Clang / GCC / MSVC)
- SQLite3 (usually system-provided on macOS/Linux)
- Network on **first** configure (CMake FetchContent downloads GoogleTest)

---

## 2. Build with tests enabled

From the repo root:

```bash
cmake -S . -B build -DTRADING_APP_BUILD_TESTS=ON
cmake --build build
```

This produces:

```text
build/apps/desktop/trading-app
build/tests/trading_unit_tests
```

Disable tests if you only want the app:

```bash
cmake -S . -B build -DTRADING_APP_BUILD_TESTS=OFF
cmake --build build
```

---

## 3. Run all tests

### Option A — ctest (recommended)

```bash
ctest --test-dir build --output-on-failure
```

Useful flags:

```bash
ctest --test-dir build --output-on-failure -j8          # parallel
ctest --test-dir build --output-on-failure -V           # verbose
ctest --test-dir build -N                               # list tests, don't run
```

### Option B — GoogleTest binary directly

```bash
./build/tests/trading_unit_tests
```

---

## 4. Run tests by phase

### Phase 0 — domain types

```bash
./build/tests/trading_unit_tests \
  --gtest_filter='ResultTest.*:AccountTest.*:UserTest.*:StockTest.*:OrderTypesTest.*:Phase0*'
```

Auth/wallet smoke tests: `Phase0AuthTest.*` / `Phase0WalletTest.*`.

### Phase 1 — SQLite auth + wallet (most common)

```bash
./build/tests/trading_unit_tests \
  --gtest_filter='PasswordHasherTest.*:SqliteInfrastructureTest.*:Phase1*'
```

Or with ctest regex:

```bash
ctest --test-dir build -R 'PasswordHasher|SqliteInfrastructure|Phase1' --output-on-failure
```

| Suite | What it covers |
|-------|----------------|
| `PasswordHasherTest.*` | `hash` / `verify` |
| `SqliteInfrastructureTest.*` | connection, transaction commit/rollback, migrations |
| `Phase1AuthWalletTest.*` | register, login, deposit, withdraw, ledger |
| `Phase1PersistenceTest.*` | restart app → still login + cash persisted |

### Phase 2 — Position + MatchingEngine + order flow

```bash
./build/tests/trading_unit_tests \
  --gtest_filter='PositionTest.*:MatchingEngineTest.*:Phase2*'
```

```bash
ctest --test-dir build -R 'PositionTest|MatchingEngineTest|Phase2' --output-on-failure
```

### Phase 3 — Qt controllers + table model

Requires Qt6 Quick/Test (same `CMAKE_PREFIX_PATH` as the desktop app).

```bash
./build/tests/trading_unit_tests \
  --gtest_filter='Phase3Fixture.*:Phase3PortfolioPresentationTest.*'
```

```bash
ctest --test-dir build -R 'Phase3' --output-on-failure
```

### Phase 4 — mock feed / mark-to-market

```bash
./build/tests/trading_unit_tests --gtest_filter='Phase4*'
```

```bash
ctest --test-dir build -R 'Phase4' --output-on-failure
```

### Phase 6 — Limit orders + mini book

```bash
./build/tests/trading_unit_tests --gtest_filter='Phase6*'
```

```bash
ctest --test-dir build -R 'Phase6' --output-on-failure
```

| Suite | What it covers |
|-------|----------------|
| `Phase6LimitOrderTest.*` | rest, fill-on-cross, cancel, reserved buying power, book aggregate |

---

## 5. Run a single test / function

List names:

```bash
./build/tests/trading_unit_tests --gtest_list_tests
```

Filter one suite:

```bash
./build/tests/trading_unit_tests --gtest_filter=PasswordHasherTest.*
```

Filter one test case:

```bash
./build/tests/trading_unit_tests \
  --gtest_filter=Phase1AuthWalletTest.Deposit_PersistsBalanceAndLedger
```

Filter several patterns (colon-separated):

```bash
./build/tests/trading_unit_tests \
  --gtest_filter='PasswordHasherTest.*:SqliteInfrastructureTest.TransactionCommits'
```

Exclude a pattern:

```bash
./build/tests/trading_unit_tests --gtest_filter=-Phase3*:Phase4*
```

---

## 6. Phase 1 quick recipes

```bash
# Password hasher only
./build/tests/trading_unit_tests --gtest_filter=PasswordHasherTest.*

# SQLite connection / transaction / migrate
./build/tests/trading_unit_tests --gtest_filter=SqliteInfrastructureTest.*

# Auth + wallet use-cases
./build/tests/trading_unit_tests --gtest_filter=Phase1AuthWalletTest.*

# Persistence across process reopen
./build/tests/trading_unit_tests --gtest_filter=Phase1PersistenceTest.*
```

---

## 7. Test layout

```text
tests/
├── CMakeLists.txt
├── support/    # AppFixture — in-memory AppBootstrap
├── phase0/     # Result, Account, User, Stock, enums, auth/wallet smoke
├── phase1/     # hasher, sqlite infra, auth/wallet, persistence
├── phase2/     # Position, MatchingEngine, order flow
├── phase3/     # Qt controllers + table model
├── phase4/     # mock feed / mark-to-market
└── phase6/     # limit rest/fill/cancel + order book
```

---

## 8. Troubleshooting

| Problem | Fix |
|---------|-----|
| GoogleTest download fails | Ensure network on first `cmake -S ...`; retry configure |
| `SQLite3` not found | Install SQLite dev package / Xcode CLT; reconfigure |
| Stale tests after file moves | `rm -rf build && cmake -S . -B build -DTRADING_APP_BUILD_TESTS=ON && cmake --build build` |
| Want less log noise | Redirect: `./build/tests/trading_unit_tests --gtest_filter=Phase1AuthWalletTest.Deposit_PersistsBalanceAndLedger 2>/dev/null` |

---

## 9. Adding a new test

1. Create `tests/phaseX/test_foo.cpp`
2. Add the file to `TRADING_TEST_SOURCES` in `tests/CMakeLists.txt`
3. Rebuild and run:

```bash
cmake --build build
./build/tests/trading_unit_tests --gtest_filter=FooTest.*
```

Conventions:

- Prefer `TEST` / `TEST_F` names like `Unit_Behavior`
- For SQLite/app tests, reuse `AppFixture` from `tests/support/app_fixture.hpp`
- Keep domain tests free of UI (`AuthAppService::login`, not stdin helpers)
