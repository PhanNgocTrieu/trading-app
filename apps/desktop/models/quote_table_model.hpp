#pragma once

#include "dto.hpp"

#include <QAbstractTableModel>
#include <QHash>
#include <QVector>

namespace desktop {

class QuoteTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    enum Roles {
        SymbolRole = Qt::UserRole + 1,
        NameRole,
        LastPriceRole
    };

    explicit QuoteTableModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setQuotes(const QVector<QuoteUiDto>& rows);

private:
    QVector<QuoteUiDto> rows_;
};

} // namespace desktop
