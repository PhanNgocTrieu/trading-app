# 07 — Side-by-side: cùng một bài toán, nhiều version

Mục tiêu: nhìn thấy **cùng ý định nghiệp vụ** được diễn đạt khác nhau thế nào.

Code đầy đủ: `samples/comparisons/`.

---

## Bài toán A — Tìm user theo tên

### C++98 tinh thần

```cpp
User* findUser(std::vector<User>& users, const std::string& name) {
    for (std::size_t i = 0; i < users.size(); ++i) {
        if (users[i].name() == name) return &users[i];
    }
    return 0; // hoặc NULL
}
```

### C++11

```cpp
User* findUser(std::vector<User>& users, const std::string& name) {
    for (auto& u : users) {
        if (u.name() == name) return &u;
    }
    return nullptr;
}
```

### C++17 (khuyến nghị)

```cpp
std::optional<User> findUser(const std::vector<User>& users, std::string_view name) {
    for (const auto& u : users) {
        if (u.name() == name) return u; // hoặc trả reference_wrapper / index tuỳ design
    }
    return std::nullopt;
}
```

### C++23

```cpp
std::expected<User, FindError> findUser(const std::vector<User>& users, std::string_view name) {
    for (const auto& u : users) {
        if (u.name() == name) return u;
    }
    return std::unexpected{FindError::NotFound};
}
```

**Bài học:** từ “con trỏ có thể null” → “optional” → “expected có lý do lỗi”.

---

## Bài toán B — Lọc lệnh FILLED và lấy symbol

### C++11

```cpp
std::vector<std::string> filledSymbols(const std::vector<Order>& orders) {
    std::vector<std::string> out;
    for (const auto& o : orders) {
        if (o.status == OrderStatus::Filled) out.push_back(o.symbol);
    }
    return out;
}
```

### C++20 Ranges

```cpp
namespace rv = std::views;
auto filledSymbols = orders
    | rv::filter([](const Order& o){ return o.status == OrderStatus::Filled; })
    | rv::transform([](const Order& o){ return o.symbol; });
// materialize nếu cần:
std::vector<std::string> out(filledSymbols.begin(), filledSymbols.end());
```

**Bài học:** ranges giảm boilerplate khi chuỗi filter/transform dài; với 5–10 dòng, vòng `for` C++11 vẫn rất tốt.

---

## Bài toán C — Ownership service

### Raw (dễ sai)

```cpp
MatchingEngine* engine = new MatchingEngine();
// ... nhiều đường return
delete engine;
```

### C++11 `unique_ptr`

```cpp
auto engine = std::unique_ptr<MatchingEngine>(new MatchingEngine());
```

### C++14+

```cpp
auto engine = std::make_unique<MatchingEngine>();
```

---

## Bài toán D — In thông tin khớp lệnh

### iostream (mọi chuẩn)

```cpp
std::cout << "Filled " << symbol << " x " << qty << " @ " << price << "\n";
```

### C++20 format

```cpp
std::cout << std::format("Filled {} x {} @ {:.2f}\n", symbol, qty, price);
```

### C++23 print

```cpp
std::println("Filled {} x {} @ {:.2f}", symbol, qty, price);
```

---

## Bài toán E — Enum status

### C typedef enum (repo hiện tại)

```cpp
typedef enum LOGIN_STATUS {
    LOGIN_SUCCESS,
    LOGIN_FAILURE
} LOGIN_STATUS;
```

### C++11+

```cpp
enum class LoginStatus { Success, Failure };
```

### C++23 lấy underlying

```cpp
auto code = std::to_underlying(LoginStatus::Success);
```

---

## Bảng “nên viết kiểu nào” năm 2026

| Tình huống | Ưu tiên |
|------------|---------|
| App Qt desktop mới | C++17 idiom |
| Optional find | `std::optional` |
| Business error có payload | `expected` (23) hoặc `Result` tự viết (17) |
| Tham số chuỗi read-only | `string_view` |
| Ownership đơn | `unique_ptr` + `make_unique` |
| Pipeline collection phức tạp | ranges (20) hoặc for rõ ràng |
| Logging đơn giản | `format`/`print` hoặc logger sẵn có |

File chạy được: `samples/comparisons/find_user_evolution.cpp` (các nhánh `#if __cplusplus`).
