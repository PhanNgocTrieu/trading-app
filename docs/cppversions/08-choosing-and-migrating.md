# 08 — Chọn version & migrate thực tế

## 1. Quyết định nhanh

```text
Bạn đang làm gì?
├─ Học modern C++ / app mới (Qt6, desktop, 2026)
│   └─ Chọn C++17 (an toàn) hoặc C++20 (nếu toolchain OK)
├─ Library phải hỗ trợ compiler rất cũ
│   └─ C++14/11 + feature test macros
├─ Muốn expected/print/flat_map ngay
│   └─ C++23 nếu CI/compiler đồng nhất; không thì polyfill
└─ Embedded cực hạn chế
    └─ Xem toolchains; đôi khi còn C++11
```

### Cho trading-app cụ thể

| Thành phần | Gợi ý chuẩn |
|------------|-------------|
| Domain + engine | C++17+ |
| SQLite repos | C++17 (`optional`, `string_view`, `filesystem`) |
| Qt6 Widgets UI | C++17 tối thiểu; Qt 6.x hỗ trợ tốt C++17/20 |
| Devcontainer Ubuntu 24.04 | GCC đủ cho C++20/23 gần như đầy đủ |

**Khuyến nghị chốt:** `CMAKE_CXX_STANDARD 17` ngay Phase 0; nâng 20 khi bắt đầu dùng concepts/ranges có chủ đích.

---

## 2. Checklist nâng C++11 → C++17 trong repo này

Hiện trạng liên quan:

- Root `CMakeLists.txt`: `cxx_std_11`
- Root `CMakeLists.txt`: `CMAKE_CXX_STANDARD 17`

### Bước migrate

1. **Thống nhất chuẩn trong CMake** (17)
2. Bật `-Wall -Wextra -Wpedantic` (đã có)
3. Thay dần:
   - `NULL`/`0` pointer → `nullptr`
   - `typedef enum` → `enum class`
   - iterator for → range-for
   - raw owning pointer → `unique_ptr`
4. Giới thiệu `std::optional` ở repository API
5. Dùng `std::string_view` cho hàm đọc symbol/side/username (không lưu view)
6. Dùng `std::filesystem` cho path DB
7. Thêm `[[nodiscard]]` cho `placeOrder` / `deposit`

### CMake mẫu

```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# đồng bộ mọi target
target_compile_features(trading-app PRIVATE cxx_std_17)
target_compile_features(service PRIVATE cxx_std_17)
```

---

## 3. Rủi ro khi nâng chuẩn

| Rủi ro | Cách xử lý |
|--------|------------|
| Code phụ thuộc `gnu++` extensions | `CMAKE_CXX_EXTENSIONS OFF`, sửa chỗ lệch |
| Thư viện bên thứ ba yêu cầu chuẩn khác | build lại tất cả cùng chuẩn / ABI |
| Học quá nhiều feature cùng lúc | chỉ adopt theo checklist use-case |
| `string_view` dangling | quy tắc: không trả/về lưu view vào temporary |
| `filesystem` trên MinGW cũ | kiểm tra link (`-lstdc++fs` từng cần ở GCC cũ) |

---

## 4. Feature test macros (viết code portable)

```cpp
#if defined(__cpp_lib_optional)
  #include <optional>
#endif

#if defined(__cpp_lib_expected)
  #include <expected>
#endif
```

Dùng khi library phải compile trên nhiều compiler version. App nội bộ có thể chỉ yêu cầu chuẩn tối thiểu và bỏ `#if` phức tạp.

---

## 5. Mapping “đau cũ” → “thuốc mới”

| Đau | Thuốc từ chuẩn nào |
|-----|--------------------|
| Iterator type dài | C++11 `auto` / range-for |
| Leak `new` | C++11 smart ptr + C++14 `make_unique` |
| Functor dài | C++11/14 lambda |
| Trả “không có giá trị” bằng nullptr | C++17 `optional` |
| Copy string chỉ để đọc | C++17 `string_view` |
| Path string concat mong manh | C++17 `filesystem` |
| Template error khó đọc | C++20 concepts |
| Filter/map dài | C++20 ranges |
| Result type tự chế | C++23 `expected` (hoặc giữ Result ở 17) |
| Format log xấu | C++20 `format` / C++23 `print` |

---

## 6. Chiến lược học theo tuần (gợi ý)

| Tuần | Tập trung |
|------|-----------|
| 1 | C++11: auto, range-for, enum class, smart ptr, move cơ bản |
| 2 | C++11: lambda, rule of 0/5, mutex cơ bản |
| 3 | C++17: optional, string_view, structured bindings |
| 4 | C++17: filesystem, nodiscard, if-init; áp vào Auth/Wallet |
| 5 | C++20 overview: concepts + ranges đọc hiểu |
| 6 | C++23 overview: expected vs Result tự viết |

Song song implement trading-app Phase 0–1.

---

## 7. Khi nào *không* nâng chuẩn?

- Đang debug production hotfix trên toolchain đóng băng
- Dependency binary-only yêu cầu ABI/compiler cụ thể
- Team chưa thống nhất coding standard (nâng chuẩn mà style vẫn C++98 sẽ loạn)

Nâng chuẩn là **phương tiện**, không phải mục tiêu.
