# Project Structure

Current layout after Phase 0–3 (CLI + Qt desktop).

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
│       ├── crypto/               # SHA-256 helper
│       └── db/                   # SQLite connection / repos / migrator
├── src/                          # Implementations for trading_core
│   ├── application/
│   └── infrastructure/db/
├── service/                      # CLI-facing adapters (Login, Bank, Trading, Logger)
├── ui/                           # Console UI (trading-app-cli)
├── apps/desktop/                 # Qt Widgets UI (trading-app)
│   ├── controllers/              # Auth / Wallet / Order controllers
│   ├── models/                   # PositionTableModel / QuoteTableModel
│   ├── market/                   # MockMarketDataFeed (QTimer)
│   ├── windows/                  # LoginWindow / MainWindow
│   └── main.cpp
├── sql/
│   ├── 001_init.sql
│   └── 002_trading.sql
├── tests/
│   ├── phase0/ … phase4/
│   └── CMakeLists.txt
└── docs/
```

## Targets (CMake)

| Target | Role |
|--------|------|
| `trading_core` | Application + infrastructure + SQLite |
| `service` | Login/Bank/Trading/Logger adapters (CLI) |
| `trading_desktop` | Qt controllers + windows library |
| `trading-app` | Qt desktop executable (requires Qt6 Widgets) |
| `trading-app-cli` | Console executable |
| `trading_unit_tests` | GoogleTest suite |

Build Qt against a local installer prefix if needed:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH="$HOME/Qt/6.11.1/macos"
cmake --build build -j
./build/apps/desktop/trading-app
./build/apps/desktop/trading-app --db /tmp/demo.db
./build/trading-app-cli
```

## Dependency direction

```text
apps/desktop  →  trading_core  →  SQLite3
ui (CLI)      →  service       →  trading_core
tests         →  service / trading_desktop / trading_core
```

Domain headers must not depend on Qt or SQL drivers directly (repos are infrastructure).
