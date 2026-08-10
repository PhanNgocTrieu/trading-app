# Phase 1 Notes — SQLite Auth + Wallet

## Đã có

- Schema: `users`, `accounts`, `ledger_entries`, `schema_version` (`sql/001_init.sql`)
- Infra: `SqliteConnection`, `Transaction`, `runMigrations`
- Repos: user / account / ledger
- `SimplePasswordHasher` (`v1$salt$sha256`)
- `AuthAppService` + `WalletAppService`
- CLI: Register / Login / Deposit / Withdraw / View ledger
- DB file mặc định: `~/.trading-app/trading.db`

## Chạy

```bash
cmake -S . -B build && cmake --build build
./build/trading-app
ctest --test-dir build --output-on-failure
```

## Kiểm chứng DoD thủ công

1. Register user `alice` / `secret1`
2. Deposit `1000`, Withdraw `250` → cash `750`, ledger có 2 dòng
3. Quit app, mở lại, Login cùng credentials → cash vẫn `750`
4. Mở DB (optional): password_hash dạng `v1$...`, không chứa `secret1`
