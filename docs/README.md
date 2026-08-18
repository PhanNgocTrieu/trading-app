# Trading App — Tài liệu kỹ thuật

Bộ tài liệu này hướng dẫn **trading-app**: **C++17**, **Qt 6 Quick**, **SQLite** paper trading. Code sống nằm trong `include/`, `src/`, `apps/desktop/`.

Mục tiêu không chỉ là “làm chạy được”, mà còn giúp bạn **đào sâu**: hiểu domain trading, kiến trúc phần mềm, lifecycle Qt, transaction SQL, và cách triển khai từng giai đoạn có kiểm soát.

---

## Lộ trình đọc đề xuất

| Thứ tự | Tài liệu | Nội dung |
|--------|----------|----------|
| 1 | [00-overview.md](./00-overview.md) | Tầm nhìn sản phẩm, phạm vi, tech stack, hiện trạng repo |
| 2 | [01-theory-foundations.md](./01-theory-foundations.md) | Lý thuyết nền: trading, C++ modern, Qt, SQL, design patterns |
| 3 | [02-system-architecture.md](./02-system-architecture.md) | Kiến trúc tổng thể (layered / hexagonal-lite) |
| 4 | [03-domain-model.md](./03-domain-model.md) | Domain model: User, Account, Order, Position, Trade |
| 5 | [04-database-design.md](./04-database-design.md) | Schema SQL, index, transaction, migration |
| 6 | [05-workflows.md](./05-workflows.md) | Workflow nghiệp vụ & kỹ thuật |
| 7 | [06-qt-ui-architecture.md](./06-qt-ui-architecture.md) | Qt UI: signals/slots, MVC/MVVM-lite, threading |
| 8 | [07-implementation-plan.md](./07-implementation-plan.md) | Kế hoạch theo giai đoạn + checklist |
| 9 | [08-coding-standards.md](./08-coding-standards.md) | Cấu trúc thư mục, chuẩn code, testing |
| 10 | [09-security-and-risk.md](./09-security-and-risk.md) | Bảo mật, rủi ro giao dịch, audit |
| 11 | [10-glossary-and-exercises.md](./10-glossary-and-exercises.md) | Glossary + bài tập đào sâu |
| — | [diagrams/README.md](./diagrams/README.md) | Mermaid: architecture, class, component, sequence, use case |
| — | [cppversions/README.md](./cppversions/README.md) | Sự khác nhau giữa các version C++ + code mẫu |
| — | [phase0-notes.md](./phase0-notes.md) | Phase 0: vì sao tách Account khỏi User |
| — | [phase1-notes.md](./phase1-notes.md) | Phase 1: SQLite auth + wallet |
| — | [phase5-notes.md](./phase5-notes.md) | Phase 5: QML bridge + UI map |
| — | [phase6-notes.md](./phase6-notes.md) | Phase 6: limit orders + mini book |
| — | [testing.md](./testing.md) | **How to build & run GoogleTest** (by phase / filter) |
| — | [project-structure.md](./project-structure.md) | Cấu trúc thư mục hiện tại |

Code live nằm trong `include/`, `src/`, `apps/desktop/`. Sơ đồ: [diagrams/README.md](./diagrams/README.md).

---

## Bản đồ thư mục docs

```text
docs/
├── README.md
├── 00-overview.md … 10-glossary-and-exercises.md
├── testing.md
├── project-structure.md
├── phase0-notes.md / phase1-notes.md / phase5-notes.md / phase6-notes.md
├── diagrams/          # architecture, class, components, sequence, use-cases
├── cppversions/
```

---

## Nguyên tắc thiết kế tài liệu

1. **Học bằng làm**: mỗi giai đoạn có lý thuyết ngắn → thiết kế → code mẫu → bài tập tự kiểm.
2. **Tách UI khỏi nghiệp vụ**: Qt chỉ là presentation; domain/engine không phụ thuộc Qt.
3. **SQL là nguồn sự thật**: số dư, lệnh, vị thế phải nhất quán nhờ transaction.
4. **Paper trading trước**: giả lập khớp lệnh nội bộ trước khi nghĩ tới broker thật.
5. **Code sống thắng skeleton**: đọc `include/` và `apps/desktop/`, không revive `service/` hay Widgets windows.

---

## Trạng thái hiện tại (tóm tắt)

- **Phase 0–6 done**: domain + SQLite + paper trading + mock feed + Qt Quick UI + resting limits
- Targets: `trading-app` (QML desktop)
- Cấu trúc code: [project-structure.md](./project-structure.md)
- Phase 5 notes: [phase5-notes.md](./phase5-notes.md)
- Phase 6 notes: [phase6-notes.md](./phase6-notes.md)
- Chạy test: [testing.md](./testing.md)

True CLOB / fees / Postgres vẫn là mở rộng tuỳ chọn trong [07-implementation-plan.md](./07-implementation-plan.md).
