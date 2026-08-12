# Phase 5 notes — QML redesign

## Why a bridge?

QML is presentation only. `TradingAppBridge` owns bootstrap/controllers/feed and exposes:

| QML uses | Bridge API |
|----------|------------|
| Login / Register buttons | `login` / `registerUser` |
| Header chips | `cash`, `equity`, `unrealizedPnl` |
| Market Watch list | `quoteModel` |
| Portfolio list | `positionModel` |
| Order ticket | `placeMarketOrder`, `symbols` |
| Feed toggle | `setFeedActive` |

Context property name: **`app`**.

## File map

```text
apps/desktop/main.cpp          # QGuiApplication + load Main.qml
apps/desktop/bridge/           # TradingAppBridge
apps/desktop/qml/Main.qml      # StackView Login <-> Shell
apps/desktop/qml/Theme.qml     # Light-studio colors/fonts
apps/desktop/qml/pages/        # LoginPage, ShellPage
apps/desktop/qml/components/   # Panels, buttons, chips
```

## Console CLI

`trading-app-cli` and `ui/` were removed. Desktop QML is the only end-user app target.
