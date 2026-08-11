#include "models/quote_table_model.hpp"

namespace desktop {

QuoteTableModel::QuoteTableModel(QObject* parent)
    : QAbstractTableModel(parent) {}

int QuoteTableModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return rows_.size();
}

int QuoteTableModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return 3;
}

QVariant QuoteTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) {
        return {};
    }
    if (index.row() < 0 || index.row() >= rows_.size()) {
        return {};
    }
    const QuoteUiDto& row = rows_.at(index.row());
    switch (index.column()) {
        case 0:
            return row.symbol;
        case 1:
            return row.name;
        case 2:
            return QString::number(row.lastPrice, 'f', 2);
        default:
            return {};
    }
}

QVariant QuoteTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
        return {};
    }
    switch (section) {
        case 0:
            return QStringLiteral("Symbol");
        case 1:
            return QStringLiteral("Name");
        case 2:
            return QStringLiteral("Last");
        default:
            return {};
    }
}

void QuoteTableModel::setQuotes(const QVector<QuoteUiDto>& rows) {
    beginResetModel();
    rows_ = rows;
    endResetModel();
}

} // namespace desktop
