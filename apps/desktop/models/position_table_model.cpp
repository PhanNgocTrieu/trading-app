#include "models/position_table_model.hpp"

namespace desktop {

PositionTableModel::PositionTableModel(QObject* parent)
    : QAbstractTableModel(parent) {}

int PositionTableModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return rows_.size();
}

int PositionTableModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return 6;
}

QVariant PositionTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) {
        return {};
    }
    if (index.row() < 0 || index.row() >= rows_.size()) {
        return {};
    }

    const PositionUiDto& row = rows_.at(index.row());
    switch (index.column()) {
        case 0:
            return row.symbol;
        case 1:
            return row.quantity;
        case 2:
            return QString::number(row.avgCost, 'f', 2);
        case 3:
            return QString::number(row.marketPrice, 'f', 2);
        case 4:
            return QString::number(row.marketValue, 'f', 2);
        case 5:
            return QString::number(row.unrealizedPnl, 'f', 2);
        default:
            return {};
    }
}

QVariant PositionTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
        return {};
    }
    switch (section) {
        case 0:
            return QStringLiteral("Symbol");
        case 1:
            return QStringLiteral("Qty");
        case 2:
            return QStringLiteral("Avg Cost");
        case 3:
            return QStringLiteral("Last");
        case 4:
            return QStringLiteral("Mkt Value");
        case 5:
            return QStringLiteral("uPnL");
        default:
            return {};
    }
}

void PositionTableModel::setPositions(const QVector<PositionUiDto>& rows) {
    beginResetModel();
    rows_ = rows;
    endResetModel();
}

} // namespace desktop
