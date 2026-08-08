#pragma once

#include <string>

// Instrument identity. Live price belongs to market quote data (Phase 2+).
class Stock {
public:
    Stock(std::string symbol, std::string name, double lastPrice = 0.0)
        : symbol_(std::move(symbol))
        , name_(std::move(name))
        , lastPrice_(lastPrice) {}

    const std::string& symbol() const { return symbol_; }
    const std::string& name() const { return name_; }
    double lastPrice() const { return lastPrice_; }

    void updatePrice(double price) { lastPrice_ = price; }

private:
    std::string symbol_;
    std::string name_;
    double lastPrice_;
};
