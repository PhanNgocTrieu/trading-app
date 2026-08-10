# Unit Testing Guide (GoogleTest)

## Layout

```text
tests/
├── CMakeLists.txt
├── phase0/          # Active — domain + services (Phase 0)
├── phase1/          # DISABLED scaffolds until SQLite auth/wallet exists
├── phase2/          # Active — Position + MatchingEngine
├── phase3/          # DISABLED scaffolds until Qt controllers exist
└── phase4/          # DISABLED scaffolds until market feed exists
```

## Run tests

```bash
cmake -S . -B build -DTRADING_APP_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
# or:
./build/tests/trading_unit_tests
./build/tests/trading_unit_tests --gtest_filter=AccountTest.*
```

Disable tests at configure time:

```bash
cmake -S . -B build -DTRADING_APP_BUILD_TESTS=OFF
```

## Coverage by phase

| Phase | Status | What is tested |
|-------|--------|----------------|
| 0 | Active | Domain units + login/bank fixtures (SQLite-backed) |
| 1 | Active | Password hasher, SQLite infra, auth/wallet persistence |
| 2 | Active | `Position::applyBuy/applySell`, `MatchingEngine::match` |
| 3 | Pending (`DISABLED_*`) | Controller/DTO checklist |
| 4 | Pending (`DISABLED_*`) | Feed / MTM / integration checklist |

`DISABLED_*` tests are **not executed**. When you implement a function, rename e.g. `DISABLED_Deposit_Persists...` → `Deposit_Persists...` and replace `GTEST_SKIP` with real assertions.

## Conventions

1. One test file per unit (class / header group)
2. Name tests `Unit_Behavior` (`AccountTest.DepositIncreasesBalance`)
3. Reset global session in fixtures via `resetCurrentSession()`
4. Prefer testing domain/services without UI (`requestLogin` stdin is not unit-tested; use `loginWithCredentials`)
5. Keep business-rule tests free of Qt/SQL when possible

## Adding a new test

1. Create `tests/phaseX/test_foo.cpp`
2. Add the file to `TRADING_TEST_SOURCES` in `tests/CMakeLists.txt`
3. Rebuild + `ctest`
