# 00 — Overview: Trading App thực tế

## 1. Sản phẩm là gì?

**Trading App** (phiên bản học tập / paper trading) là ứng dụng desktop cho phép người dùng:

1. Đăng ký / đăng nhập
2. Nạp / rút tiền giả lập (cash ledger)
3. Xem danh sách mã cổ phiếu và giá (mock market data)
4. Đặt lệnh **BUY / SELL** (Market / Limit — giai đoạn sau)
5. Theo dõi **portfolio** (vị thế, giá trị, lãi/lỗ)
6. Xem lịch sử giao dịch và audit log

Đây **không** phải broker thật kết nối sàn. Mục tiêu là xây dựng hệ thống “đúng hình dạng” phần mềm trading để học sâu C++ / Qt / SQL / kiến trúc.

---

## 2. Phạm vi (Scope)

### In-scope (MVP → v1)

| Module | Mô tả |
|--------|--------|
| Auth | Đăng ký, đăng nhập, session đơn giản |
| Wallet | Cash balance, deposit/withdraw, ledger |
| Market | Danh sách stock + giá cập nhật (mock / timer) |
| Orders | Đặt lệnh, hủy lệnh (pending), trạng thái lệnh |
| Matching | Engine khớp lệnh nội bộ (paper) |
| Portfolio | Positions, avg cost, unrealized/realized PnL |
| UI | Qt 6 Quick (QML) |
| Persistence | SQLite (dev) → có thể PostgreSQL (prod học) |

### Out-of-scope (v1)

- Kết nối broker / exchange thật (REST/FIX)
- Margin / short selling phức tạp
- Options / futures
- High-frequency trading / microsecond latency
- Multi-user concurrent server production-grade

---

## 3. Tech stack

| Tầng | Công nghệ | Lý do chọn |
|------|-----------|------------|
| Language | C++17 (khuyến nghị nâng từ C++11 hiện tại) | Kiểm soát bộ nhớ, hiệu năng, phổ biến trong hệ thống tài chính |
| UI / Platform | Qt 6 Quick (QML) | Cross-platform desktop; QML binds to `TradingAppBridge` |
| Build | CMake | Đã dùng trong repo; chuẩn công nghiệp |
| DB | SQLite (Qt SQL / sqlite3) | Zero-ops cho desktop; dễ học transaction |
| Logging | Controller / bridge messages (no password logs) | Quan sát hành vi hệ thống |
| Test | GoogleTest | Unit test domain/engine/app + Qt Test for controllers |
| Container | Docker / Dev Container | Môi trường build ổn định (đã có `.devcontainer`) |

---

## 4. Hiện trạng repo (baseline)

Cấu trúc hiện tại (rút gọn) — chi tiết: [project-structure.md](./project-structure.md):

```text
trading-app/
├── apps/desktop/       # Qt Quick UI
├── include/            # application / domain / engine / infrastructure
├── src/                # trading_core implementations
├── sql/                # SQLite migrations
├── tests/              # GoogleTest by phase
├── docs/
└── CMakeLists.txt
```

Đã có (Phase 0–6):

- Domain: `User` / `Account` / `Position` / `Result` / order enums
- SQLite: auth, wallet, ledger, orders, positions, trades
- Paper matching (market + resting limits vs last price)
- Qt Quick desktop (`trading-app`)
- GoogleTest suite via `AppFixture` / `AppBootstrap`

Còn lại (optional, xem [07-implementation-plan.md](./07-implementation-plan.md)):

- Packaging / true CLOB / integer cents — **out of v1 unless asked**

---

## 5. Mục tiêu kiến trúc (North Star)

```text
┌─────────────────────────────────────────────────────────┐
│                     Qt Quick Presentation                │
│   QML → TradingAppBridge → Controllers / models          │
└───────────────────────────┬─────────────────────────────┘
                            │ Application Services
┌───────────────────────────▼─────────────────────────────┐
│              Application / Use-case Layer                │
│   AuthAppService, WalletAppService, OrderAppService      │
└───────────────────────────┬─────────────────────────────┘
                            │
┌───────────────────────────▼─────────────────────────────┐
│                    Domain + Engine                       │
│   User, Account, Order, Position, MatchingEngine         │
└───────────────────────────┬─────────────────────────────┘
                            │ Repository interfaces
┌───────────────────────────▼─────────────────────────────┐
│                 Infrastructure (SQL / IO)                │
│   SQLite repositories, MockMarketDataFeed (desktop)      │
└─────────────────────────────────────────────────────────┘
```

**Quy tắc vàng:** Domain và Matching Engine **không** `#include` Qt. Chỉ tầng Presentation và (tuỳ chọn) một adapter dùng Qt SQL.

---

## 6. Các giai đoạn triển khai (tóm tắt)

| Phase | Tên | Kết quả chính |
|-------|-----|----------------|
| 0 | Domain hardening | Domain classes đầy đủ + CLI dùng được hơn |
| 1 | Persistence | SQLite schema + repositories + login/wallet thật |
| 2 | Paper trading engine | Order → match → trade → position |
| 3 | Qt UI | Login, dashboard, order ticket, portfolio views |
| 4 | Market + polish | Mock price feed, charts đơn giản, tests, packaging |

Chi tiết: [07-implementation-plan.md](./07-implementation-plan.md).

---

## 7. Định nghĩa “xong” cho v1 (Definition of Done)

- [x] User đăng ký/đăng nhập; password hash (không lưu plain text)
- [x] Deposit/withdraw cập nhật balance qua SQL transaction
- [x] Đặt lệnh BUY/SELL market với cash/position đủ điều kiện
- [x] Sau khớp lệnh: ledger, trade history, position cập nhật đúng
- [x] Qt Quick UI: portfolio, ticket, working orders, mini book
- [x] Unit tests cho matching + wallet + limits
- [x] Tài liệu schema và workflow khớp code

---

## 8. Cách dùng bộ docs này khi code

1. Đọc lý thuyết liên quan trong `01-theory-foundations.md`
2. Xem schema/workflow trước khi implement feature
3. Đọc header live trong `include/` (không copy skeleton cũ)
4. Đánh checklist trong phase tương ứng
5. Chỉ sang phase sau khi DoD phase hiện tại đạt ≥ 80%

---

## 9. Gợi ý học sâu song song

Trong lúc làm từng phase, hãy chủ động viết thêm note cá nhân:

- Tại sao `balance` không chỉ là một cột update “cộng trừ” tùy tiện?
- Sự khác nhau giữa **Order**, **Trade**, **Position**?
- Vì sao UI thread không được block bởi DB I/O dài?
- ACID giúp gì khi vừa khớp lệnh vừa trừ tiền?

Nếu trả lời được 4 câu trên bằng lời của bạn, bạn đang học đúng hướng.
