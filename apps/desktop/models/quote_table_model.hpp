#pragma once

#include "dto.hpp"

#include <QAbstractTableModel>
#include <QVector>

namespace desktop {

class QuoteTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit QuoteTableModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setQuotes(const QVector<QuoteUiDto>& rows);

private:
    QVector<QuoteUiDto> rows_;
};

} // namespace desktop
