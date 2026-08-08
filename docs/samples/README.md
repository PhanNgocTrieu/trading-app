# Code mẫu theo phase

Các file ở đây là **skeleton / reference** để copy-adapt vào source tree thật. Không được compile như một target độc lập (thiếu wiring đầy đủ).

## Phase 0

| File | Mục đích |
|------|----------|
| `phase0/result.hpp` | Kiểu `Result<T>` + `ErrorCode` |
| `phase0/account.hpp` | Account in-memory deposit/withdraw |
| `phase0/order_types.hpp` | Enum side/type/status |

**Cách apply:** copy vào `include/core/` hoặc `include/domain/`, sửa `#include`, nâng C++17.

## Phase 1

| File | Mục đích |
|------|----------|
| `phase1/001_init.sql` | Schema auth/wallet/ledger |
| `phase1/sqlite_connection.hpp` | Mở DB + exec |
| `phase1/transaction.hpp` | RAII BEGIN/COMMIT/ROLLBACK |
| `phase1/auth_app_service.hpp` | Register/Login use-case |

**Cách apply:** thêm `libsqlite3`, viết repository SQLite implement interfaces.

## Phase 2

| File | Mục đích |
|------|----------|
| `phase2/position.hpp` | Avg cost / sell realized |
| `phase2/matching_engine.hpp` | Market match MVP |
| `phase2/order_app_service.md` | Pseudo place order full tx |

## Phase 3

| File | Mục đích |
|------|----------|
| `phase3/login_window.*` | Login UI skeleton |
| `phase3/main_window.*` | Main trading shell |
| `phase3/CMakeLists.qt.snippet.cmake` | CMake Qt6 |

## Phase 4

| File | Mục đích |
|------|----------|
| `phase4/mock_market_feed.hpp` | QTimer random-walk prices |

## Lưu ý

1. Include path kiểu `"../phase0/result.hpp"` chỉ để đọc trong docs — khi đưa vào project hãy dùng include chuẩn (`"core/result.hpp"`).
2. Một số class có `Q_OBJECT` cần nằm trong target có `CMAKE_AUTOMOC ON`.
3. Ưu tiên hiểu và gõ lại thay vì copy nguyên xi.
