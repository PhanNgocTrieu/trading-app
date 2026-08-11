# 08 — Coding Standards & Project Conventions

## 1. Ngôn ngữ & style

- C++17 (tối thiểu)
- Tên loại: `PascalCase` (`OrderService`)
- Hàm/biến: `camelCase` hoặc `snake_case` — **chọn 1 và giữ nhất quán**
  - Khuyến nghị dần về `camelCase` cho member methods kiểu Qt, `snake_case` cho DB column
- Member variables: `m_` prefix (đang dùng trong repo) **hoặc** trailing `_` — chọn 1
- Enum: `enum class`
- Header guard hoặc `#pragma once` (thống nhất `#pragma once` cho code mới)

## 2. Tổ chức file

- 1 class chính / 1 cặp `.h/.cpp` (trừ template header-only nhỏ)
- Domain header không include Qt/SQL
- Include order:
  1. header tương ứng
  2. headers project
  3. third-party
  4. std

## 3. Error handling

- Business rule → `Result<T>` / error code
- Infrastructure bất thường → exception hoặc error code rõ
- Không `catch (...)` nuốt lỗi im lặng
- Logger ở application boundary

## 4. SQL conventions

- Keyword SQL upper-case trong migration files
- Parameterized queries only (`?` placeholders)
- Không string-concat user input vào SQL
- Mọi mutate account trong `Transaction`

## 5. Qt conventions

- `Q_OBJECT` chỉ khi cần signals/slots/properties
- `connect` dùng functor syntax (`&Class::signal`)
- Không `new` QObject mà quên parent (trừ ownership rõ ràng bằng smart ptr ngoài tree)
- UI strings có thể hardcode tiếng Việt/Anh ở v1; sau này `tr()`

## 6. Testing

- Dùng GoogleTest; hướng dẫn chạy: [testing.md](./testing.md)
- Test domain không link Qt Widgets
- Đặt tên: `Unit_Behavior` (ví dụ `AccountTest.DepositIncreasesBalance`)
- Mỗi bug production → thêm 1 regression test nếu có thể

## 7. Git

- Commit nhỏ theo use-case (`feat(wallet): persist deposit ledger`)
- Không commit `*.db`, build artifacts, secrets
- Không commit trừ khi được yêu cầu rõ

## 8. Logging levels

| Level | Khi nào |
|-------|---------|
| INFO | use-case thành công đáng quan sát |
| WARNING | reject nghiệp vụ / recoverable |
| ERROR | DB failure / invariant vỡ |

Không log password / full session token.

## 9. Documentation sync

Khi đổi schema hoặc workflow:

1. Cập nhật `04-database-design.md` hoặc `05-workflows.md`
2. Cập nhật code mẫu nếu API đổi lớn
3. Giữ ADR ngắn trong `02-system-architecture.md` nếu quyết định mới
