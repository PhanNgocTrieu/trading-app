# 06 — Kiến trúc Qt UI

> **Cập nhật Phase 5:** app desktop chính dùng **Qt Quick/QML** (`apps/desktop/qml` + `TradingAppBridge`). Console `trading-app-cli` đã gỡ; xem [phase5-notes.md](./phase5-notes.md).

## 1. Mục tiêu UI v1

Một desktop app rõ ràng, không fancy:

1. **LoginWindow** — đăng nhập / đăng ký
2. **MainWindow** — shell với trang:
   - Overview (cash, equity, PnL)
   - Market Watch
   - Order Ticket
   - Portfolio
   - Orders / Trades / Ledger history

Phong cách: Qt Widgets + layouts cơ bản.

---

## 2. Pattern: MVVM-lite / MVC

Khuyến nghị **MVC/Presenter** đơn giản:

```text
View (QWidget)  --events-->  Controller/Presenter  --commands-->  AppService
      ^                               |
      └────────── DTOs / signals ─────┘
```

- View: không gọi SQL
- Controller: mỏng, điều phối
- AppService: nghiệp vụ

Ví dụ thư mục:

```text
apps/desktop/
  main.cpp
  windows/
    login_window.h/.cpp
    main_window.h/.cpp
  views/
    portfolio_view.h/.cpp
    order_ticket_view.h/.cpp
    market_watch_view.h/.cpp
  controllers/
    auth_controller.h/.cpp
    order_controller.h/.cpp
```

---

## 3. Object tree & ownership

```text
QApplication
└─ LoginWindow / MainWindow (stack hoặc replace)
   ├─ QTabWidget
   │  ├─ OverviewPage
   │  ├─ MarketWatchPage
   │  ├─ OrderTicketPage
   │  └─ PortfolioPage
   └─ statusBar / menuBar
```

Dùng parent-child Qt để tránh leak.

---

## 4. Signals/Slots contracts

### AuthController

```cpp
class AuthController : public QObject {
    Q_OBJECT
public:
    explicit AuthController(AuthAppService& auth, QObject* parent = nullptr);

public slots:
    void login(const QString& user, const QString& pass);
    void registerUser(const QString& user, const QString& pass);

signals:
    void loginSucceeded(const SessionDto& session);
    void authFailed(const QString& message);
};
```

### OrderController

```cpp
public slots:
    void placeMarketOrder(const QString& symbol, const QString& side, int qty);

signals:
    void orderAccepted(const OrderDto& order);
    void orderRejected(const QString& reason);
    void portfolioNeedsRefresh();
```

View chỉ `connect` và cập nhật widget.

---

## 5. Threading model cho v1

**Bắt đầu đơn giản (khuyến nghị Phase 3):**

- Gọi AppService trên UI thread nếu thao tác DB < ~50ms
- SQLite local thường đủ nhanh cho paper trading quy mô nhỏ

**Khi có Market Feed timer:**

```cpp
// Feed có thể update DB trong slot QTimer (UI thread) — OK cho MVP
// hoặc chuyển QObject feed sang QThread nếu cần
```

**Khi thấy UI giật:**

```text
QtConcurrent::run([&]{ return orderService.place(...); })
  + QFutureWatcher → signal về UI
```

Đừng optimize sớm.

---

## 6. Wireframe logic (text)

### LoginWindow

```text
+---------------------------+
| Trading App               |
| User: [______________]    |
| Pass: [______________]    |
| [Login]  [Register]       |
| status: ...............   |
+---------------------------+
```

### MainWindow

```text
+--------------------------------------------------+
| File  Account  Help                  user: alice |
+--------------------------------------------------+
| Overview | Market | Trade | Portfolio | History  |
|--------------------------------------------------|
| Cash: 8,100   Equity: 12,100   uPnL: +200        |
|                                                  |
| [Order Ticket]                                   |
| Symbol [AAPL▼] Side[BUY▼] Qty[10] [Submit]       |
|                                                  |
| Positions table...                               |
+--------------------------------------------------+
| Ready                                            |
+--------------------------------------------------+
```

---

## 7. Model cho bảng (QAbstractTableModel)

Đừng nhồi `QTableWidget` setItem khắp nơi nếu bảng phức tạp. Học `QAbstractTableModel`:

```cpp
class PositionTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    int rowCount(const QModelIndex& = {}) const override { return rows_.size(); }
    int columnCount(const QModelIndex& = {}) const override { return 6; }
    QVariant data(const QModelIndex& idx, int role) const override;
    QVariant headerData(int section, Qt::Orientation, int role) const override;

    void setPositions(std::vector<PositionDto> rows);

private:
    std::vector<PositionDto> rows_;
};
```

Lợi ích: tách data khỏi view; refresh bằng `beginResetModel/endResetModel` (MVP) hoặc dataChanged.

---

## 8. CMake + Qt modules

```cmake
cmake_minimum_required(VERSION 3.21)
project(trading-app LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

add_executable(trading-app
    apps/desktop/main.cpp
    apps/desktop/windows/login_window.cpp
    apps/desktop/windows/main_window.cpp
    # domain/application/infrastructure sources...
)

target_link_libraries(trading-app PRIVATE Qt6::Widgets Qt6::Sql)
```

`CMAKE_AUTOMOC ON` bắt buộc khi có `Q_OBJECT`.

---

## 9. Không để Qt “rò” vào domain

| Được | Không được |
|------|------------|
| `QString` ở View/Controller biên | `QString` trong `MatchingEngine` |
| Convert `QString → std::string` trước khi gọi service | `#include <QWidget>` trong domain header |
| `QSqlDatabase` trong infrastructure nếu muốn | UI slot viết SQL dài |

Converter tiện:

```cpp
inline std::string toStd(const QString& s) { return s.toStdString(); }
inline QString toQ(const std::string& s) { return QString::fromStdString(s); }
```

---

## 10. UX rules tối thiểu (trading desktop)

1. Mọi action nguy hiểm cần feedback rõ (status bar / dialog)
2. Disable nút Submit khi request đang chạy (tránh double-submit)
3. Số tiền format nhất quán (2 decimals)
4. Reject phải nói **lý do nghiệp vụ** (“Insufficient funds”), không phải exception text
5. Sau fill: highlight / refresh portfolio ngay

---

## 11. Migration từ console Dashboard

Hiện `Dashboard::actionDashboard()` đang `cin` menu 1–4.

Ánh xạ:

| Console | Qt |
|---------|-----|
| showLoginDashboard | `LoginWindow` |
| option 1 View Account | Overview tab |
| option 2 Deposit | Wallet dialog/tab |
| option 3 Withdraw | Wallet dialog/tab |
| option 4 Trade | Order ticket tab |

Có thể giữ console app target `trading-app-cli` song song trong Phase 3 để so sánh/test.

---

## 12. Skeleton main.cpp (Qt)

```cpp
#include <QApplication>
#include "windows/login_window.h"
#include "windows/main_window.h"
#include "controllers/auth_controller.h"
// #include infrastructure bootstrap...

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("Trading App");

    // Bootstrap services (composition root)
    // auto services = bootstrap();

    AuthController authController{/* services.auth */};
    LoginWindow login;
    MainWindow mainWindow{/* controllers */};

    QObject::connect(&login, &LoginWindow::loginRequested,
                     &authController, &AuthController::login);
    QObject::connect(&authController, &AuthController::loginSucceeded,
                     &login, [&](const SessionDto& s){
                         login.hide();
                         mainWindow.setSession(s);
                         mainWindow.show();
                     });
    QObject::connect(&authController, &AuthController::authFailed,
                     &login, &LoginWindow::showError);

    login.show();
    return app.exec();
}
```

Chi tiết file mẫu: `docs/samples/phase3/`.
