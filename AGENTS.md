# trading-app — agent instructions

Paper-trading desktop app: **C++17**, **SQLite**, **Qt 6 Quick (QML)**.
Phases 0–5 are done. Console CLI is gone. Do not revive `ui/` or `trading-app-cli`.

**Before writing code, read [`.agents/SKILL.md`](.agents/SKILL.md).**  
Then open only the supporting file you need:

| Need | File |
|------|------|
| Where a file lives / CMake targets | [`.agents/project-map.md`](.agents/project-map.md) |
| Naming, layers, SQL, Qt rules | [`.agents/conventions.md`](.agents/conventions.md) |
| Feature / test / schema checklists | [`.agents/workflows.md`](.agents/workflows.md) |

Human docs stay in `docs/` (architecture, domain, testing). Do not duplicate them here.

## Dependency rule (non-negotiable)

```
apps/desktop  →  application  →  domain
apps/desktop  →  infrastructure   (composition / wiring only)
application   →  domain + ports
infrastructure → domain (implements ports)
engine        →  domain
```

- Domain and `MatchingEngine` **must not** `#include` Qt or SQLite.
- QML talks only to context property `app` (`TradingAppBridge`). No SQL, no app services, no domain types in QML.
- Controllers call `*AppService`. They do not run SQL.
- Cash mutations always use `Transaction` **and** a ledger row.

## Route the change

| Request | Own it here | Do not |
|---------|-------------|--------|
| Invariants, avg cost, PnL, enums, `Result` | `include/domain/` | QML / SQL |
| Matching fill/reject | `include/engine/matching_engine.hpp` | Controllers |
| Register / login / deposit / place order | `include/application/` + `src/application/` | QML, `service/` |
| SQL, repos, migrations | `sql/` + `include/infrastructure/` + `src/infrastructure/` | Domain |
| Login UI, ticket, portfolio, feed | `apps/desktop/` (bridge → controllers → models → QML) | `trading_core` |
| Tests | `tests/phaseN/` matching the phase | Ad-hoc mains |

`service/` is a **test adapter** over app services. Do not put new business rules there.

## Defaults

- C++17, `#pragma once`, `enum class`, trailing `_` members, `snake_case` files, `PascalCase` types.
- Business errors: `Result<T>` / `ErrorCode`. Do not swallow with `catch (...)`.
- Parameterized SQL only (`?`). New schema → new `sql/00N_*.sql` + `runMigrations`.
- New `.cpp` → `src/CMakeLists.txt`. New test → `tests/CMakeLists.txt`. New QML → `apps/desktop/qml.qrc`.
- Commit only when asked. Never commit `*.db`, `build/`, secrets.
- Do not rewrite layers, expand to live brokers, or migrate money to integer cents unless asked.
