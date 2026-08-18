#pragma once

#include "dto.hpp"

#include <QAbstractTableModel>
#include <QHash>
#include <QVector>

namespace desktop {

class OrderBookTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    enum Roles {
        SideRole = Qt::UserRole + 1,
        PriceRole,
        QuantityRole,
        OrderCountRole
    };

    explicit OrderBookTableModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setLevels(const QVector<BookLevelUiDto>& rows);

private:
    QVector<BookLevelUiDto> rows_;
};

} // namespace desktop
