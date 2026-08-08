// docs/samples/phase3/login_window.cpp
#include "login_window.hpp"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

LoginWindow::LoginWindow(QWidget* parent)
    : QWidget(parent) {
    setWindowTitle(QStringLiteral("Trading App — Login"));

    usernameEdit_ = new QLineEdit(this);
    passwordEdit_ = new QLineEdit(this);
    passwordEdit_->setEchoMode(QLineEdit::Password);
    statusLabel_ = new QLabel(this);
    loginButton_ = new QPushButton(QStringLiteral("Login"), this);
    registerButton_ = new QPushButton(QStringLiteral("Register"), this);

    auto* form = new QFormLayout();
    form->addRow(QStringLiteral("Username"), usernameEdit_);
    form->addRow(QStringLiteral("Password"), passwordEdit_);

    auto* buttons = new QHBoxLayout();
    buttons->addWidget(loginButton_);
    buttons->addWidget(registerButton_);

    auto* root = new QVBoxLayout(this);
    root->addLayout(form);
    root->addLayout(buttons);
    root->addWidget(statusLabel_);

    connect(loginButton_, &QPushButton::clicked, this, [this]{
        emit loginRequested(usernameEdit_->text(), passwordEdit_->text());
    });
    connect(registerButton_, &QPushButton::clicked, this, [this]{
        emit registerRequested(usernameEdit_->text(), passwordEdit_->text());
    });
}

void LoginWindow::showError(const QString& message) {
    statusLabel_->setText(message);
    statusLabel_->setStyleSheet(QStringLiteral("color: crimson;"));
}
