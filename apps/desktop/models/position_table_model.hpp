#pragma once

#include "dto.hpp"

#include <QAbstractTableModel>
#include <QVector>

namespace desktop {

class PositionTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit PositionTableModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setPositions(const QVector<PositionUiDto>& rows);

private:
    QVector<PositionUiDto> rows_;
};

} // namespace desktop
