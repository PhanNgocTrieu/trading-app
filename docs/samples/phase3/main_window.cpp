// docs/samples/phase3/main_window.cpp
#include "main_window.hpp"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QStatusBar>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setWindowTitle(QStringLiteral("Trading App"));
    resize(960, 640);

    auto* central = new QWidget(this);
    setCentralWidget(central);

    userLabel_ = new QLabel(this);
    cashLabel_ = new QLabel(QStringLiteral("Cash: -"), this);

    symbolBox_ = new QComboBox(this);
    symbolBox_->addItems({QStringLiteral("AAPL"), QStringLiteral("MSFT"), QStringLiteral("TSLA")});
    sideBox_ = new QComboBox(this);
    sideBox_->addItems({QStringLiteral("BUY"), QStringLiteral("SELL")});
    qtySpin_ = new QSpinBox(this);
    qtySpin_->setRange(1, 1'000'000);
    qtySpin_->setValue(10);
    submitButton_ = new QPushButton(QStringLiteral("Submit Order"), this);

    auto* depositSpin = new QDoubleSpinBox(this);
    depositSpin->setRange(1, 1'000'000'000);
    depositSpin->setValue(1000);
    auto* depositButton = new QPushButton(QStringLiteral("Deposit"), this);

    positionsView_ = new QTableView(this);

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
    walletRow->addWidget(depositSpin);
    walletRow->addWidget(depositButton);
    walletRow->addStretch();

    auto* root = new QVBoxLayout(central);
    root->addWidget(userLabel_);
    root->addWidget(cashLabel_);
    root->addLayout(walletRow);
    root->addLayout(tradeRow);
    root->addWidget(positionsView_);

    statusLabel_ = new QLabel(QStringLiteral("Ready"), this);
    statusBar()->addWidget(statusLabel_);

    connect(submitButton_, &QPushButton::clicked, this, [this]{
        emit placeOrderRequested(symbolBox_->currentText(),
                                 sideBox_->currentText(),
                                 qtySpin_->value());
    });
    connect(depositButton, &QPushButton::clicked, this, [depositSpin, this]{
        emit depositRequested(depositSpin->value());
    });
}

void MainWindow::setSession(const SessionDto& session) {
    session_ = session;
    userLabel_->setText(QStringLiteral("User: %1 (account %2)")
                            .arg(session.username)
                            .arg(session.accountId));
}

void MainWindow::updateCash(double cash) {
    cashLabel_->setText(QStringLiteral("Cash: %1").arg(cash, 0, 'f', 2));
}

void MainWindow::showStatus(const QString& text) {
    statusLabel_->setText(text);
}
