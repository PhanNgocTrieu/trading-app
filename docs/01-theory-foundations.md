# 01 — Nền tảng lý thuyết

Tài liệu này cung cấp “vốn lý thuyết” cần có trước và trong lúc implement. Đọc theo chủ đề; không cần thuộc lòng mọi thứ ngay.

---

## A. Domain Trading (Paper Trading)

### A.1. Các khái niệm cốt lõi

| Khái niệm | Ý nghĩa | Ví dụ |
|-----------|---------|--------|
| **Instrument / Stock** | Tài sản giao dịch | `AAPL`, giá 190.5 |
| **Cash / Wallet** | Tiền mặt khả dụng | Balance = 10,000 |
| **Order** | Ý định mua/bán | BUY 10 AAPL @ market |
| **Trade / Fill** | Kết quả khớp lệnh | Đã mua 10 @ 190.5 |
| **Position** | Số lượng đang nắm giữ | Long 10 AAPL, avg 190.5 |
| **Ledger** | Sổ cái thay đổi tiền | Deposit +1000, Buy -1905 |
| **PnL** | Lãi/lỗ | Unrealized / Realized |

### A.2. Order lifecycle (state machine)

```text
CREATED → VALIDATED → ACCEPTED → PARTIALLY_FILLED → FILLED
                ↘ REJECTED          ↘ CANCELED
```

Với paper trading MVP, có thể rút gọn:

```text
PENDING → FILLED
       ↘ REJECTED / CANCELED
```

**Bài học kiến trúc:** trạng thái lệnh phải là enum rõ ràng; không dùng “magic string” rải rác trong UI.

### A.3. Market order vs Limit order

- **Market**: khớp ngay với giá hiện tại (hoặc best available trong mock book).
- **Limit**: chỉ khớp nếu giá thị trường đạt điều kiện (`BUY` nếu market ≤ limit; `SELL` nếu market ≥ limit).

MVP Phase 2: chỉ **Market**. Phase 4+: thêm Limit + order book đơn giản.

### A.4. Công thức cơ bản

**Chi phí mua (không phí):**

```text
cost = quantity * price
```

**Average cost sau khi mua thêm:**

```text
new_avg = (old_qty * old_avg + buy_qty * buy_price) / (old_qty + buy_qty)
```

**Unrealized PnL:**

```text
unrealized = (mark_price - avg_cost) * quantity
```

**Realized PnL khi bán:**

```text
realized += (sell_price - avg_cost) * sell_qty
```

### A.5. Invariants (bất biến) phải giữ

1. `cash_balance >= 0` (trừ khi cho phép margin — v1 không)
2. `position.quantity >= 0` (v1 không short)
3. Mọi thay đổi cash phải có dòng ledger
4. Không thể `FILLED` nếu không tạo được trade + cập nhật position trong cùng transaction

---

## B. C++ cho hệ thống kiểu này

### B.1. Vì sao C++?

- Kiểm soát lifetime / ownership rõ
- Hiệu năng ổn cho engine (dù paper trading chưa cần HFT)
- Hệ sinh thái Qt / native desktop mạnh
- Rèn luyện kỹ năng hệ thống (RAII, value semantics, const-correctness)

### B.2. Khái niệm cần nắm vững

| Chủ đề | Áp dụng vào project |
|--------|---------------------|
| RAII | Connection DB, transaction guard, file logger |
| Rule of 0/3/5 | Domain objects ưu tiên rule of 0 |
| `unique_ptr` / `shared_ptr` | Ownership service/repository |
| `std::optional` | Kết quả query có/không |
| `std::variant` / Result type | Success/Error không dùng exception mọi nơi |
| Enum class | OrderSide, OrderStatus, ... |
| Const-correctness | API đọc không mutate |
| Threading | UI thread vs worker (Qt) |

### B.3. Error handling khuyến nghị

Tránh “im lặng fail”. Pattern gợi ý:

```cpp
enum class ErrorCode {
    Ok,
    NotFound,
    InsufficientFunds,
    InsufficientPosition,
    ValidationFailed,
    DbError
};

template <typename T>
struct Result {
    ErrorCode code{ErrorCode::Ok};
    T value{};
    std::string message;

    bool ok() const { return code == ErrorCode::Ok; }
};
```

Hoặc dùng exception cho lỗi hạ tầng thật sự bất thường (DB corrupt), còn business rule trả `Result`.

### B.4. Nâng C++11 → C++17

Repo đang `cxx_std_11`. Khuyến nghị chuyển **C++17** sớm (Phase 0) vì:

- `std::optional`, `std::string_view`, structured bindings
- Filesystem / parallel algorithms (tuỳ nhu cầu)
- Qt 6 cũng kỳ vọng compiler hiện đại

---

## C. Qt Framework (UI + Platform)

### C.1. Qt là gì trong project này?

Qt không chỉ “vẽ nút”:

- **UI toolkit** (Widgets / QML)
- **Event loop** (`QApplication`)
- **Signals & Slots** (observer built-in)
- **Threading helpers** (`QThread`, `QtConcurrent`)
- **SQL module** (`QSqlDatabase`, `QSqlQuery`) — dùng có kiểm soát
- **Networking** (sau này cho market feed)

### C.2. Object model

Mọi `QObject`:

- Có parent-child ownership (cây object)
- Có thể emit signal / nhận slot
- Sống trong một thread affinity

```cpp
// Khi parent bị delete → children bị delete
auto* button = new QPushButton("Buy", parentWidget);
```

### C.3. Signals & Slots (trái tim Qt)

```cpp
connect(buyButton, &QPushButton::clicked,
        controller, &OrderController::onBuyClicked);
```

Lợi ích: UI không cần biết chi tiết engine; chỉ phát sự kiện.

### C.4. Quy tắc threading quan trọng

1. **Không** đụng widget từ non-UI thread
2. DB/network nặng → chạy worker, rồi signal về UI
3. Domain engine có thể chạy sync trong MVP nếu nhanh; sau đó tách thread khi có feed realtime

### C.5. Widgets vs QML

| | Widgets | QML |
|--|---------|-----|
| Phong cách | Classic desktop | Declarative, hiện đại |
| Học | Gần C++ hơn | Cần JS/QML syntax |
| Phù hợp v1 | ✅ Nên bắt đầu | Có thể Phase 5+ |

Khuyến nghị: **Qt Widgets** cho learning path rõ ràng với C++.

---

## D. SQL & Persistence

### D.1. ACID (bắt buộc hiểu)

| Thuộc tính | Ý nghĩa trong trading-app |
|------------|---------------------------|
| Atomicity | Trừ tiền + tạo trade + cập nhật position cùng thành/cùng fail |
| Consistency | Invariants không bị phá |
| Isolation | Hai thao tác không “đè” số dư sai |
| Durability | Sau commit, dữ liệu còn sau crash |

### D.2. Vì sao cần Ledger?

Nếu chỉ có cột `balance` và `UPDATE users SET balance = balance - X`:

- Khó audit
- Khó debug lệch tiền
- Khó hoàn tác / đối soát

Ledger = lịch sử sự thật; `balance` là projection (có thể derive hoặc cache có kiểm soát).

### D.3. ORM hay raw SQL?

Cho mục tiêu học sâu: **raw SQL + repository** tốt hơn ORM nặng.

- Bạn hiểu schema
- Kiểm soát transaction
- Dễ đọc EXPLAIN / index

Qt SQL hoặc sqlite3 C API đều được. Khuyến nghị:

- Phase 1: `sqlite3` hoặc `QSqlDatabase` sau abstraction
- Domain không phụ thuộc Qt SQL

### D.4. Migration mindset

Schema sẽ thay đổi. Ngay từ đầu:

```text
migrations/
  001_init.sql
  002_add_orders.sql
  ...
```

Có bảng `schema_version`.

---

## E. Kiến trúc phần mềm (patterns dùng thật)

### E.1. Layered Architecture

```text
UI → Application Services → Domain → Infrastructure
```

Dependency chỉ hướng xuống (hoặc vào abstraction).

### E.2. Repository Pattern

```cpp
class IUserRepository {
public:
    virtual ~IUserRepository() = default;
    virtual std::optional<User> findByUsername(const std::string&) = 0;
    virtual Result<UserId> insert(const User&) = 0;
};
```

SQL implementation nằm ở infrastructure.

### E.3. Use-case / Application Service

Ví dụ `PlaceOrderService`:

1. Validate input
2. Load account + position
3. Gọi domain rules / matching
4. Persist trong transaction
5. Trả DTO cho UI

### E.4. Singleton — dùng có mức độ

Prototype từng dùng Singleton (`LoginService`, …). Code sống dùng **constructor injection** qua `AppBootstrap`.

- Prototype: Singleton chấp nhận được
- Production/test: **constructor injection**

Lý do: Singleton khó fake trong unit test và che giấu dependency.

### E.5. Transaction Script vs Domain Model

- Đầu phase: Transaction Script (service làm tuần tự) vẫn ổn
- Khi rules phức tạp: đẩy invariant vào domain entities/aggregates

---

## F. Concurrency & Consistency (mức cần biết sớm)

Dù v1 single-user desktop:

1. UI event loop và DB call có thể xen kẽ
2. Mock price feed timer cập nhật giá trong khi user đặt lệnh
3. Cần mutex hoặc serialize access vào engine/account

Mô hình đơn giản khuyến nghị:

```text
Mọi thao tác mutate account đi qua một AccountService
(serialize trên một mutex hoặc một worker queue)
```

---

## G. Testing theory ngắn

| Loại | Ví dụ |
|------|--------|
| Unit | `avgCost` sau 2 lần mua |
| Unit | Reject BUY khi thiếu tiền |
| Integration | Place market order → rows trong `trades`, `positions`, `ledger` đúng |
| UI smoke | Login window mở, bấm Buy không crash |

Quy tắc: **business rules test không cần Qt**.

---

## H. Checklist tự kiểm trước Phase 1

Trả lời được các câu sau (viết ra giấy/note):

1. Order khác Trade chỗ nào?
2. Vì sao bán làm giảm position nhưng không luôn xoá row?
3. ACID giúp gì khi deposit và place order gần nhau?
4. Vì sao domain không được include `QWidget`?
5. Ledger giải quyết bài toán gì mà cột `balance` không giải quyết?

Nếu còn mơ hồ: đọc lại mục A và D trước khi code Phase 1.
