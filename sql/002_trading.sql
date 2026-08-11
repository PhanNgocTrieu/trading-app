-- Phase 2: Paper trading (stocks, quotes, orders, trades, positions)
PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS stocks (
    symbol          TEXT PRIMARY KEY,
    name            TEXT NOT NULL,
    is_active       INTEGER NOT NULL DEFAULT 1
);

CREATE TABLE IF NOT EXISTS market_quotes (
    symbol          TEXT PRIMARY KEY,
    last_price      REAL NOT NULL CHECK (last_price > 0),
    updated_at      TEXT NOT NULL DEFAULT (datetime('now')),
    FOREIGN KEY (symbol) REFERENCES stocks(symbol)
);

CREATE TABLE IF NOT EXISTS orders (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    account_id      INTEGER NOT NULL,
    symbol          TEXT    NOT NULL,
    side            TEXT    NOT NULL CHECK (side IN ('BUY', 'SELL')),
    type            TEXT    NOT NULL CHECK (type IN ('MARKET', 'LIMIT')),
    quantity        INTEGER NOT NULL CHECK (quantity > 0),
    limit_price     REAL,
    status          TEXT    NOT NULL CHECK (status IN ('PENDING','FILLED','REJECTED','CANCELED')),
    created_at      TEXT    NOT NULL DEFAULT (datetime('now')),
    updated_at      TEXT    NOT NULL DEFAULT (datetime('now')),
    FOREIGN KEY (account_id) REFERENCES accounts(id),
    FOREIGN KEY (symbol) REFERENCES stocks(symbol)
);

CREATE TABLE IF NOT EXISTS trades (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    order_id        INTEGER NOT NULL,
    account_id      INTEGER NOT NULL,
    symbol          TEXT    NOT NULL,
    side            TEXT    NOT NULL CHECK (side IN ('BUY', 'SELL')),
    quantity        INTEGER NOT NULL CHECK (quantity > 0),
    price           REAL    NOT NULL CHECK (price > 0),
    executed_at     TEXT    NOT NULL DEFAULT (datetime('now')),
    FOREIGN KEY (order_id) REFERENCES orders(id),
    FOREIGN KEY (account_id) REFERENCES accounts(id),
    FOREIGN KEY (symbol) REFERENCES stocks(symbol)
);

CREATE TABLE IF NOT EXISTS positions (
    account_id      INTEGER NOT NULL,
    symbol          TEXT    NOT NULL,
    quantity        INTEGER NOT NULL CHECK (quantity >= 0),
    avg_cost        REAL    NOT NULL CHECK (avg_cost >= 0),
    updated_at      TEXT    NOT NULL DEFAULT (datetime('now')),
    PRIMARY KEY (account_id, symbol),
    FOREIGN KEY (account_id) REFERENCES accounts(id),
    FOREIGN KEY (symbol) REFERENCES stocks(symbol)
);

CREATE INDEX IF NOT EXISTS idx_orders_account_created
    ON orders(account_id, created_at DESC);

CREATE INDEX IF NOT EXISTS idx_trades_account_executed
    ON trades(account_id, executed_at DESC);

INSERT OR IGNORE INTO stocks(symbol, name) VALUES
 ('AAPL', 'Apple Inc.'),
 ('MSFT', 'Microsoft Corp.'),
 ('TSLA', 'Tesla Inc.');

INSERT OR IGNORE INTO market_quotes(symbol, last_price) VALUES
 ('AAPL', 190.0),
 ('MSFT', 420.0),
 ('TSLA', 250.0);
