# Samples — C++ versions

Mỗi file có thể compile độc lập (trừ ghi chú).

```bash
# ví dụ
c++ -std=c++11 -Wall -Wextra cpp11/01_auto_range_for.cpp -o /tmp/a && /tmp/a
c++ -std=c++17 -Wall -Wextra cpp17/01_optional.cpp -o /tmp/a && /tmp/a
c++ -std=c++20 -Wall -Wextra cpp20/01_concepts.cpp -o /tmp/a && /tmp/a
```

Trên macOS Apple Clang, dùng `clang++` tương đương. Với C++23, thử `-std=c++23` hoặc `-std=c++2b`.

| Thư mục | Chuẩn |
|---------|-------|
| `cpp11/` | C++11 |
| `cpp14/` | C++14 |
| `cpp17/` | C++17 |
| `cpp20/` | C++20 |
| `cpp23/` | C++23 |
| `comparisons/` | cùng bài toán, nhiều `#if __cplusplus` |
