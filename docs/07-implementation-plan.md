# 07 — Kế hoạch Implementation theo giai đoạn

Tài liệu này là **roadmap thực thi**. Mỗi phase có: mục tiêu, lý thuyết ôn lại, bước làm, code mẫu (trong `samples/`), DoD, bài tập.

Ước lượng thời gian mang tính học tập (part-time):

| Phase | Thời lượng gợi ý |
|-------|------------------|
| 0 | 2–4 ngày |
| 1 | 4–7 ngày |
| 2 | 5–8 ngày |
| 3 | 7–12 ngày |
| 4 | 5–10 ngày |

---

## Phase 0 — Domain Hardening & Foundation

### Mục tiêu

- Nâng C++17
- Làm sạch domain `User` / `Stock`
- Thêm enums + `Result` type
- CLI vẫn chạy, nhưng cấu trúc hướng kiến trúc đích
- Viết migration mental model (chưa cần DB thật)

### Ôn lý thuyết

- [01] mục B (C++), E (architecture)
- [03] domain model

### Việc cần làm

1. Đổi `CMakeLists.txt` → `cxx_std_17`
2. Tách `balance` khỏi `User` (thêm `Account` class in-memory)
3. Thêm `OrderSide`, `OrderStatus`, ...
4. Refactor `LoginService` nhận dependency logger (bước nhỏ khỏi singleton cứng)
5. Thêm GoogleTest suite theo phase (xem [testing.md](./testing.md))

### Code mẫu

Xem code live:

- `include/domain/result.hpp`
- `include/domain/account.hpp`
- `include/domain/order_types.hpp`

### DoD Phase 0

- [x] Build C++17 thành công
- [x] `User` không còn sở hữu balance như nguồn sự thật
- [x] Có `Account` in-memory deposit/withdraw
- [x] CLI login + xem account vẫn dùng được
- [x] Docs cá nhân: giải thích vì sao tách Account → [phase0-notes.md](./phase0-notes.md)

### Cách thực hiện trên repo hiện tại

```text
1) Tạo include/domain/account.h (+ src)
2) Sửa User bỏ m_balance (hoặc deprecate)
3) BankAccountService thao tác Account thay vì field rời
4) Cập nhật dashboard option 1 in balance từ Account
```

---

## Phase 1 — SQLite Persistence (Auth + Wallet)

### Mục tiêu

- Schema `users`, `accounts`, `ledger_entries`, `schema_version`
- Register/Login thật
- Deposit/Withdraw persist qua transaction
- Password hash

### Ôn lý thuyết

- [01] mục D (SQL/ACID)
- [04] database design
- [05] auth/wallet workflows

### Việc cần làm

1. Thêm dependency SQLite (`libsqlite3` hoặc Qt Sql sau)
2. Viết `001_init.sql` (có thể chưa có orders)
3. `SqliteConnection` + `Transaction` RAII
4. `UserRepository`, `AccountRepository`, `LedgerRepository`
5. `AuthAppService`, `WalletAppService`
6. CLI dùng service mới (trước khi có Qt)

### Code mẫu

- `sql/001_init.sql`
- `include/infrastructure/db/sqlite_connection.hpp`
- `include/infrastructure/db/transaction.hpp`
- `include/application/auth_app_service.hpp`

### DoD Phase 1

- [x] Tạo được user trong DB
- [x] Restart app vẫn login được
- [x] Deposit/withdraw đúng balance + ledger
- [x] Password không lưu plain text
- [x] `*.db` nằm trong `.gitignore`

### Thứ tự implement khuyến nghị

```text
Connection → migrate → UserRepo → Auth → AccountRepo/Ledger → Wallet → nối CLI
```

---

## Phase 2 — Paper Trading Engine

### Mục tiêu

- Tables: stocks, quotes, orders, trades, positions
- Market order BUY/SELL
- MatchingEngine đơn giản
- Portfolio query

### Ôn lý thuyết

- [01] mục A (trading)
- [03] order/position rules
- [05] trading workflows

### Việc cần làm

1. Migration `002_trading.sql`
2. Seed stocks + quotes
3. Domain `Position::applyBuy/applySell`
4. `MatchingEngine` (market → last price)
5. `OrderAppService::placeMarketOrder` (full transaction)
6. CLI commands: buy/sell/portfolio
7. Unit tests cho avg cost / reject

### Code mẫu

- `include/engine/matching_engine.hpp`
- `src/application/order_app_service.cpp`
- `include/domain/position.hpp`

### DoD Phase 2

- [x] 7 acceptance scenarios ở docs/05 mục 10 đều pass
- [x] Reject không làm bẩn cash/position
- [x] Có ít nhất 3 unit tests domain

### Pitfalls thường gặp

- Update position avg sai công thức
- Quên ledger khi buy/sell
- Commit dù validate fail
- Dùng giá ngoài transaction (race với feed)

---

## Phase 3 — Qt Desktop UI

### Mục tiêu

- Qt Quick app (`trading-app`) — **done** (QML, not Widgets)
- LoginPage + ShellPage
- Order ticket + portfolio table
- Wiring controllers ↔ app services

### Ôn lý thuyết

- [01] mục C (Qt)
- [06] Qt UI architecture

### Việc cần làm

1. Cài Qt6, sửa Docker/devcontainer nếu cần
2. Bật AUTOMOC, tìm package Widgets
3. Xây LoginWindow / MainWindow
4. Controllers + DTO conversions
5. PositionTableModel
6. Giữ CLI target optional

### Code mẫu

- `apps/desktop/qml/pages/LoginPage.qml`
- `apps/desktop/qml/pages/ShellPage.qml`
- `apps/desktop/CMakeLists.txt`

### DoD Phase 3

- [x] Login/Register trên UI
- [x] Deposit từ UI
- [x] Place BUY/SELL từ UI
- [x] Portfolio refresh sau giao dịch
- [x] Không có SQL trong `.ui` / window slots (chỉ gọi controller/service)

### Lộ trình UI nhỏ

```text
Week slice A: Login + Main shell + cash label
Week slice B: Deposit dialog
Week slice C: Order ticket
Week slice D: Portfolio + history tables
```

---

## Phase 4 — Market Feed, Polish, Hardening

### Mục tiêu

- Mock price feed + market watch
- Better logging, settings path for DB file
- Tests integration
- (Optional) integer cents migration
- Packaging đơn giản

### Việc cần làm

1. `MockMarketDataFeed` + `QTimer`
2. Refresh market watch / uPnL theo tick
3. Integration test place order
4. About/Help + basic validation UX
5. README chạy app (user-facing) — chỉ khi bạn muốn; docs kỹ thuật đã có

### Code mẫu

- `apps/desktop/market/mock_market_data_feed.hpp`

### DoD Phase 4 (v1 shippable for learning)

- [x] Giá thay đổi theo thời gian
- [x] Equity/uPnL cập nhật
- [x] Crash-safe: kill app giữa chừng không corrupt (SQLite)
- [x] Bộ test core xanh
- [x] Bạn giải thích được end-to-end 1 lệnh BUY trên giấy

---

## Phase 5+ (tuỳ chọn — sau v1)

### Done in Phase 5 (QML redesign)

- [x] QML light-studio UI (`apps/desktop/qml`) via `TradingAppBridge`
- [x] Keep Market orders + mock feed; Widgets windows no longer the app entry

## Phase 6 — Limit orders + mini book

### Mục tiêu

- Limit BUY/SELL: marketable khớp ngay với last price; không marketable thì **rest** (`PENDING`)
- Khi mock feed / `setQuotePrice` đưa giá xuyên limit → fill FIFO trong cùng transaction
- Mini book (bids/asks gom theo giá) + working orders + cancel
- Buying power: cash/shares trừ notional resting (chưa trừ cash đến lúc fill)

### Việc đã làm

1. `OrderAppService::placeLimitOrder` / `cancelOrder` / `workingOrders` / `orderBook`
2. `setQuotePrice` fill resting limits
3. QML ticket MARKET|LIMIT, `OrderBookPanel`, `WorkingOrdersPanel`
4. GoogleTest `tests/phase6/`

### DoD Phase 6

- [x] Limit marketable fill at last price
- [x] Limit không marketable rest, cash không đổi
- [x] Quote cross → fill + ledger/trade/position
- [x] Cancel pending; không fill sau khi cancel
- [x] Reserved buying power chặn over-commit
- [x] Mini book aggregate levels

### Still optional

- True CLOB (khớp buy vs sell, không chỉ last price)
- Multi-account
- PostgreSQL mode
- Real market data API (Yahoo/Finnhub…) với rate limit
- Fees / corporate actions giả lập

---

## Chiến lược làm việc hàng ngày (khuyến nghị)

```text
1. Chọn 1 use-case nhỏ (ví dụ Deposit)
2. Đọc workflow + schema liên quan
3. Viết/adapt code mẫu
4. Chạy manual scenario
5. Commit atomic (khi bạn yêu cầu commit)
6. Ghi note “học được gì”
```

Tránh làm song song Qt + engine + schema lớn trong một lần.

---

## Dependency graph giữa các phase

```text
Phase0 Domain
   ↓
Phase1 DB Auth/Wallet ─────┐
   ↓                       │
Phase2 Engine/Orders ──────┤→ Phase3 Qt UI
                           │
                Phase4 Feed/Polish
```

Phase 3 **cần** Phase 1; nên có Phase 2 trước khi Trade tab có ý nghĩa.

---

## Definition of Done toàn dự án v1

Xem lại [00-overview.md](./00-overview.md) mục 7. Khi đạt DoD đó, bạn đã có một trading-app “thực” theo nghĩa hệ thống phần mềm — đủ để học sâu và mở rộng.
