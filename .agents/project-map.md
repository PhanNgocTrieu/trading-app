# Project map

Current layout (Phase 6). Keep new files next to their layer.

```text
trading-app/
├── AGENTS.md                     # always-on agent rules
├── .agents/                      # this playbook
├── CMakeLists.txt                # C++17, optional Qt + tests
├── include/                      # public headers (trading_core)
│   ├── application/              # *AppService, ports, AppBootstrap
│   ├── domain/                   # header-mostly entities, Result, enums
│   ├── engine/                   # MatchingEngine
│   └── infrastructure/
│       ├── crypto/               # SHA-256 hasher bits
│       └── db/                   # connection, transaction, repos, migrator
├── src/                          # trading_core .cpp (list every file in src/CMakeLists.txt)
│   ├── application/
│   └── infrastructure/db/
├── apps/desktop/                 # Qt Quick
│   ├── main.cpp                  # composition root
│   ├── dto.hpp                   # UI DTOs
│   ├── bridge/                   # TradingAppBridge
│   ├── controllers/              # Auth / Wallet / Order
│   ├── models/                   # Quote, Position, book, working orders
│   ├── market/                   # MockMarketDataFeed
│   ├── qml/
│   └── qml.qrc
├── sql/
│   ├── 001_init.sql
│   └── 002_trading.sql
├── tests/
│   ├── support/app_fixture.hpp
│   └── phase0/ … phase6/
└── docs/
```

## CMake targets

| Target | Role | Add code here when… |
|--------|------|---------------------|
| `trading_core` | App + infra + SQLite | Domain/application/infra behavior |
| `trading_desktop` | Bridge, controllers, models, feed | Desktop/QML C++ |
| `trading-app` | Executable (`main.cpp` + `qml.qrc`) | Entry / QML resources |
| `trading_unit_tests` | GoogleTest | New tests |

Include path for core: `include/` (so `#include "domain/result.hpp"`).

Desktop public includes: `apps/desktop/` + `include/`. Namespace for UI C++: `desktop`.

## Header ↔ source

| Header | Source |
|--------|--------|
| `include/application/*.hpp` | `src/application/*.cpp` |
| `include/infrastructure/db/*.hpp` | `src/infrastructure/db/*.cpp` |
| `include/domain/*.hpp` | usually header-only |
| `include/engine/matching_engine.hpp` | header-only |
| `apps/desktop/**/*.hpp` | matching `.cpp` beside it |

## Tests

| Directory | Covers |
|-----------|--------|
| `tests/support/` | `AppFixture` (`:memory:` DB, `AppBootstrap`) |
| `tests/phase0/` | `Result`, `Account`, `User`, `Stock`, enums, auth/wallet smoke |
| `tests/phase1/` | hasher, SQLite conn/tx/migrate, auth/wallet, persistence |
| `tests/phase2/` | `Position`, `MatchingEngine`, order flow |
| `tests/phase3/` | controllers (needs `trading_desktop`) |
| `tests/phase4/` | mock feed / mark-to-market |
| `tests/phase6/` | limit rest/fill/cancel, book aggregate |

If Qt is missing, CMake compiles `test_phase3_pending.cpp` / `test_phase4_pending.cpp` instead of the real suites.

Do not revive `service/`, `apps/desktop/windows/`, or `ui/`.
