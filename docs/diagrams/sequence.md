# Sequence diagrams

Happy paths unless noted. Cash mutations always run inside `Transaction` (`BEGIN IMMEDIATE`) and write a ledger row.

## 1. Register and login

```mermaid
sequenceDiagram
  actor U as Trader
  participant Q as LoginPage
  participant B as TradingAppBridge
  participant A as AuthController
  participant S as AuthAppService
  participant DB as SQLite

  U->>Q: register username / password
  Q->>B: registerUser()
  B->>A: registerUser()
  A->>S: registerUser(username, password)
  S->>DB: BEGIN
  S->>DB: insert users password_hash
  S->>DB: insert accounts cash=0
  S->>DB: COMMIT
  S-->>A: AuthSession
  A-->>B: loginSucceeded
  B-->>Q: loggedInChanged → ShellPage
```

Login verifies the hash and loads `accountId` into `AuthSession`. Failed login returns a generic credentials error.

## 2. Deposit

```mermaid
sequenceDiagram
  actor U as Trader
  participant Q as OrderTicket
  participant B as TradingAppBridge
  participant W as WalletController
  participant S as WalletAppService
  participant DB as SQLite

  U->>Q: Deposit amount
  Q->>B: deposit(amount)
  B->>W: deposit(amount)
  W->>S: deposit(session, amount)
  S->>DB: BEGIN IMMEDIATE
  S->>DB: cash += amount
  S->>DB: ledger DEPOSIT
  S->>DB: COMMIT
  S-->>W: new balance
  W-->>B: cashUpdated
  B-->>Q: header Cash chip
```

`WalletAppService::withdraw` is the same shape with a funds check. The QML ticket currently exposes deposit only.

## 3. Place market order

```mermaid
sequenceDiagram
  actor U as Trader
  participant Q as OrderTicket
  participant B as TradingAppBridge
  participant C as OrderController
  participant S as OrderAppService
  participant E as MatchingEngine
  participant DB as SQLite

  U->>Q: Submit MARKET
  Q->>B: placeMarketOrder(symbol, side, qty)
  B->>C: placeMarketOrder(...)
  C->>S: placeMarketOrder(session, ...)
  S->>DB: BEGIN IMMEDIATE
  S->>DB: load account, quote, position
  S->>E: match Market vs last price
  E-->>S: filled at lastPrice
  alt buy and cash OK / sell and shares OK
    S->>DB: cash, position, order FILLED, trade, ledger
    S->>DB: COMMIT
    S-->>C: OrderDto FILLED
    C-->>B: orderAccepted + refresh models
  else insufficient funds or position
    S->>DB: insert order REJECTED
    S->>DB: COMMIT
    S-->>C: fail Result
    C-->>B: orderRejected
  end
```

Fill price is the quote read **inside** this transaction, not a stale feed tick.

## 4. Place limit order — rest then fill

```mermaid
sequenceDiagram
  actor U as Trader
  participant Q as OrderTicket
  participant B as TradingAppBridge
  participant S as OrderAppService
  participant E as MatchingEngine
  participant F as MockMarketDataFeed
  participant DB as SQLite

  U->>Q: Submit LIMIT below last
  Q->>B: placeLimitOrder(...)
  B->>S: placeLimitOrder(session, ...)
  S->>DB: BEGIN IMMEDIATE
  S->>E: match Limit vs last
  E-->>S: limit not marketable
  S->>DB: insert order PENDING
  Note over S,DB: cash unchanged; buying power reserved
  S->>DB: COMMIT
  S-->>B: OrderDto PENDING

  F->>S: setQuotePrice(symbol, last)
  S->>DB: BEGIN IMMEDIATE
  S->>DB: update market_quotes
  S->>E: match each PENDING FIFO
  alt now marketable and reserves OK
    S->>DB: FILLED + cash/position/trade/ledger
  else still not marketable
    S-->>S: leave PENDING
  end
  S->>DB: COMMIT
  F-->>B: quotesUpdated → refresh UI
```

Cancel: `cancelOrder` sets `PENDING` → `CANCELED` for the session’s own order only.

## 5. Mock feed tick

```mermaid
sequenceDiagram
  participant T as QTimer
  participant F as MockMarketDataFeed
  participant S as OrderAppService
  participant B as TradingAppBridge
  participant UI as ShellPage

  T->>F: timeout
  loop each symbol
    F->>S: setQuotePrice(symbol, shocked last)
    Note over S: may fill resting limits
  end
  F->>B: quotesUpdated
  B->>B: refresh quotes, portfolio, book, working orders
  B->>UI: metricsFlashed
```

## 6. Logout

```mermaid
sequenceDiagram
  actor U as Trader
  participant UI as ShellPage
  participant B as TradingAppBridge

  U->>UI: Logout
  UI->>B: logout()
  B->>B: setFeedActive false
  B->>B: clear session, models, metrics
  B-->>UI: loggedInChanged → LoginPage
```

No DB wipe. The next login reloads cash, positions, and working orders from SQLite.
