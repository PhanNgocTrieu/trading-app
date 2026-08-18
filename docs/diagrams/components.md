# Component diagrams

Runtime pieces and how they connect. Source layout: [project-structure.md](../project-structure.md).

## 1. Logical components

```mermaid
flowchart TB
  subgraph desktop["trading-app — Qt Quick"]
    QML[QML UI]
    BRIDGE[TradingAppBridge]
    CTRL[Controllers]
    MODELS[Table models]
    FEED[MockMarketDataFeed]
  end

  subgraph core["trading_core"]
    APPSVC[Auth / Wallet / Order services]
    ENGINE[MatchingEngine]
    DOMAIN[Domain types]
    REPOS[SQLite repositories]
    MIG[Migrator]
  end

  subgraph persist["Persistence"]
    SQLFILES[sql/001_init.sql<br/>sql/002_trading.sql]
    DB[(SQLite file)]
  end

  QML --> BRIDGE
  BRIDGE --> CTRL
  BRIDGE --> MODELS
  BRIDGE --> FEED
  CTRL --> APPSVC
  FEED --> APPSVC
  APPSVC --> ENGINE
  APPSVC --> DOMAIN
  APPSVC --> REPOS
  REPOS --> DB
  MIG --> SQLFILES
  MIG --> DB
```

## 2. Ports and adapters

```mermaid
flowchart LR
  subgraph hex["Application"]
    ORD[OrderAppService]
  end

  subgraph ports["Ports"]
    IQ[IQuoteRepository]
    IO[IOrderRepository]
    IP[IPositionRepository]
    IA[IAccountRepository]
    IL[ILedgerRepository]
    IT[ITradeRepository]
  end

  subgraph adapters["SQLite adapters"]
    SQ[SqliteQuoteRepository]
    SO[SqliteOrderRepository]
    SP[SqlitePositionRepository]
    SA[SqliteAccountRepository]
    SL[SqliteLedgerRepository]
    ST[SqliteTradeRepository]
  end

  ORD --> IQ
  ORD --> IO
  ORD --> IP
  ORD --> IA
  ORD --> IL
  ORD --> IT
  SQ --- IQ
  SO --- IO
  SP --- IP
  SA --- IA
  SL --- IL
  ST --- IT
```

`AuthAppService` uses `IUserRepository` + `IAccountRepository` + `IPasswordHasher`. `WalletAppService` uses `IAccountRepository` + `ILedgerRepository`.

## 3. UI components (QML)

```mermaid
flowchart TB
  MAIN[Main.qml]
  LOGIN[LoginPage]
  SHELL[ShellPage]

  MAIN --> LOGIN
  MAIN --> SHELL

  SHELL --> QUOTES[QuotePanel]
  SHELL --> TICKET[OrderTicket]
  SHELL --> PORT[PortfolioPanel]
  SHELL --> BOOK[OrderBookPanel]
  SHELL --> WORK[WorkingOrdersPanel]
  SHELL --> CHIPS[MetricChip cash / equity / uPnL]
```

All of these talk only to `app` (`TradingAppBridge`).

## 4. Test components

```mermaid
flowchart LR
  GTEST[trading_unit_tests]
  P0[phase0 domain]
  P1[phase1 sqlite auth/wallet]
  P2[phase2 matching / market orders]
  P3[phase3 controllers]
  P4[phase4 mock feed]
  P6[phase6 limits / book]

  GTEST --> P0
  GTEST --> P1
  GTEST --> P2
  GTEST --> P3
  GTEST --> P4
  GTEST --> P6
```

Phase 3/4 need `trading_desktop` (Qt). Domain tests must not construct `QApplication`.
