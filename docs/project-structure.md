# Project Structure

Current layout after cleanup (Phase 6). One live path: QML desktop → application services → domain → SQLite.

```text
trading-app/
├── CMakeLists.txt
├── Dockerfile
├── AGENTS.md
├── .agents/
├── .devcontainer/
├── include/                      # Public headers (trading_core)
│   ├── application/              # Auth, Wallet, Order, ports, AppBootstrap
│   ├── domain/                   # Pure types (no Qt / SQL)
│   ├── engine/                   # MatchingEngine
│   └── infrastructure/
│       ├── crypto/
│       └── db/
├── src/                          # trading_core implementations
│   ├── application/
│   └── infrastructure/db/
├── apps/desktop/                 # Qt Quick (trading-app)
│   ├── main.cpp
│   ├── dto.hpp
│   ├── bridge/
│   ├── controllers/
│   ├── models/
│   ├── market/
│   ├── qml/
│   └── qml.qrc
├── sql/
│   ├── 001_init.sql
│   └── 002_trading.sql
├── tests/
│   ├── support/app_fixture.hpp   # in-memory AppBootstrap fixture
│   ├── phase0/ … phase6/
│   └── CMakeLists.txt
└── docs/
```

## Targets (CMake)

| Target | Role |
|--------|------|
| `trading_core` | Application + infrastructure + SQLite |
| `trading_desktop` | Controllers, models, feed, QML bridge |
| `trading-app` | Qt Quick desktop executable |
| `trading_unit_tests` | GoogleTest suite |

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH="$HOME/Qt/6.11.1/macos"
cmake --build build -j
./build/apps/desktop/trading-app
./build/apps/desktop/trading-app --db /tmp/demo.db
```

## Dependency direction

```text
apps/desktop (QML + bridge)  →  trading_core  →  SQLite3
tests                        →  trading_core / trading_desktop
```

Domain headers must not depend on Qt or SQL drivers directly.
