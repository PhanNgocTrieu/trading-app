#pragma once

#include <QWidget>

class QLineEdit;
class QLabel;
class QPushButton;

namespace desktop {

class LoginWindow : public QWidget {
    Q_OBJECT
public:
    explicit LoginWindow(QWidget* parent = nullptr);

public slots:
    void showError(const QString& message);
    void clearStatus();
    void setBusy(bool busy);

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

} // namespace desktop
