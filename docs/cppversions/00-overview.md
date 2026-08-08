# 00 — Overview: C++ qua các version

## 1. C++ là gì theo nghĩa “version”?

Khi nói “C++17”, người ta thường gộp 3 thứ:

1. **ISO Standard** — tài liệu chuẩn (ISO/IEC 14882:xxxx)
2. **Compiler mode** — cờ biên dịch (`-std=c++17`, `/std:c++17`)
3. **Library implementation** — libstdc++ / libc++ / MSVC STL hỗ trợ bao nhiêu feature của chuẩn đó

Một compiler có thể “biên dịch được `-std=c++20`” nhưng **chưa hỗ trợ đủ** mọi feature (ví dụ Modules, Coroutines sớm từng lệch nhau).

---

## 2. Timeline chuẩn chính

```text
1998  C++98     — chuẩn ISO đầu tiên
2003  C++03     — bugfix / kỹ thuật, gần như không “feature mới”
2011  C++11     — modern C++ (auto, move, lambda, smart ptr, thread…)
2014  C++14     — hoàn thiện C++11 (generic lambda, return type deduction…)
2017  C++17     — optional/variant/any, string_view, filesystem, structured bindings…
2020  C++20     — concepts, ranges, coroutines, modules, calendar/timezone…
2023  C++23     — expected, print, mdspan, flat_map, ranges tiếp…
2026  C++26     — đang chuẩn hóa (reflection dự kiến là highlight…)
```

Chu kỳ gần đây: khoảng **3 năm / chuẩn**.

---

## 3. “Bước nhảy” quan trọng nhất để học

| Mốc | Ý nghĩa học tập |
|-----|------------------|
| C++98/03 → C++11 | Đổi cách nghĩ: RAII + move + smart pointer thay raw `new` |
| C++11 → C++17 | API chuẩn đủ dùng cho app thực tế (optional, string_view, filesystem) |
| C++17 → C++20 | Đổi cách viết generic (concepts/ranges) và async (coroutines) |
| C++20 → C++23 | Tiện dụng hơn (`expected`, `print`), chưa phải bắt buộc với mọi dự án |

Với hầu hết desktop app (Qt + SQL) năm 2026:

- **Baseline thực dụng: C++17**
- **Nâng C++20** khi team sẵn sàng và toolchain (Qt version) ổn
- **C++23** khi compiler/Qt trong môi trường deploy đã hỗ trợ tốt

---

## 4. Bật chuẩn trong CMake / compiler

### CMake (khuyến nghị)

```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # tránh -std=gnu++17 nếu muốn portable
```

Hoặc per-target:

```cmake
target_compile_features(trading-app PRIVATE cxx_std_17)
```

### GCC / Clang

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic main.cpp
clang++ -std=c++20 main.cpp
```

### MSVC

```bat
cl /std:c++17 /EHsc /W4 main.cpp
cl /std:c++20 /EHsc main.cpp
```

### Kiểm tra macro chuẩn

```cpp
#include <iostream>
int main() {
    std::cout << __cplusplus << "\n";
    // gợi ý (xấp xỉ):
    // c++98/03: 199711L
    // c++11:    201103L
    // c++14:    201402L
    // c++17:    201703L
    // c++20:    202002L
    // c++23:    202302L  (tuỳ compiler)
}
```

> MSVC mặc định có thể không đặt `__cplusplus` đúng trừ khi bật `/Zc:__cplusplus`.

---

## 5. Feature theo “tầng” (để không bị overwhelm)

Học từng tầng, không học theo danh sách proposal:

1. **Memory & ownership**: RAII, smart pointers, move semantics
2. **Expressiveness**: `auto`, range-for, lambda, structured bindings
3. **Type safety**: `enum class`, `nullptr`, `override`, `[[nodiscard]]`
4. **Error & optional values**: `optional`, `variant`, `expected`
5. **Concurrency**: `thread`, `mutex`, `atomic`, (sau) coroutines
6. **Generic programming**: templates → `auto` params → concepts → ranges
7. **Compile-time**: `constexpr` ngày càng mạnh qua các chuẩn
8. **Modularization**: headers → (C++20) modules

---

## 6. Bảng “feature headline” cực ngắn

| Feature | 11 | 14 | 17 | 20 | 23 |
|---------|----|----|----|----|-----|
| `auto` / range-for / lambda | ✅ | ⬆ | ⬆ | ⬆ | ⬆ |
| Move semantics / `unique_ptr` | ✅ | ✅ | ✅ | ✅ | ✅ |
| `std::thread` | ✅ | ✅ | ✅ | ✅ | ✅ |
| Generic lambda / `make_unique` | — | ✅ | ✅ | ✅ | ✅ |
| `optional` / `string_view` / filesystem | — | — | ✅ | ✅ | ✅ |
| Structured bindings / `if constexpr` | — | — | ✅ | ✅ | ✅ |
| Concepts / Ranges / Coroutines / Modules | — | — | — | ✅ | ⬆ |
| `std::expected` / `std::print` / `mdspan` | — | — | — | — | ✅ |

`⬆` = được mở rộng / dùng thuận tiện hơn.

---

## 7. Compatibility & ABI (ý quan trọng)

- Nâng **-std=** trong cùng major compiler thường **source-compatible** theo hướng tiến (code cũ vẫn compile), nhưng không luôn tối ưu theo idiom mới.
- Đổi chuẩn **không tự động** đổi ABI ổn định giữa các binary đã build — vẫn phải build lại nhất quán toàn project.
- Mixing object files compile bằng chuẩn khác nhau trong một binary thường được hỗ trợ, nhưng **đừng** mix CRT/STL khác nhau.

---

## 8. Cách dùng bộ docs này

1. Đọc overview (file này)
2. Đọc C++11 kỹ (nền modern)
3. Đọc C++17 kỹ (baseline trading-app)
4. Xem [07-side-by-side.md](./07-side-by-side.md) để “cảm” sự khác
5. Làm bài tập [09-exercises.md](./09-exercises.md)
6. Khi sẵn sàng: C++20/23 theo nhu cầu

Code mẫu có thể copy ra file `.cpp` rồi compile với `-std=...` tương ứng.
