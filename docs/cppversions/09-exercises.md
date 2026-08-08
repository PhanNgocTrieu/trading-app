# 09 — Bài tập tự luyện

Làm theo thứ tự. Mỗi bài: viết file `.cpp` riêng, compile đúng `-std=`.

---

## Mức 1 — C++11 bắt buộc

1. Viết `enum class OrderSide { Buy, Sell }` + hàm `toString`.
2. Dùng range-for in danh sách symbol.
3. Tạo `unique_ptr<Account>` và gọi `deposit`.
4. Viết lambda lọc `qty >= 10` trong `vector<int>`.
5. Implement move constructor cho class sở hữu `int* data` (rồi giải thích vì sao nên chuyển sang `vector`).

**Done khi:** giải thích được khác biệt copy vs move bằng lời của bạn.

---

## Mức 2 — C++14

1. Thay mọi `unique_ptr(new T)` bằng `make_unique`.
2. Generic lambda in được cả `vector<int>` và `vector<string>`.
3. Viết `constexpr` tính lãi đơn giản và `static_assert`.

---

## Mức 3 — C++17 (quan trọng)

1. `optional<User> findByName(...)`.
2. Hàm `bool validSymbol(string_view)`.
3. Duyệt `map<string,double>` bằng structured bindings.
4. Tạo directory `~/.trading-app` bằng `filesystem`.
5. Đánh dấu `[[nodiscard]]` cho `deposit` và cố tình bỏ kết quả — xem warning.

**Done khi:** áp dụng 1–2 thay đổi tương tự vào code trading-app thật.

---

## Mức 4 — C++20

1. Concept `SymbolLike` yêu cầu convertible sang `string_view`.
2. Dùng ranges filter các giá `> 0`.
3. (Đọc) coroutine notes — viết 10 dòng giải thích khi nào *không* dùng coroutine.

---

## Mức 5 — C++23

1. Viết `expected<int, OrderError> placeBuy`.
2. Dùng `println` in kết quả.
3. So sánh design `expected` với `Result<T>` trong `docs/samples/phase0/result.hpp`.

---

## Bài tổng hợp (mini project)

Viết chương trình CLI nhỏ `paper_tiny`:

- C++17
- `unordered_map<string,double>` quotes
- `optional` find quote
- `filesystem` lưu file `quotes.txt`
- `enum class` side
- Đặt market buy giả lập trừ cash

Sau đó nâng file đó lên C++20 bằng cách viết lại phần filter symbols bằng ranges (không đổi hành vi).

---

## Tự chấm

| Câu hỏi | Có/Không |
|---------|----------|
| Tôi chọn được baseline cho trading-app và giải thích vì sao? | |
| Tôi biết ít nhất 5 feature C++11 dùng hàng ngày? | |
| Tôi tránh được dangling `string_view`? | |
| Tôi biết `optional` khác `expected` chỗ nào? | |
| Tôi biết khi nào *không* cần ranges/coroutines? | |
