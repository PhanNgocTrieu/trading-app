# C++ Versions — Tài liệu tổng hợp sự khác biệt

Bộ tài liệu giúp bạn hiểu **C++ thay đổi gì qua từng chuẩn**, **feature dùng thế nào**, và **nên chọn version nào** cho dự án (điển hình: trading-app với Qt).

---

## Lộ trình đọc

| # | File | Nội dung |
|---|------|----------|
| 0 | [00-overview.md](./00-overview.md) | Timeline, compiler flags, cách chọn version |
| 1 | [01-cpp98-03.md](./01-cpp98-03.md) | Nền tảng “classic C++” |
| 2 | [02-cpp11.md](./02-cpp11.md) | Bước nhảy lớn: modern C++ bắt đầu |
| 3 | [03-cpp14.md](./03-cpp14.md) | Tinh chỉnh C++11 |
| 4 | [04-cpp17.md](./04-cpp17.md) | `optional`, `string_view`, structured bindings, filesystem… |
| 5 | [05-cpp20.md](./05-cpp20.md) | Concepts, Ranges, Coroutines, Modules, `constexpr` mạnh… |
| 6 | [06-cpp23.md](./06-cpp23.md) | `std::expected`, `std::print`, `mdspan`, ranges mở rộng… |
| 7 | [07-side-by-side.md](./07-side-by-side.md) | So sánh cùng một bài toán qua nhiều version |
| 8 | [08-choosing-and-migrating.md](./08-choosing-and-migrating.md) | Chọn version + migrate thực tế (trading-app / Qt) |
| 9 | [09-exercises.md](./09-exercises.md) | Bài tập tự luyện |
| 10 | [10-cheatsheet.md](./10-cheatsheet.md) | Cheatsheet tra cứu nhanh |

Code mẫu: [`samples/`](./samples/).

---

## Bản đồ thư mục

```text
docs/cppversions/
├── README.md
├── 00-overview.md
├── 01-cpp98-03.md … 09-exercises.md
└── samples/
    ├── cpp11/
    ├── cpp14/
    ├── cpp17/
    ├── cpp20/
    ├── cpp23/
    └── comparisons/   # cùng bài toán, nhiều version
```

---

## Quy ước trong tài liệu

- **Core language**: cú pháp / semantics của ngôn ngữ
- **Standard library**: thêm API trong `std::`
- **Code mẫu**: tập trung vào pattern dùng thật, không liệt kê 100% mọi proposal
- **Mức hỗ trợ compiler**: ghi chú GCC / Clang / MSVC mang tính định hướng; luôn kiểm tra [cppreference compiler support](https://en.cppreference.com/w/cpp/compiler_support)

---

## Liên hệ trading-app

Repo khai báo **C++17** ở root `CMakeLists.txt`.

Đọc nhanh: [08-choosing-and-migrating.md](./08-choosing-and-migrating.md).
