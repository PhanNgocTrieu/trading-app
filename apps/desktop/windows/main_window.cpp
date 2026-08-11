#include "windows/main_window.hpp"

#include <QAbstractItemView>
#include <QAction>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QStatusBar>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>

namespace desktop {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setWindowTitle(QStringLiteral("Trading App"));
    resize(1100, 720);
    buildMenus();

    auto* central = new QWidget(this);
    setCentralWidget(central);

    userLabel_ = new QLabel(this);
    cashLabel_ = new QLabel(QStringLiteral("Cash: -"), this);
    equityLabel_ = new QLabel(QStringLiteral("Equity: -"), this);
    upnlLabel_ = new QLabel(QStringLiteral("uPnL: -"), this);
    feedLabel_ = new QLabel(QStringLiteral("Feed: off"), this);

    symbolBox_ = new QComboBox(this);
    sideBox_ = new QComboBox(this);
    sideBox_->addItems({QStringLiteral("BUY"), QStringLiteral("SELL")});
    qtySpin_ = new QSpinBox(this);
    qtySpin_->setRange(1, 1'000'000);
    qtySpin_->setValue(10);
    submitButton_ = new QPushButton(QStringLiteral("Submit Order"), this);

    depositSpin_ = new QDoubleSpinBox(this);
    depositSpin_->setRange(1, 1'000'000'000);
    depositSpin_->setDecimals(2);
    depositSpin_->setValue(1000);
    depositButton_ = new QPushButton(QStringLiteral("Deposit"), this);
    refreshButton_ = new QPushButton(QStringLiteral("Refresh"), this);
    feedButton_ = new QPushButton(QStringLiteral("Start Feed"), this);
    auto* logoutButton = new QPushButton(QStringLiteral("Logout"), this);

    quoteModel_ = new QuoteTableModel(this);
    quotesView_ = new QTableView(this);
    quotesView_->setModel(quoteModel_);
    quotesView_->horizontalHeader()->setStretchLastSection(true);
    quotesView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    quotesView_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    quotesView_->setMaximumHeight(160);

    positionModel_ = new PositionTableModel(this);
    positionsView_ = new QTableView(this);
    positionsView_->setModel(positionModel_);
    positionsView_->horizontalHeader()->setStretchLastSection(true);
    positionsView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    positionsView_->setEditTriggers(QAbstractItemView::NoEditTriggers);

    auto* tradeRow = new QHBoxLayout();
    tradeRow->addWidget(new QLabel(QStringLiteral("Symbol"), this));
    tradeRow->addWidget(symbolBox_);
    tradeRow->addWidget(new QLabel(QStringLiteral("Side"), this));
    tradeRow->addWidget(sideBox_);
    tradeRow->addWidget(new QLabel(QStringLiteral("Qty"), this));
    tradeRow->addWidget(qtySpin_);
    tradeRow->addWidget(submitButton_);

    auto* walletRow = new QHBoxLayout();
    walletRow->addWidget(new QLabel(QStringLiteral("Deposit amount"), this));
    walletRow->addWidget(depositSpin_);
    walletRow->addWidget(depositButton_);
    walletRow->addWidget(refreshButton_);
    walletRow->addWidget(feedButton_);
    walletRow->addStretch();
    walletRow->addWidget(logoutButton);

    auto* headerRow = new QHBoxLayout();
    headerRow->addWidget(userLabel_);
    headerRow->addStretch();
    headerRow->addWidget(cashLabel_);
    headerRow->addWidget(equityLabel_);
    headerRow->addWidget(upnlLabel_);
    headerRow->addWidget(feedLabel_);

    auto* root = new QVBoxLayout(central);
    root->addLayout(headerRow);
    root->addLayout(walletRow);
    root->addWidget(new QLabel(QStringLiteral("Market Watch"), this));
    root->addWidget(quotesView_);
    root->addWidget(new QLabel(QStringLiteral("Order Ticket"), this));
    root->addLayout(tradeRow);
    root->addWidget(new QLabel(QStringLiteral("Portfolio"), this));
    root->addWidget(positionsView_);

    statusLabel_ = new QLabel(QStringLiteral("Ready"), this);
    statusBar()->addWidget(statusLabel_);

    connect(submitButton_, &QPushButton::clicked, this, [this] {
        if (symbolBox_->currentText().trimmed().isEmpty()) {
            showStatus(QStringLiteral("Select a symbol"));
            return;
        }
        if (qtySpin_->value() <= 0) {
            showStatus(QStringLiteral("Quantity must be > 0"));
            return;
        }
        emit placeOrderRequested(symbolBox_->currentText(),
                                 sideBox_->currentText(),
                                 qtySpin_->value());
    });
    connect(depositButton_, &QPushButton::clicked, this, [this] {
        if (depositSpin_->value() <= 0.0) {
            showStatus(QStringLiteral("Deposit amount must be > 0"));
            return;
        }
        emit depositRequested(depositSpin_->value());
    });
    connect(refreshButton_, &QPushButton::clicked, this, &MainWindow::refreshRequested);
    connect(logoutButton, &QPushButton::clicked, this, &MainWindow::logoutRequested);
    connect(feedButton_, &QPushButton::clicked, this, [this] {
        const bool enable = !feedButton_->text().contains(QStringLiteral("Stop"));
        emit feedToggled(enable);
    });
}

void MainWindow::buildMenus() {
    auto* helpMenu = menuBar()->addMenu(QStringLiteral("Help"));
    auto* aboutAction = helpMenu->addAction(QStringLiteral("About"));
    connect(aboutAction, &QAction::triggered, this, [this] {
        const QString body =
            QStringLiteral(
                "<b>Trading App</b> v1 (paper trading)<br><br>"
                "Phase 4: mock market feed + portfolio mark-to-market.<br>"
                "DB: <code>%1</code><br><br>"
                "Orders fill at last quote inside one SQLite transaction.")
                .arg(dbPath_.isEmpty() ? QStringLiteral("(unknown)") : dbPath_);
        QMessageBox::about(this, QStringLiteral("About Trading App"), body);
        emit aboutRequested();
    });
}

void MainWindow::setSession(const SessionDto& session) {
    session_ = session;
    cash_ = session.cashBalance;
    userLabel_->setText(QStringLiteral("User: %1 (account %2)")
                            .arg(session.username)
                            .arg(session.accountId));
    rebuildOverview();
}

void MainWindow::setDbPath(const QString& dbPath) {
    dbPath_ = dbPath;
}

void MainWindow::updateCash(double cash) {
    cash_ = cash;
    rebuildOverview();
}

void MainWindow::showStatus(const QString& text) {
    statusLabel_->setText(text);
}

void MainWindow::setQuotes(const QVector<QuoteUiDto>& quotes) {
    quoteModel_->setQuotes(quotes);

    const QString current = symbolBox_->currentText();
    symbolBox_->clear();
    for (const auto& quote : quotes) {
        symbolBox_->addItem(quote.symbol);
    }
    const int idx = symbolBox_->findText(current);
    if (idx >= 0) {
        symbolBox_->setCurrentIndex(idx);
    }
}

void MainWindow::setOrderBusy(bool busy) {
    submitButton_->setEnabled(!busy);
    depositButton_->setEnabled(!busy);
}

void MainWindow::applyPortfolio(const QVector<PositionUiDto>& rows) {
    positions_ = rows;
    positionModel_->setPositions(rows);
    rebuildOverview();
}

void MainWindow::setFeedActive(bool active) {
    feedButton_->setText(active ? QStringLiteral("Stop Feed") : QStringLiteral("Start Feed"));
    feedLabel_->setText(active ? QStringLiteral("Feed: on") : QStringLiteral("Feed: off"));
}

void MainWindow::rebuildOverview() {
    cashLabel_->setText(QStringLiteral("Cash: %1").arg(cash_, 0, 'f', 2));

    double marketValue = 0.0;
    double unrealized = 0.0;
    for (const auto& row : positions_) {
        marketValue += row.marketValue;
        unrealized += row.unrealizedPnl;
    }
    const double equity = cash_ + marketValue;
    equityLabel_->setText(QStringLiteral("Equity: %1").arg(equity, 0, 'f', 2));

    const QString sign = unrealized >= 0.0 ? QStringLiteral("+") : QString();
    upnlLabel_->setText(
        QStringLiteral("uPnL: %1%2").arg(sign).arg(unrealized, 0, 'f', 2));
}

} // namespace desktop
