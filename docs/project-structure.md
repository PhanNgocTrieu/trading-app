# Project Structure

Current layout after Phase 5 (QML desktop; console CLI removed).

```text
trading-app/
├── CMakeLists.txt
├── Dockerfile
├── .devcontainer/
├── include/                      # Public headers
│   ├── application/              # Use-cases (Auth, Wallet, Order, bootstrap)
│   ├── domain/                   # Pure domain types
│   ├── engine/                   # MatchingEngine
│   └── infrastructure/
│       ├── crypto/
│       └── db/
├── src/                          # trading_core implementations
├── service/                      # Adapters still used by GoogleTest fixtures
├── apps/desktop/                 # Qt Quick desktop (trading-app)
│   ├── controllers/              # Auth / Wallet / Order
│   ├── models/                   # Quote + Position table models (QML roles)
│   ├── market/                   # MockMarketDataFeed
│   ├── bridge/                   # TradingAppBridge — QML façade
│   ├── qml/                      # Light-studio UI
│   ├── windows/                  # Legacy Widgets (unused)
│   ├── qml.qrc
│   └── main.cpp
├── sql/
│   ├── 001_init.sql
│   └── 002_trading.sql
├── tests/
└── docs/
```

## Targets (CMake)

| Target | Role |
|--------|------|
| `trading_core` | Application + infrastructure + SQLite |
| `service` | Login/Bank/Trading adapters (tests) |
| `trading_desktop` | Controllers + models + feed + QML bridge |
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
tests                        →  service / trading_desktop / trading_core
```

Domain headers must not depend on Qt or SQL drivers directly.
