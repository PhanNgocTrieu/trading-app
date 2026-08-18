# Use-case diagrams

Actor is the **Trader** using the Qt Quick desktop app. The system is paper trading only (no live broker).

## 1. Overview

```mermaid
flowchart LR
  trader((Trader))

  subgraph system["Trading App"]
    uc1[Register / Login / Logout]
    uc2[Deposit cash]
    uc3[Watch quotes]
    uc4[Place market order]
    uc5[Place limit order]
    uc6[Cancel resting order]
    uc7[View portfolio]
    uc8[View mini book]
    uc9[Toggle mock feed]
  end

  trader --> uc1
  trader --> uc2
  trader --> uc3
  trader --> uc4
  trader --> uc5
  trader --> uc6
  trader --> uc7
  trader --> uc8
  trader --> uc9
```

## 2. Include / extend

```mermaid
flowchart TB
  trader((Trader))

  login[Login]
  session[Hold AuthSession]
  deposit[Deposit]
  placeM[Place market order]
  placeL[Place limit order]
  cancel[Cancel pending]
  fill[Fill vs last price]
  rest[Rest as PENDING]
  book[Refresh mini book]
  port[Refresh portfolio]
  feed[Mock feed tick]

  trader --> login
  login --> session
  trader --> deposit
  deposit -.->|include| session
  trader --> placeM
  trader --> placeL
  trader --> cancel
  placeM -.->|include| session
  placeL -.->|include| session
  cancel -.->|include| session
  placeM --> fill
  placeL -->|marketable| fill
  placeL -->|not marketable| rest
  fill --> port
  rest --> book
  feed --> fill
  feed --> port
  feed --> book
```

## 3. Per-use-case notes

| Use case | UI | Application | Notes |
|----------|----|-------------|--------|
| Register | `LoginPage` | `AuthAppService::registerUser` | Creates user + empty account in one transaction |
| Login | `LoginPage` | `AuthAppService::login` | Generic error on bad credentials |
| Logout | Shell header | `AuthController` | Stops feed; does not delete data |
| Deposit | `OrderTicket` | `WalletAppService::deposit` | Ledger `DEPOSIT` required |
| Withdraw | *(controller only)* | `WalletAppService::withdraw` | Implemented in C++; not on the QML ticket |
| Watch quotes | `QuotePanel` | `listQuotes` + feed | Last price from `market_quotes` |
| Market order | `OrderTicket` | `placeMarketOrder` | Fill or `REJECTED`; no rest |
| Limit order | `OrderTicket` | `placeLimitOrder` | Rest if not marketable; reserves buying power / shares |
| Cancel | `WorkingOrdersPanel` | `cancelOrder` | Own `PENDING` only |
| Portfolio | `PortfolioPanel` | `portfolio` | Avg cost + uPnL from last price |
| Mini book | `OrderBookPanel` | `orderBook` | Aggregated resting limits vs last price, not a CLOB |
| Toggle feed | Shell header | `MockMarketDataFeed` | Each tick calls `setQuotePrice` (can fill rests) |

## 4. Out of scope for the actor

These are **not** trader use cases in v1:

- Live broker / exchange connectivity
- Margin or short selling
- Fees, options, futures
- Multi-account switching
- True central limit order book (buy vs sell matching)
