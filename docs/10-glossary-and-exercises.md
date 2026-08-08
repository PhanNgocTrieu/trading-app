# 10 — Glossary & Bài tập đào sâu

## 1. Glossary ngắn

| Thuật ngữ | Định nghĩa ngắn |
|-----------|-----------------|
| Paper trading | Giao dịch giả lập, không tiền thật |
| Order | Yêu cầu mua/bán |
| Fill / Trade | Phần lệnh đã khớp |
| Position | Số dư chứng khoán đang giữ |
| Avg cost | Giá vốn bình quân |
| Unrealized PnL | Lãi/lỗ chưa chốt |
| Realized PnL | Lãi/lỗ đã chốt khi bán |
| Ledger | Sổ cái các thay đổi tiền |
| Matching engine | Thành phần quyết định khớp lệnh |
| Repository | Lớp truy cập persistence |
| DTO | Data transfer object cho biên UI |
| RAII | Resource gắn với lifetime object |
| ACID | Tính chất transaction DB |
| Signal/Slot | Cơ chế quan sát sự kiện của Qt |
| Composition root | Nơi wire dependencies (`main`) |

---

## 2. Bài tập lý thuyết (không cần code)

1. Vẽ trên giấy lifecycle của 1 market BUY từ click tới row `trades`.
2. Giải thích vì sao `User.balance` là thiết kế kém hơn `Account + Ledger`.
3. So sánh Market vs Limit bằng 1 ví dụ số.
4. Nêu 3 invariant của hệ thống và cách SQL/CHECK hỗ trợ.
5. Vì sao domain không được phụ thuộc Qt?

---

## 3. Bài tập code theo phase

### Phase 0

- Viết `Account` và test tay deposit/withdraw.
- Chuyển `typedef enum` trong `status.h` sang `enum class` (migrate dần).

### Phase 1

- Implement register/login với SQLite.
- Cố tình tắt máy (kill process) giữa deposit — xác nhận không corrupt (nhờ transaction).

### Phase 2

- Implement 7 scenarios ở `05-workflows.md`.
- Thêm unit test avg cost.

### Phase 3

- Làm Login + Deposit UI trước khi làm Trade tab.
- Đảm bảo double-click Submit không tạo 2 lệnh (disable button).

### Phase 4

- Feed ±0.5%/s; quan sát uPnL thay đổi.
- Viết integration test: register → deposit → buy → sell → assert cash.

---

## 4. Câu hỏi “phỏng vấn chính mình” cuối v1

1. Nếu hai thao tác withdraw và buy xảy ra gần nhau, isolation giúp gì?
2. Khi nào cần tách order service sang worker thread?
3. Đổi SQLite sang PostgreSQL, file nào phải đổi / không phải đổi?
4. Làm sao chứng minh một bug thiếu tiền là do avg cost hay do ledger?

Trả lời được = bạn đã nắm hệ thống, không chỉ “copy mẫu”.
