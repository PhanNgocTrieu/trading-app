# 04 — Thiết kế cơ sở dữ liệu (SQL)

## 1. Nguyên tắc

1. **Ledger là nguồn audit** cho tiền
2. Mọi thay đổi trading quan trọng nằm trong **transaction**
3. Schema versioning từ ngày đầu
4. Index theo access path thực tế (username, account_id, symbol)
5. Không lưu password plain text

Engine: **SQLite** cho v1 desktop.

---

## 2. ER Diagram (logical)

```text
users 1──1 accounts
accounts 1──* orders
accounts 1──* trades
accounts 1──* positions
accounts 1──* ledger_entries
stocks 1──* orders
stocks 1──* trades
stocks 1──* positions
stocks 1──1 market_quotes
```

---

## 3. Schema SQL (Phase 1–2)

File đề xuất: `src/infrastructure/db/migrations/001_init.sql`

```sql
PRAGMA foreign_keys = ON;

CREATE TABLE schema_version (
    version     INTEGER NOT NULL,
    applied_at  TEXT    NOT NULL DEFAULT (datetime('now'))
);

CREATE TABLE users (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    username        TEXT    NOT NULL UNIQUE,
    password_hash   TEXT    NOT NULL,
    phone_number    TEXT    NOT NULL DEFAULT '',
    created_at      TEXT    NOT NULL DEFAULT (datetime('now'))
);

CREATE TABLE accounts (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id         INTEGER NOT NULL UNIQUE,
    cash_balance    REAL    NOT NULL DEFAULT 0 CHECK (cash_balance >= 0),
    currency        TEXT    NOT NULL DEFAULT 'USD',
    status          TEXT    NOT NULL DEFAULT 'ACTIVE'
                    CHECK (status IN ('ACTIVE', 'FROZEN')),
    created_at      TEXT    NOT NULL DEFAULT (datetime('now')),
    FOREIGN KEY (user_id) REFERENCES users(id)
);

CREATE TABLE stocks (
    symbol          TEXT PRIMARY KEY,
    name            TEXT NOT NULL,
    is_active       INTEGER NOT NULL DEFAULT 1
);

CREATE TABLE market_quotes (
    symbol          TEXT PRIMARY KEY,
    last_price      REAL NOT NULL CHECK (last_price > 0),
    updated_at      TEXT NOT NULL DEFAULT (datetime('now')),
    FOREIGN KEY (symbol) REFERENCES stocks(symbol)
);

CREATE TABLE orders (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    account_id      INTEGER NOT NULL,
    symbol          TEXT    NOT NULL,
    side            TEXT    NOT NULL CHECK (side IN ('BUY', 'SELL')),
    type            TEXT    NOT NULL CHECK (type IN ('MARKET', 'LIMIT')),
    quantity        INTEGER NOT NULL CHECK (quantity > 0),
    limit_price     REAL,
    status          TEXT    NOT NULL CHECK (status IN ('PENDING','FILLED','REJECTED','CANCELED')),
    created_at      TEXT    NOT NULL DEFAULT (datetime('now')),
    updated_at      TEXT    NOT NULL DEFAULT (datetime('now')),
    FOREIGN KEY (account_id) REFERENCES accounts(id),
    FOREIGN KEY (symbol) REFERENCES stocks(symbol)
);

CREATE TABLE trades (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    order_id        INTEGER NOT NULL,
    account_id      INTEGER NOT NULL,
    symbol          TEXT    NOT NULL,
    side            TEXT    NOT NULL CHECK (side IN ('BUY', 'SELL')),
    quantity        INTEGER NOT NULL CHECK (quantity > 0),
    price           REAL    NOT NULL CHECK (price > 0),
    executed_at     TEXT    NOT NULL DEFAULT (datetime('now')),
    FOREIGN KEY (order_id) REFERENCES orders(id),
    FOREIGN KEY (account_id) REFERENCES accounts(id),
    FOREIGN KEY (symbol) REFERENCES stocks(symbol)
);

CREATE TABLE positions (
    account_id      INTEGER NOT NULL,
    symbol          TEXT    NOT NULL,
    quantity        INTEGER NOT NULL CHECK (quantity >= 0),
    avg_cost        REAL    NOT NULL CHECK (avg_cost >= 0),
    updated_at      TEXT    NOT NULL DEFAULT (datetime('now')),
    PRIMARY KEY (account_id, symbol),
    FOREIGN KEY (account_id) REFERENCES accounts(id),
    FOREIGN KEY (symbol) REFERENCES stocks(symbol)
);

CREATE TABLE ledger_entries (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    account_id      INTEGER NOT NULL,
    type            TEXT    NOT NULL
                    CHECK (type IN ('DEPOSIT','WITHDRAW','BUY','SELL','FEE','ADJUSTMENT')),
    amount          REAL    NOT NULL, -- signed
    balance_after   REAL    NOT NULL,
    ref_type        TEXT,
    ref_id          INTEGER,
    note            TEXT    NOT NULL DEFAULT '',
    created_at      TEXT    NOT NULL DEFAULT (datetime('now')),
    FOREIGN KEY (account_id) REFERENCES accounts(id)
);

CREATE INDEX idx_orders_account_created ON orders(account_id, created_at DESC);
CREATE INDEX idx_trades_account_executed ON trades(account_id, executed_at DESC);
CREATE INDEX idx_ledger_account_created ON ledger_entries(account_id, created_at DESC);
```

Seed mẫu:

```sql
INSERT INTO stocks(symbol, name) VALUES
 ('AAPL', 'Apple Inc.'),
 ('MSFT', 'Microsoft Corp.'),
 ('TSLA', 'Tesla Inc.');

INSERT INTO market_quotes(symbol, last_price) VALUES
 ('AAPL', 190.0),
 ('MSFT', 420.0),
 ('TSLA', 250.0);

INSERT INTO schema_version(version) VALUES (1);
```

---

## 4. Transaction mẫu: Market BUY

Pseudo-SQL trong một transaction:

```sql
BEGIN IMMEDIATE;

-- 1) lock/read account
SELECT cash_balance FROM accounts WHERE id = ?;

-- 2) read quote
SELECT last_price FROM market_quotes WHERE symbol = ?;

-- 3) validate cash >= qty * price

-- 4) insert order PENDING
INSERT INTO orders(... status='PENDING' ...);

-- 5) update cash
UPDATE accounts SET cash_balance = cash_balance - ? WHERE id = ?;

-- 6) upsert position
INSERT INTO positions(account_id, symbol, quantity, avg_cost)
VALUES (?, ?, ?, ?)
ON CONFLICT(account_id, symbol) DO UPDATE SET
  avg_cost = (positions.quantity * positions.avg_cost + excluded.quantity * excluded.avg_cost)
             / (positions.quantity + excluded.quantity),
  quantity = positions.quantity + excluded.quantity,
  updated_at = datetime('now');

-- 7) insert trade
INSERT INTO trades(...);

-- 8) update order FILLED
UPDATE orders SET status='FILLED', updated_at=datetime('now') WHERE id=?;

-- 9) ledger
INSERT INTO ledger_entries(account_id, type, amount, balance_after, ref_type, ref_id)
VALUES (?, 'BUY', -cost, new_balance, 'order', order_id);

COMMIT;
```

Nếu bất kỳ bước validate fail: `ROLLBACK` và ghi order `REJECTED` (hoặc không tạo order — chọn 1 policy và giữ nhất quán).

**Policy khuyến nghị:** vẫn tạo order `REJECTED` kèm note để audit hành vi user.

---

## 5. Repository interfaces

```cpp
class IAccountRepository {
public:
    virtual ~IAccountRepository() = default;
    virtual std::optional<Account> findByUserId(int64_t userId) = 0;
    virtual void updateCash(int64_t accountId, double newBalance) = 0;
};

class IOrderRepository {
public:
    virtual ~IOrderRepository() = default;
    virtual int64_t insert(const Order& order) = 0;
    virtual void updateStatus(int64_t orderId, OrderStatus status) = 0;
    virtual std::vector<Order> listByAccount(int64_t accountId, int limit) = 0;
};
```

Implementation SQL không lộ ra khỏi `infrastructure/`.

---

## 6. Migration runner (ý tưởng)

```cpp
int currentVersion(SqliteConnection& db);
void applyMigration(SqliteConnection& db, int version, const std::string& sql);

void runMigrations(SqliteConnection& db) {
    const int target = 1;
    int v = currentVersion(db);
    if (v < 1) applyMigration(db, 1, readFile("001_init.sql"));
}
```

---

## 7. Truy vấn UI thường dùng

### Portfolio positions + market value

```sql
SELECT p.symbol,
       p.quantity,
       p.avg_cost,
       q.last_price,
       (p.quantity * q.last_price) AS market_value,
       ((q.last_price - p.avg_cost) * p.quantity) AS unrealized_pnl
FROM positions p
JOIN market_quotes q ON q.symbol = p.symbol
WHERE p.account_id = ?
  AND p.quantity > 0
ORDER BY p.symbol;
```

### Equity

```text
equity = cash_balance + sum(quantity * last_price)
```

### Order history

```sql
SELECT id, symbol, side, type, quantity, limit_price, status, created_at
FROM orders
WHERE account_id = ?
ORDER BY created_at DESC
LIMIT 100;
```

---

## 8. Password hashing

Không tự invent crypto nếu chưa cần. Gợi ý học tập:

- Phase 1 (learning): `SHA-256(salt + password)` với salt random (chấp nhận được cho app học)
- Tốt hơn: Argon2 / bcrypt qua lib

Bảng chỉ lưu `password_hash` (và có thể tách `salt` nếu hash format chưa nhúng salt).

---

## 9. Double vs Integer cents

| | double REAL | integer cents |
|--|-------------|---------------|
| Dễ code | ✅ | hơi verbose |
| Sai số | có thể | ✅ chính xác |
| Thực tế finance | hạn chế | phổ biến |

Khuyến nghị lộ trình:

1. Phase 1–3: `REAL` để đi nhanh + hiểu flow
2. Phase 4: migrate `cash_balance_cents INTEGER`, cập nhật domain `Money`

---

## 10. Backup & file DB

SQLite file ví dụ: `~/.trading-app/trading.db`

- Không commit file DB vào git
- Có thể thêm command “Export SQL dump”
- `.gitignore` nên có `*.db`

---

## 11. Kiểm thử dữ liệu (manual SQL)

Sau một vòng deposit 1000 + buy 10 AAPL @ 190:

```sql
SELECT cash_balance FROM accounts WHERE id = 1;
-- expect 1000 - 1900 = negative? => phải reject nếu chỉ có 1000

-- với deposit 5000 rồi buy:
-- cash = 5000 - 1900 = 3100
SELECT * FROM positions WHERE account_id = 1;
SELECT * FROM ledger_entries WHERE account_id = 1 ORDER BY id;
SELECT * FROM trades WHERE account_id = 1;
```

Hãy tự đối soát tay trước khi tin UI.
