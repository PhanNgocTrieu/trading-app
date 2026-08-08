# Phase 0 Notes — Vì sao tách `Account` khỏi `User`?

## Kết luận ngắn

`User` là **định danh / hồ sơ đăng nhập**.  
`Account` là **ví tiền (cash ledger target)** dùng cho nạp/rút/giao dịch.

Gộp `balance` vào `User` làm model sai miền nghiệp vụ và khó mở rộng.

## Vì sao tách?

1. **Đúng domain trading**  
   Một người dùng có thể có nhiều tài khoản (sau này). Auth identity ≠ wallet.

2. **Invariants khác nhau**  
   - User: username unique, password hash, profile  
   - Account: `cash >= 0`, ledger, positions, orders  

3. **Giao dịch không cần đụng password/profile**  
   `deposit/withdraw/placeOrder` thao tác `Account` (+ positions), không mutate `User`.

4. **Chuẩn bị Phase 1 (SQL)**  
   Bảng `users` và `accounts` táchnaturally; ledger gắn `account_id`.

5. **Dễ test**  
   Unit test tiền/position không phải dựng full user auth object.

## Phase 0 đã hoàn thành gì?

- C++17 thống nhất qua CMake
- Domain: `User`, `Account`, `Stock`, `Result`, `OrderSide/Type/Status`, `Session`
- `LoginService` / `BankAccountService` nhận `LoggerService&` (bỏ singleton cứng ở các service này)
- CLI: login → view account / deposit / withdraw / logout
- Test nhẹ: `phase0_account_test`

## Chưa làm (cố ý — Phase 1+)

- SQLite persistence
- Password hash
- Matching engine / orders thật
- Qt UI
