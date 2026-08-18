# 02 — Kiến trúc hệ thống

## 1. Mục tiêu kiến trúc

Xây một ứng dụng desktop paper-trading có thể:

- Phát triển theo module độc lập
- Test domain/engine không cần UI
- Đổi SQLite → PostgreSQL sau này với chi phí thấp
- Đổi presentation (QML) mà không viết lại nghiệp vụ

Phong cách: **Layered + Hexagonal-lite** (ports & adapters đơn giản).

Mermaid (Phase 6, khớp code): [diagrams/README.md](./diagrams/README.md).

---

## 2. Sơ đồ tổng thể

```text
                     ┌──────────────────────┐
                     │   Qt Quick (QML)     │
                     │  LoginPage / Shell   │
                     │  context property    │
                     │  `app` (bridge)      │
                     └──────────┬───────────┘
                                │ Q_INVOKABLE / models
                     ┌──────────▼───────────┐
                     │  TradingAppBridge    │
                     │  Controllers         │
                     │  Table models / feed │
                     └──────────┬───────────┘
                                │ DTOs / commands
┌───────────────────────────────▼────────────────────────────────┐
│                     Application Layer                           │
│  AuthAppService | WalletAppService | OrderAppService            │
│  AppBootstrap (composition root)                                │
└───────────────────────────────┬────────────────────────────────┘
                                │ domain + ports
┌───────────────────────────────▼────────────────────────────────┐
│                        Domain + Engine                          │
│  User, Account, Position, Order enums, MatchingEngine           │
└───────────────────────────────┬────────────────────────────────┘
                                │ I*Repository
┌───────────────────────────────▼────────────────────────────────┐
│                   Infrastructure Layer                          │
│  Sqlite*Repository | Transaction | SimplePasswordHasher         │
│  MockMarketDataFeed lives in apps/desktop (Qt timer)            │
└────────────────────────────────────────────────────────────────┘
```

---

## 3. Mapping code sống (as-built)

| Concern | Live path |
|---------|-----------|
| Auth | `AuthAppService` |
| Wallet + ledger | `WalletAppService` |
| Market / limit orders, book | `OrderAppService` |
| Fill rules | `MatchingEngine` |
| Persistence | `sql/` + `Sqlite*Repository` |
| Desktop UI | `apps/desktop/` (QML + `TradingAppBridge`) |
| Tests | `tests/support/app_fixture.hpp` + `tests/phaseN/` |

Đã gỡ: console `ui/`, adapter `service/`, Qt Widgets `apps/desktop/windows/`. Không thêm lại.

Cây thư mục: [project-structure.md](./project-structure.md).

---

## 4. Trách nhiệm từng tầng

### 4.1. Presentation (Qt)

- Hiển thị trạng thái
- Thu thập input
- Không chứa business rule (ví dụ không tự trừ tiền trong slot)
- Convert UI event → command (`PlaceOrderCommand`)

### 4.2. Application

- Orchestrate use-case
- Mở/commit transaction
- Phân quyền session đơn giản
- Mapping entity → DTO cho UI

### 4.3. Domain

- Invariants
- Tính toán avg cost, PnL
- Matching rules
- Pure C++ (không Qt, không SQL)

### 4.4. Infrastructure

- SQL statements
- Hash password
- Đọc/ghi file log
- Mock/real market feed

---

## 5. Cấu trúc thư mục (as-built)

Xem [project-structure.md](./project-structure.md). Tóm tắt:

```text
include/{application,domain,engine,infrastructure}
src/{application,infrastructure/db}
apps/desktop/{bridge,controllers,models,market,qml}
tests/{support,phase0…phase6}
sql/
```

---

## 6. Luồng phụ thuộc (Dependency Rule)

```text
apps/desktop  → application → domain
apps/desktop  → infrastructure (chỉ để wiring/composition root)
application   → domain
application   → domain ports (interfaces)
infrastructure → domain (implement ports)
engine        → domain
```

**Composition Root** = `main.cpp` (hoặc `AppBootstrap`):

```cpp
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    auto db = SqliteConnection::open("trading.db");
    runMigrations(db);

    auto userRepo = std::make_shared<SqliteUserRepository>(db);
    auto ledgerRepo = std::make_shared<SqliteLedgerRepository>(db);
    // ...
    AuthAppService auth{userRepo, hasher};
    OrderAppService orders{/* repos + engine */};

    MainWindow window{auth, orders, /*...*/};
    window.show();
    return app.exec();
}
```

---

## 7. Thành phần runtime chính

### 7.1. Session Context

Sau login:

```cpp
struct Session {
    int userId;
    int accountId;
    std::string username;
};
```

Mọi use-case mutate tài sản cần `Session` hợp lệ.

### 7.2. Matching Engine

Input: `Order` + `MarketSnapshot` (+ optional order book)

Output:

```cpp
struct MatchResult {
    bool filled{false};
    double fillPrice{0};
    int fillQty{0};
    std::string rejectReason;
};
```

MVP: market order khớp 100% ngay với last price.

### 7.3. Unit of Work / Transaction Guard

```cpp
class Transaction {
public:
    explicit Transaction(SqliteConnection& db);
    void commit();
    void rollback();
    ~Transaction(); // rollback nếu chưa commit
};
```

Mọi `PlaceOrder` / `Deposit` đi qua transaction.

---

## 8. Giao tiếp UI ↔ Application

### Command (UI → App)

```cpp
struct PlaceOrderCommand {
    int accountId;
    std::string symbol;
    OrderSide side;      // Buy/Sell
    OrderType type;      // Market/Limit
    int quantity;
    std::optional<double> limitPrice;
};
```

### Query / DTO (App → UI)

```cpp
struct PortfolioDto {
    double cash;
    double equity;
    double unrealizedPnl;
    std::vector<PositionDto> positions;
};
```

UI bind vào DTO, không bind thẳng entity SQL.

---

## 9. Chiến lược đồng bộ dữ liệu lên UI

**MVP (Phase 3):**

- Sau mỗi action thành công → refresh query portfolio/orders
- Đơn giản, dễ hiểu

**Nâng cao (Phase 4+):**

- Domain events: `OrderFilled`, `BalanceChanged`
- UI subscribe và patch model cục bộ

Đừng làm event bus phức tạp quá sớm.

---

## 10. Quan hệ với Docker / Devcontainer

Docker hiện tại chỉ có toolchain GCC/CMake. Khi sang Qt:

- Thêm packages: `qt6-base-dev`, `libsqlite3-dev`, `cmake`, `ninja-build`
- Hoặc phát triển native trên macOS với Qt Online Installer / Homebrew

Hai môi trường đều ổn nếu CMake tìm được Qt (`CMAKE_PREFIX_PATH`).

---

## 11. Quyết định kiến trúc đã chốt (ADR ngắn)

| ID | Quyết định | Lý do |
|----|------------|-------|
| ADR-001 | Paper trading trước | Tập trung học hệ thống, không phụ thuộc broker |
| ADR-002 | SQLite cho desktop v1 | Đơn giản, ACID đủ dùng |
| ADR-003 | Domain độc lập Qt | Testable + portable |
| ADR-004 | Widgets trước QML | Learning curve thấp hơn với C++ |
| ADR-005 | Market order trước | Giảm độ phức tạp matching |
| ADR-006 | Ledger bắt buộc | Audit & correctness |

---

## 12. Anti-patterns cần tránh

1. Gọi SQL trực tiếp trong `QPushButton` slot
2. Giữ password plain text trong `User`
3. Update `balance` mà không ghi ledger
4. `system("clear")` trong Qt app (console habit)
5. Singleton chứa state DB connection toàn cục khó test
6. “God class” `ServiceAPI` ôm mọi thứ mãi mãi

---

## 13. Tiêu chí kiến trúc “đủ tốt”

Bạn đang đi đúng nếu:

- Thêm UI mới không phải sửa MatchingEngine
- Đổi SQLite API không phải sửa công thức PnL
- Viết unit test place-order không cần khởi tạo `QApplication`
