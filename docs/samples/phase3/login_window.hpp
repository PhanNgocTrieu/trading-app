#pragma once
// docs/samples/phase3/login_window.hpp
// Skeleton Qt Widgets — cần .cpp + AUTOMOC.

#include <QWidget>
#include <QString>

class QLineEdit;
class QLabel;
class QPushButton;

class LoginWindow : public QWidget {
    Q_OBJECT
public:
    explicit LoginWindow(QWidget* parent = nullptr);

public slots:
    void showError(const QString& message);

signals:
    void loginRequested(const QString& username, const QString& password);
    void registerRequested(const QString& username, const QString& password);

private:
    QLineEdit* usernameEdit_{nullptr};
    QLineEdit* passwordEdit_{nullptr};
    QLabel* statusLabel_{nullptr};
    QPushButton* loginButton_{nullptr};
    QPushButton* registerButton_{nullptr};
};
