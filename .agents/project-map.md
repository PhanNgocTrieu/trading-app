# Project map

Current layout (Phase 5). Keep new files next to their layer.

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
├── service/                      # Login/Bank/Trading adapters — tests only
├── apps/desktop/                 # Qt Quick
│   ├── main.cpp                  # composition root
│   ├── dto.hpp                   # UI DTOs (SessionDto, PositionUiDto, …)
│   ├── bridge/                   # TradingAppBridge
│   ├── controllers/              # Auth / Wallet / Order
│   ├── models/                   # QuoteTableModel, PositionTableModel
│   ├── market/                   # MockMarketDataFeed
│   ├── qml/                      # Main, Theme, pages, components
│   ├── windows/                  # leftover Widgets — do not extend
│   └── qml.qrc
├── sql/
│   ├── 001_init.sql              # users, accounts, ledger, schema_version
│   └── 002_trading.sql           # stocks, quotes, orders, trades, positions
├── tests/                        # GoogleTest by phase
└── docs/                         # human architecture docs
```

## CMake targets

| Target | Role | Add code here when… |
|--------|------|---------------------|
| `trading_core` | App + infra + SQLite | Domain/application/infra behavior |
| `service` | Test-facing adapters | Only if a fixture still needs Login/Bank wrappers |
| `trading_desktop` | Bridge, controllers, models, feed | Desktop/QML C++ |
| `trading-app` | Executable (`main.cpp` + `qml.qrc`) | Entry / QML resources |
| `trading_unit_tests` | GoogleTest | New tests |

Include path for core: `include/` (so `#include "domain/result.hpp"`).

Desktop public includes: `apps/desktop/` + `include/`. Namespace for UI C++: `desktop`.

## Header ↔ source

| Header | Source |
|--------|--------|
| `include/application/*.hpp` | `src/application/*.cpp` |
| `include/infrastructure/db/*.hpp` | `src/infrastructure/db/*.cpp` (connection/transaction may be header-only) |
| `include/domain/*.hpp` | usually header-only |
| `include/engine/matching_engine.hpp` | header-only |
| `apps/desktop/**/*.hpp` | matching `.cpp` beside it |

Domain is intentionally header-mostly so tests can use it without Qt.

## Tests

| Directory | Covers |
|-----------|--------|
| `tests/phase0/` | `Result`, `Account`, `User`, `Stock`, enums, `Session`, Login/Bank smoke |
| `tests/phase1/` | hasher, SQLite conn/tx/migrate, auth/wallet, persistence; shared `test_support.hpp` (`Phase1Fixture`, `:memory:` DB) |
| `tests/phase2/` | `Position`, `MatchingEngine`, order flow |
| `tests/phase3/` | controllers (needs `trading_desktop`) |
| `tests/phase4/` | mock feed / mark-to-market |

If Qt is missing, CMake compiles `test_phase3_pending.cpp` / `test_phase4_pending.cpp` instead of the real suites.

## Legacy — do not grow

- `apps/desktop/windows/` — Widgets UI, unused by `main.cpp`
- `service/` — wrap `*AppService` for old tests; new use-cases go to application + controllers
- `docs/samples/` — teaching snapshots; live code is `include/` / `src/` / `apps/desktop/`
