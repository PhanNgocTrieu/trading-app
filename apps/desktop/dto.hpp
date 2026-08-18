#pragma once

#include <QMetaType>
#include <QString>
#include <QVector>

#include <string>

namespace desktop {

inline std::string toStd(const QString& value) {
    return value.toStdString();
}

inline QString toQ(const std::string& value) {
    return QString::fromStdString(value);
}

struct SessionDto {
    int userId{0};
    int accountId{0};
    QString username;
    double cashBalance{0.0};
};

struct OrderUiDto {
    int orderId{0};
    QString symbol;
    QString side;
    QString type;
    int quantity{0};
    double fillPrice{0.0};
    double limitPrice{0.0};
    bool hasLimitPrice{false};
    QString status;
    bool hasRealizedPnl{false};
    double realizedPnl{0.0};
};

struct BookLevelUiDto {
    QString side;
    double price{0.0};
    int quantity{0};
    int orderCount{0};
};

struct PositionUiDto {
    QString symbol;
    int quantity{0};
    double avgCost{0.0};
    double marketPrice{0.0};
    double marketValue{0.0};
    double unrealizedPnl{0.0};
};

struct QuoteUiDto {
    QString symbol;
    QString name;
    double lastPrice{0.0};
};

} // namespace desktop

Q_DECLARE_METATYPE(desktop::SessionDto)
Q_DECLARE_METATYPE(desktop::OrderUiDto)
Q_DECLARE_METATYPE(desktop::PositionUiDto)
Q_DECLARE_METATYPE(desktop::QuoteUiDto)
Q_DECLARE_METATYPE(desktop::BookLevelUiDto)
Q_DECLARE_METATYPE(QVector<desktop::PositionUiDto>)
Q_DECLARE_METATYPE(QVector<desktop::QuoteUiDto>)
Q_DECLARE_METATYPE(QVector<desktop::OrderUiDto>)
Q_DECLARE_METATYPE(QVector<desktop::BookLevelUiDto>)
