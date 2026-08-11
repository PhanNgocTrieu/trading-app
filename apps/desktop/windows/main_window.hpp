#pragma once

#include "dto.hpp"
#include "models/position_table_model.hpp"
#include "models/quote_table_model.hpp"

#include <QMainWindow>

class QLabel;
class QComboBox;
class QSpinBox;
class QDoubleSpinBox;
class QTableView;
class QPushButton;
class QAction;

namespace desktop {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

    void setSession(const SessionDto& session);
    void setDbPath(const QString& dbPath);
    PositionTableModel* positionModel() { return positionModel_; }
    QuoteTableModel* quoteModel() { return quoteModel_; }

public slots:
    void updateCash(double cash);
    void showStatus(const QString& text);
    void setQuotes(const QVector<QuoteUiDto>& quotes);
    void setOrderBusy(bool busy);
    void applyPortfolio(const QVector<PositionUiDto>& rows);
    void setFeedActive(bool active);

signals:
    void depositRequested(double amount);
    void placeOrderRequested(const QString& symbol, const QString& side, int qty);
    void refreshRequested();
    void logoutRequested();
    void feedToggled(bool enabled);
    void aboutRequested();

private:
    void rebuildOverview();
    void buildMenus();

    SessionDto session_{};
    QString dbPath_;
    double cash_{0.0};
    QVector<PositionUiDto> positions_;

    QLabel* userLabel_{nullptr};
    QLabel* cashLabel_{nullptr};
    QLabel* equityLabel_{nullptr};
    QLabel* upnlLabel_{nullptr};
    QLabel* feedLabel_{nullptr};
    QLabel* statusLabel_{nullptr};

    QComboBox* symbolBox_{nullptr};
    QComboBox* sideBox_{nullptr};
    QSpinBox* qtySpin_{nullptr};
    QDoubleSpinBox* depositSpin_{nullptr};
    QPushButton* submitButton_{nullptr};
    QPushButton* depositButton_{nullptr};
    QPushButton* refreshButton_{nullptr};
    QPushButton* feedButton_{nullptr};
    QTableView* positionsView_{nullptr};
    QTableView* quotesView_{nullptr};
    PositionTableModel* positionModel_{nullptr};
    QuoteTableModel* quoteModel_{nullptr};
};

} // namespace desktop
