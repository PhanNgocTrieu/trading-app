#pragma once

#include <string>

enum class LedgerType { Deposit, Withdraw, Buy, Sell, Fee, Adjustment };

inline const char* toString(LedgerType type) {
    switch (type) {
        case LedgerType::Deposit:     return "DEPOSIT";
        case LedgerType::Withdraw:    return "WITHDRAW";
        case LedgerType::Buy:         return "BUY";
        case LedgerType::Sell:        return "SELL";
        case LedgerType::Fee:         return "FEE";
        case LedgerType::Adjustment:  return "ADJUSTMENT";
    }
    return "UNKNOWN";
}

inline LedgerType ledgerTypeFromString(const std::string& value) {
    if (value == "WITHDRAW") return LedgerType::Withdraw;
    if (value == "BUY") return LedgerType::Buy;
    if (value == "SELL") return LedgerType::Sell;
    if (value == "FEE") return LedgerType::Fee;
    if (value == "ADJUSTMENT") return LedgerType::Adjustment;
    return LedgerType::Deposit;
}

struct LedgerEntry {
    int id{0};
    int accountId{0};
    LedgerType type{LedgerType::Deposit};
    double amount{0.0};
    double balanceAfter{0.0};
    std::string refType;
    int refId{0};
    bool hasRefId{false};
    std::string note;
    std::string createdAt;
};
