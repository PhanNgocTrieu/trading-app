#include "models/working_order_table_model.hpp"

namespace desktop {

WorkingOrderTableModel::WorkingOrderTableModel(QObject* parent)
    : QAbstractTableModel(parent) {}

int WorkingOrderTableModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return rows_.size();
}

int WorkingOrderTableModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return 5;
}

QVariant WorkingOrderTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= rows_.size()) {
        return {};
    }
    const OrderUiDto& row = rows_.at(index.row());
    if (role == OrderIdRole) {
        return row.orderId;
    }
    if (role == SymbolRole) {
        return row.symbol;
    }
    if (role == SideRole) {
        return row.side;
    }
    if (role == QuantityRole) {
        return row.quantity;
    }
    if (role == LimitPriceRole) {
        return row.limitPrice;
    }
    if (role == StatusRole) {
        return row.status;
    }
    return {};
}

QHash<int, QByteArray> WorkingOrderTableModel::roleNames() const {
    return {
        {OrderIdRole, "orderId"},
        {SymbolRole, "symbol"},
        {SideRole, "side"},
        {QuantityRole, "quantity"},
        {LimitPriceRole, "limitPrice"},
        {StatusRole, "status"},
    };
}

void WorkingOrderTableModel::setOrders(const QVector<OrderUiDto>& rows) {
    beginResetModel();
    rows_ = rows;
    endResetModel();
}

} // namespace desktop
