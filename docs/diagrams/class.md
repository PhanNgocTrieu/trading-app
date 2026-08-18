# Class diagrams

Types as they exist after Phase 6. `Result<T>` is written `ResultT` (Mermaid). Row structs (`UserRow`, `OrderRow`, …) live in `include/application/ports.hpp`.

## 1. Domain

```mermaid
classDiagram
  class User {
    -int id_
    -string username_
    -string phoneNumber_
  }

  class Account {
    -int id_
    -int userId_
    -double cashBalance_
    +deposit(amount) ResultT
    +withdraw(amount) ResultT
  }

  class Position {
    -string symbol_
    -int qty_
    -double avgCost_
    +applyBuy(qty, price) ResultT
    +applySell(qty, price) ResultT
  }

  class ResultT {
    +ok() bool
    +code() ErrorCode
    +message() string
    +value() T
  }

  class MatchingEngine {
    +match(order, quote) MatchResult
  }

  class IncomingOrder {
    string symbol
    OrderSide side
    OrderType type
    int quantity
    double limitPrice
  }

  class MarketSnapshot {
    string symbol
    double lastPrice
  }

  class MatchResult {
    bool filled
    double fillPrice
    int fillQty
    string rejectReason
  }

  User "1" --> "1" Account : owns cash
  Account "1" --> "*" Position : per symbol
  MatchingEngine ..> IncomingOrder
  MatchingEngine ..> MarketSnapshot
  MatchingEngine ..> MatchResult
  Account ..> ResultT
  Position ..> ResultT
```

Enums: `OrderSide` (Buy/Sell), `OrderType` (Market/Limit), `OrderStatus` (Pending/Filled/Rejected/Canceled), `LedgerType`, `ErrorCode`.

## 2. Application + ports

```mermaid
classDiagram
  class AppBootstrap {
    +open(dbPath) AppBootstrap
    +auth() AuthAppService
    +wallet() WalletAppService
    +orders() OrderAppService
  }

  class AuthAppService {
    +registerUser(...) ResultT
    +login(...) ResultT
  }

  class WalletAppService {
    +deposit(session, amount) ResultT
    +withdraw(session, amount) ResultT
    +balance(session) ResultT
  }

  class OrderAppService {
    +placeMarketOrder(...) ResultT
    +placeLimitOrder(...) ResultT
    +cancelOrder(session, id) ResultT
    +workingOrders(session) ResultT
    +orderBook(symbol) ResultT
    +portfolio(session) ResultT
    +setQuotePrice(symbol, px) ResultT
  }

  class IUserRepository
  class IAccountRepository
  class ILedgerRepository
  class IQuoteRepository
  class IPositionRepository
  class IOrderRepository
  class ITradeRepository
  class IPasswordHasher

  AppBootstrap --> AuthAppService
  AppBootstrap --> WalletAppService
  AppBootstrap --> OrderAppService
  AuthAppService --> IUserRepository
  AuthAppService --> IAccountRepository
  AuthAppService --> IPasswordHasher
  WalletAppService --> IAccountRepository
  WalletAppService --> ILedgerRepository
  OrderAppService --> IAccountRepository
  OrderAppService --> ILedgerRepository
  OrderAppService --> IQuoteRepository
  OrderAppService --> IPositionRepository
  OrderAppService --> IOrderRepository
  OrderAppService --> ITradeRepository
  OrderAppService --> MatchingEngine
```

SQLite classes (`SqliteUserRepository`, …) implement the `I*` ports. `Transaction` is RAII around `BEGIN IMMEDIATE` / `COMMIT` / `ROLLBACK`.

## 3. Desktop (Qt Quick)

```mermaid
classDiagram
  class TradingAppBridge {
    +login(user, pass)
    +registerUser(user, pass)
    +deposit(amount)
    +placeMarketOrder(symbol, side, qty)
    +placeLimitOrder(symbol, side, qty, limit)
    +cancelOrder(id)
    +setFeedActive(bool)
    +setBookSymbol(symbol)
  }

  class AuthController
  class WalletController
  class OrderController
  class MockMarketDataFeed
  class QuoteTableModel
  class PositionTableModel
  class OrderBookTableModel
  class WorkingOrderTableModel

  TradingAppBridge --> AppBootstrap
  TradingAppBridge --> AuthController
  TradingAppBridge --> WalletController
  TradingAppBridge --> OrderController
  TradingAppBridge --> MockMarketDataFeed
  TradingAppBridge --> QuoteTableModel
  TradingAppBridge --> PositionTableModel
  TradingAppBridge --> OrderBookTableModel
  TradingAppBridge --> WorkingOrderTableModel
  AuthController --> AuthAppService
  WalletController --> WalletAppService
  OrderController --> OrderAppService
  MockMarketDataFeed ..> OrderAppService : setQuotePrice
```

QML binds the context property **`app`** (`TradingAppBridge`). It must not call services or SQL.
