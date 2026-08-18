#include "models/order_book_table_model.hpp"

namespace desktop {

OrderBookTableModel::OrderBookTableModel(QObject* parent)
    : QAbstractTableModel(parent) {}

int OrderBookTableModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return rows_.size();
}

int OrderBookTableModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return 3;
}

QVariant OrderBookTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= rows_.size()) {
        return {};
    }
    const BookLevelUiDto& row = rows_.at(index.row());
    if (role == SideRole) {
        return row.side;
    }
    if (role == PriceRole) {
        return row.price;
    }
    if (role == QuantityRole) {
        return row.quantity;
    }
    if (role == OrderCountRole) {
        return row.orderCount;
    }
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0:
                return QString::number(row.price, 'f', 2);
            case 1:
                return row.quantity;
            case 2:
                return row.orderCount;
            default:
                return {};
        }
    }
    return {};
}

QHash<int, QByteArray> OrderBookTableModel::roleNames() const {
    return {
        {SideRole, "side"},
        {PriceRole, "price"},
        {QuantityRole, "quantity"},
        {OrderCountRole, "orderCount"},
    };
}

void OrderBookTableModel::setLevels(const QVector<BookLevelUiDto>& rows) {
    beginResetModel();
    rows_ = rows;
    endResetModel();
}

} // namespace desktop
