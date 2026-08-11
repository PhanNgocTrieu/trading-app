#include "windows/login_window.hpp"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace desktop {

LoginWindow::LoginWindow(QWidget* parent)
    : QWidget(parent) {
    setWindowTitle(QStringLiteral("Trading App — Login"));
    setMinimumWidth(360);

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

    auto request = [this](bool isRegister) {
        const QString user = usernameEdit_->text().trimmed();
        const QString pass = passwordEdit_->text();
        if (user.size() < 3) {
            showError(QStringLiteral("Username must be at least 3 characters"));
            return;
        }
        if (pass.size() < 6) {
            showError(QStringLiteral("Password must be at least 6 characters"));
            return;
        }
        clearStatus();
        if (isRegister) {
            emit registerRequested(user, pass);
        } else {
            emit loginRequested(user, pass);
        }
    };

    connect(loginButton_, &QPushButton::clicked, this, [request] { request(false); });
    connect(registerButton_, &QPushButton::clicked, this, [request] { request(true); });
}

void LoginWindow::showError(const QString& message) {
    statusLabel_->setText(message);
    statusLabel_->setStyleSheet(QStringLiteral("color: crimson;"));
}

void LoginWindow::clearStatus() {
    statusLabel_->clear();
    statusLabel_->setStyleSheet(QString());
}

void LoginWindow::setBusy(bool busy) {
    loginButton_->setEnabled(!busy);
    registerButton_->setEnabled(!busy);
}

} // namespace desktop
