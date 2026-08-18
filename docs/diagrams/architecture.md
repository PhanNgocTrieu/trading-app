# Architecture diagrams

Layered + hexagonal-lite. Domain and `MatchingEngine` do not depend on Qt or SQLite.

## 1. System context

```mermaid
flowchart LR
  trader[Trader]
  app[Trading App<br/>Qt Quick desktop]
  db[(SQLite<br/>trading.db)]

  trader -->|register / login / orders| app
  app -->|SQL transactions| db
```

Paper trading only: no live broker. Default DB path is `~/.trading-app/trading.db` (`--db` overrides).

## 2. Layered architecture

```mermaid
flowchart TB
  subgraph presentation["Presentation — apps/desktop"]
    QML[QML pages / components]
    BR[TradingAppBridge]
    CTL[Auth / Wallet / Order controllers]
    MDL[Table models]
    FEED[MockMarketDataFeed]
    QML --> BR
    BR --> CTL
    BR --> MDL
    BR --> FEED
  end

  subgraph application["Application — include/application"]
    BOOT[AppBootstrap]
    AUTH[AuthAppService]
    WAL[WalletAppService]
    ORD[OrderAppService]
  end

  subgraph domain["Domain + engine"]
    ENT[User / Account / Position / enums]
    ENG[MatchingEngine]
  end

  subgraph infra["Infrastructure"]
    PORTS[I*Repository ports]
    SQL[(SQLite repos + Transaction)]
    HASH[SimplePasswordHasher]
  end

  CTL --> AUTH
  CTL --> WAL
  CTL --> ORD
  FEED -->|setQuotePrice| ORD
  BOOT --> AUTH
  BOOT --> WAL
  BOOT --> ORD
  AUTH --> PORTS
  WAL --> PORTS
  ORD --> PORTS
  ORD --> ENG
  AUTH --> ENT
  WAL --> ENT
  ORD --> ENT
  SQL -.->|implements| PORTS
  HASH -.->|implements| PORTS
```

**Dependency rule:** arrows point inward. Presentation may wire infrastructure only at the composition root (`main.cpp` / `AppBootstrap` / `TradingAppBridge`).

## 3. Composition root

```mermaid
flowchart TB
  MAIN[main.cpp]
  BR[TradingAppBridge]
  BOOT[AppBootstrap]
  DB[(SqliteConnection)]
  REPOS[Sqlite*Repository]
  SVC[Auth / Wallet / Order AppService]
  CTL[Controllers]
  UI[QML context property app]

  MAIN --> BR
  MAIN --> UI
  BR --> BOOT
  BR --> CTL
  BOOT --> DB
  BOOT --> REPOS
  BOOT --> SVC
  CTL --> SVC
  UI --> BR
```

## 4. CMake targets

```mermaid
flowchart LR
  CORE[trading_core]
  SVC[service]
  DESK[trading_desktop]
  APP[trading-app]
  TEST[trading_unit_tests]

  DESK --> CORE
  APP --> DESK
  SVC --> CORE
  TEST --> SVC
  TEST --> DESK
  TEST --> CORE
```

| Target | Role |
|--------|------|
| `trading_core` | App services + SQLite repos |
| `service` | Login/Bank/Trading adapters for tests |
| `trading_desktop` | Bridge, controllers, models, feed |
| `trading-app` | QML executable |
| `trading_unit_tests` | GoogleTest |

## 5. Order lifecycle

```mermaid
stateDiagram-v2
  [*] --> Pending: limit not marketable
  [*] --> Filled: market or marketable limit
  [*] --> Rejected: validation / funds / position
  Pending --> Filled: last price crosses limit
  Pending --> Canceled: cancelOrder
  Pending --> Rejected: fill-time funds/position fail
```

Matching is against **last price**, not order-vs-order. Resting limits fill inside `OrderAppService::setQuotePrice` (same transaction as the quote write).
