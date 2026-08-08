# 10 — Cheatsheet nhanh theo version

## C++11 — phải thuộc

```cpp
auto x = ...;
for (const auto& e : container) {}
nullptr;
enum class E { A, B };
std::unique_ptr / std::shared_ptr
std::move
[/*capture*/](args){ ... }
override / final / =default / =delete
std::thread / std::mutex / std::lock_guard
```

## C++14 — nên dùng ngay nếu ≥14

```cpp
std::make_unique<T>(...)
[](auto x){ ... }          // generic lambda
1'000'000                  // digit separators
constexpr với vòng lặp
```

## C++17 — baseline app

```cpp
std::optional<T>
std::string_view
std::filesystem::path
auto [a, b] = pairOrTuple;
if (auto x = f(); x) {}
if constexpr (...) {}
[[nodiscard]]
std::variant / std::any
```

## C++20 — khi toolchain sẵn sàng

```cpp
template <std::integral T> ...
ranges / views pipelines
std::span<T>
std::format(...)
std::jthread
<=>
concepts / requires
```

## C++23 — DX extras

```cpp
std::expected<T, E>
std::print / std::println
std::to_underlying(enumClass)
std::flat_map
import std;               // khi hỗ trợ
```

## Chọn nhanh cho trading-app

```text
Hôm nay: C++17
Sắp tới: C++20 (subset)
expected chuẩn: C++23 hoặc Result tự viết trên 17
```

## Compiler flags nhớ

```bash
-std=c++17 -Wall -Wextra -Wpedantic
-pthread   # khi dùng std::thread trên GCC
```

## CMake nhớ

```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
```
