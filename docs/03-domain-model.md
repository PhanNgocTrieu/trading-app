# 03 — Domain Model

Live types: `include/domain/*.hpp`. Logged-in ids live in `AuthSession` (`include/application/ports.hpp`), not a process-global session.

## 1. Mục tiêu

Định nghĩa ngôn ngữ chung (ubiquitous language) và cấu trúc đối tượng nghiệp vụ. Code domain phải đọc được gần như đặc tả.

---

## 2. Tổng quan thực thể

```text
User 1──1 Account 1──* Order
                 │
                 ├──* Trade
                 ├──* Position (per symbol)
                 └──* LedgerEntry

Stock (instrument) 1──* Order
                   1──* Trade
                   1──* Position
                   1──1 MarketQuote (runtime / table)
```

---

## 3. Entities & Value Objects

### 3.1. User

Thông tin định danh / đăng nhập.

| Field | Type | Ghi chú |
|-------|------|---------|
| id | int64 | PK |
| username | string | unique |
| passwordHash | string | không lưu plain |
| phoneNumber | string | optional |
| createdAt | datetime | |

Liên hệ code hiện tại: `include/user.h` đang giữ `m_passWord` plain — sẽ thay bằng hash.

### 3.2. Account (Wallet + Trading account)

Với v1: **1 user = 1 cash account**.

| Field | Type | Ghi chú |
|-------|------|---------|
| id | int64 | PK |
| userId | int64 | FK |
| cashBalance | decimal/double | cached balance |
| currency | string | `USD` / `VND` |
| status | enum | Active/Frozen |

> Học sâu: về lâu dài nên dùng **integer cents** (`int64`) thay `double` để tránh lỗi làm tròn. Có thể bắt đầu `double` ở Phase 1, migrate sang cents ở Phase 4.

### 3.3. Stock (Instrument)

| Field | Type |
|-------|------|
| symbol | string (PK) |
| name | string |
| isActive | bool |

Giá **không** nên là field cố định duy nhất trong entity lâu dài — giá là market data thay đổi theo thời gian.

### 3.4. MarketQuote

| Field | Type |
|-------|------|
| symbol | string |
| lastPrice | double/cents |
| updatedAt | datetime |

### 3.5. Order

| Field | Type | Ghi chú |
|-------|------|---------|
| id | int64 | |
| accountId | int64 | |
| symbol | string | |
| side | enum | Buy / Sell |
| type | enum | Market / Limit |
| quantity | int | > 0 |
| limitPrice | optional | |
| status | enum | Pending/Filled/Rejected/Canceled |
| createdAt | datetime | |
| updatedAt | datetime | |

### 3.6. Trade (Fill)

| Field | Type |
|-------|------|
| id | int64 |
| orderId | int64 |
| accountId | int64 |
| symbol | string |
| side | enum |
| quantity | int |
| price | double/cents |
| executedAt | datetime |

### 3.7. Position

| Field | Type |
|-------|------|
| accountId | int64 |
| symbol | string |
| quantity | int |
| avgCost | double/cents |
| updatedAt | datetime |

Primary key gợi ý: `(accountId, symbol)`.

### 3.8. LedgerEntry

| Field | Type | Ghi chú |
|-------|------|---------|
| id | int64 | |
| accountId | int64 | |
| type | enum | Deposit/Withdraw/Buy/Sell/Fee |
| amount | signed money | + vào, - ra |
| balanceAfter | money | snapshot |
| refType | string | `order` / `manual` |
| refId | optional int64 | |
| note | string | |
| createdAt | datetime | |

---

## 4. Enums chuẩn

```cpp
enum class OrderSide { Buy, Sell };
enum class OrderType { Market, Limit };
enum class OrderStatus { Pending, Filled, Rejected, Canceled };
enum class LedgerType { Deposit, Withdraw, Buy, Sell, Fee, Adjustment };
```

Enums dùng `enum class` (`OrderSide`, `OrderStatus`, `LedgerType`, `ErrorCode`).

---

## 5. Use-case rules (domain logic)

### 5.1. Deposit

```text
pre: amount > 0, account active
post: cash += amount
      ledger(+amount, Deposit)
```

### 5.2. Withdraw

```text
pre: amount > 0, cash >= amount
post: cash -= amount
      ledger(-amount, Withdraw)
```

### 5.3. Place Market BUY

```text
cost = qty * marketPrice
pre: qty > 0, cash >= cost
steps:
  create Order(Pending)
  match → fillPrice, fillQty
  cash -= cost
  upsert Position (increase qty, recompute avgCost)
  create Trade
  Order → Filled
  ledger(-cost, Buy, ref=orderId)
```

### 5.4. Place Market SELL

```text
pre: qty > 0, position.qty >= qty
proceeds = qty * marketPrice
steps:
  create Order(Pending)
  match → fill
  cash += proceeds
  decrease Position (if qty==0 có thể giữ row 0 hoặc xoá)
  create Trade
  Order → Filled
  ledger(+proceeds, Sell, ref=orderId)
  realizedPnL += (sellPrice - avgCost) * qty
```

### 5.5. Reject cases

- Symbol không tồn tại / inactive
- Giá thị trường không hợp lệ (`<= 0`)
- Insufficient funds / position
- Account frozen
- Quantity không nguyên dương

---

## 6. Skeleton C++ (đích)

```cpp
// domain/money.hpp (phase 4 có thể chuyển cents)
#pragma once
#include <cmath>
#include <stdexcept>

class Money {
public:
    explicit Money(double v = 0.0) : value_(v) {
        if (std::isnan(v) || std::isinf(v)) throw std::invalid_argument("invalid money");
    }
    double value() const { return value_; }
    Money operator+(Money o) const { return Money{value_ + o.value_}; }
    Money operator-(Money o) const { return Money{value_ - o.value_}; }
    bool operator>=(Money o) const { return value_ >= o.value_ - 1e-9; }
private:
    double value_;
};
```

```cpp
// domain/position.hpp
#pragma once
#include "money.hpp"
#include <string>

class Position {
public:
    Position(std::string symbol, int qty, Money avgCost)
        : symbol_(std::move(symbol)), qty_(qty), avgCost_(avgCost) {}

    const std::string& symbol() const { return symbol_; }
    int quantity() const { return qty_; }
    Money avgCost() const { return avgCost_; }

    void applyBuy(int qty, Money price) {
        if (qty <= 0) throw std::invalid_argument("qty");
        const double oldNotional = qty_ * avgCost_.value();
        const double addNotional = qty * price.value();
        qty_ += qty;
        avgCost_ = Money{(oldNotional + addNotional) / qty_};
    }

    Money applySell(int qty, Money price) {
        if (qty <= 0 || qty > qty_) throw std::invalid_argument("qty");
        Money realized{(price.value() - avgCost_.value()) * qty};
        qty_ -= qty;
        return realized; // PnL
    }

private:
    std::string symbol_;
    int qty_{0};
    Money avgCost_{0};
};
```

---

## 7. Aggregate boundary (hiểu để thiết kế transaction)

**Account Aggregate** (logic):

- Account cash
- Positions của account
- Orders/Trades/Ledger liên quan khi mutate

Trong implementation SQL, aggregate được bảo vệ bằng **một transaction** trên `account_id` (và optional `BEGIN IMMEDIATE` ở SQLite).

---

## 8. Tách biệt Domain vs DTO

| Domain | DTO (cho UI) |
|--------|----------------|
| Có behavior (`applyBuy`) | Chủ yếu data |
| Bảo vệ invariant | Có thể flat/denormalized |
| Không biết Qt | Có thể dùng `QString` ở biên UI |

Ví dụ Portfolio DTO:

```cpp
struct PositionDto {
    std::string symbol;
    int quantity;
    double avgCost;
    double marketPrice;
    double marketValue;
    double unrealizedPnl;
};
```

---

## 9. Tiến hóa từ class hiện tại

### User (hiện tại → đích)

Hiện tại:

```cpp
class User {
  int m_id;
  std::string m_userName;
  std::string m_passWord;
  std::string m_phoneNumber;
  double m_balance; // ❌ balance nên thuộc Account
};
```

Đích:

- `User` không giữ `balance`
- `Account` giữ cash
- Password chỉ là hash

### Stock

Giữ symbol/name; tách `updatePrice` sang `MarketQuote` / feed.

---

## 10. Bài tập domain (không cần UI)

1. Mua 10 @ 100, mua thêm 10 @ 120 → avg = 110, qty = 20
2. Bán 5 @ 130 → realized = 100, qty = 15, avg vẫn 110
3. Thử bán 100 → phải fail
4. BUY khi cash thiếu → fail trước khi tạo trade

Implement các case này thành unit tests ở Phase 2.
