#pragma once

#include "dto.hpp"

#include <QAbstractTableModel>
#include <QHash>
#include <QVector>

namespace desktop {

class WorkingOrderTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    enum Roles {
        OrderIdRole = Qt::UserRole + 1,
        SymbolRole,
        SideRole,
        QuantityRole,
        LimitPriceRole,
        StatusRole
    };

    explicit WorkingOrderTableModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setOrders(const QVector<OrderUiDto>& rows);

private:
    QVector<OrderUiDto> rows_;
};

} // namespace desktop
