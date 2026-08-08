#pragma once
// docs/samples/phase3/main_window.hpp

#include <QMainWindow>
#include <QString>

struct SessionDto {
    int userId{};
    int accountId{};
    QString username;
};

class QLabel;
class QLineEdit;
class QComboBox;
class QSpinBox;
class QTableView;
class QPushButton;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

    void setSession(const SessionDto& session);

public slots:
    void updateCash(double cash);
    void showStatus(const QString& text);

signals:
    void depositRequested(double amount);
    void placeOrderRequested(const QString& symbol, const QString& side, int qty);
    void refreshRequested();

private:
    SessionDto session_{};
    QLabel* userLabel_{nullptr};
    QLabel* cashLabel_{nullptr};
    QLabel* statusLabel_{nullptr};

    QComboBox* symbolBox_{nullptr};
    QComboBox* sideBox_{nullptr};
    QSpinBox* qtySpin_{nullptr};
    QPushButton* submitButton_{nullptr};
    QTableView* positionsView_{nullptr};
};
