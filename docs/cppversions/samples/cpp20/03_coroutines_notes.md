# Coroutines notes (C++20)

File này không compile — coroutines cần **coroutine type** (promise_type) hoặc thư viện.

## Khi nào hợp lý?

- Generator/lazy sequence dài
- Async I/O với framework hỗ trợ `co_await`
- Pipeline task trong server

## Khi nào *không* cần cho trading-app v1?

- Market tick bằng `QTimer` đã đủ
- Place order là use-case sync + SQLite local
- Thêm coroutine runtime sớm làm tăng độ phức tạp học tập

## Đọc thêm

- cppreference: Coroutines
- “C++ Coroutines TL;DR” articles
- Nếu dùng Qt: tìm adapter `QFuture` / thư viện co_await Qt (tuỳ version)

## Mental model ngắn

```text
hàm coroutine trả về object đặc biệt
  → co_await tạm dừng cho đến khi awaitable sẵn sàng
  → co_yield trả một giá trị rồi tạm dừng (generator)
  → co_return kết thúc
```
