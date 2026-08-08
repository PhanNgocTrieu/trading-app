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
| UI | Qt Widgets (sau này có thể mở rộng QML) |
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
| UI / Platform | Qt 6 (Widgets trước) | Cross-platform, signals/slots, tooling mạnh, phù hợp desktop trading |
| Build | CMake | Đã dùng trong repo; chuẩn công nghiệp |
| DB | SQLite (Qt SQL / sqlite3) | Zero-ops cho desktop; dễ học transaction |
| Logging | spdlog hoặc logger tự viết (bắt đầu từ `LoggerService`) | Quan sát hành vi hệ thống |
| Test | Catch2 hoặc GoogleTest | Unit test domain/engine |
| Container | Docker / Dev Container | Môi trường build ổn định (đã có `.devcontainer`) |

---

## 4. Hiện trạng repo (baseline)

Cấu trúc hiện tại (đơn giản hóa):

```text
trading-app/
├── ui/                 # Console UI (main, dashboard)
├── include/            # Domain headers (User, Stock)
├── src/                # Domain implementations
├── service/            # Login, Bank, ServiceAPI, Logger
├── sql/                # Dự kiến SQL service (đang trống)
├── db/                 # Dự kiến DB layer (đang trống)
├── CMakeLists.txt
└── Dockerfile
```

Điểm mạnh đã có:

- Tách `service` khỏi `ui` / `domain` (ý tưởng đúng hướng)
- Singleton services (tạm ổn cho prototype; sau sẽ refactor DI)
- Status enums cho login/account/transaction
- Devcontainer + CMake sẵn sàng mở rộng

Hạn chế cần khắc phục theo giai đoạn:

- UI console → Qt
- Chưa có persistence thật
- Domain model chưa đủ cho trading
- Service còn stub
- Thiếu transaction / audit / order state machine

---

## 5. Mục tiêu kiến trúc (North Star)

```text
┌─────────────────────────────────────────────────────────┐
│                     Qt Presentation                      │
│   (Views / ViewModels / Controllers + signals/slots)     │
└───────────────────────────┬─────────────────────────────┘
                            │ Application Services
┌───────────────────────────▼─────────────────────────────┐
│              Application / Use-case Layer                │
│   AuthService, WalletService, OrderService, Portfolio    │
└───────────────────────────┬─────────────────────────────┘
                            │
┌───────────────────────────▼─────────────────────────────┐
│                    Domain + Engine                       │
│   User, Account, Order, Position, MatchingEngine         │
└───────────────────────────┬─────────────────────────────┘
                            │ Repository interfaces
┌───────────────────────────▼─────────────────────────────┐
│                 Infrastructure (SQL / IO)                │
│   SQLite repositories, MarketDataFeed (mock), Logger     │
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

- [ ] User đăng ký/đăng nhập; password hash (không lưu plain text)
- [ ] Deposit/withdraw cập nhật balance qua SQL transaction
- [ ] Đặt lệnh BUY/SELL market với cash/position đủ điều kiện
- [ ] Sau khớp lệnh: ledger, trade history, position cập nhật đúng
- [ ] Qt UI hiển thị portfolio + order history
- [ ] Có unit tests cho matching + wallet
- [ ] Có tài liệu schema và workflow khớp code

---

## 8. Cách dùng bộ docs này khi code

1. Đọc lý thuyết liên quan trong `01-theory-foundations.md`
2. Xem schema/workflow trước khi implement feature
3. Copy code mẫu từ `samples/phaseX/` rồi adapt vào repo
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
