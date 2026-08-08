# 09 — Bảo mật, rủi ro & tuân thủ học tập

> Đây là app **paper trading học tập**, không phải sản phẩm tài chính được cấp phép. Vẫn nên luyện thói quen bảo mật đúng mực.

---

## 1. Bảo mật ứng dụng

### 1.1. Password

- Không lưu plain text (repo hiện tại đang có hướng plain — phải bỏ ở Phase 1)
- Hash + salt
- Message lỗi login chung: “Invalid username or password”

### 1.2. SQL Injection

Luôn dùng prepared statements:

```cpp
sqlite3_prepare_v2(db, "SELECT id FROM users WHERE username = ?;", -1, &stmt, nullptr);
sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
```

### 1.3. Sensitive data trên UI/log

- Không log password
- Không hardcode credentials trong source
- File DB user-level permissions (filesystem)

### 1.4. Dependency / supply chain

- Pin toolchain trong Docker khi có thể
- Thận trọng copy-paste crypto tự chế

---

## 2. Rủi ro nghiệp vụ (trading system)

| Rủi ro | Mitigations trong v1 |
|--------|----------------------|
| Double-submit order | Disable button + transaction |
| Negative cash | CHECK + validate trước commit |
| Oversell position | Validate qty |
| Race quote vs order | Đọc quote trong cùng transaction |
| Partial failure | RAII rollback |
| Audit gap | Ledger bắt buộc |

---

## 3. Rủi ro kiến trúc

- UI chứa business rules → khó test, dễ lệch
- Singleton DB global → khó kiểm soát lifecycle
- `double` money → sai số; ghi nhận để migrate cents

---

## 4. Những gì KHÔNG được hiểu nhầm

1. App này **không** kết nối sàn thật
2. Giá mock **không** phải giá giao dịch thực
3. Không dùng để quyết định đầu tư thật
4. Nếu sau này gắn API broker: cần OAuth/keys, risk controls, và hiểu pháp lý — ngoài scope v1

---

## 5. Checklist bảo mật trước khi demo

- [ ] Password hash
- [ ] Không SQL concatenate
- [ ] `*.db` không public trên git
- [ ] Reject path không crash
- [ ] Ledger đối soát được một vòng giao dịch mẫu
