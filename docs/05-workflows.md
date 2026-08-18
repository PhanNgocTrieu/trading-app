# 05 — Workflows

Tài liệu mô tả luồng nghiệp vụ và luồng kỹ thuật. Dùng làm đặc tả khi implement service/UI.

---

## 1. Workflow tổng ứng dụng

```text
Start App
  → Init DB + migrations
  → Show Login
      ├─ Register → create user+account → Login
      └─ Login success → Session
            → Main Dashboard
                 ├─ Wallet (deposit/withdraw)
                 ├─ Market watch
                 ├─ Place order
                 ├─ Portfolio
                 └─ History (orders/trades/ledger)
  → Logout / Exit (flush logs, close DB)
```

---

## 2. Auth workflows

### 2.1. Register

```text
Actor: User
Input: username, password, phone?
Steps:
  1. Validate username format / password strength (tối thiểu length)
  2. Check username chưa tồn tại
  3. BEGIN
       insert users(password_hash)
       insert accounts(user_id, cash=0)
     COMMIT
  4. Return success → chuyển login hoặc auto-login
Errors:
  - UsernameTaken
  - ValidationFailed
  - DbError
```

### 2.2. Login

```text
Input: username, password
Steps:
  1. Find user by username
  2. Verify password hash
  3. Load account id
  4. Create Session{userId, accountId, username}
  5. UI navigate MainWindow
Errors:
  - InvalidCredentials (message chung, tránh lộ user tồn tại)
```

### 2.3. Logout

```text
Clear Session → back to LoginWindow
(Không cần xoá dữ liệu DB)
```

---

## 3. Wallet workflows

### 3.1. Deposit

```text
Input: amount > 0
BEGIN
  read cash
  cash' = cash + amount
  update accounts
  insert ledger(DEPOSIT, +amount, cash')
COMMIT
UI refresh balance
```

### 3.2. Withdraw

```text
Input: amount > 0
BEGIN
  read cash
  if cash < amount => REJECT
  cash' = cash - amount
  update + ledger(WITHDRAW, -amount, cash')
COMMIT
```

Sequence:

```text
UI WalletView
  → WalletAppService.deposit(session, amount)
    → Transaction
      → AccountRepo + LedgerRepo
  → return newBalance
  → UI update labels
```

---

## 4. Trading workflows

### 4.1. Place Market Order (happy path)

```text
UI OrderTicket
  symbol, side, qty
    → OrderAppService.placeMarketOrder(session, cmd)
      → BEGIN IMMEDIATE
         load account, quote, position
         validate
         insert order PENDING
         MatchingEngine.match(order, quote) => fill
         apply cash/position
         insert trade
         update order FILLED
         insert ledger BUY/SELL
      → COMMIT
    → return OrderResult
UI refresh portfolio + order table
```

### 4.2. Reject path

```text
validate fail (insufficient funds/position/bad symbol)
  → insert order REJECTED (optional but recommended)
  → COMMIT/ROLLBACK theo policy
  → return error message to UI
```

### 4.3. Sequence diagram (text)

```text
User    OrderTicket   OrderAppService   MatchingEngine   Repos/DB
 |          |               |                 |            |
 |--Buy---->|               |                 |            |
 |          |--placeMarket->|                 |            |
 |          |               |--BEGIN--------->|------------|
 |          |               |--load data----->|------------|
 |          |               |--match--------->|            |
 |          |               |<--fill----------|            |
 |          |               |--persist------->|------------|
 |          |               |--COMMIT-------->|------------|
 |          |<--result------|                 |            |
 |<--update-|               |                 |            |
```

### 4.4. Place Limit Order (Phase 6)

```text
UI OrderTicket (LIMIT, limitPrice)
  → OrderAppService.placeLimitOrder(session, symbol, side, qty, limitPrice)
      → BEGIN IMMEDIATE
         match against last price
         if marketable → fill at last price (same as market)
         else rest PENDING (cash unchanged; buying power reserved)
      → COMMIT
Feed / setQuotePrice(symbol, last)
  → BEGIN IMMEDIATE
     update market_quotes
     FIFO pending for symbol: if now marketable → fill
  → COMMIT
Cancel
  → only PENDING owned by session → CANCELED
```

Mini book: aggregate PENDING by (side, limitPrice). Not a CLOB — matching is still vs last price.

---

## 5. Portfolio / query workflows

Các luồng này **read-only** (không cần IMMEDIATE), nhưng vẫn nên nhất quán:

### 5.1. View Portfolio

```text
load cash
load positions join quotes
compute marketValue, unrealized, equity
return PortfolioDto
```

### 5.2. View History

```text
orders by created_at desc
trades by executed_at desc
ledger by created_at desc
```

---

## 6. Market data workflow (mock)

```text
App start
  → MockMarketDataFeed.start(interval=1s)
     loop:
       for each symbol:
         price' = price * (1 + random(-0.5%, +0.5%))
         update market_quotes
       emit signal quotesUpdated()
UI MarketWatch / Portfolio listen → refresh
```

**Lưu ý concurrency:**

- Feed update quote và user place order có thể xen kẽ
- Dùng DB transaction + đọc price trong cùng transaction của order để nhất quán
- UI chỉ đọc sau khi nhận signal

---

## 7. App lifecycle (Qt)

```text
main()
  QApplication
  Bootstrap:
    open DB
    migrate
    construct services
    wire controllers
  show LoginWindow
  app.exec()  // event loop
shutdown:
  stop market feed
  close DB
```

So với console loop hiện tại trong `ui/main.cpp` (`while(g_running) ... system("clear")`):

- Qt event loop thay cho sleep/clear loop
- Signal `SIGINT` vẫn có thể hook, nhưng UX chính là đóng window

---

## 8. Error & logging workflow

Mọi use-case:

```text
try/Result
  on business error → logWarning + user-friendly message
  on db error → logError + generic message (không leak SQL)
```

Correlation tối thiểu:

```text
[INFO] user=alice action=PLACE_ORDER symbol=AAPL side=BUY qty=10 status=FILLED orderId=42
```

---

## 9. Mapping workflow → class (implementation cheat-sheet)

| Workflow | Application class | Domain/Engine | Infra |
|----------|-------------------|---------------|-------|
| Register/Login | `AuthAppService` | `User` | `UserRepository`, `PasswordHasher` |
| Deposit/Withdraw | `WalletAppService` | `Account` | `AccountRepo`, `LedgerRepo` |
| Place order | `OrderAppService` | `Order`, `Position`, `MatchingEngine` | Order/Trade/Position/Quote repos |
| Portfolio | `PortfolioAppService` | `PortfolioCalculator` | Position/Quote/Account repos |
| Quotes stream | `MarketAppService` | — | `MockMarketDataFeed` |

---

## 10. Acceptance scenarios (dùng làm test case)

1. **Register + Login**
2. **Deposit 10_000** → balance = 10000, ledger 1 dòng
3. **Buy 10 AAPL @ 190** → cash 8100, position 10/190, trade 1
4. **Buy thêm 10 @ 210** → qty 20, avg 200
5. **Sell 5 @ 220** → qty 15, cash += 1100, realized = 100
6. **Sell 1000** → rejected, không đổi cash/position
7. **Restart app** → dữ liệu vẫn còn (persistence)

Nếu 7 scenarios này pass, core hệ thống đã “thật”.
